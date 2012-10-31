// Header plus data for each packet
const int MAX_PAYLOAD = 5*1024;
const int MAX_HEADER = 1 + 4 + 4;
const int MAX_DATA = MAX_HEADER + MAX_PAYLOAD;

#include <fstream>
#include <stdio.h>

// Packet used for receiving data
struct Packet
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

	// TODO: This does not work if payload is full!!!
	void print()
	{
		printf("%c %d %d\n%s\n",
			   type(),
			   seq(),
			   length(),
			   payload());
	}

private:
	char values_[MAX_DATA];
};
