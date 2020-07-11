#include <WProgram.h>
#include "can.h"
#include "canmgr_proto.h"
#include "canmgr.h"
#include "activity.h"
#include "heartbeat.h"

static uint8_t addr_mod;
static uint8_t addr_node;

void canmgr_setup (uint8_t mod, uint8_t node)
{
    int i;
    addr_node = node;
    addr_mod = mod;
    can0_begin (CANMGR_DST_MASK);
    for (i = 0; i < 8; i++)
        can0_setfilter (addr_node<<CANMGR_DST_SHIFT, i);
}

int lcan_recv (struct canmgr_frame *fr, uint16_t timeout_ms)
{
    struct rawcan_frame raw;

    if (!can0_read (&raw.id, &raw.dlen, &raw.data[0], timeout_ms))
        return -1;
    if (canmgr_decode (fr, &raw) < 0)
        return -1;
    if (fr->dst != (addr_node))
        return -1;
    activity_lcan_pulse ();
    return 0;
}

int lcan_send (struct canmgr_frame *fr, uint16_t timeout_ms)
{
    struct rawcan_frame raw;

    if (canmgr_encode (fr, &raw) < 0)
        return -1;
    if (!can0_write (raw.id, raw.dlen, &raw.data[0], timeout_ms))
        return -1;
    activity_lcan_pulse ();
    return 0;
}

int canmgr_power_send (uint8_t node, uint8_t state)
{
    struct canmgr_frame in;

    in.pri = 1;
    in.dst = node;
    in.src = CANMGR_MODULE_CTRL;

    in.xpri = 1;
    in.type = CANMGR_TYPE_WO;
    in.node = node;
    in.module = CANMGR_ADDR_NOROUTE;
    in.object = CANOBJ_TARGET_POWER;
    in.data[0] = state;
    in.dlen = 1;
    return lcan_send (&in, 100);
}

int canmgr_identify_send (uint8_t node, uint8_t state)
{
    struct canmgr_frame in;

    in.pri = 1;
    in.dst = node;
    in.src = CANMGR_MODULE_CTRL;

    in.xpri = 1;
    in.type = CANMGR_TYPE_WO;
    in.node = node;
    in.module = CANMGR_ADDR_NOROUTE;
    in.object = CANOBJ_LED_IDENTIFY;
    in.data[0] = state;
    in.dlen = 1;
    return lcan_send (&in, 100);
}

void canmgr_ack (struct canmgr_frame *fr, int type, uint8_t *data, int len)
{
    int tmpaddr = fr->src;
    fr->src = fr->dst;
    fr->dst = tmpaddr;
    fr->pri = 0; // high priority
    fr->type = type;

    if (len > canmgr_maxdata (fr->object))
        return;
    memcpy (fr->data, data, len);
    fr->dlen = len;

    lcan_send (fr, 100);
}

void canobj_echo (struct canmgr_frame *fr)
{
    switch (fr->type) {
        case CANMGR_TYPE_WO:
        case CANMGR_TYPE_RO:
        case CANMGR_TYPE_DAT:
            canmgr_ack (fr, CANMGR_TYPE_ACK, fr->data, fr->dlen);
            break;
        case CANMGR_TYPE_ACK:
        case CANMGR_TYPE_NAK:
            break;
        default:
            break;
    }
}

void canobj_heartbeat (struct canmgr_frame *fr)
{
    heartbeat_node_set (fr->src, fr->data[0]);
}

void canobj_unknown (struct canmgr_frame *fr)
{
    switch (fr->type) {
        case CANMGR_TYPE_WO:
        case CANMGR_TYPE_RO:
        case CANMGR_TYPE_DAT:
            canmgr_ack (fr, CANMGR_TYPE_NAK, NULL, 0);
            break;
        default:
            break;
    }
}

void canmgr_dispatch (struct canmgr_frame *fr)
{
    switch (fr->object) {
        case CANOBJ_ECHO:
            canobj_echo (fr);
            break;
        case CANOBJ_HEARTBEAT:
            canobj_heartbeat (fr);
            break;
        default:
            canobj_unknown (fr);
            break;
    }
}

void canmgr_update (void)
{
    struct canmgr_frame fr;

    if (can0_available ()) {
        if (lcan_recv (&fr, 0) == 0)
            canmgr_dispatch (&fr);
    }
}

void canmgr_finalize (void)
{
    can0_end ();
}

/*
 * vi:tabstop=4 shiftwidth=4 expandtab
 */
