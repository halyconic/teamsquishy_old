/*
 * main.cpp
 *
 *  Created on: Oct 6, 2012
 *      Author: stephen
 */

#include <unistd.h> //getopt
#include <ctype.h>  //isprint
#include <stdlib.h> //exit
#include <stdio.h>
#include <vector>
#include <algorithm>//sort
#include <istream>

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

#include "packet.h"
#include "counter.h"

/*
 * TODO:
 *   Comment!
 */
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

	char* arg_sender_port = NULL;		//p
	char* arg_requester_port = NULL;	//g
	char* arg_rate = NULL;				//r
	char* arg_seq_no = NULL;			//q
	char* arg_length = NULL;			//l

	// Our personal debug options
	bool debug = false;					//d
	char* arg_debug = NULL;				//d

	while ((cmd = getopt(argc, argv, "p:g:r:q:l:d:")) != -1)
	{
		switch (cmd)
		{
		case 'p':
			arg_sender_port = optarg;
			break;
		case 'g':
			arg_requester_port = optarg;
			break;
		case 'r':
			arg_rate = optarg;
			break;
		case 'q':
			arg_seq_no = optarg;
			break;
		case 'l':
			arg_length = optarg;
			break;
		case 'd':
			debug = true;
			arg_debug = optarg;
			break;
		case '?':
			if (optopt == 'p' || optopt == 'g' || optopt == 'r' || optopt == 'q' || optopt == 'l' || optopt == 'd')
				fprintf (stderr, "Option -%c requires an argument.\n", optopt);
			else if (isprint(optopt))
				fprintf (stderr, "Unknown option `-%c'.\n", optopt);
			else
				// Better functionality would output the hexadecimal
				fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
			return 0;
			break;
		default:
			// Error!
			exit(-1);
			break;
		}
	}

	// Verify all required arguments are supplied here

	if (!arg_sender_port)
	{
		printf("Please supply a sender port (Usage: -p <port>).\n");
		return 0;
	}
	else if (!arg_requester_port)
	{
		printf("Please supply a requester port (Usage: -g <port>).\n");
		return 0;
	}
	else if (!arg_rate)
	{
		printf("Please supply a rate (Usage: -r <rate>).\n");
		return 0;
	}
	else if (!arg_seq_no)
	{
		printf("Please supply a sequence number (Usage: -q <seq_no>).\n");
		return 0;
	}
	else if (!arg_length)
	{
		printf("Please supply a length (Usage: -l <length>).\n");
		return 0;
	}

	// Convert arguments to usable form

	unsigned long int sender_port = strtoul(arg_sender_port, NULL, 0);
	unsigned long int requester_port = strtoul(arg_requester_port, NULL, 0);
	double rate = strtoul(arg_rate, NULL, 0);
	unsigned long int seq_no = strtoul(arg_seq_no, NULL, 0);
	unsigned long int length = strtoul(arg_length, NULL, 0);

	// Verify variables are within the correct range

	if (sender_port < 1024 || sender_port > 65536
			|| requester_port < 1024 || requester_port > 65536)
	{
		printf("Please supply port numbers between 1025 and 65535.");
		return(0);
	}

	// Set up socket connection

	int sock;
	int bytes_read; // <- note how this is now on its own line!
	socklen_t addr_len; // <- and this too, with a different type.
	char recv_data[MAX_DATA];

	struct sockaddr_in requester_addr, sender_addr;

	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("Socket");
		exit(1);
	}

	// Own address
	sender_addr.sin_family = AF_INET;
	sender_addr.sin_port = htons(sender_port);
	sender_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bzero(&(sender_addr.sin_zero), 8);

	// Bind port to listen on
	if (bind(sock, (struct sockaddr *) &sender_addr, sizeof(struct sockaddr)) == -1)
	{
		perror("Bind");
		exit(1);
	}

	addr_len = sizeof(struct sockaddr);

	// Listen for incoming requests

	printf("Sender waiting for requester on port %ld\n", sender_port);
	fflush(stdout);

	char buf_send_packet[MAX_DATA];

	while (1)
	{
		bytes_read = recvfrom(sock, recv_data, sizeof(recv_data), 0,
			(struct sockaddr *) &requester_addr, &addr_len);

		Packet recv_packet = recv_data;

		if (debug)
		{
			printf("Packet being received:\n");
			printf("%c %d %d\n",
					recv_packet.type,
					recv_packet.seq,
					recv_packet.length);
			printf("Payload: %s\n", recv_packet.payload);
			printf("Raw payload: %s\n", &recv_data[9]);
		    printf("Origin: %s %u\n",
				   inet_ntoa(requester_addr.sin_addr),
				   ntohs(requester_addr.sin_port));
		}

		// Set destination port of requester
		requester_addr.sin_port = htons(requester_port);

		if (recv_packet.type == 'R')
		{
			// Break requested file into packets
			char* recv_filename = (char*)recv_packet.payload; // Alias only, be careful!
			if (debug)
				printf("Filename: %s\n", recv_filename);

			std::fstream filestr;
			filestr.open ((char*)recv_packet.payload, std::fstream::out);

			unsigned int i = 0;

			Counter counter = Counter(rate);
			/*
			 * CONTAIN IN WHILE LOOP
			 */
			while (!filestr.eof())
			{
				Packet send_packet;
				send_packet.type = 'D';
				send_packet.seq = i;
				filestr.write(send_packet.payload, sizeof(send_packet.payload));
				send_packet.length = (unsigned int)filestr.gcount();
				if (debug)
				{
					printf("Packet being sent:\n");
					printf("%c %d %d\n",
							send_packet.type,
							send_packet.seq,
							send_packet.length);
					printf("Payload: %s\n", send_packet.payload);
				    printf("Destination: %s %u\n",
						   inet_ntoa(requester_addr.sin_addr),
						   ntohs(requester_addr.sin_port));
				}

				char* buf_send_packet = new char[send_packet.length + MAX_HEADER];

				// Copy to byte form (inefficient)
				memcpy(&buf_send_packet[0], &send_packet.type, sizeof(char));
				memcpy(&buf_send_packet[1], &send_packet.seq, sizeof(unsigned int));
				memcpy(&buf_send_packet[5], &send_packet.length, sizeof(unsigned int));
				memcpy(&buf_send_packet[9], &send_packet.payload, send_packet.length);
				i++;

				sendto(sock, buf_send_packet, sizeof(buf_send_packet), 0,
						(struct sockaddr *)&requester_addr, sizeof(struct sockaddr));

				counter.wait();
			}
			/*
			 * CONTAIN IN WHILE LOOP
			 */

			filestr.close();

			char buf_end_packet[MAX_HEADER] = {0};
			buf_end_packet[0] = 'E';

			if (debug)
			{
				printf("End packet being sent:\n");
				printf("%c %d %d\n",
						buf_end_packet[0],
						(int)buf_end_packet[1],
						(int)buf_end_packet[5]);
			}

			sendto(sock, buf_end_packet, sizeof(buf_end_packet), 0,
					(struct sockaddr *)&requester_addr, sizeof(struct sockaddr));
		}
		else
		{
			// Drop packet
		}
	}
}
