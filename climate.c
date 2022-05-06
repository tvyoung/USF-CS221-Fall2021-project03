/*
Project 3
Vicki Young
student ID: 20591845
start date: 2021.11.17
end date: 2021.11.29 at 2359
CS 221-02
*/

/**
 * climate.c
 *
 * Performs analysis on climate data provided by the
 * National Oceanic and Atmospheric Administration (NOAA).
 *
 * Input:    Tab-delimited file(s) to analyze.
 * Output:   Summary information about the data.
 *
 * Compile:  run make
 *
 * Example Run:      ./climate data_tn.tdv data_wa.tdv
 *
 *
 * Opening file: data_tn.tdv
 * Opening file: data_wa.tdv
 * States found: TN WA
 * -- State: TN --
 * Number of Records: 17097
 * Average Humidity: 49.4%
 * Average Temperature: 58.3F
 * Max Temperature: 110.4F 
 * Max Temperatuer on: Mon Aug  3 11:00:00 2015
 * Min Temperature: -11.1F
 * Min Temperature on: Fri Feb 20 04:00:00 2015
 * Lightning Strikes: 781
 * Records with Snow Cover: 107
 * Average Cloud Cover: 53.0%
 * -- State: WA --
 * Number of Records: 48357
 * Average Humidity: 61.3%
 * Average Temperature: 52.9F
 * Max Temperature: 125.7F
 * Max Temperature on: Sun Jun 28 17:00:00 2015
 * Min Temperature: -18.7F 
 * Min Temperature on: Wed Dec 30 04:00:00 2015
 * Lightning Strikes: 1190
 * Records with Snow Cover: 1383
 * Average Cloud Cover: 54.5%
 *
 * TDV format:
 *
 * CA» 1428300000000»  9prcjqk3yc80»   93.0»   0.0»100.0»  0.0»95644.0»277.58716
 * CA» 1430308800000»  9prc9sgwvw80»   4.0»0.0»100.0»  0.0»99226.0»282.63037
 * CA» 1428559200000»  9prrremmdqxb»   61.0»   0.0»0.0»0.0»102112.0»   285.07513
 * CA» 1428192000000»  9prkzkcdypgz»   57.0»   0.0»100.0»  0.0»101765.0» 285.21332
 * CA» 1428170400000»  9prdd41tbzeb»   73.0»   0.0»22.0»   0.0»102074.0» 285.10425
 * CA» 1429768800000»  9pr60tz83r2p»   38.0»   0.0»0.0»0.0»101679.0»   283.9342
 * CA» 1428127200000»  9prj93myxe80»   98.0»   0.0»100.0»  0.0»102343.0» 285.75
 * CA» 1428408000000»  9pr49b49zs7z»   93.0»   0.0»100.0»  0.0»100645.0» 285.82413
 *
 * Each field is separated by a tab character \t and ends with a newline \n.
 *
 * Fields:
 *      state code (e.g., CA, TX, etc),
 *      timestamp (time of observation as a UNIX timestamp),
 *      geolocation (geohash string),
 *      humidity (0 - 100%),
 *      snow (1 = snow present, 0 = no snow),
 *      cloud cover (0 - 100%),
 *      lightning strikes (1 = lightning strike, 0 = no lightning),
 *      pressure (Pa),
 *      surface temperature (Kelvin)
 */

#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NUM_STATES 50
#define TRUE 1
#define FALSE 0

/* TODO: Add elements to the climate_info struct as necessary. */
struct climate_info {
    char code[3];
    unsigned long num_records;
    long double sum_humidity;
    long double sum_temperature;
    long double max_temperature;
    time_t max_temperature_timestamp;
    long double min_temperature;
    time_t min_temperature_timestamp;
    unsigned long lightning_strikes;
    unsigned long snow_covers;
    long double cloud_covers;
};

void analyze_file(FILE *file, struct climate_info *states[], int num_states);
void print_report(struct climate_info *states[], int num_states);

