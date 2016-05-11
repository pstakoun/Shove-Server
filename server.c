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

struct timespec startTime;

int main(int argc, char **argv)
{
  int port = 7000;
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
		printf("Waiting for packet\n");

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
  float startX = NAN;
  float startY = NAN;
	float touchX = NAN;
	float touchY = NAN;
  long touchTime = timeFrom(startTime);

	char *ptr;
	ptr = strtok(input, " ");
	while (ptr != NULL)
	{
		if (strlen(displayName) == 0) {
			strcpy(displayName, ptr);
		} else if (isnan(startX)) {
      if (strcmp(ptr, "null") == 0) {
        // TODO assign random
      } else {
        startX = atof(ptr);
      }
    } else if (isnan(startY)) {
      if (strcmp(ptr, "null") == 0) {
        // TODO assign random
      } else {
        startY = atof(ptr);
      }
    } else if (isnan(touchX)) {
      if (strcmp(ptr, "null") == 0) {
        // TODO assign random
      } else {
        touchX = atof(ptr);
      }
    } else if (isnan(touchY)) {
      if (strcmp(ptr, "null") == 0) {
        // TODO assign random
      } else {
        touchY = atof(ptr);
      }
    }
  	ptr = strtok(NULL, " ");
	}
	printf("Touch: %s %f %f %f %f %lu\n", displayName, startX, startY, touchX, touchY, touchTime);

	Player *foundPlayer = getPlayer(displayName);
	if (foundPlayer == NULL) {
		Player newPlayer;
		strcpy(newPlayer.displayName, displayName);
    newPlayer.startX = startX;
    newPlayer.startY = startY;
		newPlayer.touchX = touchX;
		newPlayer.touchY = touchY;
		newPlayer.touchTime = touchTime;
		addPlayer(newPlayer);
	} else if (1) { // TODO if time is greater than max time?
    foundPlayer->startX = startX;
    foundPlayer->startY = startY;
		foundPlayer->touchX = touchX;
		foundPlayer->touchY = touchY;
		foundPlayer->touchTime = touchTime;
	}

	printPlayers();
}

int getOutput(char *output, size_t outputSize)
{
	memset(output, '\0', outputSize);

	int size = playersToString(output);

	return size;
}

long timeFrom(struct timespec start)
{
	struct timespec tt;
	clock_gettime(CLOCK_MONOTONIC, &tt);
	return (tt.tv_sec + ((double)tt.tv_nsec)/1000000000L) * 1000 - (start.tv_sec + start.tv_nsec/1000000000L) * 1000;
}
