#ifndef COMM_H
#define COMM_H

#include <stdint.h>
#include <stddef.h>

/* ── Protocol types ────────────────────────────────────────── */
typedef enum {
    PROTO_UART   = 0,
    PROTO_CAN    = 1,
    PROTO_MODBUS = 2
} Protocol;

/* ── Frame structure ───────────────────────────────────────── */
#define MAX_PAYLOAD 256

typedef struct {
    Protocol    proto;
    uint8_t     src_id;
    uint8_t     dst_id;
    uint16_t    length;
    uint8_t     payload[MAX_PAYLOAD];
    uint16_t    crc;
} Frame;

/* ── Channel handle ────────────────────────────────────────── */
typedef struct {
    Protocol proto;
    int      fd;           /* file descriptor / simulated handle */
    char     device[64];
    int      baud_rate;
    int      is_open;
} Channel;

/* ── API ───────────────────────────────────────────────────── */
Channel *channel_open (Protocol proto, const char *device, int baud);
int      channel_send (Channel *ch, const Frame *frame);
int      channel_recv (Channel *ch, Frame *frame);
void     channel_close(Channel *ch);

/* Frame helpers */
Frame   *frame_create (Protocol proto, uint8_t src, uint8_t dst,
                        const uint8_t *data, uint16_t len);
void     frame_destroy(Frame *f);
int      frame_validate(const Frame *f);
void     frame_print  (const Frame *f);

/* CRC */
uint16_t crc16(const uint8_t *data, size_t len);

/* Diagnostics */
void     diag_report(const Channel *ch, const char *event, int error_code);

#endif /* COMM_H */
