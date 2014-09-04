#include "Parse.h"
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

using namespace std;


int Parse::tokenizer(char *str, char del, char* out[20]) {
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

void Parse::errline(const char* line, const char* field) {
    printf("Get field (%s) from line (%s) failed\n", field, line);
    exit(1);
}

void Parse::convert_name(char *new_name, const char *name) {
    int convert = 1;
    const char *p;
    int index = 0;
    char tmp_name[256];
    char *tmp_p;

    if (name == NULL)
    {
        new_name[index] = '\0';
        return;
    }

    strcpy(tmp_name, name);
    tmp_p = strstr(tmp_name, " FS ");
    if (tmp_p != NULL)
    {
        tmp_p++;
        *tmp_p++ = 'A';
        *tmp_p++ = 't';
    }
    p = tmp_name;

    if (strncmp(p, "WB ", 3) == 0 || strncmp(p, "EB ", 3) == 0 || strncmp(p, "NB ", 3) == 0 || strncmp(p, "SB ", 3) == 0)
    {
        new_name[index++] = *p++;
        new_name[index++] = *p++;
        convert = 0;
    }
    while (*p != '\0')
    {
        if (convert == 1)
        {
            if (*p >= 'a' && *p <= 'z')
            {
                new_name[index++] = 'A' + (*p - 'a');
            } else
            {
                new_name[index++] = *p;
            }
        } else
        {
            if (*p >= 'A' && *p <= 'Z')
            {
                new_name[index++] = 'a' + (*p - 'A');
            } else
            {
                new_name[index++] = *p;
            }
        }
        if (*p == ' ')
        {
            convert = 1;
        } else
        {
            convert = 0;
        }
        p++;
    }
    new_name[index] = '\0';
    {
        char *x = strstr(new_name, " \(stop Id");
        if (x != NULL)
        {
            *x = '\0';
        }
    }
}

int Parse::calculate_time(const char* time) {
    int h, m, s, ret;
    char *ph, *pm, *ps;
    char timestr[30];

    strcpy(timestr, time);

    ph = timestr;
    pm = strchr(ph, ':');
    *pm++ = '\0';
    ps = strchr(pm, ':');
    *ps++ = '\0';

    h = atoi(ph);
    m = atoi(pm);
    s = atoi(ps);

    ret = h*3600 + m*60 + s;

    return ret;
}

string Parse::convert_serviceid(string service_id) {
    string ret;

    if (service_id.compare("WEEKDAY") == 0)
    {
        ret = "1";
    } else if (service_id.compare("SATURDAY") == 0)
    {
       ret = "2";
    } else if (service_id.compare("SUNDAY") == 0)
    {
        ret = "3";
    } else
    {
        ret = "4";
    }

    return ret;
}


int main(int argc, char** argv) {
	Parse parsing;
	string dataLocation = "data/";
	string csvLocation = "generated/";

    int count, i;
    char* result[20];
    char line[120];
    char new_stop_name[256];
    FILE *fpi, *fpo, *fpo1;
    int kept_stopid=0, kept_headid=0, kept_tripid=0, kept_arrtime=0;
    int arrtime;

    fpi = fopen((dataLocation + "stops.txt").c_str(), "r");
    fpo = fopen((csvLocation + "stops.csv").c_str(), "w");

    if (fpi == NULL || fpo == NULL)
    {
        printf("Cannot either open stops.txt or stops.csv\n");
        exit(-1);
    }

    /* get rid of the first line */
    fgets(line, 1024, fpi);
    while (!feof(fpi))
    {
        if (fgets(line, 1024, fpi) != NULL)
        {
            count = parsing.tokenizer(line, ',', result);
            parsing.convert_name(new_stop_name, result[2]);
            fprintf(fpo, "%s,%s,%s,%s\n", result[1], new_stop_name, result[4], result[5]);
			
            parsing.stopid2code[result[0]] = result[1];           
         }
    }
    fclose(fpi); fclose(fpo);

    fpi = fopen((dataLocation + "routes.txt").c_str(), "r");
    if (fpi == NULL)
    {
        printf("Cannot open routes.txt\n");
        exit(-2);
    }

    /* get rid of the first line */
    fgets(line, 1024, fpi);
    while (!feof(fpi))
    {
        if (fgets(line, 1024, fpi) != NULL)
        {
            count = parsing.tokenizer(line, ',', result);
            parsing.routeid2num[result[0]] = result[2];          
        }
    }

    fclose(fpi);

    fpi = fopen((dataLocation + "trips.txt").c_str(), "r");
    if (fpi == NULL)
    {
        printf("Cannot open trips.txt\n");
        exit(-3);
    }

    fpo = fopen((csvLocation + "headsigns.csv").c_str(), "w");
    if (fpo == NULL)
    {
        printf("Cannot open headsigns.csv\n");
        exit(-3);
    }

    /* get rid of the first line */
    fgets(line, 1024, fpi);
    while (!feof(fpi))
    {
        if (fgets(line, 1024, fpi) != NULL)
        {
            string trip_id, route_id, headsign, service_id;
            map<string, TripT*>::iterator itr1;
            map<string ,int>::iterator itr2;
            string route_num;
            map<string, string>::iterator itr3;
            int headsignid;
            count = parsing.tokenizer(line, ',', result);
            route_id = result[0];
            service_id = result[1];
            trip_id = result[2];
            headsign = result[3];
            
            // Clean the headsign because there could be invalid characters or spaces
            headsign.erase(std::remove(headsign.begin(), headsign.end(), '"'), headsign.end());
            headsign.erase(std::remove(headsign.begin(), headsign.end(), ';'), headsign.end());
            // Trim whitespace
            headsign.erase(headsign.begin(), std::find_if(headsign.begin(), headsign.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
			headsign.erase(std::find_if(headsign.rbegin(), headsign.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), headsign.end());
			// Trim leading zeros
			int firstNotOfZero = headsign.find_first_not_of('0', 0);
			if (firstNotOfZero > 0) {
				headsign.erase(0, firstNotOfZero);
			} 

			/* Translink has some weird service IDs this time... ignore entries that are weird */
			if (service_id.compare("1") == 0 || service_id.compare("2") == 0 || service_id.compare("3") == 0) {
				// Good they are a valid service ID
			} else {
				printf("Trip %s is not a valid trip. \n", trip_id.c_str());
				continue;
			}
			
            itr3 = parsing.routeid2num.find(route_id);
            if (itr3 == parsing.routeid2num.end())
            {
                printf("route id (%s) service id (%s) trip id (%s) headsign (%s) route id does not have num\n", route_id.c_str(), service_id.c_str(), trip_id.c_str(), headsign.c_str());
                continue;
            }
            route_num = itr3->second;
            itr2 = parsing.headsign2id.find(headsign);
            if (itr2 == parsing.headsign2id.end())
            {
                /* new head sign */
                headsignid = parsing.headsign2id.size();
                parsing.headsign2id[headsign] = headsignid;
                fprintf(fpo, "%d,%s,%s\n", headsignid, headsign.c_str(), route_num.c_str());
            } else
            {
                headsignid = itr2->second;
            }

            itr1 = parsing.tripId2Trip.find(trip_id);

            if (itr1 == parsing.tripId2Trip.end())
            {
                TripT *t = new TripT();
                t->routeid = route_id;
                t->headsignid = headsignid;
                t->serviceid = service_id;
                parsing.tripId2Trip[trip_id] = t;
            }
        }
    } /* while */

    fclose(fpi);
    fclose(fpo);

    fpi = fopen((dataLocation + "stop_times.txt").c_str(), "r");
    if (fpi == NULL)
    {
        printf("Cannot open stop_times.txt\n");
        exit(-4);
    }

    fpo = fopen((csvLocation + "stop_seq.csv").c_str(), "w");
    if (fpo == NULL)
    {
        printf("Cannot open stop_seq.csv\n");
        exit(-5);
    }

    fpo1 = fopen((csvLocation + "stop_times.csv").c_str(), "w");
    if (fpo1 == NULL)
    {
        printf("Cannot open stop_times.csv\n");
        exit(-6);
    }

    /* get rid of the first line */
    fgets(line, 1024, fpi);
    while (!feof(fpi))
    {
        if (fgets(line, 1024, fpi) != NULL)
        {
            string trip_id, arr_time, dep_time, stop_id, stop_seq;
            map<string, TripT*>::iterator itr1;
            map<string, int>::iterator itr2;
            map<string, string>::iterator itr3;
            set<TripSeq, Comp>::iterator itr4;

            TripT *trip;
            TripSeq tseq;

            count = parsing.tokenizer(line, ',', result);
            trip_id = result[0];
            arr_time = result[1];
            dep_time = result[2];
            stop_id = result[3];
            stop_seq = result[4];

            itr1 = parsing.tripId2Trip.find(trip_id);
            if (itr1 == parsing.tripId2Trip.end())
            {
                printf("trip id (%s) arr time (%s) dep time (%s) stop id (%s) stop seq (%s) Trip Id is not valid\n", trip_id.c_str(), arr_time.c_str(), dep_time.c_str(), stop_id.c_str(), stop_seq.c_str());
                continue;
            }

            trip = itr1->second;

            itr3 = parsing.stopid2code.find(stop_id);
            if (itr3 == parsing.stopid2code.end())
            {
                printf("trip id (%s) arr time (%s) dep time (%s) stop id (%s) stop seq (%s) Stop Id is not valid\n", trip_id.c_str(), arr_time.c_str(), dep_time.c_str(), stop_id.c_str(), stop_seq.c_str());
                continue;
            }

            tseq.headsignid = trip->headsignid;
            tseq.stop_code = atoi((itr3->second).c_str());

            itr4 = parsing.tripseqs.find(tseq);
            if (itr4 == parsing.tripseqs.end())
            {
                parsing.tripseqs.insert(tseq);
                fprintf(fpo, "%d,%d,%d\n", tseq.stop_code, tseq.headsignid, atoi(stop_seq.c_str()));
            }

            arrtime = parsing.calculate_time(arr_time.c_str());

            if (kept_stopid == tseq.stop_code && kept_headid == tseq.headsignid && kept_tripid == atoi(trip_id.c_str()) && kept_arrtime == arrtime)
            {
                printf("Stop times duplicated\n");
            } else
            {
                fprintf(fpo1, "%d,%d,%s,%d\n", tseq.stop_code, tseq.headsignid, trip->serviceid.c_str(), arrtime);
                kept_stopid = tseq.stop_code;
                kept_headid = tseq.headsignid;
                kept_tripid = atoi(trip_id.c_str());
                kept_arrtime = arrtime;
            }
        } /* if */
    } /* while */
    fclose(fpi);
    fclose(fpo);
    fclose(fpo1);
}