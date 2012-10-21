#include <vector>   //vector

/*
 * Tracker struct containing entries from file
 */
class TrackerEntry
{
public:

	char* filename;
	unsigned int id;
	char* machinename;
	unsigned int port;

	TrackerEntry() :
		filename(NULL),
		id(0),
		machinename(NULL),
		port(0) {;}

	TrackerEntry(char* f, unsigned int i, char* m, unsigned int p) :
		filename(f),
		id(i),
		machinename(m),
		port(p) {;}
};

/*
 * Return entries from file
 */
std::vector<TrackerEntry> get_tracker_from_file(char* filename);

