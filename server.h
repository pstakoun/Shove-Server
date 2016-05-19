#include "playerlist.h"

#define NUM_COLORS 4
#define GAME_SIZE 400
#define PLAYER_RADIUS 15
#define MOVE_DELAY 20
#define SPEED 5.0f
#define COLLISION_DISTANCE 125.0f

int usedColors[NUM_COLORS];

void handleInput(char *input);

int getOutput(char *output, size_t outputSize);

long timeFrom(struct timespec start);

void updateLocation(Player *Player);
