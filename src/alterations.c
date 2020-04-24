/* Nicholas Dullam, alterations.c, CS 24000, Spring 2020
 * Last updated April 9, 2020
 */

/* Add any includes here */

#include "alterations.h"

#include<assert.h>
#include<stdlib.h>
#include<string.h>

/*
 * change_event_octave
 * changes the octave
 * byte of the data
 */

int change_event_octave(event_t *event, int *octave) {
  if (event_type(event) != MIDI_EVENT_T) {
    return 0;
  }

  int type = event->midi_event.status & 0xF0;

  if ((type >= 0x80) && (type < 0xB0)) {
    int temp = event->midi_event.data[0];
    temp = temp + (*octave * OCTAVE_STEP);

    if ((temp > 127) || (temp < 0)) {
      return 0;
    } else {
      event->midi_event.data[0] = temp;
      return 1;
    }
  }
  return 0;
} /* change_event_octave() */

/*
 * change_event_time
 * changes the delta_time
 * for the event
 */

int change_event_time(event_t *event, float *time) {
  int orig_vlq = 1;
  int new_vlq = 1;
  int iterator = 8;

  if (event->delta_time > 0x7F) {
    orig_vlq = orig_vlq + 1;
    if (event->delta_time > 0x3FFF) {
      orig_vlq = orig_vlq + 1;
      if (event->delta_time > 0x1FFFFF) {
        orig_vlq = orig_vlq + 1;
      }
    }
  }

  event->delta_time = event->delta_time * *time;
  if (event->delta_time > 0x7F) {
    new_vlq = new_vlq + 1;
    if (event->delta_time > 0x3FFF) {
      new_vlq = new_vlq + 1;
      if (event->delta_time > 0x1FFFFF) {
        new_vlq = new_vlq + 1;
      }
    }
  }

  return new_vlq - orig_vlq;
} /* change_event_time() */

/*
 * change_event_delay
 * adds the delay to the
 * event delta_time
 */

int change_event_delay(event_t *event, unsigned int delay) {
  int orig_vlq = 1;
  int new_vlq = 1;
  int iterator = 8;

  if (event->delta_time > 0x7F) {
    orig_vlq = orig_vlq + 1;
    if (event->delta_time > 0x3FFF) {
      orig_vlq = orig_vlq + 1;
      if (event->delta_time > 0x1FFFFF) {
        orig_vlq = orig_vlq + 1;
      }
    }
  }

  event->delta_time = event->delta_time + delay;
  if (event->delta_time > 0x7F) {
    new_vlq = new_vlq + 1;
    if (event->delta_time > 0x3FFF) {
      new_vlq = new_vlq + 1;
      if (event->delta_time > 0x1FFFFF) {
        new_vlq = new_vlq + 1;
      }
    }
  }

  return new_vlq - orig_vlq;

} /* change_event_delay() */

/*
 * change_event_instrument
 * changes the instrument
 * byte of the event
 */

int change_event_instrument(event_t *event, remapping_t instruments) {
  if (event_type(event) != MIDI_EVENT_T) {
    return 0;
  }

  if (strcmp(event->midi_event.name, "Program Change") == 0) {
    event->midi_event.data[0] = instruments[event->midi_event.data[0]];
    return 1;
  }
  return 0;
} /* change_event_instrument() */

/*
 * change_event_note
 * changes the note byte
 * of the event
 */

int change_event_note(event_t *event, remapping_t note) {
  if (event_type(event) != MIDI_EVENT_T) {
    return 0;
  }

  if ((strcmp(event->midi_event.name, "Note On") == 0) ||
    (strcmp(event->midi_event.name, "Note Off") == 0) ||
    (strcmp(event->midi_event.name, "Polyphonic Key") == 0)) {
    event->midi_event.data[0] = note[event->midi_event.data[0]];
    return 1;
  }
  return 0;
} /* change_event_note() */

/*
 * apply_to_events
 * changes the events
 * with the given function
 */

int apply_to_events(song_data_t *song, event_func_t func, void *data) {
  return 0;
} /* apply_to_events() */

