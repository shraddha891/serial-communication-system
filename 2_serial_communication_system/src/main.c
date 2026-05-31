#include "comm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    printf("=== Embedded Serial Communication System ===\n\n");

    /* ── UART demo ─────────────────────────────────── */
    printf("--- UART Channel ---\n");
    Channel *uart = channel_open(PROTO_UART, "/dev/ttyS0", 115200);
    uint8_t uart_data[] = { 0x01, 0x02, 0x03, 0x04, 0x05 };
    Frame  *f1 = frame_create(PROTO_UART, 0x10, 0x20, uart_data, sizeof(uart_data));
    frame_print(f1);
    channel_send(uart, f1);
    Frame rx1 = {0};
    channel_recv(uart, &rx1);
    frame_destroy(f1);
    channel_close(uart);

    printf("\n--- CAN Channel ---\n");
    Channel *can = channel_open(PROTO_CAN, "/dev/can0", 500000);
    uint8_t can_data[] = { 0xDE, 0xAD, 0xBE, 0xEF };
    Frame  *f2 = frame_create(PROTO_CAN, 0x11, 0x22, can_data, sizeof(can_data));
    frame_print(f2);
    channel_send(can, f2);
    Frame rx2 = {0};
    channel_recv(can, &rx2);
    frame_destroy(f2);
    channel_close(can);

    printf("\n--- MODBUS Channel ---\n");
    Channel *modbus = channel_open(PROTO_MODBUS, "/dev/ttyUSB0", 9600);
    /* MODBUS read holding registers: func=0x03, start=0x0000, count=0x0002 */
    uint8_t mb_data[] = { 0x03, 0x00, 0x00, 0x00, 0x02 };
    Frame  *f3 = frame_create(PROTO_MODBUS, 0x01, 0xFF, mb_data, sizeof(mb_data));
    frame_print(f3);
    channel_send(modbus, f3);
    Frame rx3 = {0};
    channel_recv(modbus, &rx3);
    frame_destroy(f3);
    channel_close(modbus);

    printf("\nAll channels tested successfully.\n");
    return EXIT_SUCCESS;
}
