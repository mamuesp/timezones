#ifndef __MGOS_DEVICES_I2C_H
#define __MGOS_DEVICES_I2C_H

#include "mgos.h"
#include "mgos_app.h"
#include "mgos_i2c.h"
#include "mgos_timers.h"
//#include "esp32_hw_timers.h"

#define MODE_SEQ   0
#define MODE_BLINK 1

struct device_ctrl {
	int addr;
	uint8_t mode;
	uint8_t bitmask;
	uint8_t last;
	uint8_t curr;
	uint8_t steps;
	uint8_t red;
	uint8_t yellow;
	uint8_t green;
	uint16_t delay;
	uint8_t *seq;
	mgos_timer_id timerId;
	struct mgos_i2c *i2c;
};

bool mgos_devices_i2c_init(void);

void set_device_i2c(uint16_t addr, uint8_t pins, void *oldDevice);
void *test_lamp(int red, int yellow, int green, int delay, int addr, int mask);
void *blink_lamp(int pins, int delay, int addr, int mask);

IRAM void handleSequence(struct device_ctrl *currDevice);
IRAM void _set_device_i2c(struct device_ctrl *currDevice);
IRAM void device_cb(void *arg);

#endif // __MGOS_DEVICES_I2C_H