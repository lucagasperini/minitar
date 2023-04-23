#include "stack.h"

struct minitar_dir_stack* minitar_dir_stack_push(struct minitar_dir_stack* head, struct minitar_header_entry *entry)
{
        if (head == NULL) {
                head = (struct minitar_dir_stack*)malloc(sizeof(struct minitar_dir_stack));
                head->entry = minitar_header_entry_copy(entry);
                return head;
        }

        struct minitar_dir_stack* new_head = (struct minitar_dir_stack*)malloc(sizeof(struct minitar_dir_stack));
        new_head->entry = minitar_header_entry_copy(entry);
        new_head->next = head;
        return new_head;
}

struct minitar_dir_stack* minitar_dir_stack_pop(struct minitar_dir_stack* head, struct minitar_header_entry *entry)
{
        if(head == NULL) {
                return NULL;
        }

        struct minitar_dir_stack* new_head = head->next;
        minitar_header_entry_override(head->entry ,entry);
        minitar_header_entry_free(head->entry);
        free(head);
        return new_head;
}

struct minitar_dir_stack* minitar_dir_stack_next(struct minitar_dir_stack* head, struct minitar_header_entry *entry)
{
        if(head == NULL) {
                return NULL;
        }
        
        minitar_header_entry_override(head->entry,entry);
        return head->next;
}
