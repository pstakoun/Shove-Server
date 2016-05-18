#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "server.h"

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

void removePlayer(Player player)
{
	Node *current = head;
	Node *previous = NULL;

	while(current != NULL) {
		if (strcmp(player.displayName, current->value.displayName) == 0) {
			usedColors[current->value.color] = 0;
			if (current == head) {
				head = head->next;
			} else {
				previous->next = current->next;
			}
			free(current);
			break;
		}
		previous = current;
		current = current->next;
	}
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
		if (currentTime - current->value.touchTime > 500) {
			removePlayer(current->value);
		}
		current = current->next;
	}

	current = head;
	while (current != NULL) {
		float dist;
		if (!isnan(current->value.collisionTarget.x) && !isnan(current->value.collisionTarget.y)) {
			dist = getDistance(current->value.location, current->value.collisionTarget);
			if (dist < SPEED * 1.5) {
				current->value.collisionTarget.x = NAN;
				current->value.collisionTarget.y = NAN;
				current->value.collisionTime = NAN;
			} else if (currentTime - current->value.lastMoveTime > MOVE_DELAY) {
				current->value.location.x = current->value.location.x + SPEED * 3 * (current->value.collisionTarget.x - current->value.location.x) / dist;
				current->value.location.y = current->value.location.y + SPEED * 3 * (current->value.collisionTarget.y - current->value.location.y) / dist;
				current->value.lastMoveTime = currentTime;
			}
		} else {
			dist = getDistance(current->value.location, current->value.touch);
			if (dist >= SPEED && currentTime - current->value.lastMoveTime > MOVE_DELAY) {
				current->value.location.x = current->value.location.x + SPEED * (current->value.touch.x - current->value.location.x) / dist;
				current->value.location.y = current->value.location.y + SPEED * (current->value.touch.y - current->value.location.y) / dist;
				current->value.lastMoveTime = currentTime;
			}
		}

		Node *other = head;
		while (other != NULL) {
			if (current->value.displayName != other->value.displayName &&
			getDistance(current->value.location, other->value.location) < PLAYER_RADIUS * 2 &&
			!isnan(current->value.collisionTime) && currentTime - current->value.collisionTime > MOVE_DELAY * 2) {
				handleCollision(&(current->value), &(other->value));
				current->value.collisionTime = currentTime;
				other->value.collisionTime = currentTime;
			}
			other = other->next;
		}
		current = current->next;
	}
}

void handleCollision(Player *p1, Player *p2)
{
	printf("Collision: %s %s\n", p1->displayName, p2->displayName);
	float p1CollisionDist = getDistance(p1->location, p2->touch);
	float p2CollisionDist = getDistance(p2->location, p1->touch);
	p1->collisionTarget.x = p1->location.x + COLLISION_DISTANCE * (p2->touch.x - p1->location.x) / p1CollisionDist;
	p1->collisionTarget.y = p1->location.y + COLLISION_DISTANCE * (p2->touch.y - p1->location.y) / p1CollisionDist;
	p2->collisionTarget.x = p2->location.x + COLLISION_DISTANCE * (p1->touch.x - p2->location.x) / p2CollisionDist;
	p2->collisionTarget.y = p2->location.y + COLLISION_DISTANCE * (p1->touch.y - p2->location.y) / p2CollisionDist;
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
		sprintf(temp, "%f ", current->value.location.y);
		strcat(result, temp);
		sprintf(temp, "%i", current->value.color);
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
