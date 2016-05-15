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

void handleCollision(Player *p1, Player *p2);

int playersToString(char *result);

void printPlayers();

float getDistance(Location loc1, Location loc2);
