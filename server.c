#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "playerlist.h"

void handleInput(char *input, size_t inputSize);

int getOutput(char *output, size_t outputSize);

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

  	printf("Server started on port %i\n", port);

	srand(1); // Initialize random number generator

  	// Receive any incoming packets
	while (1)
	{
		printf("Waiting for packet\n");

		memset(buffer, '\0', sizeof buffer);

		// Get packet from client
		int numBytes = recvfrom(udpSocket, buffer, 1024, 0, (struct sockaddr *) &storage, &addressSize);

		printf("Input: %s\nnumBytes = %i\n", buffer, numBytes);

		handleInput(buffer, numBytes);

		numBytes = getOutput(buffer, sizeof buffer);

		printf("Output: %s\nnumBytes = %i\n", buffer, numBytes);

		// Send packet to client
		sendto(udpSocket, buffer, numBytes, 0, (struct sockaddr *) &storage, addressSize);
	}

	return 0;
}

void handleInput(char *input, size_t inputSize)
{
	Player player = {"", NAN, NAN};
	char *ptr;
  	ptr = strtok(input, " ");
  	while (ptr != NULL)
  	{
  		if (strlen(player.displayName) == 0) {
  			strcpy(player.displayName, ptr);
  		} else if (isnan(player.x)) {
  			if (strcmp(ptr, "null") == 0) {
  				player.x = rand() % 100;
  			} else {
	  			player.x = atof(ptr);
	  		}
  		} else if (isnan(player.y)) {
  			if (strcmp(ptr, "null") == 0) {
  				player.y = rand() % 100;
  			} else {
	  			player.y = atof(ptr);
	  		}
  		}
    	ptr = strtok(NULL, " ");
  	}
  	printf("Adding player: %s %f %f\n", player.displayName, player.x, player.y);
  	if (countPlayers() == 0) {
  		initPlayerList(player);
  	} else {
  		Player *foundPlayer = getPlayer(player.displayName);
  		if (foundPlayer == NULL) {
  			addPlayer(player);
  		} else {
  			*foundPlayer = player;
  		}
  	}
  	printf("Printing player list:\n");
  	printPlayers();
}

int getOutput(char *output, size_t outputSize)
{
	memset(output, '\0', outputSize);

	int size = playersToString(output);

	return size;
}
