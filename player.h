#include <sys/socket.h>

#include "location.h"

typedef struct Players
{
	char displayName[16];
	Location location;
	Location touch;
	long touchTime;
	long lastMoveTime;
	Location collisionTarget;
	long collisionTime;
	int color;
} Player;
