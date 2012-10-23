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

//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
//#include <stdio.h>
//#include <unistd.h>
//#include <errno.h>
#include <string.h>
#include <cstdlib> //strtol

#include "tracker.h"
#include "packet.h"

const char* domain = ".cs.wisc.edu";

int main(int argc, char **argv)
{
	// Handle arguments

	// If no commands, do nothing
	if (argc <= 1)
	{
		printf("Please supply arguments.\n");
		return 0;
	}

	int cmd;

	char* arg_port = NULL;				//p
	char* arg_file_option = NULL;		//o

	// Our personal debug options
	bool debug = false;					//d
	char* arg_debug = NULL;				//d

	while ((cmd = getopt(argc, argv, "p:o:d:")) != -1)
	{
		switch (cmd)
		{
		case 'p':
			arg_port = optarg;
			break;
		case 'o':
			arg_file_option = optarg;
			break;
		case 'd':
			debug = true;
			arg_debug = optarg;
			break;
		case '?':
			if (optopt == 'p' || optopt == 'o' || optopt == 'd')
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

	if (!arg_port)
	{
		printf("Please supply a port (Usage: -p <port>).\n");
		return 0;
	}
	else if (!arg_file_option)
	{
		printf("Please supply a file option (Usage: -o <file_option>).\n");
		return 0;
	}

	// Convert arguments to usable form

	unsigned long int port = strtoul(arg_port, NULL, 0);
	char* file_option = arg_file_option;                 // Alias

	// Verify variables are within the correct range

	if (!debug && (port < 1024 || port > 65536))
	{
		printf("Please supply a port number between 1025 and 65535.\n");
		return 0;
	}

	// Parse tracker.txt

	std::vector<TrackerEntry> tracker = get_tracker_from_file("tracker.txt", debug);

	if (0 && debug)
	{
		printf("Output entries:\n");
		for (unsigned int i = 0; i < tracker.size(); i++)
		{
			printf("%s", tracker[i].filename);
			printf(" ");
			printf("%d", tracker[i].id);
			printf(" ");
			printf("%s", tracker[i].machinename);
			printf(" ");
			printf("%d", tracker[i].port);
			printf("\n");
		}
	}

	// Set up socket connection

	int sock;
	struct sockaddr_in requester_addr, sender_addr;
	struct hostent* send_ent;
	char recv_data[MAX_DATA];
	int bytes_read;
	socklen_t addr_len;

	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("Socket");
		exit(1);
	}

	// Where we are sending to
	sender_addr.sin_family = AF_INET;
	addr_len = sizeof(struct sockaddr);

	// Make requests

	unsigned int number_of_parts = 0;
	for (unsigned int i = 0; i < tracker.size(); i++)
	{
		if (strcmp(file_option, tracker[i].filename) == 0)
		{
			if (debug)
			{
				printf("Entry acknowledged:\n");
				printf("%s %d %s %d\n",
					tracker[i].filename,
					tracker[i].id,
					tracker[i].machinename,
					tracker[i].port);
			}

			// Send request

			// Adjust where we are sending to
			char* ip_lookup = new char[strlen(tracker[i].machinename) + strlen(domain)];
			ip_lookup = strcat(tracker[i].machinename, domain);
			send_ent = (struct hostent *) gethostbyname(ip_lookup);

			// Verify sender exists
			if ((struct hostent *) send_ent == NULL)
			{
				// TODO: Gracefully handle missing sender
				printf("Host was not found by the name of %s\n", arg_debug);
				exit(1);
			}

			// Set up address
			sender_addr.sin_port = htons(tracker[i].port);
			sender_addr.sin_addr = *((struct in_addr *)send_ent->h_addr);
			bzero(&(sender_addr.sin_zero), 8);

			// Form packet
			Packet send_packet;
			send_packet.type = 'R';
			send_packet.seq = 0;
			send_packet.length = 0;
			if (debug)
			{
				printf("Packet being sent:\n");
				printf("%c %d %d\n",
						send_packet.type,
						send_packet.seq,
						send_packet.length);
				printf("Payload: %s\n", send_packet.payload);
			}

			char* buf_send_packet = new char[send_packet.length + MAX_HEADER];

			// Copy to byte form (inefficient)
			memcpy(&buf_send_packet[0], &send_packet.type, sizeof(char));
			memcpy(&buf_send_packet[1], &send_packet.seq, sizeof(unsigned int));
			memcpy(&buf_send_packet[5], &send_packet.length, sizeof(unsigned int));
			memcpy(&buf_send_packet[9], file_option, strlen(file_option));

			sendto(sock, buf_send_packet, sizeof(buf_send_packet), 0,
					(struct sockaddr *)&sender_addr, sizeof(struct sockaddr));

			if (debug)
				printf("Packet sent\n");

			number_of_parts++;
		}
	}

	if (number_of_parts <= 0)
	{
		printf("Tracker file does not contain any files by the name %s.\n", file_option);
		return(0);
	}

	// Listen for packets (Listen until end packet)

	// Own address
	requester_addr.sin_family = AF_INET;
	requester_addr.sin_port = htons(port);
	requester_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(requester_addr.sin_zero), 8);

	// Bind port to listen on
	if (bind(sock, (struct sockaddr *) &requester_addr, sizeof(struct sockaddr)) == -1)
	{
		perror("Bind");
		exit(1);
	}

	if (debug)
	{
		printf("Requester waiting for sender on port %d\n", port);
		fflush(stdout);
	}

	while (1)
	{
		bytes_read = recvfrom(sock, recv_data, sizeof(recv_data), 0,
			(struct sockaddr *) &sender_addr, &addr_len);

		printf("Packet received\n");

		Packet recv_packet = recv_data;

		if (recv_packet.type == 'D')
		{
			unsigned int i = 0;
			/*
			 * CONTAIN IN WHILE LOOP
			 */
			if (debug)
			{
				printf("Packet being received:\n");
				printf("%c %d %d\n",
						recv_packet.type,
						recv_packet.seq,
						recv_packet.length);
				printf("Payload: %s\n", recv_packet.payload);
			}
		}
		else
		{
			// Drop packet
		}
	}

	// Reorder packets

	// Print out to file


	// Initialize the server to be ready to send
}

