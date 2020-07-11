#include <WProgram.h>
#include "canmgr.h"
#include "address.h"
#include "activity.h"
#include "encoder.h"
#include "heartbeat.h"

int main (void)
{
    uint8_t mod, node;

    activity_setup ();
    encoder_setup ();
    address_setup ();
    address_get (&mod, &node);
    canmgr_setup (mod, node);
    heartbeat_setup ();

    while (1) {
        canmgr_update ();
        activity_update ();
        encoder_update ();
        heartbeat_update ();
    }

    heartbeat_finalize ();
    canmgr_finalize ();
    address_finalize ();
    encoder_finalize ();
    activity_finalize ();
}

/*
 * vi:tabstop=4 shiftwidth=4 expandtab
 */

