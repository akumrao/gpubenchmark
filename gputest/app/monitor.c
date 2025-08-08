/*
 * Copyright (c) 2024-25 akumrao@yahoo.com
 *
 * Filename:        Gpu_001.c
 * TestCaseID:      GPU_001
 * Description:     GPU Stress Test
 *                  This file implement a function to stress the GPU and
 *                  capture the parameter like- temperature, frequency, power
 *                  voltage and measure the GPU performance.
 *                  Capture all logs in output log file for further analysis.
 *
 * Author:          Arvind Umrao <aumrao@google.com>
 */



#include "gpu_common.h"
#include "common.h"
#include "gpu_parameter.h"
#include "monitor.h"
#include "json.h"
#include <sys/utsname.h>
#include "condwait.h"

/* Array of throttle load values for the sinusoidal
 * workload test, ranging from 10% to 100% */
#if 0
int default_varying_load_arr[NUM_LOAD_ELM_MAX] = {30, 20, 40, 70, 80, 70, 40, 20};
int varying_load_arr[NUM_LOAD_ELM_MAX] = {0};
int num_load_elm = NUM_LOAD_ELM_MAX;
#endif

/* This variable holds the configuration file parameters and their values */
extern config_t config_parameter;

/* Global variable to store  time value */
extern char *duration;
extern int stress_time;
/* Default stressor mode */
const char *default_tstressor_mode = "tiles" ;

/* Default logging level for console output */
extern int console_loglevel;
/* Variable holds type of log have to be used */
extern log_type_t log_type;
/* variable to store parameter threshold data */
extern parameter_threshold_t threshold_data;
/* variable to store load type */
extern gpuload_type_t load_type;


/**
 * Function:        void gpu_001_usage(const char *program_name)
 * Description:     Displays the usage information for the GPU_001 program. It provides
 *                  details on available command-line options, their descriptions, and
 *                  example usages to guide the user on how to run the program with
 *                  different configurations.
 * Parameters:      program_name: const char* - The name of the program, typically argv[0],
 *                                               used in displaying usage examples.
 * Returns:         void - This function does not return a value. It exits the program
 *                         after displaying the usage information.
 */

void gpu_001_usage(const char *program_name) {
    printf("\n");
    printf("Usages: %s -h [-d log_level] [-m stressor_mode] [-t duration] \n", program_name);
    printf("Options:\n");
    printf("  -h print usage function\n");
    printf("  -d level<0 ~ 3>: Used to set the log level e,g:\n");
    printf("      0: Set the log level ERROR\n");
    printf("      1: Set the log level WARN\n");
    printf("      2: Set the log level INFO\n");
    printf("      3: Set the log level DEBUG\n");
    printf("  -t duration: Enter the duration in seconds e.g: 30\n");
    printf("  -m stressor_mode: Enter the load mode to stress GPU e.g: \n");
    printf("      stress      - Run GPU in stress mode \n");
    printf("      tiles       - Run tiles load\n");
    printf("      shader      - Run shader load \n");
    printf("      pow         - Run the load that consume maximum power\n");
    printf("      throttle    - Generate varying cyclic load\n");
    printf("      blank       - Generate idle load\n");
    printf("      cpu         - Generate cpu load\n");
    printf("      install     - Install Android APP\n");
    printf("      usage       - High Usage of GPU\n");
    printf("      idle        - Idle GPU\n");
    printf("\n");
    printf("Example: ./gpu_001\n");
    printf("Example: ./gpu_001 -t 20 -m tiles\n");
    printf("Example: ./gpu_001 -m stress\n");
    printf("Example: ./gpu_001 -m shader -t 120\n");
    printf("Example: ./gpu_001 -t 180 -m throttle\n");
    printf("Example: ./gpu_001 -m cpu\n");
    printf("Example: ./gpu_001 -m blank\n");
    printf("Example: ./gpu_001 -m pow\n");
    printf("Example: ./gpu_001 -m install\n");
    printf("Example: ./gpu_001 -m usage\n");
    printf("Example: ./gpu_001 -m idle\n");
    exit(EXIT_FAILURE);
}

/**
 * Function:        void print_banner(void)
 * Description:     Displays a stylized banner with GPU test information.
 *                  This function prints a color-coded banner to the console,
 *                  providing a visual identifier for the GPU_001.c test.
 * Parameters:      None.
 * Returns:         void - This function does not return a value.
 */
void print_banner() {
    printf(COLOR_BLUE "********************************" COLOR_RESET);
    printf(COLOR_GREEN "*          GPU_001.c           *" COLOR_RESET);
    printf(COLOR_YELLOW"*     GPU           Test001    *" COLOR_RESET);
    printf(COLOR_BLUE "********************************" COLOR_RESET);
}

#if 0
/**
 * Function:        int get_mapped_row(int in_load)
 * Description:     Maps a given input load value to its corresponding row number based on predefined mappings.
 *                  If the input load value does not exist in the mappings, an error message is displayed,
 *                  and a failure code is returned.
 * Parameters:      in_load: int - The input load value to be mapped.
 * Returns:         int - The corresponding row number if the input load is found; otherwise, returns FAILURE.
 */
