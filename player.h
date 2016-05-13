#include "location.h"

typedef struct Players
{
	char displayName[16];
	Location start;
	Location touch;
	Location current;
	long touchTime;
} Player;
