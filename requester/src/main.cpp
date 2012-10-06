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

	char* port = NULL;				//p
	char* file_option = NULL;		//o

	while ((cmd = getopt(argc, argv, "p:o:")) != -1)
	{
		switch (cmd)
		{
		case 'p':
			port = optarg;
			break;
		case 'o':
			file_option = optarg;
			break;
		case '?':
			if (optopt == 'p' || optopt == 'o')
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
