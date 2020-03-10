#include "io_ports.h"
#include "iomacro.h"
#include "photobooth-psu.h"
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/delay.h>

// Three timers:
// 1. LED timer that sets the PWM values
// 2. LED timer that does the PWM
// 3. Button timer for debouncing


static volatile bool m_last_button_state = false;
static volatile bool m_next_button_state = false;
static volatile bool m_button_pressed = false;
static volatile eLedMode m_led_mode = LED_MODE_OFF;
static volatile bool m_led_counting_up = true;
static volatile uint8_t m_led_blink_count = 0;
static const uint8_t m_led_inc = 2;
static const uint16_t m_debounce_val = (65535 - 18750); // 300ms

void io_ports_init(void) {
    SET_OUTPUT(PIN_PSON);
    SET_OUTPUT(PIN_PWR_LED);
    SET_OUTPUT(PIN_RELAIS1);
    SET_OUTPUT(PIN_RELAIS2);
    SET_OUTPUT(PIN_RELAIS3);
    SET_OUTPUT(PIN_RELAIS4);
    SET_INPUT(PIN_PWR_BTN);
    SET_INPUT(PIN_RASPBERRY_STATE);
    SET_INPUT(PIN_RASPBERRY_BOOT_COMPLETE);
    // Hack: Set the pin to hi-z
    //SET_OUTPUT(PIN_RASPBERRY_SHUTDOWN);
    SET_INPUT(PIN_RASPBERRY_SHUTDOWN);
    SET(PIN_RASPBERRY_SHUTDOWN);
    RESET(PIN_PSON);
    SET(PIN_RELAIS1);
    SET(PIN_RELAIS2);
    SET(PIN_RELAIS3);
    SET(PIN_RELAIS4);
    RESET(PIN_PWR_LED);
    SET(PIN_PWR_BTN); // Internal Pull-Up
    SET(PIN_RASPBERRY_STATE);

    // Enable Pin Change Interrupt (Power Button)
    PCICR |= (1 << 0);
    PCMSK0 |= (1 << 5);

    // PWM -> Timer 1
    OCR1BL = 0x00;
    // Enable PWM, PWM at B6, Fast PWM Mode
    TCCR1A |= (1 << 5) | (1 << 0);
    // Set Clock / 1024, Fast PWM Mode
    //TCCR1B |= (1 << 3) | (1 << 2) | (1 << 0);
    // Phase Correct 8-bit PWM
    TCCR1B |= (1 << 3) | (1 << 0);

    // LED Effects -> Timer 0
    TIMSK0 |= (1 << 1); // OCIE0A
    // Prescaler 1024, CTC Mode
    TCCR0A |= (1 << 1); // WGM1
    TCCR0B |= (1 << 2) | (1 << 0); // CS02, CS00
    // OC value of 156 gives 10ms timer interval
    OCR0A = 156;
    // Initialize Counter
    TCNT0 = 0;

    // Button debounce -> Timer 3
    // Prescaler 256
    TCCR3B |= (1 << 2); // CS32
    // Set counter value
    TCNT3 = m_debounce_val; // 100ms

    m_button_pressed = false;
}

static void debounce_start(void) {
    TCNT3 = m_debounce_val;
    TIMSK3 |= (1 << 0); // Enable timer interrupt
}

ISR(TIMER3_OVF_vect) {
    TIMSK3 &= ~(1 << 0); // Disable timer interrupt
    // We fake a rising edge interrupt by looking whether the
    // button is still pressed after the timer has expired. If not,
    // it was either too short or a falling edge.
    if(!IS_SET(PIN_PWR_BTN)) {
        m_button_pressed = true;
    }
}

ISR(TIMER0_COMPA_vect) {
    uint8_t val;
    switch(m_led_mode) {
    case LED_MODE_ON:
        OCR1BL = 0xff;
        break;
    case LED_MODE_OFF:
        OCR1BL = 0x00;
        break;
    case LED_MODE_PULSE:
        val = OCR1BL;
        if(m_led_counting_up) {
            if(val <= (255 - m_led_inc)) {
                val += m_led_inc;
            } else {
                m_led_counting_up = false;
                val -= m_led_inc;
            }
        } else {
            if(val >= m_led_inc) {
                val -= m_led_inc;
            } else {
                m_led_counting_up = true;
                val += m_led_inc;
            }
        }
        OCR1BL = val;
        break;
    case LED_MODE_BLINK:
        if(m_led_blink_count == 30) {
            val = OCR1BL;
            if(val > 0) {
                val = 0;
            } else {
                val = 0xff;
            }
            OCR1BL = val;
            m_led_blink_count = 0;
        } else {
            m_led_blink_count++;
        }
        break;
    }
}


ISR(PCINT0_vect) {
    // We start debouncing only if
    // the button state is pressed
    if(!IS_SET(PIN_PWR_BTN)) {
        debounce_start();
    }
}

void button_handled(void) {
    m_button_pressed = false;
}

bool button_pressed(void) {
    return m_button_pressed;
}

void led_set_state(eLedState state) {
    switch(state) {
    case LED_STATE_RUNNING:
        m_led_mode = LED_MODE_ON;
        break;
    case LED_STATE_POWERED_OFF:
        m_led_mode = LED_MODE_OFF;
        break;
    case LED_STATE_BOOTING:
        m_led_mode = LED_MODE_PULSE;
        break;
    case LED_STATE_SHUTTING_DOWN:
        m_led_mode = LED_MODE_BLINK;
        break;
    }
}

eState gpio_get_raspberry_state(void) {
    if(IS_SET(PIN_RASPBERRY_STATE)) {
        return STATE_POWERED_OFF;
    } else {
        if(IS_SET(PIN_RASPBERRY_BOOT_COMPLETE)) {
            return STATE_RUNNING;
        } else {
            return STATE_BOOTING;
        }
    }
}

void system_power_on(void) {
    SET(PIN_PSON);
}

void system_power_off(void) {
    RESET(PIN_PSON);
}

void raspberry_power_off_gpio(void) {
    // Hack: go from hi-z to low to hi-z
    // without going to high
    RESET(PIN_RASPBERRY_SHUTDOWN);
    SET_OUTPUT(PIN_RASPBERRY_SHUTDOWN);
    _delay_ms(300);
    SET_INPUT(PIN_RASPBERRY_SHUTDOWN);
    SET(PIN_RASPBERRY_SHUTDOWN);
}
