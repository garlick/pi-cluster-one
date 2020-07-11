#include <WProgram.h>
#include "canmgr_proto.h"
#include "heartbeat.h"

/* These nodes are compute controllers only, 0x10 thru CANMGR_MODULE_CTRL - 1
 */
#define MAXNODES (CANMGR_MODULE_CTRL - 0x10)

struct node_state {
    elapsedMillis since;
    int active;
    uint8_t data;
};

struct node_state statetab[MAXNODES];

const int heartbeat_thresh = 5000; // millisec

int heartbeat_node_set (uint8_t node, uint8_t val)
{
    uint8_t i = node - 0x10;

    if (i >= MAXNODES)
        return -1;
    statetab[i].data = val;
    statetab[i].active = 1;
    statetab[i].since = 0;
    return 0;
}

int heartbeat_node_get (uint8_t node, uint8_t *val, uint8_t *active)
{
    uint8_t i = node - 0x10;

    if (i >= MAXNODES)
        return -1;
    if (val)
        *val = statetab[i].data;
    if (active)
        *active  = statetab[i].active;
    return 0;
}

uint8_t heartbeat_slots (void)
{
    return MAXNODES;
}

void heartbeat_setup (void)
{
    memset (statetab, 0, sizeof (statetab));
}

void heartbeat_finalize (void)
{
}

void heartbeat_update (void)
{
    int i;

    for (i = 0; i < MAXNODES; i++) {
        if (statetab[i].active) {
            if (statetab[i].since > heartbeat_thresh)
                statetab[i].active = 0;
        }
    }
}

/*
 * vi:tabstop=4 shiftwidth=4 expandtab
 */
