#include "uart.h"
#include <stdbool.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#define BAUD 38400
#include <util/setbaud.h>

static volatile tUartPacket rxPacket;
static volatile tUartPacket txPacket;
static volatile uint8_t txCounter = 0;
static volatile uint8_t rxCounter = 0;
static volatile bool txComplete = true;
static volatile bool rxComplete = false;

void uart_init(void) {

    UBRR1H = UBRRH_VALUE;
    UBRR1L = UBRRL_VALUE;
    #if USE_2X
    UCSR1A |= (1 << 1);
    #else
    UCSR1A &= ~(1 << 1);
    #endif

    // RXCIE1, TXCIE1, RXEN1, TXEN1
    UCSR1B |= (1 << 7) | (1 << 6) | (1 << 4) | (1 << 3);

}

eUartCommand uart_handle_command(void) {
    if(!rxComplete) {
        return COMMAND_NO_COMMAND;
    }
    eUartCommand ret = COMMAND_NO_COMMAND;
    if(rxPacket.packet_magic == DATA_PACKET_MAGIC) {
        switch(rxPacket.packet_cmd) {
        case DATA_COMMAND_BOOTUP_COMPLETE:
            ret = COMMAND_BOOTUP_COMPLETE;
            break;
        case DATA_COMMAND_SYSTEM_SHUTDOWN:
            ret = COMMAND_SHUTDOWN;
            break;
        }
    }
    rxComplete = false;
    return ret;
}

ISR(USART1_RX_vect) {
    rxPacket.data[rxCounter] = UDR1;
    rxCounter++;
    if(rxCounter == DATA_PACKET_LEN) {
        rxComplete = true;
    }
}

ISR(USART1_TX_vect) {
    if(txCounter < DATA_PACKET_LEN) {
        UDR1 = txPacket.data[txCounter];
        txCounter ++;
    }
}

void uart_send_command(eUartCommand cmd) {
    while(!txComplete) {

    }
    switch(cmd) {
    case COMMAND_SHUTDOWN:
        txPacket.packet_cmd = DATA_COMMAND_SYSTEM_SHUTDOWN;
        break;
    default:
        return;
    }

    txComplete = false;
    txPacket.packet_magic = DATA_PACKET_MAGIC;
    txPacket.packet_payload0 = 0;
    txPacket.packet_payload1 = 0;
    txCounter = 1;
    UDR1 = txPacket.data[0];
}