int get_mapped_row(int in_load) {
    /* Define the mappings */
    LoadMapping mappings[] = {
            {20, 1},
            {25, 3},
            {30, 4},
            {35, 6},
            {40, 7},
            {45, 8},
            {50, 10},
            {55, 11},
            {60, 12},
            {65, 14},
            {70, 15},
            {75, 17},
            {80, 18}
    };

    /* Calculate the number of mappings */
    size_t num_mappings = sizeof (mappings) / sizeof (mappings[0]);

    /* Search for the in_load in the mappings */
    for (size_t i = 0; i < num_mappings; ++i) {
        if (mappings[i].in_load == in_load) {
            return mappings[i].in_row;
        }
    }

    /* Return an error code if the in_load is not found */
    fprintf(stderr, "Load %d not found.\n", in_load);
    /* Error code indicating in_load not found */
    return FAILURE;
}

#endif

/**
 * Function:        int config_parser(const char *filename, config_t *config)
 * Description:     Parses a configuration file, extracting key-value pairs and storing
 *                  them in a config_t structure. Supports multiple GPU monitoring
 *                  parameters such as power, voltage, temperature, frequency, and logging settings.
 * Parameters:      filename: const char* - Path to the configuration file.
 *                  config: config_t* - Pointer to the configuration structure to store parsed values.
 * Returns:         int - SUCCESS (0) on success, FAILURE (-1) if an error occurs.
 */

int config_parser(const char *filename, config_t *config) {
    /* Open configuration file */
    FILE* config_file = fopen(filename, "r");
    if (config_file == NULL) {
        log_message(LOG_ERR, NULL, ETAG "Failed to open configuration file!", FL);
        return FAILURE;
    }

    char line[1024] = {0}; /* Buffer for reading lines */
    /* Process each line in the configuration file */
    while (fgets(line, sizeof (line), config_file) != NULL) {
        if (strlen(line) == 0 || line[0] == '#') { /* Skip empty lines or comments by # */
            continue;
        }
        /* Get the key and value from config file line */
        char *key = strtok(line, " =");
        char *value = strtok(NULL, "\n");

        /* store value in config_t structure member */
        if (key != NULL && value != NULL) {
            key = trim(key);
            value = trim(value);

            log_message(LOG_DEBUG, NULL, TAG, "Add node to json: %s:%s ", key, value);

            if (strcmp(key, "test_id") == 0) {
                strcpy(config->test_id, value);
            } else if (strcmp(key, "kernel_version") == 0) {
                strcpy(config->kernel_version, value);
            } else if (strcmp(key, "build_version") == 0) {
                strcpy(config->build_version, value);
            } else if (strcmp(key, "board_name") == 0) {
                strcpy(config->board_name, value);
            } else if (strcmp(key, "power") == 0) {
                if (strcmp(value, "enable") == 0) {
                    config->power = 1;
                }
            } else if (strcmp(key, "voltage") == 0) {
                if (strcmp(value, "enable") == 0) {
                    config->voltage = 1;
                }
            } else if (strcmp(key, "temperature") == 0) {
                if (strcmp(value, "enable") == 0) {
                    config->temperature = 1;
                }
            } else if (strcmp(key, "frequency") == 0) {
                if (strcmp(value, "enable") == 0) {
                    config->frequency = 1;
                }
            } else if (strcmp(key, "utilization") == 0) {
                if (strcmp(value, "enable") == 0) {
                    config->utilization = 1;
                }
            } else if (strcmp(key, "log_interval") == 0) {
                config->log_interval = atoi(value);
            } else if (strcmp(key, "log_mode") == 0) {
                strcpy(config->log_mode, value);
                if (set_log_type(config->log_mode) != 0) {
                    return FAILURE;
                }
            } else if (strcmp(key, "output_directory_path") == 0) {
                strcpy(config->output_path, value);
            } else if (strcmp(key, "node_path_for_frequency") == 0) {
                strcpy(config->frequency_curr_node, value);
            } else if (strcmp(key, "node_path_for_temperature") == 0) {
                strcpy(config->temperature_node, value);
            } else if (strcmp(key, "node_path_for_power") == 0) {
                strcpy(config->power_node, value);
            } else if (strcmp(key, "gpu_power_rail") == 0) {
                strcpy(config->gpu_rail, value);
            } else if (strcmp(key, "node_path_for_voltage") == 0) {
                strcpy(config->voltage_node, value);
            } else if (strcmp(key, "address_for_voltage") == 0) {
                strcpy(config->voltage_address, value);
            } else if (strcmp(key, "node_path_for_utilization") == 0) {
                strcpy(config->utilization_node, value);
            } else {

                if (editJsonConfig(key, value) < 0) {
                    log_message(LOG_DEBUG, NULL, TAG, ETAG "Json config file is missing");
                }
            }
        }
    }
    fclose(config_file);

    return 0;
}


void print_config() {
    /* Print parsed configuration if debug enable*/
    log_message(LOG_DEBUG, NULL, TAG, "test_id: %s", config_parameter.test_id);
    log_message(LOG_DEBUG, NULL, TAG, "kernel_version: %s", config_parameter.kernel_version);
    log_message(LOG_DEBUG, NULL, TAG, "build_version: %s", config_parameter.build_version);
    log_message(LOG_DEBUG, NULL, TAG, "power: %d", config_parameter.power);
    log_message(LOG_DEBUG, NULL, TAG, "voltage: %d", config_parameter.voltage);
    log_message(LOG_DEBUG, NULL, TAG, "temperature: %d", config_parameter.temperature);
    log_message(LOG_DEBUG, NULL, TAG, "frequency: %d", config_parameter.frequency);
    log_message(LOG_DEBUG, NULL, TAG, "log_mode: %s", config_parameter.log_mode);
    log_message(LOG_DEBUG, NULL, TAG, "output_directory_path: %s", config_parameter.output_path);
    log_message(LOG_DEBUG, NULL, TAG, "nodes_path_for_frequency: %s", config_parameter.frequency_curr_node);
    log_message(LOG_DEBUG, NULL, TAG, "node_path_for_temperature: %s", config_parameter.temperature_node);
    log_message(LOG_DEBUG, NULL, TAG, "node_path_for_power: %s", config_parameter.power_node);
    log_message(LOG_DEBUG, NULL, TAG, "node_path_for_voltage: %s", config_parameter.voltage_node);
    log_message(LOG_DEBUG, NULL, TAG, "address_for_voltage: %s", config_parameter.voltage_address);
    log_message(LOG_DEBUG, NULL, TAG, "node_path_for_utilization: %s", config_parameter.utilization_node);

}

