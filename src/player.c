#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_hints.h>

#include "config.h"
#include "player.h"

// --------

static void print_sdl_error(const char* s);

static int shutdown(Player* player, int status);

// --------

static bool initialized = false;

// --------

static void print_sdl_error(const char* s) {
	fputs(s, stderr);
	fputs(": ", stderr);
	fputs(SDL_GetError(), stderr);
	fputs("\n", stderr);
	return;
}

static int shutdown(Player* player, int status) {

	if(player != NULL) {

		Mixer_destroy_at(&(player->mixer));

		if(player->device != 0) {
			SDL_CloseAudioDevice(player->device);
		}
		player->paused = true;
	}

	SDL_Quit();

	initialized = false;

	return status;
}

// --------

int Player_init_at(Player* player) {

	SDL_AudioSpec desired, obtained;
	long pagesize;

	if(player == NULL) {
		fputs("WARNING: Ignoring call to Player_init_at(NULL) and not initializing SDL.\n", stderr);
		return 1;
	}

	memset(player, 0, sizeof(Player));

	if(initialized) {
		fputs("WARNING: Ignoring attempt to initialize multiple Players.\n", stderr);
		return 2;
	}

	SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "1");

	int status = SDL_Init(SDL_INIT_AUDIO);
	if(status < 0) {
		print_sdl_error("SDL_Init");
		return status;
	}

	initialized = true;

	errno = 0;
	pagesize = sysconf(_SC_PAGESIZE);
	if(pagesize == -1) {
		if(errno) {
			perror("sysconf");
		}
		else {
			fputs("Cannot determine sysconf(_SC_PAGESIZE).\n", stderr);
		}
		return shutdown(player, -1);
	}

	status = Mixer_init_at(&(player->mixer));
	if(status != 0) {
		return shutdown(player, status);
	}

	memset(&desired, 0, sizeof(desired));
	memset(&obtained, 0, sizeof(obtained));

	desired.freq = SAMPLE_FRAMES_PER_SECOND;
	desired.format = AUDIO_F32SYS;
	desired.channels = 1;
	desired.samples = pagesize / sizeof(float);
	desired.callback = NULL;
	desired.userdata = NULL;

	player->device = SDL_OpenAudioDevice(NULL, 0, &desired, &obtained, 0);
	if(player->device == 0) {
		return shutdown(player, -2);
	}

	player->paused = true;

	return 0;
}

void Player_destroy_at(Player* player) {

	if(player == NULL) {
		fputs("WARNING: Ignoring call to Player_destroy_at(NULL) and not shutting down SDL.\n", stderr);
		return;
	}

	shutdown(player, 0);

	return;
}

int Player_play_bar(Player* player) {

	if(player == NULL) {
		return 1;
	}

	if(player->paused) {
		SDL_PauseAudioDevice(player->device, 0);
		player->paused = false;
	}

	int status = SDL_QueueAudio(player->device, (player->mixer).buffer, ((player->mixer).num_samples) * sizeof(float));
	if(status < 0) {
		print_sdl_error("SDL_QueueAudio");
		return status;
	}

	while(SDL_GetQueuedAudioSize(player->device) > 2 * ((player->mixer).num_samples) * sizeof(float)) {
		sleep(BAR_LENGTH_IN_SECONDS);
	}

	return 0;
}

void Player_finish(Player* player) {

	if(player->device) {
		Uint32 queued_audio_size;
		while((queued_audio_size = SDL_GetQueuedAudioSize(player->device)) != 0) {
			sleep(1);
		}
	}

	return;
}

