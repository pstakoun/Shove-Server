#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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

void updatePlayerLocations(long currentTime)
{
    Node *current = head;
    while (current != NULL) {
        long timeDiff = currentTime - current->value.touchTime;
        float x1 = current->value.startX;
        float x2 = current->value.touchX;
        float y1 = current->value.startY;
        float y2 = current->value.touchY;
        float dist = getDistance(x1, x2, y1, y2);
        printf("%f\n", dist);
        if (isnan(dist)) {
            current->value.newX = x1;
            current->value.newY = y1;
        } else {
            current->value.newX = x1 + (x2 - x1) * (timeDiff / (dist * 10));
            current->value.newY = y1 + (y2 - y1) * (timeDiff / (dist * 10));
        }
        current = current->next;
    }
}

int playersToString(char *result)
{
	Node *current = head;
    char temp[32];
    while (current != NULL) {
        sprintf(temp, "%s ", current->value.displayName);
        strcat(result, temp);
        sprintf(temp, "%f ", current->value.newX);
        strcat(result, temp);
        sprintf(temp, "%f", current->value.newY);
        strcat(result, temp);
        if (current->next != NULL) {
            strcat(result, ",");
        }
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

float getDistance(float x1, float x2, float y1, float y2)
{
    if ((x2 - x1) == 0 && (y2 - y1) == 0) {
        return NAN;
    }
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}