/**
 * Function:        bool validate_kernel_and_build_version(void)
 * Description:     Validates the current system's kernel and build versions against the expected versions specified in the configuration parameters. It retrieves the system's kernel version using `uname` and the build version using a specified command, then compares these values to ensure they match the configured expectations. Logs appropriate messages based on the comparison results.
 * Parameters:      None.
 * Returns:         bool - Returns `true` if both the kernel and build versions match the configured values; otherwise, returns `false`.
 */

bool validate_kernel_and_build_version() {
    static struct version_information version_info;
    /* Extract major and minor version from kernel version */
    char major_minor_version[VER_BUFFER_SIZE] = {0};

    /* Get the kernel version */
    struct utsname uname_data;
    if (uname(&uname_data) != 0) {
        slog_message(LOG_ERR, ETAG "Failed: uname, required to read kernel version", FL);
        return false;
    }
    strncpy(version_info.kernel_version, uname_data.release, sizeof (version_info.kernel_version) - 1);
    version_info.kernel_version[sizeof (version_info.kernel_version) - 1] = '\0';

    /* Get the incremental build version */
    if (execute_version_command(BUILD_VERSION_CMD, version_info.build_version, sizeof (version_info.build_version)) != 0) {
        slog_message(LOG_ERR, ETAG "Failed: to execute build version command", FL);
        return false;
    }

    /* Get version before any dash */
    sscanf(version_info.kernel_version, "%[^-]", major_minor_version);

    /* Compare the kernel version with the expected kernel version */
    if (strcmp(major_minor_version, config_parameter.kernel_version) == 0) {
        slog_message(LOG_DEBUG, ETAG "Running Kernel version: %s", FL,
                     major_minor_version);
    } else {
        slog_message(LOG_WARN, ETAG "Failed: Kernel version mismatch, running: %s, configured: %s", FL,
                     major_minor_version, config_parameter.kernel_version);
        return false;
    }

    /* Compare the build version with the expected build version */
    if (strcmp(version_info.build_version, config_parameter.build_version) == 0) {
        slog_message(LOG_DEBUG, TAG, "Running Build version: %s", FL, version_info.build_version);
    } else {
        slog_message(LOG_WARN, ETAG "Failed: Build version mismatch, running: %s, configured: %s", FL,
                     version_info.build_version, config_parameter.build_version);
        return false;
    }


    slog_message(LOG_DEBUG, ETAG "Running Build Version: %s, and Kernel Version: %s", FL,
                 config_parameter.build_version, config_parameter.kernel_version);

    /* Return the populated structure */
    return true;
}

/**
 * Function:        int is_digit_string(const char *str)
 * Description:     Determines if the provided string consists entirely of digits.
 *                  Converts the string to a long integer and checks for any non-digit characters.
 * Parameters:      str: const char* - Pointer to the string to be evaluated.
 * Returns:         int - Returns the numeric value of the string if all characters are digits.
 *                        Returns -1 if the string contains any non-digit characters.
 */

int is_digit_string(const char *str) {
    char *endptr;
    /* Here Base is 10 (Decimal Number) */
    long num = strtol(str, &endptr, 10);

    /* Check for errors and if the entire string was converted */
    if (*endptr != '\0') {
        /* Not a pure digit string */
        return -1;
    }
    /* All digits */
    return num;
}

/**
 * Function:        gpuload_type_t get_gpuload_type(char *str)
 * Description:     Determines the GPU load type based on the input string.
 *                  Matches the input string to predefined GPU load types
 *                  such as "stress", "blank", "tiles", "shader", "pow", and
 *                  "throttle". If the input does not match any known type,
 *                  it logs an error message and returns INVALID.
 * Parameters:      str: char* - Pointer to the input string representing the
 *                  GPU load type.
 * Returns:         gpuload_type_t - Enum value corresponding to the GPU load
 *                  type. Returns INVALID if the input string does not match
 *                  any known type.
 */

gpuload_type_t get_gpuload_type(char *str) {
    if (strcmp(str, "stress") == 0) {
        return STRESS;
    } else if (strcmp(str, "blank") == 0) {
        return BLANK;
    } else if (strcmp(str, "cpu") == 0) {
        return CPU;
    }else if (strcmp(str, "tiles") == 0) {
        return TILES;
    } else if (strcmp(str, "shader") == 0) {
        return SHADER;
    } else if (strcmp(str, "pow") == 0) {
        return POWER;
    } else if (strcmp(str, "throttle") == 0) {
        return THROTTLE;
    } else if (strcmp(str, "install") == 0) {
        return INSTALL;
    } else if (strcmp(str, "usage") == 0) {
        return USAGE;
    } else if (strcmp(str, "idle") == 0) {
        return IDLE;
    } else {
        slog_message(LOG_ERR, ETAG "Failed: Invalid load type: %s", FL, str);
        return INVALID;
    }
}
#if 0
/**
 * Function:        parse_load_values
 * Description:     Parses a comma-separated string of load values, validates them within a
 *                  specified range (10-100), and stores them in an array. Ensures the number
 *                  of values is within the defined limits.
 * Parameters:      const char *load_str - String containing load values separated by commas
 *                  int *varying_load_arr - Array to store parsed load values
 * Returns:         void - Exits the program on failure
 */

