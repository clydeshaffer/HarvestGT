#ifndef MUSIC_H
#define MUSIC_H

#define MUSIC_TRACK_NONE 0
#define MUSIC_TRACK_MAIN 4
#define MUSIC_TRACK_BATTLE 5

#define REPEAT_NONE 0
#define REPEAT_LOOP 1
#define REPEAT_RESUME 2

void init_music();

void play_track(char track, char loop);

void tick_music();

void do_noise_effect(char note, char bend, char duration);

void stop_music();

void pause_music();

void unpause_music();

#endif