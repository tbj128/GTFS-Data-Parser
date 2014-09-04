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
	// input = &output;
}


int main(int arg1, char** arg2) {
	FILE* inputCSV;
	FILE* outputCSV;
	char line[120];
	char linecopy[512];
	int count;
	char* result[256];
	string stop_id;
	
	inputCSV = fopen("generated/headsigns.csv", "r");
	outputCSV = fopen("generated/headsigns_final.csv", "w");
	
	if (inputCSV == NULL || outputCSV == NULL) {
		printf("Error opening file");
	}
	
	while (!feof(inputCSV)) {
		if (fgets(line, 1024, inputCSV) != NULL) {
			deleteChar(line, '"');
			deleteChar(line, ';');
			fprintf(outputCSV, "%s", line);
		}
	}
	fclose(inputCSV);
	fclose(outputCSV);
	
}