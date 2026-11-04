/* LXR-K2 contributors, 2026. GPL-3.0-only. Unofficial firmware.
 * Configuration and validation ONLY; not attached to sound DSP or kit I/O. */
#include "Soundengine/K2ModState.h"
#include <string.h>

K2ModState k2_mod_state; /* zero means OFF, safe startup state */

static bool valid_pos(uint8_t track, uint8_t slot)
{
    return track < K2_MOD_TRACK_COUNT && slot < K2_MOD_SLOT_COUNT;
}
static bool valid_fields(const K2ModSlot *p)
{
    return p->depth >= K2_MOD_DEPTH_MIN && p->depth <= K2_MOD_DEPTH_MAX &&
           p->type < K2_MOD_TYPE_COUNT && p->rate <= K2_MOD_RATE_MAX;
}
void k2_mod_state_init(K2ModState *state)
{
    if (state) memset(state, 0, sizeof(*state));
}
const K2ModSlot *k2_mod_state_get(const K2ModState *state, uint8_t track, uint8_t slot)
{
    if (!state || !valid_pos(track, slot)) return NULL;
    return &state->slots[track][slot];
}
bool k2_mod_state_target_available(const K2ModState *state, uint8_t track, uint8_t slot, uint16_t destination)
{
    if (!state || !valid_pos(track, slot)) return false;
    return destination == K2_MOD_TARGET_OFF ||
           state->slots[track][slot ^ 1u].destination != destination;
}
K2ModResult k2_mod_state_set_destination(K2ModState *state, uint8_t track, uint8_t slot, uint16_t destination)
{
    if (!state) return K2_MOD_ERR_NULL;
    if (!valid_pos(track, slot)) return K2_MOD_ERR_INDEX;
    if (!k2_mod_state_target_available(state, track, slot, destination)) return K2_MOD_ERR_CONFLICT;
    state->slots[track][slot].destination = destination;
    return K2_MOD_OK;
}
K2ModResult k2_mod_state_set_depth(K2ModState *state, uint8_t track, uint8_t slot, int8_t depth)
{
    if (!state) return K2_MOD_ERR_NULL;
    if (!valid_pos(track, slot)) return K2_MOD_ERR_INDEX;
    if (depth < K2_MOD_DEPTH_MIN || depth > K2_MOD_DEPTH_MAX) return K2_MOD_ERR_RANGE;
    state->slots[track][slot].depth = depth;
    return K2_MOD_OK;
}
K2ModResult k2_mod_state_set_type(K2ModState *state, uint8_t track, uint8_t slot, uint8_t type)
{
    if (!state) return K2_MOD_ERR_NULL;
    if (!valid_pos(track, slot)) return K2_MOD_ERR_INDEX;
    if (type >= K2_MOD_TYPE_COUNT) return K2_MOD_ERR_RANGE;
    state->slots[track][slot].type = type;
    return K2_MOD_OK;
}
K2ModResult k2_mod_state_set_rate(K2ModState *state, uint8_t track, uint8_t slot, uint8_t rate)
{
    if (!state) return K2_MOD_ERR_NULL;
    if (!valid_pos(track, slot)) return K2_MOD_ERR_INDEX;
    if (rate > K2_MOD_RATE_MAX) return K2_MOD_ERR_RANGE;
    state->slots[track][slot].rate = rate;
    return K2_MOD_OK;
}
K2ModResult k2_mod_state_load_track(K2ModState *state, uint8_t track, const K2ModSlot incoming[2])
{
    K2ModSlot next[2];
    bool repaired;
    if (!state || !incoming) return K2_MOD_ERR_NULL;
    if (track >= K2_MOD_TRACK_COUNT) return K2_MOD_ERR_INDEX;
    next[0] = incoming[0];
    next[1] = incoming[1];
    if (!valid_fields(&next[0]) || !valid_fields(&next[1])) return K2_MOD_ERR_RANGE;
    repaired = next[0].destination != K2_MOD_TARGET_OFF &&
               next[0].destination == next[1].destination;
    if (repaired) next[1].destination = K2_MOD_TARGET_OFF;
    state->slots[track][0] = next[0];
    state->slots[track][1] = next[1];
    return repaired ? K2_MOD_REPAIRED : K2_MOD_OK;
}
K2ModResult k2_mod_state_load_all(K2ModState *state, const K2ModState *incoming, uint8_t *repaired_count)
{
    K2ModState next;
    uint8_t track, repaired = 0;
    if (!state || !incoming) return K2_MOD_ERR_NULL;
    k2_mod_state_init(&next);
    for (track = 0; track < K2_MOD_TRACK_COUNT; track++) {
        K2ModResult r = k2_mod_state_load_track(&next, track, incoming->slots[track]);
        if (r < 0) return r; /* no partial commit */
        if (r == K2_MOD_REPAIRED) repaired++;
    }
    *state = next;
    if (repaired_count) *repaired_count = repaired;
    return repaired ? K2_MOD_REPAIRED : K2_MOD_OK;
}
