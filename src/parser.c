/* Name, parser.c, CS 24000, Spring 2020
 * Last updated March 27, 2020
 */

/* Add any includes here */

#include "parser.h"

#include<assert.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>

uint8_t g_midi_status = 0;

/*
 * parse_file
 * parses the given file
 * reading in all midi data
 */

song_data_t *parse_file(const char *file_path) {
  song_data_t *parse_data = NULL;
  assert(file_path != NULL);
  FILE *fp = fopen(file_path, "r");
  assert(fp != NULL);
  parse_data = malloc(sizeof(song_data_t));
  parse_data->track_list = NULL;
  parse_data->path = malloc(strlen(file_path) + 1);
  strncpy(parse_data->path, file_path, strlen(file_path));
  parse_data->path[strlen(file_path)] = '\0';
  parse_header(fp, parse_data);
  int test = ftell(fp);
  fseek(fp, 0, SEEK_END);
  assert(test == ftell(fp));
  fclose(fp);
  return parse_data;
} /* parse_file() */

/*
 * parse_header
 * parses the given file
 * for the midi_header
 */

void parse_header(FILE *fp, song_data_t *parse_data) {
  char type[4] = "";
  uint32_t length = 0;
  uint8_t ntrks_buffer[2] = {0};
  uint8_t length_buffer[4] = {0};

  assert(fp != NULL);
  assert(fread(type, 4, 1, fp));
  assert(strcmp(type, "MThd") == 0);

  for (int i = 0; i < 4; i++) {
    assert(fread(&length_buffer[i], sizeof(uint8_t), 1, fp));
  }

  length = end_swap_32(length_buffer);
  assert(length == 6);

  uint8_t format_buffer[2] = {0};
  uint16_t format = 0;
  for (int i = 0; i < 2; i++) {
    assert(fread(&format_buffer[i], 1, 1, fp));
  }

  format = end_swap_16(format_buffer);
  assert((format == 0)
    || (format == 1)
    || (format == 2));

  parse_data->format = format;
  for (int i = 0; i < 2; i++) {
    assert(fread(&ntrks_buffer[i], 1, 1, fp));
  }

  parse_data->num_tracks = end_swap_16(ntrks_buffer);

  for (int i = 0; i < 2; i++) {
    assert(fread(&ntrks_buffer[i], 1, 1, fp));
  }

  uint16_t division = end_swap_16(ntrks_buffer);

  if ((division & 0x8000) != 0x8000) {
    parse_data->division.uses_tpq = true;
    parse_data->division.ticks_per_qtr = division;
  } else {
    parse_data->division.uses_tpq = false;
    parse_data->division.frames_per_sec = division >> 8;
    parse_data->division.ticks_per_frame = division & 0x00FF;
  }

  for (int i = 0; i < parse_data->num_tracks; i++) {
    parse_track(fp, parse_data);
  }
} /* parse_header() */

/*
 * parse_track
 * parses the given file
 * for associated track data
 */

void parse_track(FILE *fp, song_data_t *parse_data) {
  track_node_t *curr_node = NULL;

  if (parse_data->track_list == NULL) {
    parse_data->track_list = malloc(sizeof(track_node_t));
    parse_data->track_list->next_track = NULL;
    parse_data->track_list->track = malloc(sizeof(track_t));
    curr_node = parse_data->track_list;
  } else {
    curr_node = parse_data->track_list;
    while (curr_node->next_track != NULL) {
      curr_node = curr_node->next_track;
    }
    curr_node->next_track = malloc(sizeof(track_node_t));
    curr_node->next_track->track = malloc(sizeof(track_t));
    curr_node = curr_node->next_track;
    curr_node->next_track = NULL;
  }

  curr_node->track->event_list = malloc(sizeof(event_node_t));
  curr_node->track->event_list->event = NULL;
  event_node_t *curr_enode = curr_node->track->event_list;

  char type[4] = "";
  uint8_t length[4] = {0};
  assert(fp != NULL);
  assert(fread(type, 4, 1, fp));
  assert(strcmp(type, "MTrk") == 0);

  for (int i = 0; i < 4; i++) {
    assert(fread(&length[i], 1, 1, fp));
  }
  curr_node->track->length = end_swap_32(length);

  while (1) {
    curr_enode->event = parse_event(fp);
    curr_enode->next_event = NULL;
    if ((event_type(curr_enode->event) == META_EVENT_T) &&
      (strcmp(curr_enode->event->meta_event.name, "End of Track") == 0)) {
      break;
    }
    curr_enode->next_event = malloc(sizeof(event_node_t));
    curr_enode->next_event->event = NULL;
    curr_enode = curr_enode->next_event;
  }
} /* parse_track() */

/*
 * parse_event
 * parses the given file
 * chosing the event_type
 */

event_t *parse_event(FILE *fp) {
  event_t *event = malloc(sizeof(event_t));
  event->delta_time = parse_var_len(fp);
  assert(fread(&(event->type), 1, 1, fp));
  switch (event_type(event)) {
    case SYS_EVENT_T:
      event->sys_event = parse_sys_event(fp, event->type);
      break;
    case META_EVENT_T:
      event->meta_event = parse_meta_event(fp);
      break;
    case MIDI_EVENT_T:
      event->midi_event = parse_midi_event(fp, event->type);
  }
  return event;
} /* parse_event() */

