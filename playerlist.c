#include <stdio.h>

#include "playerlist.h"

void initPlayerList(Player player)
{
	head = malloc(sizeof(Node));
	head->value = player;
	head->next = NULL;
}

int countPlayers()
{
	Node *current = head;
	int count = 0;
    while (current != NULL) {
    	++count;
        current = current->next;
    }
    return count;
}

void addPlayer(Player player)
{
    printf("Adding player: %s %f %f %f %f %lu\n", player.displayName, player.startX, player.startY, player.touchX, player.touchY, player.touchTime);

    if (countPlayers() == 0) {
        initPlayerList(player);
        return;
    }

    Node *current = head;
    while (current->next != NULL) {
        current = current->next;
    }

    current->next = malloc(sizeof(Node));
    current->next->value = player;
    current->next->next = NULL;
}

Player *getPlayer(char *displayName)
{
	Node *current = head;
    while (current != NULL) {
        if (strcmp(displayName, current->value.displayName) == 0) {
        	return &current->value;
        }
        current = current->next;
    }
    return NULL;
}

int playersToString(char *result)
{
	Node *current = head;
    char temp[32];
    while (current != NULL) {
        sprintf(temp, "%s ", current->value.displayName);
        strcat(result, temp);
        sprintf(temp, "%f ", current->value.touchX); // TODO calculate x value
        strcat(result, temp);
        sprintf(temp, "%f ", current->value.touchY); // TODO calculate y value
        strcat(result, temp);
        current = current->next;
    }
    return strlen(result);
}

void printPlayers()
{
    printf("Printing player list:\n");
    Node *current = head;
    while (current != NULL) {
        printf("%s %f %f %f %f %lu\n", current->value.displayName, current->value.startX, current->value.startY, current->value.touchX, current->value.touchY, current->value.touchTime);
        current = current->next;
    }
}
