// Header plus data for each packet
const int MAX_PAYLOAD = 5*1024;
const int MAX_HEADER = 1 + 4 + 4;
const int MAX_DATA = MAX_HEADER + MAX_PAYLOAD;

#include <fstream>

#include <stdio.h>

// Packet used for receiving data
struct Packet
{
	char type;
	unsigned int seq;
	unsigned int length;
	char payload[MAX_PAYLOAD];

	Packet() :
		type(0),
		seq(0),
		length(0),
		payload({0}) { };

	Packet(char data[MAX_DATA]) :
		type(data[0]),
		seq((int) (data[1])),
		length((int) (data[5]))
	{
		payload[0] = data[9];
		//memcpy(payload, &data[9], sizeof(payload));
	};

	Packet(char t, unsigned int s, unsigned int l, char p[MAX_PAYLOAD]) :
		type(t),
		seq(s),
		length(l)
	{
		memcpy(payload, p, sizeof(payload));
	}
};
