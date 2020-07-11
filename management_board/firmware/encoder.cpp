#include <WProgram.h>
#include "Encoder.h"
#include "Bounce.h"
#include "encoder.h"
#include "heartbeat.h"
#include "canmgr.h"

const uint8_t encoder_a_pin = 20;
const uint8_t encoder_b_pin = 21;
const uint8_t encoder_switch_pin = 22;
const uint8_t encoder_red_pin = 23;
const uint8_t encoder_green_pin = 29;
const uint8_t encoder_blue_pin = 30;

static long encoder_value = 0;

static Encoder encoder = Encoder (encoder_b_pin, encoder_a_pin);
static Bounce button = Bounce (encoder_switch_pin, 5);

static int encoder_node_cursor = -1;
static uint8_t encoder_node_status = 0xff; // invalid

/* FIXME: binary only (use PWM for full RGB)
 */
static void setrgb (uint8_t r, uint8_t g, uint8_t b)
{
    digitalWriteFast (encoder_red_pin, r ? LOW : HIGH);
    digitalWriteFast (encoder_green_pin, g ? LOW : HIGH);
    digitalWriteFast (encoder_blue_pin, b ? LOW : HIGH);
}

void encoder_setup (void)
{
    pinMode (encoder_red_pin, OUTPUT);
    pinMode (encoder_green_pin, OUTPUT);
    pinMode (encoder_blue_pin, OUTPUT);
    pinMode (encoder_switch_pin, INPUT);
    // N.B. A and B inputs already configured by Encoder constructor

    /* quick visual test of LEDs */
    setrgb (1,0,0);
    delay (200);
    setrgb (0,1,0);
    delay (200);
    setrgb (0,0,1);
    delay (200);
    setrgb (0,0,0);

    encoder_value = encoder.read() >> 2;
}

void encoder_finalize (void)
{
}

static int next_active_node (int cur)
{
    int next = (cur == -1) ? 0x10 : cur + 1;
    uint8_t active;

    while (heartbeat_node_get (next, NULL, &active) == 0) {
        if (active == 1)
            return next;
        next++;
    }
    return -1;
}

static int prev_active_node (int cur)
{
    int prev = (cur == -1) ? heartbeat_slots () + 0x10 - 1 : cur - 1;;
    uint8_t active;

    while (heartbeat_node_get (prev, NULL, &active) == 0) {
        if (active == 1)
            return prev;
        prev--;
    }
    return -1;
}

void encoder_update (void)
{
    long val;
    int keypress = 0;

    button.update ();
    if (button.risingEdge ()) {
        keypress = 1;
    }
    val = encoder.read() >> 2; // detents every 4 ticks

    int target = encoder_node_cursor;
    while (encoder_value < val) {
        encoder_value++;
        target = next_active_node (target);
    }
    while (encoder_value > val) {
        encoder_value--;
        target = prev_active_node (target);
    }
    /* New target
     */
    if (target != encoder_node_cursor) {
        if (encoder_node_cursor != -1)
            canmgr_identify_send (encoder_node_cursor, 0); // off
        if (target != -1)
            canmgr_identify_send (target, 1); // on
        encoder_node_cursor = target;
    }
    /* Key pressed with node selected
     */
    if (keypress && encoder_node_cursor != -1) {
        canmgr_power_send (encoder_node_cursor, 3); // 3=toggle
    }
    /* Color of RGB LED will reflect power status of selected
     * node, as reported periodically in heartbeat payload.
     */
    uint8_t status;
    if (encoder_node_cursor != -1)
        heartbeat_node_get (encoder_node_cursor, &status, NULL);
    else
        status = 0xff; // invalid
    if (encoder_node_status != status) {
        if (status == 0)
            setrgb (0,1,0);
        else if (status == 1)
            setrgb (1,0,0);
        else if (status == 2)
            setrgb (0,0,1);
        else
            setrgb (0,0,0);
        encoder_node_status = status;
    }
}

/*
 * vi:tabstop=4 shiftwidth=4 expandtab
 */
