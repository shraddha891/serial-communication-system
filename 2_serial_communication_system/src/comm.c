#include "comm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ─── CRC-16 (CCITT) ──────────────────────────────────────────────────── */

uint16_t crc16(const uint8_t *data, size_t len) {
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < len; i++) {
        crc ^= (uint16_t)data[i] << 8;
        for (int j = 0; j < 8; j++)
            crc = (crc & 0x8000) ? (crc << 1) ^ 0x1021 : crc << 1;
    }
    return crc;
}

/* ─── protocol name helper ─────────────────────────────────────────────── */

static const char *proto_name(Protocol p) {
    switch (p) {
        case PROTO_UART:   return "UART";
        case PROTO_CAN:    return "CAN";
        case PROTO_MODBUS: return "MODBUS";
        default:           return "UNKNOWN";
    }
}

/* ─── channel ──────────────────────────────────────────────────────────── */

Channel *channel_open(Protocol proto, const char *device, int baud) {
    Channel *ch = calloc(1, sizeof(Channel));
    if (!ch) return NULL;

    ch->proto    = proto;
    ch->baud_rate = baud;
    strncpy(ch->device, device, sizeof(ch->device) - 1);

    /* In a real system: open(device, O_RDWR | O_NOCTTY) + termios setup.
       Here we simulate success. */
    ch->fd      = 100 + (int)proto;   /* simulated fd */
    ch->is_open = 1;

    printf("[Channel] Opened %s on '%s' @ %d baud (fd=%d)\n",
           proto_name(proto), device, baud, ch->fd);
    return ch;
}

int channel_send(Channel *ch, const Frame *frame) {
    if (!ch || !ch->is_open || !frame) return -1;

    /* Validate before sending */
    if (!frame_validate(frame)) {
        diag_report(ch, "TX_CRC_ERROR", -2);
        return -2;
    }

    printf("[%s TX] src=0x%02X dst=0x%02X len=%u crc=0x%04X\n",
           proto_name(ch->proto), frame->src_id, frame->dst_id,
           frame->length, frame->crc);

    /* Simulate protocol-specific framing */
    if (ch->proto == PROTO_MODBUS) {
        printf("  [MODBUS] Function code: 0x%02X\n", frame->payload[0]);
    } else if (ch->proto == PROTO_CAN) {
        printf("  [CAN] CAN-ID: 0x%03X\n", (frame->src_id << 4) | frame->dst_id);
    }

    return (int)frame->length;
}

int channel_recv(Channel *ch, Frame *frame) {
    if (!ch || !ch->is_open || !frame) return -1;

    /* Simulate receiving a response frame */
    frame->proto  = ch->proto;
    frame->src_id = frame->dst_id;
    frame->dst_id = 0x01;
    frame->length = 4;
    frame->payload[0] = 0xAC;   /* ACK byte */
    frame->payload[1] = 0x00;
    frame->payload[2] = 0x00;
    frame->payload[3] = 0x01;
    frame->crc = crc16(frame->payload, frame->length);

    printf("[%s RX] ACK received | crc=0x%04X\n", proto_name(ch->proto), frame->crc);
    return (int)frame->length;
}

void channel_close(Channel *ch) {
    if (!ch) return;
    printf("[Channel] Closing %s (fd=%d)\n", proto_name(ch->proto), ch->fd);
    ch->is_open = 0;
    free(ch);
}

/* ─── frame helpers ────────────────────────────────────────────────────── */

Frame *frame_create(Protocol proto, uint8_t src, uint8_t dst,
                    const uint8_t *data, uint16_t len) {
    if (len > MAX_PAYLOAD) return NULL;
    Frame *f = calloc(1, sizeof(Frame));
    if (!f) return NULL;

    f->proto  = proto;
    f->src_id = src;
    f->dst_id = dst;
    f->length = len;
    memcpy(f->payload, data, len);
    f->crc = crc16(f->payload, f->length);
    return f;
}

void frame_destroy(Frame *f) { free(f); }

int frame_validate(const Frame *f) {
    if (!f) return 0;
    uint16_t expected = crc16(f->payload, f->length);
    return (expected == f->crc) ? 1 : 0;
}

void frame_print(const Frame *f) {
    if (!f) return;
    printf("Frame [%s] src=0x%02X dst=0x%02X len=%u crc=0x%04X\n  Payload: ",
           proto_name(f->proto), f->src_id, f->dst_id, f->length, f->crc);
    for (int i = 0; i < f->length; i++) printf("%02X ", f->payload[i]);
    printf("\n");
}

/* ─── diagnostics ──────────────────────────────────────────────────────── */

void diag_report(const Channel *ch, const char *event, int error_code) {
    time_t now = time(NULL);
    char ts[32];
    strftime(ts, sizeof(ts), "%H:%M:%S", localtime(&now));
    printf("[DIAG %s] proto=%s device=%s event=%s code=%d\n",
           ts, proto_name(ch->proto), ch->device, event, error_code);
}
