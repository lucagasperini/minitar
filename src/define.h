#pragma once

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define FILE_CHUNK 1024 * 1024 * 4
#define FILE_NAME_MAX 256
#define FILE_HEADER_INFO_BYTES 16

#define _BSD_SOURCE
