#include <string.h>
#include <stdio.h>
#include <string>
#include <map>
#include <set>

using namespace std;

#ifndef PARSE_H
#define PARSE_H
 
class TripSeq {
	public:
		int headsignid;
		int stop_code;
};

class Comp {
	public:
		bool operator()(TripSeq t1, TripSeq t2)
		{
			if (t1.headsignid < t2.headsignid) return true;
			else if (t1.headsignid == t2.headsignid && t1.stop_code < t2.stop_code) return true;
			return false;
		}
};

typedef struct _Trip_t {
    string routeid;
    int headsignid;
    string serviceid;
} TripT;


class Parse {
	private:
 
	public:
		map<string, string> stopid2code;
		map<string, string> routeid2num;
		map<string, TripT*> tripId2Trip;
		map<string, int> headsign2id;
		map<string, string> headsign2routeid;
		set<TripSeq, Comp> tripseqs;
		
		int tokenizer(char *str, char del, char* out[20]);
		void errline(const char* line, const char* field);
		void convert_name(char *new_name, const char *name);
		int calculate_time(const char* time);
		string convert_serviceid(string service_id);
};
 
#endif