void parse_load_values(const char *load_str, int *varying_load_arr) {
    /* Tokenize the comma-separated string into integers */
    char *token = strtok((char *) load_str, ",");
    int count = 0;

    while (token != NULL) {
        int load_value = atoi(token);
        /* Check if the load value is between 1 and 100 */
        if (load_value < 10 || load_value > 100) {
            slog_message(LOG_ERR, ETAG "Failed: Load value '%d' is out of range. Must be between 10 and 100.", FL, load_value);
            exit(EXIT_FAILURE);
        }
        if (count >= NUM_LOAD_ELM_MAX) {
            slog_message(LOG_ERR, ETAG "Failed: You cannot pass more than %d load values.", FL, NUM_LOAD_ELM_MAX);
            exit(EXIT_FAILURE);
        } else {
            varying_load_arr[count++] = load_value;
            token = strtok(NULL, ",");
        }
    }
    if (count < NUM_LOAD_ELM_MIN) {
        slog_message(LOG_ERR, ETAG "Failed: You must pass between %d and %d load values separated with comma", FL, NUM_LOAD_ELM_MIN, NUM_LOAD_ELM_MAX);
        exit(EXIT_FAILURE);
    }
    num_load_elm = count;
}
#endif

/**
 * Function:        parse_validate_optarguments
 * Description:     Parses and validates command-line arguments for GPU stress testing,
 *                  including stress mode, duration, and log level. Initializes configurations accordingly.
 * Parameters:      int argc - Argument count
 *                  char *argv[] - Argument vector
 * Returns:         int - SUCCESS on valid input, FAILURE on error
 */

int parse_validate_optarguments(int argc, char *argv[]) {
    int num, opt;
    char  stress_mode[256]={'\0'};
    strncpy(stress_mode, default_tstressor_mode, 255);

    const char *optstring = "m:t:d:h";
    while ((opt = getopt(argc, argv, optstring)) != -1) {
        switch (opt) {
            case 't':
                num = is_digit_string(optarg);
                if ((num == -1) || (num < 0) || (num > 3600)) {
                    slog_message(LOG_ERR, ETAG "Duration command string is not a pure digit", FL);
                    return FAILURE;
                } else {
                    duration = optarg;
                    stress_time = atoi(duration);
                }
                break;
            case 'm':
                /* Stressor Mode check for stress, tiles, pow, throttle*/

                load_type = get_gpuload_type(optarg);
                strncpy(stress_mode, optarg,255);

                if (load_type == INVALID) {
                    slog_message(LOG_ERR, ETAG "Failed: Invalid stressor mode: %s", FL, optarg);
                    return FAILURE;
                }
                break;
            case 'd':
                num = is_digit_string(optarg);
                if ((num == -1) || (num < 0) || (num > 3)) {
                    slog_message(LOG_ERR, ETAG "Log level is not a pure digit", FL);
                    return FAILURE;
                } else {
                    console_loglevel = num;
                }
                break;
            case 'h':
            default:
                gpu_001_usage(argv[0]);
                break;
        }
    }
#if 0
    /* If load values are not provided via -s, use the default array */
    if (stressor_mode == THROTTLE) {
        if (th_load_list == NULL) {
            slog_message(LOG_WARN, ETAG "No throttle load values provided with '-s'. Using default throttle load values.", FL);
            memcpy(varying_load_arr, default_varying_load_arr, sizeof (default_varying_load_arr));
            /* Set the number of load elements to 8 (default array size) */
            num_load_elm = NUM_LOAD_ELM_MAX;
        } else {
            /* Parse the comma-separated load values from the -s option */
            parse_load_values(th_load_list, varying_load_arr);
        }
    }
#endif

    slog_message(LOG_INFO,  TAG, "Stressor mode: %s , duration: %d", stress_mode, stress_time);
    if( load_type >=  BLANK  && load_type <= POWER )
            initJsonConfig(stress_mode, stress_time, config_parameter.log_interval, config_parameter.temperature_node);

    return SUCCESS;
}

/**
 * Function:        monitor_gpu_parameter
 * Description:     Monitors and logs GPU parameters such as frequency, temperature,
 *                  power, voltage, and utilization while validating them against thresholds.
 * Parameters:      ThLoader* th - Pointer to the thread loader structure.
 * Returns:         void
 */

