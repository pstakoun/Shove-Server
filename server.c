#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "server.h"

const Location center = { GAME_SIZE / 2, GAME_SIZE / 2 };

struct timespec startTime;

int main(int argc, char **argv)
{
	int port = 7000;
	int numReceived = 0;
	char buffer[1024];

	struct sockaddr_in address;
	socklen_t addressSize;
	struct sockaddr_storage storage;

	int udpSocket = socket(AF_INET, SOCK_DGRAM, 0);

	printf("udpSocket = %i\n", udpSocket);

	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = htonl(INADDR_ANY);

	memset(address.sin_zero, '\0', sizeof address.sin_zero);

	bind(udpSocket, (struct sockaddr *) &address, sizeof(address));

	addressSize = sizeof storage;

	srand(time(NULL)); // Initialize random number generator

	clock_gettime(CLOCK_MONOTONIC, &startTime);

	printf("Server started on port %i\n", port);

	// Receive any incoming packets
	while (1)
	{
		printf("Waiting for packet %i\n", numReceived++);

		memset(buffer, '\0', sizeof buffer);

		// Get packet from client
		int numBytes = recvfrom(udpSocket, buffer, sizeof buffer, 0, (struct sockaddr *) &storage, &addressSize);

		printf("Input: %s\nnumBytes = %i\n", buffer, numBytes);

		handleInput(buffer);

		numBytes = getOutput(buffer, sizeof buffer);

		printf("Output: %s\nnumBytes = %i\n", buffer, numBytes);

		// Send packet to client
		sendto(udpSocket, buffer, numBytes, 0, (struct sockaddr *) &storage, addressSize);
	}

	return 0;
}

void handleInput(char *input)
{
	char displayName[16];
	memset(displayName, '\0', sizeof displayName);
	Location touch = { NAN, NAN };
	int touched = 1;
	long currentTime = timeFrom(startTime);

	char *ptr;
	ptr = strtok(input, " ");
	while (ptr != NULL)
	{
		if (strlen(displayName) == 0) {
			strcpy(displayName, ptr);
		} else if (isnan(touch.x)) {
			if (strcmp(ptr, "null") == 0) {
				touch.x = 0;
				touched = 0;
				break;
			} else {
				touch.x = atof(ptr);
			}
		} else if (isnan(touch.y)) {
			if (strcmp(ptr, "null") == 0) {
				touch.y = 0;
				touched = 0;
			} else {
				touch.y = atof(ptr);
			}
			break;
		}
		ptr = strtok(NULL, " ");
	}

	printf("Touch: %s %f %f %lu\n", displayName, touch.x, touch.y, currentTime);

	Player *foundPlayer = getPlayer(displayName);
	if (foundPlayer == NULL) {
		Player newPlayer;
		strcpy(newPlayer.displayName, displayName);
		resetPlayer(&newPlayer, currentTime);

		// Assign random color to player
		int tempColor = rand() % NUM_COLORS;
		newPlayer.color = (tempColor + 1) % NUM_COLORS;
		while (usedColors[newPlayer.color] && newPlayer.color != tempColor) {
			newPlayer.color = (newPlayer.color + 1) % NUM_COLORS;
		}
		usedColors[newPlayer.color] = 1;

		// Add player to linked list
		addPlayer(newPlayer);
	} else {
		foundPlayer->touchTime = currentTime;
		if (touched) {
			foundPlayer->touch = touch;
		} else {
			foundPlayer->touch = foundPlayer->location;
		}
	}
	
	printPlayers();
}

int getOutput(char *output, size_t outputSize)
{
	memset(output, '\0', outputSize);

	updatePlayerLocations(timeFrom(startTime));

	int size = playersToString(output);

	return size;
}

long timeFrom(struct timespec start)
{
	struct timespec tt;
	clock_gettime(CLOCK_MONOTONIC, &tt);
	return (tt.tv_sec + ((double)tt.tv_nsec)/1000000000L) * 1000 - (start.tv_sec + start.tv_nsec/1000000000L) * 1000;
}
