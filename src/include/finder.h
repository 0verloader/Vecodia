#ifndef FINDER_H
#define FINDER_H

#include "./structs.h"
#include <stdio.h>
#include <stdlib.h>

struct pullback_point finderInPlace(uint, uint*, uint*, uint*, struct limits);
struct pullback_point finderOutOfPlace(uint, uint*, uint*, uint*, struct limits);

#endif