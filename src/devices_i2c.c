#include <stdbool.h>
#include "devices_i2c.h"

/*
 * Hardware timer functions
 * 
 */
void set_device_i2c(uint16_t addr, uint8_t pins, void *oldDevice) {
	
	struct device_ctrl *currDevice = (struct device_ctrl *) oldDevice;
  static uint8_t currPins;
	struct mgos_i2c *i2c;
	
	// if a timer is active, we stop it
	if (currDevice->timerId != 0		) {
		mgos_clear_timer(currDevice->timerId);
	}

	i2c = mgos_i2c_get_global();
  if (i2c == NULL) {
    LOG(LL_DEBUG, ("I2C is disabled - error: %d", 503));
  } else {
 		if (!mgos_i2c_read(i2c, addr, &currPins, 1, true)) {
    	LOG(LL_DEBUG, ("I2C read failed - error: %d", 503));
 		} else {
			LOG(LL_INFO, ("I2C read - addr: %d - pins: %d", addr, currPins));
		 	pins = (currPins & 0xF8) | pins;
			LOG(LL_INFO, ("I2C write - addr: %d - pins: %d", addr, pins));
		  if (!mgos_i2c_write(i2c, addr, &pins, 1, true)) {
  	  	LOG(LL_DEBUG, ("I2C write failed - error: %d", 503));
			}
 		}
  }
}

IRAM void handleSequence(struct device_ctrl *currDevice) {
	if (currDevice->curr >= currDevice->steps) {
		if (currDevice->addr == -1) {
			mgos_gpio_write(currDevice->red, 0);
			mgos_gpio_write(currDevice->yellow, 0);
			mgos_gpio_write(currDevice->green, 0);
		} else {
			currDevice->curr = 0;
			_set_device_i2c(currDevice);
		}
		(void) arg;
		return;
	}
	if (currDevice->addr == -1) {
		if (currDevice->curr > 0) {
			mgos_gpio_write(currDevice->seq[currDevice->curr], 0);
		}
	}
	currDevice->curr++;
	if (currDevice->curr < currDevice->steps) {
		if (currDevice->addr == -1) {
			if (currDevice->seq[currDevice->curr] > 0) {
				mgos_gpio_write(currDevice->seq[currDevice->curr], 1);
			}
		} else {
			_set_device_i2c(currDevice);
		}
		currDevice->timerId = mgos_set_hw_timer(1000 * currDevice->delay, MGOS_ESP32_HW_TIMER_IRAM, device_cb, currDevice); 
	}

}

IRAM void handleBlink(struct device_ctrl *currDevice) {
	_set_device_i2c(currDevice);
	currDevice->last = (currDevice->last == 1) ? 0 : 1;
	currDevice->timerID = mgos_set_hw_timer(1000 * currDevice->delay, MGOS_ESP32_HW_TIMER_IRAM, device_cb, currDevice); 
}

IRAM void _set_device_i2c(struct device_ctrl *currDevice) {

  static uint8_t currPins;
  static uint8_t pins;
  static uint8_t newPins = currDevice->seq[currDevice->curr];
  
  if (currDevice->i2c != NULL) {
 		if (mgos_i2c_read(currDevice->i2c, currDevice->addr, &currPins, 1, true)) {
			if (currDevice->mode == MODE_BLINK) {
				newPins = currDevice->mask & (currDevice->last == 0 ? ~currDevice->curr : currDevice->curr);
		  } 
		 	pins = (currPins & ~currDevice->mask) | (newPins & currDevice->mask);
		  mgos_i2c_write(currDevice->i2c, currDevice->addr, &pins, 1, true);
 		}
  }
}

IRAM void device_cb(void *arg) {

  struct device_ctrl *currDevice = (struct device_ctrl *) arg;;
  
  switch (currDevice->mode) {
  	case MODE_BLINK:
			handleBlink(currDevice);
  	break;
  	case MODE_SEQ:
  	default:
  		handleSequence(currDevice);
  	break;
  }

 	(void) arg;
}

void *blink_lamp(int pin, int delay, int addr, int mask) {

	static struct device_ctrl currDevice;

	struct mgos_i2c *myI2C = mgos_i2c_get_global();
  
  currDevice.mode = MODE_BLINK;
  currDevice.mask = mask;
  currDevice.curr = pin;
  currDevice.last = 0;
  currDevice.delay = delay;
  currDevice.addr = addr;
  currDevice.i2c = myI2C;
 
 	if (currDevice.addr == -1) {
		mgos_gpio_set_mode(red, MGOS_GPIO_MODE_OUTPUT);
		mgos_gpio_set_pull(red, MGOS_GPIO_PULL_DOWN);
		mgos_gpio_write(red, 0);
		mgos_gpio_set_mode(yellow, MGOS_GPIO_MODE_OUTPUT);
		mgos_gpio_set_pull(yellow, MGOS_GPIO_PULL_DOWN);
		mgos_gpio_write(yellow, 0);
		mgos_gpio_set_mode(green, MGOS_GPIO_MODE_OUTPUT);
		mgos_gpio_set_pull(green, MGOS_GPIO_PULL_DOWN);
		mgos_gpio_write(green, 0);
 	} else {
	 	_set_device_i2c(&currDevice);
 	}
	currDevice.timerID = mgos_set_hw_timer(1000 * currDevice.delay, MGOS_ESP32_HW_TIMER_IRAM, device_cb, &currDevice); 
	return &currDevice;
}

void *test_lamp(int red, int yellow, int green, int delay, int addr, int mask) {

	static struct device_ctrl currDevice;

	struct mgos_i2c *myI2C = mgos_i2c_get_global();
  static uint8_t seqI2C[5] = { 0, 1, 2, 4, 0 };
  static uint8_t seqGPIO[5];
  
  seqGPIO[0] = 0;
  seqGPIO[1] = red;
  seqGPIO[2] = yellow;
  seqGPIO[3] = green;
  seqGPIO[4] = 0;

  currDevice.mode = MODE_SEQ;
  currDevice.curr = 0;
  currDevice.last = 0;
  currDevice.steps = 5;
  currDevice.seq = (addr == -1) ? seqGPIO : seqI2C;
  currDevice.red = red;
  currDevice.yellow = yellow;
  currDevice.green = green;
  currDevice.mask = mask;
  currDevice.delay = delay;
  currDevice.addr = addr;
  currDevice.i2c = myI2C;
 
 	if (currDevice.addr == -1) {
		mgos_gpio_set_mode(red, MGOS_GPIO_MODE_OUTPUT);
		mgos_gpio_set_pull(red, MGOS_GPIO_PULL_DOWN);
		mgos_gpio_write(red, 0);
		mgos_gpio_set_mode(yellow, MGOS_GPIO_MODE_OUTPUT);
		mgos_gpio_set_pull(yellow, MGOS_GPIO_PULL_DOWN);
		mgos_gpio_write(yellow, 0);
		mgos_gpio_set_mode(green, MGOS_GPIO_MODE_OUTPUT);
		mgos_gpio_set_pull(green, MGOS_GPIO_PULL_DOWN);
		mgos_gpio_write(green, 0);
 	} else {
	 	_set_device_i2c(&currDevice);
 	}
	currDevice.timerID = mgos_set_hw_timer(1000 * currDevice.delay,  MGOS_ESP32_HW_TIMER_IRAM, device_cb, &currDevice ); 
	return &currDevice;
}

bool mgos_devices_i2c_init(void) {
  return true;
}
