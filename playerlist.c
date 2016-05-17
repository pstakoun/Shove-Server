#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "playerlist.h"

const float SPEED = 5.0f;

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
	printf("Adding player: %s %f %f %f %f %lu\n", player.displayName, player.location.x, player.location.y, player.touch.x, player.touch.y, player.touchTime);

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
		// TODO remove if timeDiff too big
		float dist = getDistance(current->value.location, current->value.touch);
		if (dist >= SPEED) {
			current->value.location.x = current->value.location.x + SPEED * (current->value.touch.x - current->value.location.x) * (timeDiff + 1) / dist;
			current->value.location.y = current->value.location.y + SPEED * (current->value.touch.y - current->value.location.y) * (timeDiff + 1) / dist;
		}
		Node *other = head;
		while (other != NULL) {
			if (current->value.displayName != other->value.displayName &&
			getDistance(current->value.location, other->value.location) < 20) {
				handleCollision(&(current->value), &(other->value));
			}
			other = other->next;
		}
		current = current->next;
	}
}

void handleCollision(Player *p1, Player *p2)
{
	printf("Collision: %s %s\n", p1->displayName, p2->displayName);
}

int playersToString(char *result)
{
	Node *current = head;
	char temp[32];
	while (current != NULL) {
		sprintf(temp, "%s ", current->value.displayName);
		strcat(result, temp);
		sprintf(temp, "%f ", current->value.location.x);
		strcat(result, temp);
		sprintf(temp, "%f", current->value.location.y);
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
		printf("%s %f %f %f %f %lu\n", current->value.displayName, current->value.location.x, current->value.location.y, current->value.touch.x, current->value.touch.y, current->value.touchTime);
		current = current->next;
	}
}

float getDistance(Location loc1, Location loc2)
{
	return sqrt((loc2.x - loc1.x) * (loc2.x - loc1.x) + (loc2.y - loc1.y) * (loc2.y - loc1.y));
}
