#include "globals.h"
int temp1;
int temp2;
int temp3;
int temp4;
coordinate player_x = {0}, player_y = {0};
unsigned char player_state_offsets[5] = {3, 15, 27, 0, 27};
signed char player_dir_x = 0, player_dir_y = 16;
unsigned char player_hitbox_damage = 0;
unsigned char player_anim_frame = 0;
unsigned char player_anim_dir = 4;
unsigned char player_anim_flip = 0;
unsigned char player_health = 0;
unsigned char player_anim_state = PLAYER_STATE_NEUTRAL;
unsigned char player_max_health = INITIAL_MAX_HEALTH;
unsigned char player_action_type = 0;
unsigned char do_fill_map = 0;
unsigned char temp_msg_counter = 0;
char *message_string;