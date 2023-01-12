#ifndef COMMANDS_COMPUTATION_H
#define COMMANDS_COMPUTATION_H

#include "./structs.h"
#include "./finder.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>

struct proc_commands computeCommands(struct limits, uint*, uint*, uint*, uint8_t*, uint, uint8_t*, int);

#endif
