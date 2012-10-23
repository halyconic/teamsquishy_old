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

const char * domain = ".cs.wisc.edu";

int main(int argc, char **argv)
{
/*	char string1[]="test string";
	char string2[80];
	char string3[80];
	char *string4;
	strcpy (string2,string1);
	strcpy (string3,"strcopy worked.");
	string4 = strcpy(string2, "strcpy return example");
	printf ("string1: %s\nstring2: %s\nstring3: %s\n",string1,string2,string3);
	printf ("string4: %s\n",string4);

	char * temp;
	char family[80];
	temp = strcpy(family, "kevin");
	printf("%s", temp);*/

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

	if (port < 1024 || port > 65536)
	{
		printf("Please supply a port number between 1025 and 65535.\n");
		return 0;
	}

	// Parse tracker.txt

	std::vector<TrackerEntry> tracker = get_tracker_from_file("tracker.txt", debug);



	if (debug)
	{
		printf("\n");
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

	/////////////////
	//
	// Set up socket connection
	//
	/////////////////

	int sock;
	int bytes_read; // <- note how this is now on its own line!
	socklen_t addr_len; // <- and this too, with a different type.
	char recv_data[MAX_DATA];
	struct hostent* recv_ent;
	struct sockaddr_in requester_addr, sender_addr;

	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("Socket");
		exit(1);
	}

	//TEMP
	if (debug)
	{
		recv_ent = (struct hostent *) gethostbyname(arg_debug);

		if ((struct hostent *) recv_ent == NULL)
		{
			printf("Host was not found by the name of %s\n", arg_debug);
			exit(1);
		}
	}

	requester_addr.sin_family = AF_INET;
	requester_addr.sin_port = htons(port);
	requester_addr.sin_addr = *((struct in_addr *)recv_ent->h_addr);
	bzero(&(requester_addr.sin_zero), 8);


/*	// Bind port to listen on
	if (bind(sock, (struct sockaddr *) &requester_addr, sizeof(struct sockaddr)) == -1)
	{
		perror("Bind");
		exit(1);
	}*/

	addr_len = sizeof(struct sockaddr);

	// Make requests

	unsigned int number_of_parts = 0;
	for (unsigned int i = 0; i < tracker.size(); i++)
	{
		/*printf("file option: %s\n", file_option);
		printf("file name: %s\n", tracker[i].filename);*/

		if (strcmp(file_option, tracker[i].filename) == 0)
		{
			// Send request

			Packet send_packet;
			send_packet.type = 'R';
			send_packet.seq = 0;
			send_packet.length = 0;
			if (debug)
			{
				printf("Packet:\n");
				printf("%c",send_packet.type);
				printf("\n");
				printf("%d",send_packet.seq);
				printf("\n");
				printf("%d",send_packet.length);
				printf("\n");
			}

			char* buf_send_packet = new char[send_packet.length + MAX_HEADER];

			// Copy to byte form (inefficient)
			memcpy(&buf_send_packet[0], &send_packet.type, sizeof(char));
			memcpy(&buf_send_packet[1], &send_packet.seq, sizeof(unsigned int));
			memcpy(&buf_send_packet[5], &send_packet.length, sizeof(unsigned int));
			memcpy(&buf_send_packet[9], file_option, strlen(file_option));
			i++;

			printf("about to send packets\n");
			sendto(sock, buf_send_packet, sizeof(buf_send_packet), 0,
					(struct sockaddr *)&requester_addr, sizeof(struct sockaddr));
		}
	}

	if (number_of_parts <= 0)
	{
		printf("Tracker file does not contain any files by the name %s.\n", file_option);
		return(0);
	}

	// Listen for packets (Listen until end packet)

	while (1)
	{
		bytes_read = recvfrom(sock, recv_data, sizeof(recv_data), 0,
			(struct sockaddr *) &sender_addr, &addr_len);

		Packet recv_packet = recv_data;

		if (recv_packet.type == 'D')
		{
			unsigned int i = 0;
			/*
			 * CONTAIN IN WHILE LOOP
			 */
			if (debug)
			{
				printf("Received packet:\n");
				printf("%d",recv_packet.type);
				printf("\n");
				printf("%d",recv_packet.seq);
				printf("\n");
				printf("%d",recv_packet.length);
				printf("\n");
				printf(recv_packet.payload);
				printf("\n");
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


	//int sock;
//	int bytes_read; // <- note how this is now on its own line!
//	socklen_t addr_len; // <- and this too, with a different type.
//	char recv_data[1024];
//
//	struct sockaddr_in server_addr, client_addr;
//
//	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
//	{
//		perror("Socket");
//		exit(1);
//	}
//
//	server_addr.sin_family = AF_INET;
//	server_addr.sin_port = htons(port);
//	server_addr.sin_addr.s_addr = INADDR_ANY;
//	bzero(&(server_addr.sin_zero), 8);
//
//	if (bind(sock, (struct sockaddr *) &server_addr, sizeof(struct sockaddr))
//			== -1)
//	{
//		perror("Bind");
//		exit(1);
//	}
//
//	addr_len = sizeof(struct sockaddr);
//
//	printf("\nUDPServer Waiting for client on port %d", port);
//	fflush(stdout);
//
//	while (1)
//	{
//		bytes_read = recvfrom(sock, recv_data, 1024, 0,
//				(struct sockaddr *) &client_addr, &addr_len);
//
//		recv_data[bytes_read] = '\0';
//
//		printf("\n(%s , %d) said : ", inet_ntoa(client_addr.sin_addr), ntohs(
//				client_addr.sin_port));
//
//		// print out the string array
//		printf("%s", recv_data);
//		fflush(stdout);
//	}
	/**/


	//	/*
	//	 * Initialize the client to request
	//	 */

/*	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(server_addr.sin_zero), 8);

	int sock;
	struct sockaddr_in server_addr;
	struct hostent *host;
	char send_data[1024];

	if (debug)
	{
		host = (struct hostent *) gethostbyname(arg_debug);

		if ((struct hostent *) host == NULL)
		{
			printf("Host was not found by the name of %s\n", arg_debug);
			exit(1);
		}
	}
	else
		host = (struct hostent *) gethostbyname((char *)"127.0.0.1");

	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		exit(1);
	}

	// Where we are sending to
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(requester_port);
	server_addr.sin_addr = *((struct in_addr *)host->h_addr);
	bzero(&(server_addr.sin_zero),8); // TODO: convert to memset

	while (1)
	{
		printf("Type Something (q or Q to quit):");
		gets(send_data);

		if ((strcmp(send_data , "q") == 0) || strcmp(send_data , "Q") == 0)
			break;

		else
			sendto(sock, send_data, strlen(send_data), 0,
					(struct sockaddr *) &server_addr, sizeof(struct sockaddr));
	}*/
}

