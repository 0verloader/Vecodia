#ifndef SUFFIX_ARRAY_H
#define SUFFIX_ARRAY_H

#include <stdio.h>
#include <sys/mman.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include "./structs.h"

uint* buildSuffixArray(uint8_t*, uint);
uint* buildRankArray(uint*, uint);
uint* buildLCPArray(uint*, uint*, uint8_t*, uint);

#endif