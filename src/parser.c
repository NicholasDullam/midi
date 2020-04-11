/* Name, parser.c, CS 24000, Spring 2020
 * Last updated March 27, 2020
 */

/* Add any includes here */

#include "parser.h"

#include<assert.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
/* Define parse_file here */

song_data_t *parse_file(const char *file_path) {
  assert(file_path != NULL);
  song_data_t *song = NULL;
  return song;
}

/* Define parse_header here */

void parse_header(FILE *fp, song_data_t *parse_data) {
  char type[4] = "";
  uint32_t length = 0;
  uint8_t length_buffer[4] = {0};
  uint8_t format = 0;

  assert(fp != NULL);
  fread(type, 4, 1, fp);
  assert(strcmp(type, "MThd") == 0);

  for (int i = 0; i < 4; i++) {
    fread(&length_buffer[i], sizeof(uint8_t), 1, fp);
  }

  length = end_swap_32(length_buffer);
  assert(length == 6);

  fread(&(parse_data->format), sizeof(uint8_t), 1, fp);
  fread(&(parse_data->format), sizeof(uint8_t), 1, fp);
  assert((parse_data->format == 0)
    || (parse_data->format == 1)
    || (parse_data->format == 2));
}

/* Define parse_track here */

void parse_track(FILE *fp, song_data_t *parse_data) {
  char type[4] = "";
  uint32_t length = 0;
  assert(fp != NULL);
  fread(type, 4, 1, fp);
  assert(strcmp(type, "MTrk") == 0);
  fread(&length, sizeof(uint32_t), 1, fp);
}

/* Define parse_event here */

event_t *parse_event(FILE *fp) {
  event_t *event = NULL;
  return event;
}

/* Define parse_sys_event here */

sys_event_t parse_sys_event(FILE *fp, uint8_t data) {
  sys_event_t event = {};
  return event;
}

/* Define parse_meta_event here */

meta_event_t parse_meta_event(FILE *fp) {
  meta_event_t event = {};
  return event;
}

/* Define parse_midi_event here */

midi_event_t parse_midi_event(FILE *fp, uint8_t data) {
  assert(fp != NULL);
  midi_event_t event = {};
  return event;
}

/* Define parse_var_len here */

uint32_t parse_var_len(FILE *fp) {
  int count = 0;
  uint32_t length = 0;
  uint8_t buffer[4] = {0};

  while(1) {
    fread(&buffer[count], sizeof(uint8_t), 1, fp);
    if ((buffer[count] & 0x80) == 0x80) {
      count += 1;
      continue;
    }
    break;
  }

  length = ((buffer[0] & 0x7F) << 21)
    | ((buffer[1] & 0x7F) << 14)
    | ((buffer[2] & 0x7F) << 7)
    | ((buffer[3] & 0x7F));
  return length;
}

/* Define event_type here */

uint8_t event_type(event_t *event) {
   switch (event->type) {
    case SYS_EVENT_1:
    case SYS_EVENT_2:
      return SYS_EVENT_T;
      break;
    case META_EVENT:
      return META_EVENT_T;
      break;
    default:
      return MIDI_EVENT_T;
  }
}

/* Define free_song here */

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
}

/* Define free_track_node here */

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
}

/* Define free_event_node here */

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
      //free(event_node->event->meta_event.name);
      free(event_node->event->meta_event.data);
      break;
    default:
      //free(event_node->event->midi_event.name);
      free(event_node->event->midi_event.data);
  }
  free(event_node->event);
  free(event_node);
}

/* Define end_swap_16 here */

uint16_t end_swap_16(uint8_t buffer[2]) {
  uint16_t reverse = (buffer[0] << 8)
    | buffer[1];
  return reverse;
}

/* Define end_swap_32 here */

uint32_t end_swap_32(uint8_t buffer[4]) {
  uint32_t reverse = (buffer[0] << 24)
    | (buffer[1] << 16)
    | (buffer[2] << 8)
    | (buffer[3]);
  return reverse;
}
