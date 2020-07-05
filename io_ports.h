#ifndef _IO_PORTS_H
#define _IO_PORTS_H

#include <stdbool.h>
#include "state_machine.h"

typedef enum {
    LED_STATE_BOOTING,
    LED_STATE_SHUTTING_DOWN,
    LED_STATE_POWERED_OFF,
    LED_STATE_RUNNING,

} eLedState;

typedef enum {
    LED_MODE_PULSE,
    LED_MODE_BLINK,
    LED_MODE_ON,
    LED_MODE_OFF
} eLedMode;

void io_ports_init(void);
void button_handled(void);
void button_middle_handled(void);
void button_long_handled(void);
bool button_pressed(void);
bool button_middle_pressed(void);
bool button_long_pressed(void);
void led_set_state(eLedState state);
eState gpio_get_raspberry_state(void);
void system_power_on(void);
void system_power_off(void);
void raspberry_power_off_gpio(void);
void raspberry_trigger_gpio(void);

#endif //_IO_PORTS_H