void monitor_gpu_parameter(ThLoader* th) {
    slog_message(LOG_INFO, TAG, "Parameter monitoring thread Start");
    char mbstr[100];
    char data[1024];
    char temp_buf[128];

    char curr_frequency[12] = {0};
    double temperature;
    double voltage;
    double power = 0;
    int utilization;

    int first_read = 1;
    long int delta_value;
    long int delta_time;
    power_data_point_t previous_data_point, current_data_point;

    FILE *fp = th->thread_log;
    if (fp == NULL) {
        slog_message(LOG_ERR, ETAG "fopen failed: %s", FL, strerror(errno));
        pthread_exit(THREAD_FAILURE);
    }

    strcpy(data, "Date_Time\t\t");



    if (config_parameter.frequency) {

        config_parameter.freqFile = fopen(config_parameter.frequency_curr_node, "r");
        if (!config_parameter.freqFile) {
            slog_message(LOG_ERR, ETAG "failed: Error to fetch the node: %s", FL, config_parameter.frequency_curr_node);
            config_parameter.frequency = 0;
        } else {
            setvbuf(config_parameter.freqFile, NULL, _IONBF, 0);
            strcat(data, "Frequency\t");
        }

    }
    if (config_parameter.temperature) {

        config_parameter.tempFile = fopen(config_parameter.temperature_node, "r");
        if (!config_parameter.tempFile) {
            slog_message(LOG_ERR, ETAG "failed: Error to fetch the node: %s", FL, config_parameter.temperature_node);
            config_parameter.temperature = 0;
        } else {
            setvbuf(config_parameter.tempFile, NULL, _IONBF, 0);
            strcat(data, "Temperature\t");
            get_temperature_type(config_parameter.temperature_node);
        }
    }

    if (config_parameter.power) {

        config_parameter.powFile = fopen(config_parameter.power_node, "r");
        if (config_parameter.powFile == NULL) {
            slog_message(LOG_ERR, ETAG "Failed: to open power node file: %s", FL, strerror(errno));
            config_parameter.power = 0;
        } else {
            setvbuf(config_parameter.powFile, NULL, _IONBF, 0);
            strcat(data, "Power\t\t");
        }

    }

    if (config_parameter.voltage) {
        mount_debugfs();
        strcat(data, "Voltage\t\t");
    }
    if (config_parameter.utilization) {

        config_parameter.utilFile = fopen(config_parameter.utilization_node, "r");
        if (config_parameter.utilFile == NULL) {
            slog_message(LOG_ERR, ETAG"Failed: to open GPU Utilisation node", FL);
            config_parameter.utilization = 0;
        } else {
            setvbuf(config_parameter.utilFile, NULL, _IONBF, 0);
            strcat(data, "Utilization\t");
        }
    }
    /* Append a newline character to complete the header */
    strcat(data, "\n");

    if (config_parameter.freqFile)
        fclose(config_parameter.freqFile);

    if (config_parameter.tempFile)
        fclose(config_parameter.tempFile);

    if (config_parameter.powFile)
        fclose(config_parameter.powFile);

    if (config_parameter.utilFile)
        fclose(config_parameter.utilFile);

    size_t items_written = fwrite(data, sizeof (char), strlen(data), fp);
    if (items_written != strlen(data)) {
        slog_message(LOG_ERR, ETAG "fwrite failed: %s", FL, strerror(errno));
        fclose(fp);
        pthread_exit(THREAD_FAILURE);
    }

    Condwait condwait = (Condwait){condwait_int, condwait_wait, condwait_signal, condwait_stop};

    condwait.init(&condwait);

    while (atomic_load_explicit(&th->keeprunning, memory_order_relaxed)) {
        time_t t = time(NULL);
        struct tm *local_time = localtime(&t);
        /* Format the date and time string using strftime */
        strftime(mbstr, sizeof (mbstr), "%b-%d %H:%M:%S", local_time);

        struct timeval tv;
        gettimeofday(&tv, NULL);
        int msec = tv.tv_usec / 1000;

        sprintf(data, "%s.%03d\t", mbstr, msec);

        if (config_parameter.frequency) {
            get_gpu_frequency(config_parameter.frequency_curr_node, curr_frequency);
            strcat(data, curr_frequency);
            strcat(data, "\t");
            /* To validate the GPU frequency */
            if (validate_gpu_frequency(atof(curr_frequency)) == TRUE) {
                threshold_data.freq_status.pass++;
            } else {
                threshold_data.freq_status.fail++;
            }
        }
        if (config_parameter.temperature) {
            temperature = get_gpu_temperature(config_parameter.temperature_node);
            sprintf(temp_buf, "%lf\t", temperature);
            strcat(data, temp_buf);
            /* To validate the GPU temperature */
            if (validate_gpu_temperature(temperature) == TRUE) {
                threshold_data.temp_status.pass++;
            } else {
                threshold_data.temp_status.fail++;
            }
        }
        if (config_parameter.power) {
            if (get_rail_power(config_parameter.power_node, config_parameter.gpu_rail, &current_data_point) != 0) {
                slog_message(LOG_ERR, ETAG "Rail %s not found: %s", FL, config_parameter.gpu_rail, strerror(errno));
                pthread_exit(THREAD_FAILURE);
            }
            if (first_read) {
                previous_data_point = current_data_point;
                strcat(data, "\t\t");
            } else {
                delta_value = current_data_point.energy_value - previous_data_point.energy_value;
                delta_time = current_data_point.timestamp - previous_data_point.timestamp;
                if (delta_time > 0) {
                    /* Power in mW */
                    power = (double) delta_value / (double) delta_time;
                }

                sprintf(temp_buf, "%lf\t", power);
                strcat(data, temp_buf);
                previous_data_point = current_data_point;

                /* To validate the GPU power */
                if (validate_gpu_power(power) == TRUE) {
                    threshold_data.power_status.pass++;
                } else {
                    threshold_data.power_status.fail++;
                }
            }
        }
        if (config_parameter.voltage) {

            bool ret = get_gpu_voltage(config_parameter.voltage_node, config_parameter.voltage_address, &voltage);
            if (ret) {
                sprintf(temp_buf, "%lf\t", voltage);
                strcat(data, temp_buf);
                /* To validate the GPU Voltage */
                if (validate_gpu_voltage(voltage) == TRUE) {
                    threshold_data.volt_status.pass++;
                } else {
                    threshold_data.volt_status.fail++;
                }
            } else
                config_parameter.voltage = 0;
        }

        if (config_parameter.utilization) {
            utilization = get_gpu_utilization(config_parameter.utilization_node, config_parameter.board_name);
            sprintf(temp_buf, "%d\t", utilization);
            strcat(data, temp_buf);
            /* To validate the GPU temperature */
            if (validate_gpu_utilization(utilization) == TRUE) {
                threshold_data.util_status.pass++;
            } else {
                threshold_data.util_status.fail++;
            }
        }
        strcat(data, "\n");

        if(!first_read)
        {
            size_t items_written = fwrite(data, sizeof (char), strlen(data), fp);
            if (items_written != strlen(data)) {
                log_message(LOG_ERR, fp, ETAG "fwrite failed: %s", FL, strerror(errno));
                pthread_exit(THREAD_FAILURE);
            }
        }
        else
             first_read = 0;

        condwait.wait(&condwait, 0, config_parameter.log_interval);
    }



    slog_message(LOG_INFO, TAG, "Parameter monitoring thread Stop and exit");

}