/*
 * change_octave
 * changes the octave
 * of every event
 */

int change_octave(song_data_t *song, int octave) {
  int sum_changes = 0;
  track_node_t *curr_track = song->track_list;
  while (curr_track != NULL) {
    event_node_t *curr_event = curr_track->track->event_list;
    while (curr_event != NULL) {
      sum_changes = sum_changes +
        (change_event_octave(curr_event->event, &octave));
      curr_event = curr_event->next_event;
    }
    curr_track = curr_track->next_track;
  }
  return sum_changes;
} /* change_octave() */

/*
 * warp_time
 * changes the delta_time
 * for every event
 */

int warp_time(song_data_t *song, float multiplier) {
  int sum_changes = 0;
  track_node_t *curr_track = song->track_list;
  while (curr_track != NULL) {
    int track_changes = 0;
    event_node_t *curr_event = curr_track->track->event_list;
    while (curr_event != NULL) {
      track_changes = track_changes +
        (change_event_time(curr_event->event, &multiplier));
      curr_event = curr_event->next_event;
    }
    curr_track->track->length = curr_track->track->length + track_changes;
    sum_changes = sum_changes + track_changes;
    curr_track = curr_track->next_track;
  }
  return sum_changes;
} /* warp_time() */

/*
 * remap_instruments
 * changes the instrument
 * for every given event
 */

int remap_instruments(song_data_t *song, remapping_t instruments) {
  int sum_changes = 0;
  track_node_t *curr_track = song->track_list;
  while (curr_track != NULL) {
    event_node_t *curr_event = curr_track->track->event_list;
    while (curr_event != NULL) {
      sum_changes = sum_changes +
        (change_event_instrument(curr_event->event, instruments));
      curr_event = curr_event->next_event;
    }
    curr_track = curr_track->next_track;
  }
  return sum_changes;
} /* remap_instruments() */

/*
 * remap_notes
 * changes the notes
 * for every event
 */

int remap_notes(song_data_t *song, remapping_t notes) {
  int sum_changes = 0;
  track_node_t *curr_track = song->track_list;
  while (curr_track != NULL) {
    event_node_t *curr_event = curr_track->track->event_list;
    while (curr_event != NULL) {
      sum_changes = sum_changes + (change_event_note(curr_event->event, notes));
      curr_event = curr_event->next_event;
    }
    curr_track = curr_track->next_track;
  }
  return sum_changes;
} /* remap_notes() */

/*
 * add_round
 * adds a duplicate track
 * with the given changes
 */

