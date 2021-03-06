#include <sys/socket.h>

#include "location.h"

typedef struct Players
{
	int id;
	char displayName[17];
	Location location;
	Location touch;
	long touchTime;
	long lastMoveTime;
	Location collisionTarget;
	long collisionTime;
	int color;
} Player;
