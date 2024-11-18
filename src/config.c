#include <stdbool.h>

#include "config.h"
#include "error_codes.h"

// --------

typedef struct Config {
	bool locked;
	uint64_t sample_frames_per_second;
	float beats_per_minute;
	uint8_t quarter_notes_per_bar;
} Config;

// --------

static Config config = {
	.locked = false,
	.sample_frames_per_second = 48000,
	.beats_per_minute = 72,
	.quarter_notes_per_bar = 4,
};

// --------

uint64_t Config_get_framerate() {
	config.locked = true;
	return config.sample_frames_per_second;
}

float Config_get_bpm() {
	config.locked = true;
	return config.beats_per_minute;
}

uint8_t Config_get_quarter_notes_per_bar() {
	config.locked = true;
	return config.quarter_notes_per_bar;
}

// --------

int Config_set_framerate(uint64_t sample_frames_per_second) {

	if(config.locked) {
		return ERROR_CODE_CONFIG_WRITE_AFTER_READ;
	}

	config.sample_frames_per_second = sample_frames_per_second;

	return 0;
}

int Config_set_bpm(float beats_per_minute) {

	if(config.locked) {
		return ERROR_CODE_CONFIG_WRITE_AFTER_READ;
	}

	config.beats_per_minute = beats_per_minute;

	return 0;
}

int Config_set_quarter_notes_per_bar(uint8_t quarter_notes_per_bar) {

	if(config.locked) {
		return ERROR_CODE_CONFIG_WRITE_AFTER_READ;
	}

	config.quarter_notes_per_bar = quarter_notes_per_bar;

	return 0;
}

// --------

float Config_get_beats_per_second() {
	return Config_get_bpm() / 60;
}

float Config_get_quarter_note_length_in_seconds() {
	return 1 / Config_get_beats_per_second();
}

unsigned int Config_get_bar_length_in_seconds() {
	return Config_get_quarter_notes_per_bar() * Config_get_quarter_note_length_in_seconds();
}

size_t Config_get_sample_frames_per_bar() {
	return (((Config_get_quarter_notes_per_bar() * 60) * Config_get_framerate()) / Config_get_bpm());
}

