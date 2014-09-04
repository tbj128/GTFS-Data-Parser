#include <string.h>
#include <stdio.h>
#include <string>
#include <map>

using namespace std;

	
struct StopCodePair
{
	int usePos;
	int insertPos;
	int stopCodes[2];
};

void deleteChar(char *input, char letterToDelete) {
	if (input == NULL) {
		printf("No input!");
		return;
	}
	
	char output[120];
	int outputPointerPos = 0;
	for(int i = 0; input[i] != '\0'; i++) {
    	if (input[i] != letterToDelete) {
    		output[outputPointerPos] = input[i];
    		outputPointerPos++;
    	}
	}
	output[outputPointerPos] = '\0';
	strcpy(input, output);
}

int tokenizer(char *str, char del, char* out[20]) {
    char *p;
    char *start;
    int count = 0;

    if (str == NULL) return 0;

    start = str;
    while (start != NULL) {
        out[count++] = start;
        p = strchr(start, del);
        if (p != NULL)
        {
            *p++ = '\0';
        }
        start = p;
    }
    return count;
}

int main(int arg1, char** arg2) {
	FILE* correctCSV;
	FILE* originalCSV;
	FILE* tmpWriteCSV;
	FILE* tmpReadCSV;
	FILE* finalCSV;
	
	map<string, int> stopentry2code;
	
	char line[120];
	char linecopy[512];
	int count;
	char* result[256];
	string stop_id;
	
	correctCSV = fopen("generated/stops_correct.csv", "r");
	originalCSV = fopen("generated/stops.csv", "r");
	tmpWriteCSV = fopen("generated/stops_temp.csv", "w");
	finalCSV = fopen("generated/stops_final.csv", "w");
	
	if (correctCSV == NULL || originalCSV == NULL || tmpWriteCSV == NULL) {
		printf("Error opening file");
	}
	
	// Get all the correct stops
	while (!feof(correctCSV)) {
		if (fgets(line, 1024, correctCSV) != NULL) {
			string line_str(line);
			unsigned pos_of_comma = line_str.find(",");
			if (pos_of_comma != 5) {
				// Corrupt line
				printf("Error - corrupt original entry! \n");
			} else {
				string stopentry = line_str.substr(pos_of_comma + 1);
				stopentry2code[stopentry] = atoi(line_str.substr(0, pos_of_comma).c_str());
				// printf("%s - %s\n", line_str.substr(0, pos_of_comma).c_str(), stopentry.c_str());
			}
		}
	}
	
	// Populate the missing fields in the new stops CSV
	while (!feof(originalCSV)) {
		if (fgets(line, 1024, originalCSV) != NULL) {
			string line_str(line);
			unsigned pos_of_comma = line_str.find(",");
			if (pos_of_comma != 5) {
				// Corrupt line
				string stopentry = line_str.substr(pos_of_comma + 1);
				map<string, int>::iterator itr;
				itr = stopentry2code.find(stopentry);
				if (itr == stopentry2code.end()) {
					fprintf(tmpWriteCSV, "%s", line);
					//printf("Error - could not find stop in original entry! \n");
					//printf("%s \n", stopentry.c_str());
				} else {
					int stopcode = itr->second;
					fprintf(tmpWriteCSV, "%i,%s", stopcode, stopentry.c_str());
					//printf("Replaced %i - %s \n", stopcode, stopentry.c_str());
				}
			} else {
				// No problems with this line; just print
				fprintf(tmpWriteCSV, "%s", line);
			}
		}
	}
	
	fclose(tmpWriteCSV);
	
	// ===============
	// But what if the lat/lon changes for the skytrain stops?
	// They shouldn't but they always do; we will just randomly assign stops based on 
	// their names
	
	map<string, StopCodePair> stopentry2codes;
	
	rewind(correctCSV);
	// Get all the correct stops
	while (!feof(correctCSV)) {
		if (fgets(line, 1024, correctCSV) != NULL) {
			string line_str(line);
			unsigned pos_of_comma = line_str.find(",");
			unsigned pos_of_second_comma = line_str.find(",", 6);
			if (pos_of_comma != 5 || pos_of_second_comma <= 6) {
				// Corrupt line
				printf("Error - corrupt original entry! \n");
			} else {
				int substrLength = (pos_of_second_comma - pos_of_comma) - 1;
				string stopentry = line_str.substr(pos_of_comma + 1, substrLength);
				
				map<string, StopCodePair>::iterator itr;
				itr = stopentry2codes.find(stopentry);
				StopCodePair stopCodePair;
				if (itr != stopentry2codes.end()) {
					stopCodePair = itr->second;
					//printf("%i\n", stopCodePair.insertPos);
				} else {
					stopCodePair.usePos = 0;
					stopCodePair.insertPos = 0;
				}
				
				stopCodePair.stopCodes[stopCodePair.insertPos] = atoi(line_str.substr(0, pos_of_comma).c_str());
				stopCodePair.insertPos++;
				
				stopentry2codes[stopentry] = stopCodePair;
				//printf("%s\n", stopentry.c_str());
			}
		}
	}
	
	tmpReadCSV = fopen("generated/stops_temp.csv", "r");
	rewind(tmpReadCSV);
	// Populate the missing fields in the new stops CSV
	while (!feof(tmpReadCSV)) {
		if (fgets(line, 1024, tmpReadCSV) != NULL) {
			string line_str(line);
			unsigned pos_of_comma = line_str.find(",");
			unsigned pos_of_second_comma = line_str.find(",", 6);
			if (pos_of_comma != 5) {
				int substrLength = (pos_of_second_comma - pos_of_comma) - 1;
				string stopentry = line_str.substr(pos_of_comma + 1, substrLength);
				string fullStopEntry = line_str.substr(pos_of_comma + 1);
				
				map<string, StopCodePair>::iterator itr;
				itr = stopentry2codes.find(stopentry);
				if (itr == stopentry2codes.end()) {
					fprintf(finalCSV, "%s", line);
					printf("Error - could not find stop in original entry! \n");
					printf("%s \n", stopentry.c_str());
				} else {
					StopCodePair stopcodePair = itr->second;
					int stopcode = stopcodePair.stopCodes[stopcodePair.usePos];
					fprintf(finalCSV, "%i,%s", stopcode, fullStopEntry.c_str());
					printf("Replaced %i - %s at %i \n", stopcode, stopentry.c_str(), stopcodePair.usePos);
					stopcodePair.usePos++;
					
					itr->second = stopcodePair;
					//printf("Replaced %i - %s \n", stopcode, stopentry.c_str());
				}
			} else {
				// No problems with this line; just print
				fprintf(finalCSV, "%s", line);
			}
		}
	}
	
	
	fclose(correctCSV);
	fclose(originalCSV);
	fclose(tmpReadCSV);
	fclose(finalCSV);
	
}