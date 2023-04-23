#include "queue.h"

struct minitar_dir_queue *
minitar_dir_queue_enqueue(struct minitar_dir_queue *tail,
                          struct minitar_header_entry *entry) 
{
        if(tail == NULL) {
                tail = (struct minitar_dir_queue*)malloc(sizeof(struct minitar_dir_queue));
                tail->entry = minitar_header_entry_copy(entry);
                tail->next = NULL;
                return tail;
        }

        struct minitar_dir_queue* new_tail = (struct minitar_dir_queue*)malloc(sizeof(struct minitar_dir_queue));
        new_tail->entry = minitar_header_entry_copy(entry);
        new_tail->next = NULL;
        tail->next = new_tail;
        return new_tail;
}

struct minitar_dir_queue *
minitar_dir_queue_dequeue(struct minitar_dir_queue *head,
                          struct minitar_header_entry *entry)
{
        if(head == NULL) {
                return NULL;
        }

        struct minitar_dir_queue* new_head = head->next;
        minitar_header_entry_override(head->entry, entry);
        minitar_header_entry_free(head->entry);
        free(head);
        return new_head;
}

static struct minitar_dir_queue *
minitar_dir_queue_next(struct minitar_dir_queue *head,
                       struct minitar_header_entry *entry) 
{
        if(head == NULL) {
                return NULL;
        }

        minitar_header_entry_override(head->entry, entry);
        return head->next;
}