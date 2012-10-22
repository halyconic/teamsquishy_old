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

//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
#include <netdb.h>
//#include <stdio.h>
//#include <unistd.h>
//#include <errno.h>
#include <string.h>
#include <cstdlib> //strtol

const int MAX_DATA = 1 + 4 + 4 + 5*1024;

/*
 * TODO:
 *   Comment!
 */
int main(int argc, char **argv)
{
	/*
	 * Handle arguments
	 */

	// If no commands, do nothing
	if (argc <= 1)
		return 0;

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

	// Convert arguments to usable form

	unsigned long int sender_port = strtoul(arg_sender_port, NULL, 0);
	unsigned long int requester_port = strtoul(arg_requester_port, NULL, 0);

	// Verify variables are within the correct range

	if (sender_port < 1024 || sender_port > 65536
			|| requester_port < 1024 || requester_port > 65536)
	{
		printf("Please supply a port number between 1025 and 65535.");
		exit(-1);
	}

	/*
	 * Initialize the client to request
	 */

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
					(struct sockaddr *)&server_addr, sizeof(struct sockaddr));
	}
}
