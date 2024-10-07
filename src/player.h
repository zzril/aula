#ifndef PLAYER_H
#define PLAYER_H
// --------

#include <stdbool.h>
#include <stddef.h>

#include <SDL2/SDL_audio.h>

#include "mixer.h"

// --------

struct Player;

// --------

typedef struct Player Player;

// --------

struct Player {
	Mixer mixer;
	SDL_AudioDeviceID device;
	bool paused;
};

// --------

int Player_init_at(Player* player);

void Player_destroy_at(Player* player);

int Player_play_bar(Player* player);

void Player_finish(Player* player);

// --------
#endif

