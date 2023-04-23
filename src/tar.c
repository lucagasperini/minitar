#include "queue.h"
#include "stack.h"

static ssize_t extract_write_single_file(const char* dest_file, int fd_src_tar, ssize_t file_length)
{
        int fd = open(dest_file, O_WRONLY | O_CREAT, 0664);
        if(fd == -1) {
                fprintf(stderr, "extract_write_single_file open error: %d\n", errno);
                return -1;
        }

        char buffer[FILE_CHUNK];
        ssize_t bytes_read = 0;
        ssize_t bytes_write = 0;
        ssize_t total_bytes_write = 0;
        size_t chunk_to_write = FILE_CHUNK;
        do {
                if(file_length - total_bytes_write > FILE_CHUNK) {
                        chunk_to_write = FILE_CHUNK;
                } else {
                        chunk_to_write = file_length - total_bytes_write;
                }

                bytes_read = read(fd_src_tar, buffer, chunk_to_write);
                if(bytes_read == -1) {
                        fprintf(stderr, "extract_write_single_file read error: %d\n", errno);
                        return -1; 
                }

                bytes_write = write(fd, buffer, chunk_to_write);
                if(bytes_write == -1) {
                        fprintf(stderr, "extract_write_single_file write error: %d\n", errno);
                        return -1; 
                }
                
                total_bytes_write += bytes_write;

        } while (file_length > total_bytes_write);

        close(fd);
        return bytes_write;
}

static ssize_t extract_write_data(struct minitar_dir_queue* queue_head, const char* dest_path, int fd_src_tar)
{
        struct minitar_header_entry buffer_entry;
        char buffer_file[FILE_NAME_MAX];
        char buffer_fullpath_file[FILENAME_MAX];
        size_t dest_path_length = strlen(dest_path);
        ssize_t total_bytes_write = 0;
        bool has_ending_seperator = false;
        if(dest_path[dest_path_length - 1] == '/') {
                has_ending_seperator = true;
        }

        while(queue_head) {
                queue_head = minitar_dir_queue_dequeue(queue_head, &buffer_entry);
                strcpy(buffer_fullpath_file, dest_path);
                if(!has_ending_seperator) {
                        strcat(buffer_fullpath_file, "/");
                }
                strcat(buffer_fullpath_file, buffer_entry.name);
                extract_write_single_file(buffer_fullpath_file, fd_src_tar, buffer_entry.length);
        }

        return total_bytes_write;
}

int minitar_extract(const char *src_tar, const char *dest_path)
{
        int fd_src_tar = open(src_tar, O_RDONLY);
        if(fd_src_tar == -1) {
                fprintf(stderr, "minitar_create open error: %d\n", errno);
                return -1;
        }

        uint64_t number_elements;
        ssize_t bytes_read = read(fd_src_tar, &number_elements, sizeof(uint64_t));
        if(bytes_read == -1) {
                fprintf(stderr, "minitar_list read error: %d\n", errno);
                return -1;
        }

        struct minitar_dir_queue* queue_head = NULL;
        struct minitar_dir_queue* queue_tail = NULL;
        struct minitar_header_entry buffer_entry;

        for(uint64_t i = 0; i < number_elements; i++) {
                
                bytes_read = read(fd_src_tar, buffer_entry.name, FILE_NAME_MAX);
                if(bytes_read == -1) {
                        fprintf(stderr, "minitar_list read error: %d\n", errno);
                        return -1;
                }

                bytes_read = read(fd_src_tar, &buffer_entry.start, sizeof(uint64_t));
                if(bytes_read == -1) {
                        fprintf(stderr, "minitar_list read error: %d\n", errno);
                        return -1;
                }

                bytes_read = read(fd_src_tar, &buffer_entry.length, sizeof(uint64_t));
                if(bytes_read == -1) {
                        fprintf(stderr, "minitar_list read error: %d\n", errno);
                        return -1;
                }

                if(queue_head == NULL) {
                        queue_tail = queue_head = minitar_dir_queue_enqueue(NULL, &buffer_entry);
                } else {
                        queue_tail = minitar_dir_queue_enqueue(queue_tail, &buffer_entry);
                }
        }

        

        extract_write_data(queue_head, dest_path, fd_src_tar);

        close(fd_src_tar);

        return 0;
}

static ssize_t create_write_single_file(const char* src_file, int fd_dest_tar, ssize_t file_offset)
{
        int fd = open(src_file, O_RDONLY);
        if(fd == -1) {
                fprintf(stderr, "create_write_single_file open error: %d\n", errno);
                return -1;
        }

        char buffer[FILE_CHUNK];
        ssize_t bytes_read = 0;
        ssize_t bytes_write = 0;
        do {
                bytes_read = read(fd, buffer, FILE_CHUNK);
                if(bytes_read == -1) {
                        fprintf(stderr, "create_write_single_file read error: %d\n", errno);
                        return -1; 
                }
                if(bytes_read == 0) {
                        break;
                }

                bytes_write = write(fd_dest_tar, buffer, bytes_read);
                if(bytes_write == -1) {
                        fprintf(stderr, "create_write_single_file write error: %d\n", errno);
                        return -1; 
                }

        } while (true);

        close(fd);
        return bytes_write;
}

