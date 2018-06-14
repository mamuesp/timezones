#include <stdbool.h>
#include "devices_i2c.h"

/*
 * Hardware timer functions
 * 
 */

void stopOldTimer() {
	// if a timer is active, we stop it
	if (currDevice.timerId != 0) {
		mgos_clear_timer(currDevice.timerId);
		currDevice.timerId = 0;
 	}
}

void set_device_i2c(uint16_t addr, uint8_t pins) {
	
  uint8_t currPins;
	struct mgos_i2c *i2c;

	stopOldTimer();

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

IRAM void handleSequence() {
	if (currDevice.curr >= currDevice.steps) {
		if (currDevice.addr == -1) {
			mgos_gpio_write(currDevice.red, 0);
			mgos_gpio_write(currDevice.yellow, 0);
			mgos_gpio_write(currDevice.green, 0);
		} else {
			currDevice.curr = 0;
			_set_device_i2c(currDevice);
		}
		return;
	}
	if (currDevice.addr == -1) {
		if (currDevice.curr > 0) {
			mgos_gpio_write(currDevice.seq[currDevice.curr], 0);
		}
	}
	currDevice.curr++;
	if (currDevice.curr < currDevice.steps) {
		if (currDevice.addr == -1) {
			if (currDevice.seq[currDevice.curr] > 0) {
				mgos_gpio_write(currDevice.seq[currDevice.curr], 1);
			}
		} else {
			_set_device_i2c(currDevice);
		}
		currDevice.timerId = mgos_set_hw_timer(1000 * currDevice.delay, MGOS_ESP32_HW_TIMER_IRAM, device_cb, &currDevice); 
	}
}

IRAM void handleBlink() {
	_set_device_i2c();

	if (currDevice.curr != 0) {
		currDevice.last = (currDevice.last == 1) ? 0 : 1;
		currDevice.timerId = mgos_set_hw_timer(1000 * currDevice.delay, MGOS_ESP32_HW_TIMER_IRAM, device_cb, &currDevice); 
	}
}

IRAM void _set_device_i2c() {

  uint8_t currPins;
  uint8_t pins;
  uint8_t newPins;

	stopOldTimer();

  if (currDevice.i2c != NULL) {
 		if (mgos_i2c_read(currDevice.i2c, currDevice.addr, &currPins, 1, true)) {
			if (currDevice.mode == MODE_BLINK) {
				newPins = currDevice.bitmask & (currDevice.last == 0 ? (currDevice.curr ^ currDevice.curr) : currDevice.curr);
		  } else {
				newPins = currDevice.seq[currDevice.curr];
		  }
		 	pins = (currPins & ~currDevice.bitmask) | (newPins & currDevice.bitmask);
		  mgos_i2c_write(currDevice.i2c, currDevice.addr, &pins, 1, true);
 		}
  }
}

IRAM void device_cb() {

  switch (currDevice.mode) {
  	case MODE_BLINK:
			handleBlink();
  	break;
  	case MODE_SEQ:
  	default:
  		handleSequence();
  	break;
  }
}

void blink_lamp(int pin, int delay, int addr, int mask) {
	
	stopOldGimer();
	 
  currDevice.mode = MODE_BLINK;
  currDevice.bitmask = mask;
  currDevice.curr = pin;
  currDevice.last = 1;
  currDevice.delay = delay;
  currDevice.addr = addr;
  currDevice.i2c = mgos_i2c_get_global();
 
 	if (currDevice.addr == -1) {
		mgos_gpio_set_mode(pin, MGOS_GPIO_MODE_OUTPUT);
		mgos_gpio_set_pull(pin, MGOS_GPIO_PULL_DOWN);
		mgos_gpio_write(pin, 0);
 	} else {
	 	_set_device_i2c(&currDevice);
 	}
	currDevice.timerId = mgos_set_hw_timer(1000 * currDevice.delay, MGOS_ESP32_HW_TIMER_IRAM, device_cb, &currDevice); 
	return currDevice.timerId;
}

void test_lamp(char *args, int argLen) {

	struct lamp_config *lcCurr = getLampConfig(args, argLen);
  static uint8_t seqI2C[5] = { 0, 1, 2, 4, 0 };
  static uint8_t seqGPIO[5];

	stopOldTimer();
	  
  seqGPIO[0] = 0;
  seqGPIO[1] = lcCurr->RED;
  seqGPIO[2] = lcCurr->YELLOW;
  seqGPIO[3] = lcCurr->GREEN;
  seqGPIO[4] = 0;

  currDevice.mode = MODE_SEQ;
  currDevice.curr = 0;
  currDevice.last = 0;
  currDevice.steps = 5;
  currDevice.seq = (lcCurr->addr == -1) ? seqGPIO : seqI2C;
  currDevice.red = lcCurr->RED;
  currDevice.yellow = lcCurr->YELLOW;
  currDevice.green = lcCurr->GREEN;
  currDevice.bitmask = lcCurr->mask;
  currDevice.delay = lcCurr->delay;
  currDevice.addr = lcCurr->addr;
  currDevice.i2c = mgos_i2c_get_global();
 
 	if (currDevice.addr == -1) {
		mgos_gpio_set_mode(currDevice.red, MGOS_GPIO_MODE_OUTPUT);
		mgos_gpio_set_pull(currDevice.red, MGOS_GPIO_PULL_DOWN);
		mgos_gpio_write(currDevice.red, 0);
		mgos_gpio_set_mode(currDevice.yellow, MGOS_GPIO_MODE_OUTPUT);
		mgos_gpio_set_pull(currDevice.yellow, MGOS_GPIO_PULL_DOWN);
		mgos_gpio_write(currDevice.yellow, 0);
		mgos_gpio_set_mode(currDevice.green, MGOS_GPIO_MODE_OUTPUT);
		mgos_gpio_set_pull(currDevice.green, MGOS_GPIO_PULL_DOWN);
		mgos_gpio_write(currDevice.green, 0);
 	} else {
	 	_set_device_i2c(&currDevice);
 	}
	currDevice.timerId = mgos_set_hw_timer(1000 * currDevice.delay, MGOS_ESP32_HW_TIMER_IRAM, device_cb, &currDevice); 
}

struct lamp_config *getLampConfig(const char *args, int argLen) {

  static int RED, YELLOW, GREEN, delay, mask, addr;
	
  LOG(LL_ERROR, ("getLampConfig: %.*s", argLen, args));
  json_scanf(args, argLen, "{ RED:%d, YELLOW:%d, GREEN:%d, delay:%d, mask:%d, addr:%d }", &RED, &YELLOW, &GREEN, &delay, &mask, &addr);
  
  lcCurr.RED = RED;
  lcCurr.YELLOW = YELLOW;
  lcCurr.GREEN = GREEN;
  lcCurr.delay = delay;
  lcCurr.mask = mask;
  lcCurr.addr = addr;
  
  return &lcCurr;
}

bool mgos_devices_i2c_init(void) {
  return true;
}