void add_round(song_data_t *song, int index, int octave,
  unsigned int time, uint8_t instrument) {
  int iterator = 0;
  assert(song->format != 2);
  track_node_t *curr_track = song->track_list;
  printf("%s", song->path);
  while (curr_track != NULL) {
    if (index == iterator) {
      break;
    } else {
      curr_track = curr_track->next_track;
      iterator = iterator + 1;
    }
  }

  assert(!(iterator < index));

  track_node_t *new_track = malloc(sizeof(track_node_t));
  new_track->track = malloc(sizeof(track_t));
  new_track->track->event_list = NULL;
  new_track->next_track = NULL;

  new_track->track->length = curr_track->track->length;
  event_node_t *curr_event = curr_track->track->event_list;

  event_node_t *new_event = NULL;
  event_node_t *curr_new_event = NULL;

  while (curr_event != NULL) {
    new_event = malloc(sizeof(event_node_t));
    new_event->event = NULL;
    new_event->event = malloc(sizeof(event_t));
    new_event->next_event = NULL;
    new_event->event->type = curr_event->event->type;
    new_event->event->delta_time = curr_event->event->delta_time;
    switch (event_type(curr_event->event)) {
      case SYS_EVENT_T:
        new_event->event->sys_event.data_len =
          curr_event->event->sys_event.data_len;
        new_event->event->sys_event.data = NULL;
        if (new_event->event->sys_event.data_len != 0) {
          new_event->event->sys_event.data =
            malloc(curr_event->event->sys_event.data_len * sizeof(uint8_t));
          for (int i = 0; i < curr_event->event->sys_event.data_len; i++) {
            new_event->event->sys_event.data[i] =
              curr_event->event->sys_event.data[i];
          }
        }
        break;
      case META_EVENT_T:
        new_event->event->meta_event.data_len =
          curr_event->event->meta_event.data_len;
        new_event->event->meta_event.name = curr_event->event->meta_event.name;
        new_event->event->meta_event.data = NULL;
        if (new_event->event->meta_event.data_len != 0) {
          new_event->event->meta_event.data =
            malloc(curr_event->event->meta_event.data_len * sizeof(uint8_t));
          for (int i = 0; i < curr_event->event->meta_event.data_len; i++) {
            new_event->event->meta_event.data[i] =
              curr_event->event->meta_event.data[i];
          }
        }

        break;
      case MIDI_EVENT_T:
        new_event->event->midi_event.status =
          curr_event->event->midi_event.status;
        new_event->event->midi_event.data_len =
          curr_event->event->midi_event.data_len;
        new_event->event->midi_event.name = curr_event->event->midi_event.name;
        new_event->event->midi_event.data = NULL;
        if (new_event->event->midi_event.data_len != 0){
          new_event->event->midi_event.data =
            malloc(curr_event->event->midi_event.data_len * sizeof(uint8_t));
          for (int i = 0; i < curr_event->event->midi_event.data_len; i++) {
            new_event->event->midi_event.data[i] =
              curr_event->event->midi_event.data[i];
          }
        }
        break;
    }
    curr_event = curr_event->next_event;

    if (new_track->track->event_list == NULL) {
      new_track->track->event_list = new_event;
      curr_new_event = new_event;
    } else {
      curr_new_event->next_event = new_event;
      curr_new_event = new_event;
    }
  }

  //adjustments

  new_event = new_track->track->event_list;

  int byte_difference = 0;
  if (new_event != NULL) {
    byte_difference = change_event_delay(new_event->event, time);
  }

  new_track->track->length = new_track->track->length + byte_difference;
  song->num_tracks = song->num_tracks + 1;
  song->format = 1;
  int channels[16] = {};
  while (new_event != NULL) {
    if (event_type(new_event->event) == MIDI_EVENT_T) {
      if (new_event->event->midi_event.data != NULL) {
        change_event_octave(new_event->event, &octave);
      }
      int type = new_event->event->midi_event.status & 0xF0;
      if (new_event->event->midi_event.data != NULL) {
        if ((type >= 0x80) && (type < 0xB0))  {
          new_event->event->midi_event.data[0] = instrument;
        }
      }
      if ((type >= 0x80) && (type < 0xF0)) {
        channels[(new_event->event->midi_event.status & 0x0F)] = 1;
      }
    }
    new_event = new_event->next_event;
  }

  int smallest = 0;

  for (int i = 0; i < 16; i++) {
    if (channels[i] == 0) {
      smallest = i;
      break;
    }
    assert(i < 15);
  }

  new_event = new_track->track->event_list;
  while (new_event != NULL) {
    if (event_type(new_event->event) == MIDI_EVENT_T) {
      int type = new_event->event->midi_event.status & 0xF0;
      if ((type >= 0x80) && (type < 0xF0)) {
        new_event->event->midi_event.status =
          (new_event->event->midi_event.status & 0xF0) + smallest;
        if ((type & 0x80)) {
          new_event->event->type = new_event->event->midi_event.status;
        } else {
          new_event->event->type = new_event->event->midi_event.data[0];
        }
      }
    }
    new_event = new_event->next_event;
  }

  while (curr_track->next_track != NULL) {
    curr_track = curr_track->next_track;
  }
  curr_track->next_track = new_track;
} /* add_round() */

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
    N_LOWER[i] = i - 1;
  }
  //  Swap F# for G
  for (int i = 6; i <= 0xFF; i += 12) {
    N_LOWER[i] = i + 1;
  }
} /* build_mapping_tables() */
