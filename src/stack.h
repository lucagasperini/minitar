#pragma once

#include "types.h"

struct minitar_dir_stack {
  struct minitar_dir_stack *next;
  struct minitar_header_entry *entry;
};

struct minitar_dir_stack *
minitar_dir_stack_push(struct minitar_dir_stack *head,
                       struct minitar_header_entry *entry);

struct minitar_dir_stack *
minitar_dir_stack_pop(struct minitar_dir_stack *head,
                      struct minitar_header_entry *entry);

struct minitar_dir_stack *
minitar_dir_stack_next(struct minitar_dir_stack *head,
                       struct minitar_header_entry *entry);
