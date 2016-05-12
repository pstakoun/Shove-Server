#include "player.h"

typedef struct node {
	Player value;
  	struct node *next;
} Node;

Node *head;

void initPlayerList(Player player);

int countPlayers();

void addPlayer(Player player);

Player *getPlayer(char *displayName);

void updatePlayerLocations();

int playersToString(char *result);

void printPlayers();

float getDistance(float x1, float x2, float y1, float y2);
