#include "tracker.h"


#include <fstream> //ifstream
#include <stdio.h>
#include <string.h>
#include <stdlib.h> //exit

std::vector<TrackerEntry> get_tracker_from_file(char* filename, bool debug)
{
	// Stores file into an iterable array
	std::vector<TrackerEntry> tracker;

	std::ifstream fin;
	fin.open(filename);
	if (!fin.good())
	{
		printf("Unable to open %s\n", filename);
		exit(-1);
	}

	const int MAX_CHARS_PER_LINE = 512;
	char buffer[MAX_CHARS_PER_LINE];
	const int MAX_TOKENS_PER_LINE = 4;
	const char* const DELIMITER = " ";

	if (0 && debug)
		printf("Input entries:\n");

	// read each line of the file
	while (!fin.eof())
	{

		fin.getline(buffer, MAX_CHARS_PER_LINE);

		// array to store memory addresses of the tokens in buf
		char* token[MAX_TOKENS_PER_LINE] = {0}; // initialize to 0

		// parse the line
		token[0] = strtok(buffer, DELIMITER); // first token

		// TODO: fail here if < 4 items
		int n = 0; // for-loop index
		if (token[0]) // zero if line is blank
		{
			// get the rest of the tokens
			for (n = 1; n < MAX_TOKENS_PER_LINE; n++)
			{
				token[n] = strtok(0, DELIMITER); // subsequent tokens
				if (!token[n])
					break; // no more tokens
			}

			// Print file as inputting
			if (0 && debug)
			{
				for (int i = 0; i < MAX_TOKENS_PER_LINE; i++)
				{
					printf(token[i]);
					printf(" ");
				}
				printf("\n");
			}

			//printf("the first token: %s\n", token[0]);
			tracker.push_back(TrackerEntry(
					strdup(token[0]),
					strtoul(token[1], NULL, 0),
					token[2],
					strtoul(token[3], NULL, 0)));
		}
	}

	//printf("TRACKER SIZE: %d", tracker.size());

	return tracker;
}
