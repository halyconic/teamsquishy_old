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

struct _Packet
{
	//  Aliased:
	//	char type;
	//	unsigned int seq;
	//	unsigned int length;
	//	char payload[MAX_PAYLOAD];

	char& type()
	{
		return values_[0];
	}
	unsigned int& seq()
	{
		return (unsigned int&)values_[1];
	}
	unsigned int& length()
	{
		return (unsigned int&)values_[5];
	}
	char* payload()
	{
		return &values_[9];
	}

	char  operator [] (unsigned i) const { return this->values_[i]; }
	char& operator [] (unsigned i)       { return this->values_[i]; }
	operator char*()                     { return this->values_; }

private:
	char values_[MAX_DATA];
};
