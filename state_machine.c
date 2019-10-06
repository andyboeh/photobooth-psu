#include "state_machine.h"

static eState m_printer_state = STATE_POWERED_OFF;
static eState m_raspberry_state = STATE_POWERED_OFF;
static eState m_camera_state = STATE_POWERED_OFF;
static eState m_light_state = STATE_POWERED_OFF;

void state_machine_init(void) {
    state_set_raspberry(STATE_POWERED_OFF);
    state_set_printer(STATE_POWERED_OFF);
    state_set_camera(STATE_POWERED_OFF);
    state_set_light(STATE_POWERED_OFF);
}

eState state_get_camera(void) {
    return m_camera_state;
}

eState state_get_light(void) {
    return m_light_state;
}

eState state_get_printer(void) {
    return m_printer_state;
}

eState state_get_raspberry(void) {
    return m_raspberry_state;
}

void state_set_raspberry(eState state) {
    m_raspberry_state = state;
}

void state_set_printer(eState state) {
    m_printer_state = state;
}

void state_set_camera(eState state) {
    m_camera_state = state;
}

void state_set_light(eState state) {
    m_light_state = state;
}
