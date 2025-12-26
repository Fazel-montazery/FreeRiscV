#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

/* Read a file into the provided buffer
 * Append a null char at the end (not in binary mode)
 * Return a negative number on fail
 * Return the size of the file if (buf == NULL)
 * Return the bytes read on success
 */
int64_t readFileToBuf(const char* path, uint8_t* buf, const size_t bufsize, const bool bin);