static ssize_t create_write_data(struct minitar_dir_stack* stack, const char* src_path, int fd_dest_tar, ssize_t file_offset)
{
        char buffer_file[FILE_NAME_MAX];
        char buffer_fullpath_file[FILENAME_MAX];
        size_t src_path_length = strlen(src_path);
        ssize_t total_bytes_write = 0;
        bool has_ending_seperator = false;
        if(src_path[src_path_length - 1] == '/') {
                has_ending_seperator = true;
        }

        while(stack) {
                strcpy(buffer_fullpath_file, src_path);

                if(!has_ending_seperator) {
                        strcat(buffer_fullpath_file, "/");
                }

                strcat(buffer_fullpath_file, stack->entry->name);
                stack->entry->start = file_offset;
                stack->entry->length = create_write_single_file(buffer_fullpath_file, fd_dest_tar, file_offset);
                file_offset += stack->entry->length;
                total_bytes_write += stack->entry->length;
                stack = stack->next;
        }

        return total_bytes_write;
}

static ssize_t create_write_header_info(struct minitar_dir_stack* stack, int fd_dest_tar) 
{
        lseek(fd_dest_tar, 8, SEEK_SET);

        uint64_t start, length;
        ssize_t bytes_write = 0;
        struct minitar_header_entry buffer_entry;
        while(stack) {
                stack = minitar_dir_stack_pop(stack, &buffer_entry);
                lseek(fd_dest_tar, FILE_NAME_MAX, SEEK_CUR);
                bytes_write += write(fd_dest_tar, &buffer_entry.start, sizeof(uint64_t));
                bytes_write += write(fd_dest_tar, &buffer_entry.length, sizeof(uint64_t));
        }
}

static ssize_t create_write_header(struct minitar_dir_stack* stack, uint64_t number_elements, int fd_dest_tar)
{
        struct minitar_header_entry buffer_entry;
        ssize_t total_bytes_write = 0;
        ssize_t bytes_write_name = 0;
        ssize_t bytes_write_info = 0;
        ssize_t bytes_write_number_elements = 0;

        bytes_write_number_elements = write(fd_dest_tar, &number_elements, sizeof(uint64_t));
        if(bytes_write_number_elements == -1) {
                fprintf(stderr, "create_write_header write error: %d\n", errno);
                return -1; 
        }

        total_bytes_write = bytes_write_number_elements;

        while(stack) {
                stack = minitar_dir_stack_next(stack, &buffer_entry);
        
                bytes_write_name = write(fd_dest_tar, buffer_entry.name, FILE_NAME_MAX);
                if(bytes_write_name == -1) {
                        fprintf(stderr, "create_write_header write error: %d\n", errno);
                        return -1; 
                }

                bytes_write_info = write(fd_dest_tar, buffer_entry.name, FILE_HEADER_INFO_BYTES);
                if(bytes_write_info == -1) {
                        fprintf(stderr, "create_write_header write error: %d\n", errno);
                        return -1; 
                }

                total_bytes_write += bytes_write_name + bytes_write_info;
        }

        return total_bytes_write;
}

int minitar_create(const char *src_path, const char *dest_tar)
{
        DIR* dir = opendir(src_path);
        struct dirent* direntry;
        if(dir == NULL) {
                fprintf(stderr, "minitar_create opendir error: %d\n", errno);
                return -1;
        }

        struct minitar_dir_stack* stack = NULL;
        struct minitar_header_entry buffer_entry;
        uint64_t number_elements = 0;
        do {

                errno = 0;
                direntry = readdir(dir);
                if(direntry == NULL && errno != 0) {
                        fprintf(stderr, "minitar_create readdir error: %d\n", errno);
                        return -1;
                }
                if(direntry == NULL && errno == 0) {
                        break;
                }

                if(direntry->d_type != DT_DIR) {
                        minitar_header_entry_init(&buffer_entry,direntry->d_name);
                        stack = minitar_dir_stack_push(stack, &buffer_entry);
                        number_elements++;
                }

        } while(true);

        closedir(dir);

        int fd_dest_tar = open(dest_tar, O_CREAT | O_WRONLY, 0664);
        if(fd_dest_tar == -1) {
                fprintf(stderr, "minitar_create open error: %d\n", errno);
                return -1;
        }

        struct minitar_dir_stack* copy_ptr_stack = stack;

        ssize_t offset = create_write_header(copy_ptr_stack, number_elements, fd_dest_tar);
        if(offset == -1) {
                return -1;
        }
        copy_ptr_stack = stack;
        ssize_t data_section_size = create_write_data(copy_ptr_stack, src_path, fd_dest_tar, offset);
        if(data_section_size == -1) {
                return -1;
        }

        create_write_header_info(stack, fd_dest_tar);

        close(fd_dest_tar);

        return 0;
}



int minitar_list(const char *src_tar)
{
        int fd_src_tar = open(src_tar, O_RDONLY);
        if(fd_src_tar == -1) {
                fprintf(stderr, "minitar_create open error: %d\n", errno);
                return -1;
        }

        char buffer_filename[FILE_NAME_MAX];
        uint64_t number_elements;
        ssize_t number_elements_bytes_read = read(fd_src_tar, &number_elements, sizeof(uint64_t));
        if(number_elements_bytes_read == -1) {
                fprintf(stderr, "minitar_list read error: %d\n", errno);
                return -1;
        }

        for(uint64_t i = 0; i < number_elements; i++) {
                
                ssize_t filename_bytes_read = read(fd_src_tar, buffer_filename, FILE_NAME_MAX);
                if(filename_bytes_read == -1) {
                        fprintf(stderr, "minitar_list read error: %d\n", errno);
                        return -1;
                }

                lseek(fd_src_tar, FILE_HEADER_INFO_BYTES, SEEK_CUR);

                printf("%s\n", buffer_filename);
        }

        close(fd_src_tar);

        return 0;
}