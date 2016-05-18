#include "playerlist.h"

#define NUM_COLORS 4
#define GAME_SIZE 500
#define PLAYER_RADIUS 15
#define MOVE_DELAY 100
#define SPEED 15.0f
#define COLLISION_DISTANCE 100.0f

int usedColors[NUM_COLORS];

void handleInput(char *input);

int getOutput(char *output, size_t outputSize);

long timeFrom(struct timespec start);

void updateLocation(Player *Player);
