#ifndef _UART_H
#define _UART_H

#include <stdint.h>

#define DATA_PACKET_LEN                 4
#define DATA_PACKET_MAGIC               0x12
#define DATA_COMMAND_CAMERA_ON          0x10
#define DATA_COMMAND_CAMERA_OFF         0x19
#define DATA_COMMAND_CAMERA_RESET       0x11
#define DATA_COMMAND_LIGHT_ON           0x30
#define DATA_COMMAND_LIGHT_OFF          0x39
#define DATA_COMMAND_RPINTER_ON         0x40
#define DATA_COMMAND_PRINTER_OFF        0x49
#define DATA_COMMAND_SYSTEM_SHUTDOWN    0x99
#define DATA_COMMAND_BOOTUP_COMPLETE    0x01

typedef enum {
    COMMAND_NO_COMMAND,
    COMMAND_BOOTUP_COMPLETE,
    COMMAND_SHUTDOWN,
} eUartCommand;

typedef struct {
    union {
        uint8_t data[DATA_PACKET_LEN];
        struct
        {
            uint8_t packet_magic;
            uint8_t packet_cmd;
            uint8_t packet_payload0;
            uint8_t packet_payload1;
        };
    };
} tUartPacket;

void uart_init(void);
eUartCommand uart_handle_command(void);
void uart_send_command(eUartCommand cmd);

#endif //_UART_H
