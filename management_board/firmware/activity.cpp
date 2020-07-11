#include <WProgram.h>
#include "activity.h"

const uint8_t activity_lcan_pin = 24;
elapsedMillis activity_lcan_since;
uint8_t activity_lcan_state = LOW;

const uint8_t activity_xcan_pin = 25;
elapsedMillis activity_xcan_since;
uint8_t activity_xcan_state = LOW;

const uint8_t activity_period = 10;// millisec



void activity_setup (void)
{
    pinMode (activity_lcan_pin, OUTPUT);
    pinMode (activity_xcan_pin, OUTPUT);

    /* quick visual test */
    digitalWriteFast (activity_lcan_pin, HIGH);
    delay (200);
    digitalWriteFast (activity_lcan_pin, LOW);
    digitalWriteFast (activity_xcan_pin, HIGH);
    delay (200);
    digitalWriteFast (activity_xcan_pin, LOW);

    digitalWriteFast (activity_lcan_pin, activity_lcan_state);
    digitalWriteFast (activity_xcan_pin, activity_xcan_state);
}

void activity_finalize (void)
{
}

void activity_lcan_pulse (void)
{
    activity_lcan_state = HIGH;
    digitalWriteFast (activity_lcan_pin, activity_lcan_state);
    activity_lcan_since = 0;
}

void activity_xcan_pulse (void)
{
    activity_xcan_state = HIGH;
    digitalWriteFast (activity_xcan_pin, activity_xcan_state);
    activity_xcan_since = 0;
}

void activity_update (void)
{
    if (activity_lcan_since >= activity_period) {
        if (activity_lcan_state == HIGH) {
            activity_lcan_state = LOW;
            digitalWriteFast (activity_lcan_pin, activity_lcan_state);
        }
        activity_lcan_since = 0;
    }
    if (activity_xcan_since >= activity_period) {
        if (activity_xcan_state == HIGH) {
            activity_xcan_state = LOW;
            digitalWriteFast (activity_xcan_pin, activity_xcan_state);
        }
        activity_xcan_since = 0;
    }
}

/*
 * vi:tabstop=4 shiftwidth=4 expandtab
 */
