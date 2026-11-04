/* LXR-K2 config-only host regression tests. SPDX-License-Identifier: GPL-3.0-only */
#include "Soundengine/K2ModState.h"
#include <stdio.h>
#define CHECK(x) do { if(!(x)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#x);return 1;} } while(0)
static int test_model(void)
{
    K2ModState s, input;
    K2ModSlot pair[2]={{42,-10,K2_MOD_SINE,63},{42,20,K2_MOD_TRI,11}};
    uint8_t n=255;
    unsigned t,k;
    k2_mod_state_init(&s);
    for (t=0;t<K2_MOD_TRACK_COUNT;t++)
        for(k=0;k<K2_MOD_SLOT_COUNT;k++) {
            const K2ModSlot *p=k2_mod_state_get(&s,(uint8_t)t,(uint8_t)k);
            CHECK(p && p->destination==K2_MOD_TARGET_OFF && p->depth==0 && p->type==K2_MOD_OFF && p->rate==0);
        }
    CHECK(k2_mod_state_get(&s,7,0)==NULL);
    CHECK(k2_mod_state_get(&s,0,2)==NULL);
    CHECK(k2_mod_state_get(NULL,0,0)==NULL);
    CHECK(!k2_mod_state_target_available(&s,7,0,42));
    CHECK(k2_mod_state_set_destination(NULL,0,0,42)==K2_MOD_ERR_NULL);
    CHECK(k2_mod_state_set_destination(&s,7,0,42)==K2_MOD_ERR_INDEX);
    CHECK(k2_mod_state_set_destination(&s,0,0,42)==K2_MOD_OK);
    CHECK(!k2_mod_state_target_available(&s,0,1,42));
    CHECK(k2_mod_state_set_destination(&s,0,1,42)==K2_MOD_ERR_CONFLICT);
    CHECK(s.slots[0][1].destination==K2_MOD_TARGET_OFF);
    CHECK(k2_mod_state_set_destination(&s,0,1,43)==K2_MOD_OK);
    CHECK(k2_mod_state_set_destination(&s,0,0,43)==K2_MOD_ERR_CONFLICT);
    CHECK(s.slots[0][0].destination==42);
    CHECK(k2_mod_state_set_depth(&s,0,0,0)==K2_MOD_OK);
    CHECK(k2_mod_state_set_type(&s,0,0,K2_MOD_OFF)==K2_MOD_OK);
    CHECK(k2_mod_state_set_destination(&s,0,1,42)==K2_MOD_ERR_CONFLICT);
    CHECK(k2_mod_state_set_destination(&s,0,0,K2_MOD_TARGET_OFF)==K2_MOD_OK);
    CHECK(k2_mod_state_set_destination(&s,0,1,42)==K2_MOD_OK);
    CHECK(k2_mod_state_set_destination(&s,1,0,42)==K2_MOD_OK); /* independent tracks */
    CHECK(k2_mod_state_set_destination(&s,K2_TRACK_CLAP_CYMBAL,0,42)==K2_MOD_OK);
    CHECK(k2_mod_state_set_destination(&s,K2_TRACK_CLAP_CYMBAL,1,42)==K2_MOD_ERR_CONFLICT);
    CHECK(k2_mod_state_set_destination(&s,K2_TRACK_CLOSED_HAT,0,42)==K2_MOD_OK);
    CHECK(k2_mod_state_set_destination(&s,K2_TRACK_OPEN_HAT,0,42)==K2_MOD_OK);
    CHECK(k2_mod_state_set_depth(&s,2,0,-64)==K2_MOD_ERR_RANGE);
    CHECK(k2_mod_state_set_depth(&s,2,0,63)==K2_MOD_OK);
    CHECK(k2_mod_state_set_depth(&s,2,0,64)==K2_MOD_ERR_RANGE);
    CHECK(s.slots[2][0].depth==63);
    CHECK(k2_mod_state_set_type(&s,2,0,K2_MOD_TYPE_COUNT)==K2_MOD_ERR_RANGE);
    CHECK(k2_mod_state_set_type(&s,2,0,K2_MOD_EXP_DOWN)==K2_MOD_OK);
    CHECK(k2_mod_state_set_rate(&s,2,0,128)==K2_MOD_ERR_RANGE);
    CHECK(k2_mod_state_set_rate(&s,2,0,127)==K2_MOD_OK);
    CHECK(k2_mod_state_load_track(&s,K2_TRACK_CLAP_CYMBAL,pair)==K2_MOD_REPAIRED);
    CHECK(s.slots[4][0].destination==42 && s.slots[4][1].destination==K2_MOD_TARGET_OFF);
    CHECK(s.slots[4][1].depth==20 && s.slots[4][1].type==K2_MOD_TRI && s.slots[4][1].rate==11);
    CHECK(pair[1].destination==42);
    pair[1].destination=43;
    CHECK(k2_mod_state_load_track(&s,4,pair)==K2_MOD_OK);
    pair[1].depth=120;
    CHECK(k2_mod_state_load_track(&s,4,pair)==K2_MOD_ERR_RANGE);
    CHECK(s.slots[4][1].destination==43);
    k2_mod_state_init(&input);
    input.slots[0][0].destination=50;
    input.slots[0][1].destination=50;
    input.slots[1][0].destination=51;
    input.slots[1][1].destination=51;
    input.slots[6][0].destination=52;
    CHECK(k2_mod_state_load_all(&s,&input,&n)==K2_MOD_REPAIRED);
    CHECK(n==2 && s.slots[0][1].destination==0 && s.slots[1][1].destination==0);
    CHECK(s.slots[6][0].destination==52);
    input.slots[6][1].type=255;
    n=101;
    CHECK(k2_mod_state_load_all(&s,&input,&n)==K2_MOD_ERR_RANGE);
    CHECK(n==101 && s.slots[6][0].destination==52);
    CHECK(k2_mod_state_load_all(NULL,&input,NULL)==K2_MOD_ERR_NULL);
    puts("PASS: two local MOD slots, destination exclusivity and import repair");
    return 0;
}
int main(void) { return test_model(); }
