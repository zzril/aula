#ifndef CONFIG_H
#define CONFIG_H
// --------

#include <stddef.h>
#include <stdint.h>

// --------

uint64_t Config_get_framerate();
float Config_get_bpm();
uint8_t Config_get_quarter_notes_per_bar();

int Config_set_framerate(uint64_t sample_frames_per_second);
int Config_set_bpm(float beats_per_minute);
int Config_set_quarter_notes_per_bar(uint8_t quarter_notes_per_bar);

float Config_get_quarter_note_length_in_seconds();
unsigned int Config_get_bar_length_in_seconds();
size_t Config_get_sample_frames_per_bar();

// --------
#endif

