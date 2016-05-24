#include "player.h"

typedef struct node {
	Player value;
	struct node *next;
} Node;

Node *head;

void initPlayerList(Player player);

int countPlayers();

void resetPlayer(Player *player, long currentTime);

void addPlayer(Player player);

void removePlayer(Player player);

Player *getPlayer(int id);

void updatePlayerLocations();

void handleCollision(Player *p1, Player *p2);

int playersToString(char *result);

void printPlayers();

Location randomLocation();

float getDistance(Location loc1, Location loc2);
