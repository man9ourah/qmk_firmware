#include "qmk_secrets.h"

enum LAYER_NUMS
{
    __BASE = 0,
    __FUN,
    __SEC,
};

enum LED_STATES
{
    ALL = 0,   // All leds on
    SIDES, // Only side leds on
    OFF,   // Nothing is on
} led_state = SIDES; // Default is only sides

#define SEC_TOLERANCE 0
const static uint8_t sec_combo[]     = SEC_COMBO;
const static size_t  combo_len       = sizeof(sec_combo);
static bool          direction_to_go = true;
static uint8_t       state           = 0, clicks = 0, grace = 0;

void reset_sec_state(void){
    state           = clicks = grace = 0;
    direction_to_go = true;
}

void emit_passphrase_secret(void) {
    // Send the secret phrase
    SEND_STRING(SEC_PHRASE);

    // Automatically move to the base layer
    layer_move(__BASE);
}

void update_dial(bool direction) {
    if (direction == direction_to_go) {              // check correct direction
        if (++clicks == sec_combo[state] - SEC_TOLERANCE) {  // increment clicks and check if dialed enough
            clicks = grace  = 0;
            direction_to_go = !direction_to_go;  // flip expected direction
            if (++state == combo_len) {          // increment state then check if we are at the end
                reset_sec_state();
                emit_passphrase_secret();
            }
        }
    } else {                                           // wrong direction so fail, except...
        if (clicks != 0 || ++grace > 2 * SEC_TOLERANCE) {  // a few extra ticks just after changing direction is acceptable
            reset_sec_state();
        }
    }
}
