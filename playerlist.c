#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "server.h"

// Initialize the Player list by setting head to the given Player
void initPlayerList(Player player)
{
	head = malloc(sizeof(Node));
	head->value = player;
	head->next = NULL;
}

// Return the number of Players
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

// Reset the given Player's properties
void resetPlayer(Player *player, long currentTime)
{
	player->location = randomLocation();
	player->touch = player->location;
	player->touchTime = currentTime;
	player->lastMoveTime = 0;
	Location emptyLocation = { NAN, NAN };
	player->collisionTarget = emptyLocation;
	player->collisionTime = NAN;
}

// Add the given Player
void addPlayer(Player player)
{
	printf("Adding player: %i %s %f %f %f %f %lu\n", player.id, player.displayName, player.location.x, player.location.y, player.touch.x, player.touch.y, player.touchTime);

	// Initialize list with Player if empty
	if (countPlayers() == 0) {
		initPlayerList(player);
		return;
	}

	// Get last Node in list
	Node *current = head;
	while (current->next != NULL) {
		current = current->next;
	}

	// Set last Node's child to Player
	current->next = malloc(sizeof(Node));
	current->next->value = player;
	current->next->next = NULL;
}

// Remove the given Player
void removePlayer(Player player)
{
	Node *current = head;
	Node *previous = NULL;

	while(current != NULL) {
		if (player.id == current->value.id) {
			// Clear the color used by the Player
			usedColors[current->value.color]--;
			// Remove Player
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

// Get Player by id
Player *getPlayer(int id)
{
	Node *current = head;
	while (current != NULL) {
		if (id == current->value.id) {
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
		// Remove Players if no packets were sent for 1000ms
		if (currentTime - current->value.touchTime > 1000) {
			removePlayer(current->value);
		// Reset Players if out of bounds
		} else if (getDistance(current->value.location, center) > GAME_SIZE / 2 - PLAYER_RADIUS) {
			resetPlayer(&(current->value), currentTime);
		}
		current = current->next;
	}

	current = head;
	while (current != NULL) {
		float dist;
		// Handle movement if collided
		if (!isnan(current->value.collisionTarget.x) && !isnan(current->value.collisionTarget.y)) {
			dist = getDistance(current->value.location, current->value.collisionTarget);
			// Reset if target reached
			if (dist < SPEED * 3) {
				current->value.collisionTarget.x = NAN;
				current->value.collisionTarget.y = NAN;
				current->value.collisionTime = NAN;
			} else if (currentTime - current->value.lastMoveTime > MOVE_DELAY) {
				current->value.location.x = current->value.location.x + SPEED * 2.5f * (current->value.collisionTarget.x - current->value.location.x) / dist;
				current->value.location.y = current->value.location.y + SPEED * 2.5f * (current->value.collisionTarget.y - current->value.location.y) / dist;
				current->value.lastMoveTime = currentTime;
			}
		// Move to touch location
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
			// Handle collisions
			if (current->value.id != other->value.id &&
			getDistance(current->value.location, other->value.location) < PLAYER_RADIUS * 2 &&
			(isnan(current->value.collisionTime) || (!isnan(current->value.collisionTime) && currentTime - current->value.collisionTime > MOVE_DELAY * 3))) {
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
	// Log collision
	printf("Collision: %i %s %i %s\n", p1->id, p1->displayName, p2->id, p2->displayName);
	float p1CollisionDist;
	float p2CollisionDist;
	// Handle collision if second Player has touch
	if (p2->touch.x != p2->location.x && p2->touch.y != p2->location.y) {
		p1CollisionDist = getDistance(p1->location, p2->touch);
		p1->collisionTarget.x = p1->location.x + COLLISION_DISTANCE * (p2->touch.x - p1->location.x) / p1CollisionDist;
		p1->collisionTarget.y = p1->location.y + COLLISION_DISTANCE * (p2->touch.y - p1->location.y) / p1CollisionDist;
	// Handle collision if second Player has collision target
	} else if (!isnan(p2->collisionTarget.x) && !isnan(p2->collisionTarget.y)) {
		p1CollisionDist = getDistance(p1->location, p2->collisionTarget);
		p1->collisionTarget.x = p1->location.x + COLLISION_DISTANCE * (p2->collisionTarget.x - p1->location.x) / p1CollisionDist;
		p1->collisionTarget.y = p1->location.y + COLLISION_DISTANCE * (p2->collisionTarget.y - p1->location.y) / p1CollisionDist;
	}
	// Handle collision if first Player has touch
	if (p1->touch.x != p1->location.x && p1->touch.y != p1->location.y) {
		p2CollisionDist = getDistance(p2->location, p1->touch);
		p2->collisionTarget.x = p2->location.x + COLLISION_DISTANCE * (p1->touch.x - p2->location.x) / p2CollisionDist;
		p2->collisionTarget.y = p2->location.y + COLLISION_DISTANCE * (p1->touch.y - p2->location.y) / p2CollisionDist;
	// Handle collision if first Player has collision target
	} else if (!isnan(p1->collisionTarget.x) && !isnan(p1->collisionTarget.y)) {
		p2CollisionDist = getDistance(p2->location, p1->collisionTarget);
		p2->collisionTarget.x = p2->location.x + COLLISION_DISTANCE * (p1->collisionTarget.x - p2->location.x) / p2CollisionDist;
		p2->collisionTarget.y = p2->location.y + COLLISION_DISTANCE * (p1->collisionTarget.y - p2->location.y) / p2CollisionDist;
	}
}

// Store player information in result
int playersToString(char *result)
{
	Node *current = head;
	char temp[32];
	while (current != NULL) {
		// Add Player properties to result
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

// Print Player information to console
void printPlayers()
{
	printf("Printing player list:\n");
	Node *current = head;
	while (current != NULL) {
		printf("%i %s %f %f %f %f %lu\n", current->value.id, current->value.displayName, current->value.location.x, current->value.location.y, current->value.touch.x, current->value.touch.y, current->value.touchTime);
		current = current->next;
	}
}

// Return random Location in game
Location randomLocation()
{
	Location newLocation = { rand() % GAME_SIZE, rand() % GAME_SIZE };
	// Ensure Location is inside circular bounds
	while (getDistance(newLocation, center) > GAME_SIZE / 2 - PLAYER_RADIUS * 2) {
		newLocation.x = rand() % GAME_SIZE;
		newLocation.y = rand() % GAME_SIZE;
	}
	return newLocation;
}

// Return distance between two Locations
float getDistance(Location loc1, Location loc2)
{
	return sqrt((loc2.x - loc1.x) * (loc2.x - loc1.x) + (loc2.y - loc1.y) * (loc2.y - loc1.y));
}