int main(int argc, char *argv[]) {

    /* TODO: fix this conditional. You should be able to read multiple files. */
    //argc refers to the number of arguments passed
    //argv[] is a pointer array which points to each argument passed to the program
    //argv[0] holds the name of the program itself and argv[1] is a pointer to the first command line argument supplied
    if (argc == 1) { //could be argc == 1 or argc < 2
        printf("Usage: %s tdv_file1 tdv_file2 ... tdv_fileN \n", argv[0]);
        return EXIT_FAILURE;
    }

    /* Let's create an array to store our state data in. As we know, there are
     * 50 US states. */
    struct climate_info *states[NUM_STATES] = { NULL };

    int i;
    for (i = 1; i < argc; ++i) {
        /* TODO: Open the file for reading */
        FILE* file;
        file = fopen(argv[i], "r");
        printf("Opening file: %s\n", argv[i]);
        /* TODO: If the file doesn't exist, print an error message and move on
         * to the next file. */
        if (file == NULL) {
            printf("Error in opening file %s\n", argv[i]);
            continue; 
        }
        /* TODO: Analyze the file */
        analyze_file(file, states, NUM_STATES);
        //closes file ONLY if it was successfully accessed
        if (file != NULL) {
            fclose(file);
        }
    }
    
    /* Now that we have recorded data for each file, we'll summarize them: */
    print_report(states, NUM_STATES);

    //free each state struct climate_info entry
    for (i = 0; i < NUM_STATES; i++) {
        free(states[i]);
    }

    return 0;
}

//states is a pointer to array of pointers to 50 struct climate_info
//**states = *states[]
void analyze_file(FILE *file, struct climate_info **states, int num_states) {
    const int line_sz = 100;
    char line[line_sz];

    char *token;
    int state_index = 0;
    int existing_entry;
    //reads line from file and stores it in line, up until (line_siz - 1) characters are read, newline is read, or end-of-file is reached
    while (fgets(line, line_sz, file) != NULL) {

        /* TODO: We need to do a few things here:
         *
         *       * Tokenize the line.
         *       * Determine what state the line is for. This will be the state
         *         code, stored as our first token.
         *       * If our states array doesn't have a climate_info entry for
         *         this state, then we need to allocate memory for it and put it
         *         in the next open place in the array. Otherwise, we reuse the
         *         existing entry.
         *       * Update the climate_info structure as necessary.
         */

        //stores the state code as the first token
        token = strtok(line, "\t\n");
        existing_entry = FALSE;
        //loops through number of states to check if the state code exists in climate_info (if there is a climate_info entry for the given state)
        for (state_index = 0; state_index < num_states; state_index++) {
            //if there is already an existing climate_info entry for given state, we keep the state_index and break out of the for loop
            if (states[state_index] != NULL) {
                if (strcmp(token, states[state_index]->code) == 0) {
                    existing_entry = TRUE;
                    break;
                } 
            }
        }
        //if there is no existing climate_info entry for the given state, we allocate memory space for it in the next open place in the array + keep the state_index
        if (existing_entry == FALSE) {
            //for loop finds the next open place in the states array to allocate memory for new climate_info entry
            //also stores state code token in climate_info's information
            for (state_index = 0; state_index < num_states; state_index++) {
                if (states[state_index] == NULL) {
                    states[state_index] = (struct climate_info*) malloc(sizeof(struct climate_info));
                    //check that memory was properly allocated
                    if (states[state_index] == NULL) {
                        printf("Space not properly allocated.\n");
                        exit(1);
                    }
                    strcpy((states[state_index])->code, token);
                    break;
                }
            }
        }
        struct climate_info *info = states[state_index];
        info->num_records = info->num_records + 1;
        time_t current_timestamp;
        long double current_temperature;
        while (token != NULL) {
            //2) token = TIMESTAMP
            token = strtok(NULL, "\t\n");
            current_timestamp = atol(token) / 1000;
            //3) token = GEOLOCATION
            token = strtok(NULL, "\t\n");
            //4) token = HUMIDITY
            token = strtok(NULL, "\t\n");
            info->sum_humidity = info->sum_humidity + atoi(token);
            //5) token = SNOW
            token = strtok(NULL, "\t\n");
            info->snow_covers = info->snow_covers + atoi(token); //(1 = snow present, 0 = no snow)
            //6) token = CLOUD COVER
            token = strtok(NULL, "\t\n");
            info->cloud_covers = info->cloud_covers + atoi(token); //(0 - 100)
            //7) token = LIGHTNING STRIKES
            token = strtok(NULL, "\t\n");
            info->lightning_strikes = info->lightning_strikes + atoi(token); //(1 = lightning strike, 0 = no lightning)
            //8) token = PRESSURE
            token = strtok(NULL, "\t\n");
            //9) token = SURFACE TEMPERATURE
            token = strtok(NULL, "\t\n");
            current_temperature = (long double)atof(token);
            info->sum_temperature = info->sum_temperature + current_temperature;
            //find the maximum temperature and maximum temperature timestamp
            //first checks if a max_temperature has been stored yet by checking if this is the first state record entry; if it is, current_temperature and current_timestamp become the current max information
            if (info->num_records == 1) {
                info->max_temperature = current_temperature;
                info->max_temperature_timestamp = current_timestamp;
            } else {
                if (current_temperature > info->max_temperature) {
                    info->max_temperature = current_temperature;
                    info->max_temperature_timestamp = current_timestamp;
                }
            }
            //finds the minimum temperature and minimum temperature timestamp
            //first checks if a min_temperature has been stored yet by checking if this is the first state record entry; if it is, current_temperature and current_timestamp become the current min information
            if (info->num_records == 1) {
                info->min_temperature = current_temperature;
                info->min_temperature_timestamp = current_timestamp;
            } else {
                if (current_temperature < info->min_temperature) {
                    info->min_temperature = current_temperature;
                    info->min_temperature_timestamp = current_timestamp;
                }
            }
            //token = NULL
            token = strtok(NULL, "\t\n");
        }
    }
}

