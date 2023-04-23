#pragma once

#include "types.h"

struct minitar_dir_queue {
  struct minitar_dir_queue *next;
  struct minitar_header_entry *entry;
};

struct minitar_dir_queue *
minitar_dir_queue_enqueue(struct minitar_dir_queue *head,
                          struct minitar_header_entry *entry);

struct minitar_dir_queue *
minitar_dir_queue_dequeue(struct minitar_dir_queue *head,
                          struct minitar_header_entry *entry);

static struct minitar_dir_queue *
minitar_dir_queue_next(struct minitar_dir_queue *head,
                       struct minitar_header_entry *entry);
