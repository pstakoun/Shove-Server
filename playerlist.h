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

int playersToString(char *result);

void printPlayers();