/**
 * Function:        void check_dmesg_for_errors(FILE *perf_fp)
 * Description:     Executes the dmesg command to check for system errors related to
 *                  GPU performance. Filters and logs relevant error messages if found.
 * Parameters:      perf_fp: FILE* - Pointer to the performance log file (unused in function).
 * Returns:         void - This function does not return a value.
 */

void check_dmesg_for_errors(FILE *perf_fp) {
    char buffer[BUFFER_SIZE] = {0};
    FILE *pipe = NULL;

    /* Execute the dmesg command with the grep filter */
    pipe = popen(DMESG_CMD, "r");
    if (pipe == NULL) {
        slog_message(LOG_ERR, ETAG "popen failed: %s", FL, strerror(errno));
    }

    while (fgets(buffer, BUFFER_SIZE, pipe) != NULL) {
        slog_message(LOG_ERR, ETAG "dmesg Error: [%s]", FL, buffer);
    }
    pclose(pipe);
}

#if 0
/**
 * Function:        void capture_gpu_performance(ThLoader* th)
 * Description:     Monitors and logs GPU performance metrics, including utilization,
 *                  power consumption, and clock switch latency. The function continuously
 *                  checks GPU utilization and logs relevant data if a significant change
 *                  is detected. It also verifies dmesg logs for errors.
 * Parameters:      th: ThLoader* - Pointer to the thread loader structure containing
 *                  logging and control information.
 * Returns:         void - This function does not return a value but may terminate the
 *                  calling thread in case of critical failures.
 */

void capture_gpu_performance(ThLoader* th) {
    char mbstr[100];
    char data[1024];
    char temp_buf[128];

    int first_read = 1;
    power_data_point_t previous_data_point, current_data_point;
    int previous_utilization, current_utilization;

    int utilization_threshold = 20;
    int utilization_change;

    FILE *fp = th->thread_log;
    if (fp == NULL) {
        slog_message(LOG_ERR, ETAG "fopen failed: %s", FL, strerror(errno));
        return; /* Exit the function if file opening fails */
    }

    slog_message(LOG_INFO, TAG, "Started capturing Performance Metrics");

    char buffer[1024];
    FILE *cmd_out = popen("dmesg -C", "r");
    if (cmd_out == NULL) {
        slog_message(LOG_ERR, ETAG "popen failed: %s", FL, strerror(errno));
        return;
    }
    pclose(cmd_out);

    cmd_out = popen("echo \"****** KPI for GPU Clock Switch Test ******\"", "r");
    while (fgets(buffer, sizeof (buffer), cmd_out) != NULL) {
        fprintf(fp, "%s", buffer);
    }
    pclose(cmd_out);

    cmd_out = popen("date", "r");
    while (fgets(buffer, sizeof (buffer), cmd_out) != NULL) {
        fprintf(fp, "%s", buffer);
    }
    pclose(cmd_out);

    strcpy(data, "Date_Time\t\t");
    strcat(data, "Clock_Switch_Latency\t");
    strcat(data, "Frames_Randered\t");
    strcat(data, "Power_Used\t");

    /* Append a newline character to complete the header */
    strcat(data, "\n");

    size_t items_written = fwrite(data, sizeof (char), strlen(data), fp);
    if (items_written != strlen(data)) {
        slog_message(LOG_ERR, ETAG "fwrite failed: %s", FL, strerror(errno));
        pthread_exit(THREAD_FAILURE);
    }

    Condwait condwait = (Condwait){condwait_int, condwait_wait, condwait_signal, condwait_stop};
    condwait.init(&condwait);

    while (atomic_load_explicit(&th->keeprunning, memory_order_relaxed)) {
        current_utilization = get_gpu_utilization(config_parameter.utilFile, config_parameter.board_name);
        if (previous_utilization != 0) {
            utilization_change = abs(previous_utilization - current_utilization);
        }

        if (utilization_change > utilization_threshold) {
            time_t t = time(NULL);
            struct tm *local_time = localtime(&t);
            /* Format the date and time string using strftime */
            strftime(mbstr, sizeof (mbstr), "%b-%d_%T", local_time);

            struct timeval tv;
            gettimeofday(&tv, NULL);
            int msec = tv.tv_usec / 1000;

            sprintf(data, "%s.%03d\t", mbstr, msec);

            if (get_rail_power(config_parameter.powFile, config_parameter.gpu_rail, &current_data_point) != 0) {
                slog_message(LOG_ERR, ETAG "Rail %s not found: %s", FL, config_parameter.gpu_rail, strerror(errno));
                pthread_exit(NULL); // Exit thread on failure
            }

            if (first_read) {
                previous_data_point = current_data_point;
                first_read = 0;
                fprintf(fp, "NA\t\t");
                slog_message(LOG_INFO, TAG, "First power reading captured.");
            } else {
                long delta_value = current_data_point.energy_value - previous_data_point.energy_value;
                long delta_time = current_data_point.timestamp - previous_data_point.timestamp;

                if (delta_time > 0) { // Ensure no division by zero
                    double power = (double) delta_value / (double) delta_time; // Power in mW
                    sprintf(temp_buf, "%lf\t", power);
                    strcat(data, temp_buf);
                }
                previous_data_point = current_data_point;
            }

            strcat(data, "\n");

            size_t items_written = fwrite(data, sizeof (char), strlen(data), fp);
            if (items_written != strlen(data)) {
                log_message(LOG_ERR, fp, ETAG "fwrite failed: %s", FL, strerror(errno));

                pthread_exit(THREAD_FAILURE);
            }

            previous_utilization = current_utilization;

            check_dmesg_for_errors(fp);

            condwait.wait(&condwait, 0, config_parameter.log_interval);

        }
    }

    slog_message(LOG_INFO, TAG, "KPI Metrics monitoring thread Stop and exit");


}

