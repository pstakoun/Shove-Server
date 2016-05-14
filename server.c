#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "playerlist.h"

void handleInput(char *input);

int getOutput(char *output, size_t outputSize);

long timeFrom(struct timespec start);

void updateLocation(Player *Player);

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

	srand(1); // Initialize random number generator

	clock_gettime(CLOCK_MONOTONIC, &startTime);

	printf("Server started on port %i\n", port);

	// Receive any incoming packets
	while (1)
	{
		printf("Waiting for packet %i\n", numReceived++);

		memset(buffer, '\0', sizeof buffer);

		// Get packet from client
		int numBytes = recvfrom(udpSocket, buffer, 1024, 0, (struct sockaddr *) &storage, &addressSize);

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
	Location start = { NAN, NAN };
	Location touch = { NAN, NAN };
	long touchTime = timeFrom(startTime);

	char *ptr;
	ptr = strtok(input, " ");
	while (ptr != NULL)
	{
		if (strlen(displayName) == 0) {
			strcpy(displayName, ptr);
		} else if (isnan(start.x)) {
			if (strcmp(ptr, "null") == 0) {
				start.x = rand() % 500;
			} else {
				start.x = atof(ptr);
			}
		} else if (isnan(start.y)) {
			if (strcmp(ptr, "null") == 0) {
				start.y = rand() % 500;
			} else {
				start.y = atof(ptr);
			}
		} else if (isnan(touch.x)) {
			if (strcmp(ptr, "null") == 0) {
				touch.x = start.x;
			} else {
				touch.x = atof(ptr);
			}
		} else if (isnan(touch.y)) {
			if (strcmp(ptr, "null") == 0) {
				touch.y = start.y;
			} else {
				touch.y = atof(ptr);
			}
		}
		ptr = strtok(NULL, " ");
	}
	printf("Touch: %s %f %f %f %f %lu\n", displayName, start.x, start.y, touch.x, touch.y, touchTime);
	
	Player *foundPlayer = getPlayer(displayName);
	if (foundPlayer == NULL) {
		Player newPlayer;
		strcpy(newPlayer.displayName, displayName);
		newPlayer.start.x = start.x;
		newPlayer.start.y = start.y;
		newPlayer.touch.x = touch.x;
		newPlayer.touch.y = touch.y;
		newPlayer.touchTime = touchTime;
		addPlayer(newPlayer);
	} else if (abs(foundPlayer->current.x - foundPlayer->touch.x) < 10 &&
	abs(foundPlayer->current.y - foundPlayer->touch.y) < 10) {
		foundPlayer->start.x = foundPlayer->touch.x;
		foundPlayer->start.y = foundPlayer->touch.y;
		foundPlayer->touch.x = touch.x;
		foundPlayer->touch.y = touch.y;
		foundPlayer->touchTime = touchTime;
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
