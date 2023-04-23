#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tar.h"

#define HELP_STR "How to use it" \
                 "x/extract <src_tar> <dest_path>" \
                 "c/create <src_path> <dest_tar>" \
                 "l/list <src_tar>" \
                 "i/info <src_tar>"

int main(int argc, char **argv)
{
        if(argc < 2) {
                perror("Cannot use minitar without arguments!");
                perror(HELP_STR);
                exit(1);
        }
        if(strcmp(argv[1],"x") == 0 || strcmp(argv[1],"extract") == 0) {
                if(argc < 3) {
                        perror("Invalid arguments provided to minitar extract!");
                        perror(HELP_STR);
                }
                return minitar_extract(argv[2], argv[3]);

        } else if (strcmp(argv[1],"c") == 0 || strcmp(argv[1],"create") == 0) {
                if(argc < 3) {
                        perror("Invalid arguments provided to minitar create!");
                        perror(HELP_STR);
                }
                return minitar_create(argv[2], argv[3]);

        } else if (strcmp(argv[1],"l") == 0 || strcmp(argv[1],"list") == 0) {
                return minitar_list(argv[2]);
        } else {
                perror("Invalid arguments provided to minitar!");
                perror(HELP_STR);
                exit(1);
        }
}