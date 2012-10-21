#include "tracker.h"

#include <fstream> //ifstream
#include <stdio.h>
#include <string.h>
#include <stdlib.h> //exit

std::vector<TrackerEntry> get_tracker_from_file(char* filename)
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
	char buf[MAX_CHARS_PER_LINE];
	const int MAX_TOKENS_PER_LINE = 4;
	const char* const DELIMITER = " ";

	// read each line of the file
	while (!fin.eof())
	{
	    fin.getline(buf, MAX_CHARS_PER_LINE);

	    // array to store memory addresses of the tokens in buf
	    char* token[MAX_TOKENS_PER_LINE] = {0}; // initialize to 0

	    // parse the line
	    token[0] = strtok(buf, DELIMITER); // first token

	    // TODO: fail here if < 4 items
	    int n = 0; // for-loop index
	    if (token[0]) // zero if line is blank
	    {
	    	for (n = 1; n < MAX_TOKENS_PER_LINE; n++)
	    	{
	    		token[n] = strtok(0, DELIMITER); // subsequent tokens
	    		if (!token[n]) break; // no more tokens
	    	}

	    	printf(token[0]);
	    	printf("\n");

		    tracker.push_back(TrackerEntry(
		    		token[0],
		    		strtoul(token[1], NULL, 0),
		    		token[2],
		    		strtoul(token[3], NULL, 0)));

	    	printf("%s", tracker[0].filename);
	    	printf("%d", tracker[0].id);
	    	printf("%d", tracker[0].port);
	    	printf("\n");
	    }
	}

	return tracker;
}