void print_report(struct climate_info *states[], int num_states) {
    printf("States found: ");
    int i;
    for (i = 0; i < num_states; ++i) {
        if (states[i] != NULL) {
            struct climate_info *info = states[i];
            printf("%s ", info->code);
        }
    }
    printf("\n");
    /* TODO: Print out the summary for each state. See format above. */
    for (i = 0; i < num_states; ++i) {
        if (states[i] != NULL) {
            struct climate_info *info = states[i];
            printf("-- State: %s --\n", info->code);
            printf("Number of Records: %lu\n", info->num_records);
            long double average_humidity = (long double)(info->sum_humidity/info->num_records);
            printf("Average Humidity: %.1Lf%%\n", average_humidity);
            long double average_temperature = (long double)(info->sum_temperature/info->num_records);
            average_temperature = average_temperature * 1.8 - 459.67;
            printf("Average Temperature: %.1LfF\n", average_temperature);
            long double converted_max_temp = info->max_temperature * 1.8 - 459.67;
            printf("Max Temperature: %.1LfF\n", converted_max_temp);
            printf("Max Temperature on: %s", ctime(&(info->max_temperature_timestamp)));
            long double converted_min_temp = info->min_temperature * 1.8 - 459.67;
            printf("Min Temperature: %.1LfF\n", converted_min_temp);
            printf("Min Temperature on: %s", ctime(&(info->min_temperature_timestamp)));
            printf("Lightning Strikes: %lu\n", info->lightning_strikes);
            printf("Records with Snow Cover: %lu\n", info->snow_covers);
            long double average_cloud_cover = (long double)(info->cloud_covers/info->num_records);
            printf("Average Cloud Cover: %.1Lf%%\n", average_cloud_cover);
        }
    }
}
