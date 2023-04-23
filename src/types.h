#pragma once

#include "define.h"

struct minitar_header_entry {
  char name[FILE_NAME_MAX];
  uint64_t start;
  uint64_t length;
};

static inline void minitar_header_entry_init(struct minitar_header_entry *entry, const char *_name) {
  entry->start = 0;
  entry->length = 0;
  strncpy(entry->name, _name, FILE_NAME_MAX);
}

static inline struct minitar_header_entry *
minitar_header_entry_new(const char *_name, uint64_t _start, uint64_t _length) {

  struct minitar_header_entry *ptr = (struct minitar_header_entry *)malloc(
      sizeof(struct minitar_header_entry));

  ptr->start = _start;
  ptr->length = _length;
  strncpy(ptr->name, _name, FILE_NAME_MAX);
}

static inline struct minitar_header_entry *
minitar_header_entry_copy(const struct minitar_header_entry *entry) {

  struct minitar_header_entry *ptr = (struct minitar_header_entry *)malloc(
      sizeof(struct minitar_header_entry));

  ptr->start = entry->start;
  ptr->length = entry->length;
  strncpy(ptr->name, entry->name, FILE_NAME_MAX);
}

static inline struct minitar_header_entry *
minitar_header_entry_override(const struct minitar_header_entry *src, struct minitar_header_entry *dest) {

  dest->start = src->start;
  dest->length = src->length;
  strncpy(dest->name, src->name, FILE_NAME_MAX);
}

static inline void minitar_header_entry_free(struct minitar_header_entry *entry)
{
        free(entry);
}