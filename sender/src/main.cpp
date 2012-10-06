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

int main(int argc, char **argv)
{
	// If no commands, do nothing
	if (argc <= 1)
	{
		return 0;
	}

	int cmd;

	char* sender_port = NULL;		//p
	char* requester_port = NULL;	//g
	char* rate = NULL;				//r
	char* seq_no = NULL;			//q
	char* length = NULL;			//l

	while ((cmd = getopt(argc, argv, "p:g:r:q:l:")) != -1)
	{
		switch (cmd)
		{
		case 'p':
			sender_port = optarg;
			break;
		case 'g':
			requester_port = optarg;
			break;
		case 'r':
			rate = optarg;
			break;
		case 'q':
			seq_no = optarg;
			break;
		case 'l':
			length = optarg;
			break;
		case '?':
			if (optopt == 'p' || optopt == 'g' || optopt == 'r' || optopt == 'q' || optopt == 'l')
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
}
