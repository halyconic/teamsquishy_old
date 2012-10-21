/*
 * main.cpp
 *
 *  Created on: Oct 6, 2012
 *      Author: KevStev
 */

#include <unistd.h> //getopt
#include <ctype.h>  //isprint
#include <stdlib.h> //exit
#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "tracker.h"

int main(int argc, char **argv)
{
	/*
	 * Handle arguments
	 */

	// If no commands, do nothing
	if (argc <= 1)
		return 0;

	int cmd;

	char* arg_port = NULL;				//p
	char* arg_file_option = NULL;		//o

	while ((cmd = getopt(argc, argv, "p:o:")) != -1)
	{
		switch (cmd)
		{
		case 'p':
			arg_port = optarg;
			break;
		case 'o':
			arg_file_option = optarg;
			break;
		case '?':
			if (optopt == 'p' || optopt == 'o')
				fprintf(stderr, "Option -%c requires an argument.\n", optopt);
			else if (isprint(optopt))
				fprintf(stderr, "Unknown option `-%c'.\n", optopt);
			else
				// Better functionality would output the hexadecimal
				fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
			return 0;
			break;
		default:
			// Error!
			exit(-1);
			break;
		}
	}

	// Verify all required arguments are supplied here

	// Convert arguments to usable form

	unsigned long int port = strtoul(arg_port, NULL, 0);
	char* file_option = arg_file_option;                 // Alias

	// Verify variables are within the correct range

	if (port < 1024 || port > 65536)
	{
		printf("Please supply a port number between 1025 and 65535.");
		exit(-1);
	}

	/*
	 * Parse tracker.txt
	 */

//	std::vector<TrackerEntry> tracker = get_tracker_from_file("tracker.txt");
//
//	printf("\n\n");
//	printf("%s", tracker[0].filename);
//	printf("%d", tracker[0].id);
//	printf("%d", tracker[0].port);

	/*
	 * Initialize the server to be ready to send
	 */

	int sock;
	int bytes_read; // <- note how this is now on its own line!
	socklen_t addr_len; // <- and this too, with a different type.
	char recv_data[1024];

	struct sockaddr_in server_addr, client_addr;

	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("Socket");
		exit(1);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(server_addr.sin_zero), 8);

	if (bind(sock, (struct sockaddr *) &server_addr, sizeof(struct sockaddr))
			== -1)
	{
		perror("Bind");
		exit(1);
	}

	addr_len = sizeof(struct sockaddr);

	printf("\nUDPServer Waiting for client on port %d", port);
	fflush(stdout);

	while (1)
	{
		bytes_read = recvfrom(sock, recv_data, 1024, 0,
				(struct sockaddr *) &client_addr, &addr_len);

		recv_data[bytes_read] = '\0';

		printf("\n(%s , %d) said : ", inet_ntoa(client_addr.sin_addr), ntohs(
				client_addr.sin_port));

		// print out the string array
		printf("%s", recv_data);
		fflush(stdout);
	}
	/**/
	return 0;

}

