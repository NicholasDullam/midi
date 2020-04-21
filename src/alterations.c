/* Nicholas Dullam, alterations.c, CS 24000, Spring 2020
 * Last updated April 9, 2020
 */

/* Add any includes here */

#include "alterations.h"

#include<assert.h>
#include<stdlib.h>
#include<string.h>

/*Define change_event_octave here */

int change_event_octave(event_t *event, int *octave) {
  assert(event_type(event) == MIDI_EVENT_T);
  if (strcmp(event->midi_event.name, "Note on") == 0 ||
    strcmp(event->midi_event.name, "Note off") == 0 ||
    strcmp(event->midi_event.name, "Polyphonic Key Pressure")) {
    uint8_t temp = event->midi_event.data[0];
    temp = temp + *octave;

    if (temp > 127 || temp < 0) {
      return 0;
    } else {
      event->midi_event.data[0] = temp;
      return 1;
    }
  }
  return 0;
}

/*Define change_event_time here */

int change_event_time(event_t *event, float *time) {
  return 0;
}

/*Define change_event_instrument here */

int change_event_instrument(event_t *event, remapping_t instruments) {
  return 0;
}

/*Define change_event_note here */

int change_event_node(event_t *event, remapping_t note) {
  return 0;
}

/*Define apply_to_events here */

int apply_to_events(song_data_t *song, event_func_t, void *data) {
  return 0;
}

/*Define change_octave here */

int change_octave(song_data_t *song, int octave) {
  return 0;
}

/*Define warp_time here */

int warp_time(song_data_t *song, float multiplier) {
  return 0;
}

/*Define remap_instruments here */

int remap_instruments(song_data_t *song, remapping_t instruments) {
  return 0;
}

/*Define remap_notes here */

int remap_notes(song_data_t *song, remapping_t notes) {
  return 0;
}

/*Define add_round here */

void add_round(song_data_t *song, int index, int octave, unsigned int time, uint8_t instrument) {

}

/*
 * Function called prior to main that sets up random mapping tables
 */

void build_mapping_tables()
{
  for (int i = 0; i <= 0xFF; i++) {
    I_BRASS_BAND[i] = 61;
  }

  for (int i = 0; i <= 0xFF; i++) {
    I_HELICOPTER[i] = 125;
  }

  for (int i = 0; i <= 0xFF; i++) {
    N_LOWER[i] = i;
  }
  //  Swap C# for C
  for (int i = 1; i <= 0xFF; i += 12) {
    N_LOWER[i] = i-1;
  }
  //  Swap F# for G
  for (int i = 6; i <= 0xFF; i += 12) {
    N_LOWER[i] = i+1;
  }
} /* build_mapping_tables() */
