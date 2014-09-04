#include <string.h>
#include <stdio.h>
#include <string>
#include <map>

using namespace std;

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
	FILE* originalCSV;
	
	map<string, int> stopcode2code;
	
	char line[120];
	char linecopy[512];
	int count;
	char* result[256];
	string stop_id;
	
	originalCSV = fopen("generated/stops.csv", "r");
	
	if (originalCSV == NULL) {
		printf("Error opening file");
	}
	
	// Get all the correct stops
	while (!feof(originalCSV)) {
		if (fgets(line, 1024, originalCSV) != NULL) {
			string line_str(line);
			unsigned pos_of_comma = line_str.find(",");
			string stopentry = line_str.substr(pos_of_comma + 1);
			string stopcode = line_str.substr(0, pos_of_comma);
			map<string, int>::iterator itr;
			itr = stopcode2code.find(stopcode);
			if (itr == stopcode2code.end()) {
				// Unique
				stopcode2code[stopcode] = 1;
			} else {
				printf("%s - Not unique\n", stopcode.c_str());
			}
		}
	}
	
	fclose(originalCSV);
	
}