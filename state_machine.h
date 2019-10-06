#ifndef _STATE_MACHINE_H
#define _STATE_MACHINE_H

typedef enum {
    STATE_POWERED_OFF,
    STATE_SHUTTING_DOWN,
    STATE_RUNNING,
    STATE_BOOTING,
} eState;

void state_machine_init(void);
eState state_get_printer(void);
eState state_get_raspberry(void);
eState state_get_camera(void);
eState state_get_light(void);

void state_set_raspberry(eState state);
void state_set_printer(eState state);
void state_set_camera(eState state);
void state_set_light(eState state);
#endif //_STATE_MACHINE_H