/**
 * Function:        void run_stress()
 * Description:     Initiates a GPU stress test by forking a new process. The child
 *                  process redirects its output to a file, disables SELinux enforcement,
 *                  stops any existing GPU load application, and starts a new stress test
 *                  using an Android activity manager command. The parent process waits
 *                  for the child process to complete and logs the outcome.
 * Parameters:      None
 * Returns:         void - This function does not return a value.
 */

void run_stress(void) {
    stress_pid = fork();
    if (stress_pid < 0) {
        log_message(LOG_ERR, log_fp, ETAG "fork failed: %s", FL, strerror(errno));
        exit(EXIT_FAILURE);
    } else if (stress_pid == 0) {
        /* Redirect output to TEST_OUTPUT_FILE */
        FILE *fp_out = freopen(TEST_OUTPUT_FILE, "w", stdout);
        if (fp_out == NULL) {
            log_message(LOG_ERR, NULL, ETAG "freopen failed: %s", FL, strerror(errno));
            exit(EXIT_FAILURE);
        }


        system("setenforce 0");
        system("am force-stop org.gpu.glload.debug");
        //system("settings put system accelerometer_rotation 0");
        //system("input keyevent 26"); // to invoke from sleep
        //system("input keyevent 82"); // to invoke from sleep


        char am_command[512];
        snprintf(am_command, sizeof (am_command),
                "am start -n org.gpu.glload.debug/org.gpu.glload.MainActivity "
                "-a android.intent.action.MAIN -e TYPE LOW -e DURATION %s -e ROW %d",
                duration, row);

        int status = system(am_command);
        if (status == -1) {
            log_message(LOG_ERR, stderr, ETAG "Error executing am start command: %s\n", FL, strerror(errno));
            exit(EXIT_FAILURE);
        } else if (WEXITSTATUS(status) != 0) {
            log_message(LOG_ERR, stderr, ETAG "am start command failed with exit code %d\n", FL, WEXITSTATUS(status));
            exit(EXIT_FAILURE);
        }
    } else {
        /* Wait for child process to finish */
        int status;
        waitpid(stress_pid, &status, 0);
        if (WIFEXITED(status)) {
            int exit_status = WEXITSTATUS(status);
            if (exit_status == EXIT_FAILURE) {
                slog_message(LOG_ERR, log_fp, ETAG "Child process exited with failure", FL);
                exit(EXIT_FAILURE);
            } else {
                slog_message(LOG_INFO, log_fp, TAG, "Child process exited successfully");
            }
        } else {
            slog_message(LOG_ERR, log_fp, ETAG "Child process terminated abnormally", FL);
        }
    }
}

/**
 * Function:        void run_gpu_stress()
 * Description:     Executes the GPU stress test based on the selected stressor mode.
 *                  Supports different modes including LIMIT, MAX, RANDOM, and TH_CYCLIC.
 *                  In TH_CYCLIC mode, the function ensures that the test runs for
 *                  the required duration and dynamically adjusts load levels.
 * Parameters:      None
 * Returns:         void - This function does not return a value.
 */