/*
 * parse_sys_event
 * parses the given file
 * for a sys_event
 */

sys_event_t parse_sys_event(FILE *fp, uint8_t data) {
  sys_event_t event = {};
  event.data_len = parse_var_len(fp);
  if (event.data_len > 0) {
    event.data = malloc(event.data_len * sizeof(uint8_t));
  }
  for (int i = 0; i < event.data_len; i++) {
    assert(fread(&(event.data[i]), 1, 1, fp));
  }
  return event;
} /* parse_sys_event() */

/*
 * parse_meta_event
 * parses the given file
 * for a meta_event
 */

meta_event_t parse_meta_event(FILE *fp) {
  meta_event_t event = {};
  uint8_t type = 0;
  assert(fread(&type, 1, 1, fp));
  event.name = META_TABLE[type].name;
  assert(event.name);
  event.data_len = parse_var_len(fp);
  if (META_TABLE[type].data_len != 0) {
    assert(event.data_len == META_TABLE[type].data_len);
  }
  if (event.data_len > 0) {
    event.data = malloc(event.data_len * sizeof(uint8_t));
  }
  for (int i = 0; i < event.data_len; i++) {
    assert(fread(&(event.data[i]), 1, 1, fp));
  }
  return event;
} /* parse_meta_event() */

/*
 * parse_midi_event
 * parses the given file
 * for a midi_event
 */

midi_event_t parse_midi_event(FILE *fp, uint8_t status) {
  int count = 0;
  midi_event_t event = {};
  if (status & 0x80) {
    g_midi_status = status;
  } else {
    count = 1;
  }

  event.status = g_midi_status;
  event.name = MIDI_TABLE[event.status].name;
  assert(event.name);
  event.data_len = MIDI_TABLE[event.status].data_len;
  if (event.data_len > 0) {
    event.data = malloc(event.data_len * sizeof(uint8_t));
  }
  if ((count) && ((count - 1) < event.data_len)) {
    event.data[0] = status;
  }

  while (count < event.data_len) {
    assert(fread(&(event.data[count]), 1, 1, fp));
    count = count + 1;
  }

  return event;
} /* parse_midi_event() */

/*
 * parse_var_len
 * parses the given file
 * for a variable length quantity
 */

uint32_t parse_var_len(FILE *fp) {
  uint32_t length = 0;
  uint8_t buffer = 0;

  assert(fread(&buffer, 1, 1, fp));
  length = buffer & 0x7F;

  while (1) {
    if ((buffer & 0x80) != 0x80) {
      break;
    }
    assert(fread(&buffer, 1, 1, fp));
    length = length << 7;
    length = length + (buffer & 0x7F);
  }

  return length;
} /* parse_var_len() */

/*
 * event_type
 * takes the given event values
 * and returns a general type
 */

uint8_t event_type(event_t *event) {
  switch (event->type) {
    case SYS_EVENT_1:
      return SYS_EVENT_T;
      break;
    case SYS_EVENT_2:
      return SYS_EVENT_T;
      break;
    case META_EVENT:
      return META_EVENT_T;
      break;
    default:
      return MIDI_EVENT_T;
  }
} /* event_type() */

/*
 * free_song
 * frees all memory
 * associated with the given song
 */

void free_song(song_data_t *song) {
  assert(song != NULL);
  track_node_t *curr_track = song->track_list;
  track_node_t *temp_track = NULL;

  while (curr_track != NULL) {
    temp_track = curr_track->next_track;
    free_track_node(curr_track);
    curr_track = temp_track;
  }

  free(song->path);
  free(song);
  song = NULL;
} /* free_song() */

/*
 * free_track_node
 * frees all associated data
 * for the given track
 */

void free_track_node(track_node_t *track_node) {
  assert(track_node != NULL);
  event_node_t *curr_event = track_node->track->event_list;
  event_node_t *temp_event = NULL;

  while (curr_event != NULL) {
    temp_event = curr_event->next_event;
    free_event_node(curr_event);
    curr_event = temp_event;
  }

  free(track_node->track);
  free(track_node);
  track_node = NULL;
} /* free_track_node() */

/*
 * free_event_node
 * frees all associated data
 * for the given event
 */

void free_event_node(event_node_t *event_node) {
  assert(event_node != NULL);
  switch (event_node->event->type) {
    case SYS_EVENT_1:
      free(event_node->event->sys_event.data);
      break;
    case SYS_EVENT_2:
      free(event_node->event->sys_event.data);
      break;
    case META_EVENT:
      free(event_node->event->meta_event.data);
      break;
    default:
      free(event_node->event->midi_event.data);
  }
  free(event_node->event);
  free(event_node);
  event_node = NULL;
} /* free_event_node() */

/*
 * end_swap_16
 * flips the endian of the given
 * 16 bit value
 */

uint16_t end_swap_16(uint8_t buffer[2]) {
  uint16_t reverse = (buffer[0] << 8)
    | buffer[1];
  return reverse;
} /* end_swap_16() */

/*
 * end_swap_32
 * flips the endian of the given
 * 32bit value
 */

uint32_t end_swap_32(uint8_t buffer[4]) {
  uint32_t reverse = (buffer[0] << 24)
    | (buffer[1] << 16)
    | (buffer[2] << 8)
    | (buffer[3]);
  return reverse;
} /* end_swap_32() */
