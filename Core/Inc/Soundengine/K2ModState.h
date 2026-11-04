/* LXR-K2 contributors, 2026. GPL-3.0-only. Unofficial firmware.
 * Local per-track MOD configuration; DSP, UI and preset files unchanged. */
#ifndef LXR_K2_MOD_STATE_H
#define LXR_K2_MOD_STATE_H
#include <stdbool.h>
#include <stdint.h>

#define K2_MOD_TRACK_COUNT 7u
#define K2_MOD_SLOT_COUNT 2u
#define K2_MOD_TARGET_OFF 0u
#define K2_MOD_DEPTH_MIN (-63)
#define K2_MOD_DEPTH_MAX 63
#define K2_MOD_RATE_MAX 127u

enum K2ModTrack {
    K2_TRACK_DRUM1=0, K2_TRACK_DRUM2, K2_TRACK_DRUM3,
    K2_TRACK_SNARE, K2_TRACK_CLAP_CYMBAL,
    K2_TRACK_CLOSED_HAT, K2_TRACK_OPEN_HAT
};
enum K2ModType {
    K2_MOD_OFF=0, K2_MOD_SINE, K2_MOD_TRI, K2_MOD_SAW_UP,
    K2_MOD_SAW_DOWN, K2_MOD_PULSE, K2_MOD_SAMPLE_HOLD,
    K2_MOD_EXP_UP, K2_MOD_EXP_DOWN, K2_MOD_TYPE_COUNT
};
typedef struct {
    uint16_t destination; /* stable semantic ID; 0=OFF, not a PAR_* index */
    int8_t depth;         /* -63..63 */
    uint8_t type;         /* K2ModType */
    uint8_t rate;         /* 0..127; actual Hz mapping not implemented */
} K2ModSlot;
typedef struct {
    K2ModSlot slots[K2_MOD_TRACK_COUNT][K2_MOD_SLOT_COUNT];
} K2ModState;
typedef enum {
    K2_MOD_OK=0, K2_MOD_REPAIRED=1,
    K2_MOD_ERR_NULL=-1, K2_MOD_ERR_INDEX=-2,
    K2_MOD_ERR_CONFLICT=-3, K2_MOD_ERR_RANGE=-4
} K2ModResult;

/* Central singleton is BSS-zeroed (zero is OFF); UI/DSP binding is future work. */
extern K2ModState k2_mod_state;
void k2_mod_state_init(K2ModState *state);
const K2ModSlot *k2_mod_state_get(const K2ModState *state, uint8_t track, uint8_t slot);
bool k2_mod_state_target_available(const K2ModState *state, uint8_t track, uint8_t slot, uint16_t destination);
K2ModResult k2_mod_state_set_destination(K2ModState *state, uint8_t track, uint8_t slot, uint16_t destination);
K2ModResult k2_mod_state_set_depth(K2ModState *state, uint8_t track, uint8_t slot, int8_t depth);
K2ModResult k2_mod_state_set_type(K2ModState *state, uint8_t track, uint8_t slot, uint8_t type);
K2ModResult k2_mod_state_set_rate(K2ModState *state, uint8_t track, uint8_t slot, uint8_t rate);
/* Future K2-only import; duplicate destinations repaired in RAM, slot 2 => OFF. */
K2ModResult k2_mod_state_load_track(K2ModState *state, uint8_t track, const K2ModSlot incoming[2]);
K2ModResult k2_mod_state_load_all(K2ModState *state, const K2ModState *incoming, uint8_t *repaired_count);
#endif