void run_gpu_stress(void) {


    switch (stressor_mode) {
        case LIMIT:
            //log_message(LOG_INFO, log_fp, TAG, "Stress test is running with LIMIT load");
            row = get_mapped_row(load);
            run_stress();
            break;
        case MAX:
            //log_message(LOG_INFO, log_fp, TAG, "Stress test is running with MAX load");
            row = get_mapped_row(80);
            run_stress();
            break;
        case RANDOM:
            //log_message(LOG_INFO, log_fp, TAG, "Stress test is running with RANDOM load");
            srand(time(NULL));
            load = (rand() % 80) + 1;
            row = get_mapped_row(load);
            run_stress();
            break;
        case TH_CYCLIC:
        {
            if (stress_time < TH_DURATION * num_load_elm) {
                log_message(LOG_ERR, log_fp, ETAG "For throttle mode please give duration more than %d sec", FL, (TH_DURATION * num_load_elm));
            }
            //log_message(LOG_INFO, log_fp, TAG, "Stress test is running in THROTTLE CYCLIC mode");
            int repeat_count = stress_time / (num_load_elm * TH_DURATION);
            int new_duration = TH_DURATION + ((stress_time % (TH_DURATION * num_load_elm)) / num_load_elm);
            FILE *fp = fopen(TEST_OUTPUT_FILE, "w");
            if (fp == NULL) {
                log_message(LOG_ERR, log_fp, ETAG "Failed: Error opening stressor output file", FL);
                exit(EXIT_FAILURE);
            }


            for (int repeat_index = 0; repeat_index < repeat_count; repeat_index++) {
                for (int load_index = 0; load_index < num_load_elm; load_index++) {
                    load = varying_load_arr[load_index];
                    row = get_mapped_row(load);
                    char am_command[512];
                    snprintf(am_command, sizeof (am_command),
                            "am start -n org.gpu.glload.debug/org.gpu.glload.MainActivity "
                            "-a android.intent.action.MAIN -e TYPE LOW -e DURATION %d -e ROW %d",
                            new_duration, row);

                    int status = system(am_command);
                    if (status == -1) {
                        log_message(LOG_ERR, stderr, ETAG "Error executing am start command: %s\n", FL, strerror(errno));
                        exit(EXIT_FAILURE);
                    } else if (WEXITSTATUS(status) != 0) {
                        log_message(LOG_ERR, stderr, ETAG "am start command failed with exit code %d\n", FL, WEXITSTATUS(status));
                        exit(EXIT_FAILURE);
                    }
                }
            }
        }
            break;
        default:
            log_message(LOG_ERR, log_fp, ETAG "INVALID stressor mode", FL);
            break;
    }
}
#endif

/**
 * Function:        int verify_parameter_output()
 * Description:     Logs the pass/fail count of various system parameters such as
 *                  frequency, temperature, power, voltage, and utilization. The function
 *                  checks the corresponding configuration flags before logging the data.
 * Parameters:      None
 * Returns:         int - SUCCESS (0) upon completion.
 */

int verify_parameter_output() {
    if (config_parameter.frequency) {
        slog_message(LOG_INFO, TAG, "Frequency - Pass/Fail Count: %d/%d", threshold_data.freq_status.pass, threshold_data.freq_status.fail);
    }
    if (config_parameter.temperature) {
        slog_message(LOG_INFO, TAG, "Temperature - Pass/Fail Count: %d/%d", threshold_data.temp_status.pass, threshold_data.temp_status.fail);
    }
    if (config_parameter.power) {
        slog_message(LOG_INFO, TAG, "Power - Pass/Fail Count: %d/%d", threshold_data.power_status.pass, threshold_data.power_status.fail);
    }
    if (config_parameter.voltage) {
        slog_message(LOG_INFO, TAG, "Voltage - Pass/Fail Count: %d/%d", threshold_data.volt_status.pass, threshold_data.volt_status.fail);
    }
    if (config_parameter.voltage) {
        slog_message(LOG_INFO, TAG, "Utilization - Pass/Fail Count: %d/%d", threshold_data.util_status.pass, threshold_data.util_status.fail);
    }
    return SUCCESS;
}

/**
 * Function:        int verify_stress_output()
 * Description:     Verifies the output of the GPU stress test by reading a log file
 *                  and checking for failure indicators. It searches for the strings
 *                  "failed:" and "metrics untrustworthy:" in the file and determines
 *                  whether the GPU test has passed or failed based on the extracted values.
 * Parameters:      None
 * Returns:         int - SUCCESS (0) if execution completes, FAILURE (-1) if the file
 *                  cannot be opened or if no relevant data is found.
 */

int verify_stress_output() {
    FILE *fp = NULL;
    char line[100];
    /* Initialize to an invalid value */
    int failed_value = -1;
    int metrics_untrustworthy = -1;


    char test_output_file[256] = {'\0'};
    sprintf(test_output_file, "%s/%s", config_parameter.output_path, "gpu001_stress_output.txt");


    fp = fopen(test_output_file, "r");
    if (fp == NULL) {
        slog_message(LOG_ERR, ETAG "Failed: Error opening stressor output file", FL);
        return FAILURE;
    }

    while (fgets(line, sizeof (line), fp) != NULL) {
        char *failed_pos = strstr(line, "failed: ");
        if (failed_pos != NULL) {
            char *num_str = failed_pos + strlen("failed: ");
            failed_value = atoi(num_str);
        }

        char *metrics_pos = strstr(line, "metrics untrustworthy: ");
        if (metrics_pos != NULL) {
            char *num_str = metrics_pos + strlen("metrics untrustworthy: ");
            metrics_untrustworthy = atoi(num_str);
        }
    }


    if (failed_value != -1 || metrics_untrustworthy != -1) {
        if (failed_value == 0 || metrics_untrustworthy == 0) {
            slog_message(LOG_INFO, TAG, "Verified: GPU Test PASS!!!");
        } else {
            slog_message(LOG_INFO, TAG, "Verified: GPU Test FAILED: %d", failed_value);
        }
    } else {
        slog_message(LOG_ERR, ETAG "Failed: stressor output file not found.", FL);
    }
    fclose(fp);
    return SUCCESS;
}

