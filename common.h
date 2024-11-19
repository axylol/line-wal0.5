#pragma once
#include <stdio.h>
#include <stdlib.h>

#define panic(fmt, args...) printf(fmt, ## args); exit(0);