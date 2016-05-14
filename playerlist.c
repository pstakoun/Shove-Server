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
	printf("Adding player: %s %f %f %f %f %lu\n", player.displayName, player.start.x, player.start.y, player.touch.x, player.touch.y, player.touchTime);

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
		float x1 = current->value.start.x;
		float x2 = current->value.touch.x;
		float y1 = current->value.start.y;
		float y2 = current->value.touch.y;
		float dist = getDistance(x1, x2, y1, y2);
		printf("%f\n", dist);
		if (dist == 0) {
			current->value.current.x = x1;
			current->value.current.y = y1;
		} else {
			current->value.current.x = x1 + (x2 - x1) * (timeDiff / (dist * 10));
			current->value.current.y = y1 + (y2 - y1) * (timeDiff / (dist * 10));
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
		sprintf(temp, "%f ", current->value.current.x);
		strcat(result, temp);
		sprintf(temp, "%f", current->value.current.y);
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
		printf("%s %f %f %f %f %lu\n", current->value.displayName, current->value.start.x, current->value.start.y, current->value.touch.x, current->value.touch.y, current->value.touchTime);
		current = current->next;
	}
}

float getDistance(float x1, float x2, float y1, float y2)
{
	return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}
