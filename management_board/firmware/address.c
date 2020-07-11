#include <WProgram.h>
#include "address.h"

const uint8_t ma0_pin = 9;
const uint8_t ma1_pin = 8;
const uint8_t ma2_pin = 7;
const uint8_t ma3_pin = 6;

void address_setup (void)
{
    pinMode (ma0_pin, INPUT_PULLUP);
    pinMode (ma1_pin, INPUT_PULLUP);
    pinMode (ma2_pin, INPUT_PULLUP);
    pinMode (ma3_pin, INPUT_PULLUP);
}

void address_finalize (void)
{
}

void address_get (uint8_t *mod, uint8_t *node)
{
    uint8_t ma;

    ma = digitalRead (ma0_pin);
    ma |= (digitalRead (ma1_pin) << 1);
    ma |= (digitalRead (ma2_pin) << 2);
    ma |= (digitalRead (ma3_pin) << 3);

    *node = 0x1d;
    *mod = ma;
}

/*
 * vi:tabstop=4 shiftwidth=4 expandtab
 */
