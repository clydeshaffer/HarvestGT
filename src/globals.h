#include "coordinate.h"

#define PLAYER_STATE_NEUTRAL 0
#define PLAYER_STATE_ATTACK 1
#define PLAYER_STATE_HITSTUN 2
#define PLAYER_STATE_SPELLCAST 3
#define PLAYER_STATE_DEAD 4
#define INITIAL_MAX_HEALTH 3
#define PLAYER_ACTION_SEEDS 6
#define PLAYER_ACTION_WATER 9
#define PLAYER_ACTION_HARVEST 12

extern int temp1;
extern int temp2;
extern int temp3;
extern int temp4;
extern coordinate player_x, player_y;
extern unsigned char player_state_offsets[5];
extern signed char player_dir_x, player_dir_y;
extern unsigned char player_hitbox_damage;
extern unsigned char player_anim_frame;
extern unsigned char player_anim_dir;
extern unsigned char player_anim_flip;
extern unsigned char player_health;
extern unsigned char player_anim_state;
extern unsigned char player_action_type;
extern unsigned char romBankMirror;
extern unsigned char player_max_health;
extern unsigned char do_fill_map;
extern unsigned char temp_msg_counter;
extern char *message_string;