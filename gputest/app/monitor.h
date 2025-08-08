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

#ifndef VALIDATE_H
#define VALIDATE_H

#include "threadload.h"

#define TAG "TC_GPU_001"

#define THREAD_FAILURE (void*)-1
#define DMESG_CMD "dmesg -T | grep -i 'error|fatal|panic|cpu|memory|malloc|kfree|kmalloc|page|swap|oom'"

#define NUM_LOAD_ELM_MIN 2
#define NUM_LOAD_ELM_MAX 8

/* GPU_001 Configuration file */
#define TEMP_PATH "/data/local/tmp/gpu001"
#define CONFIG_FILE_NAME  TEMP_PATH"/gpu_001.conf"

#define BUILD_VERSION_CMD     "getprop ro.product.build.version.incremental"


/* Structure to hold build information */
struct version_information {
  char build_version[VER_BUFFER_SIZE];
  char kernel_version[VER_BUFFER_SIZE];
};

/* Config file parameters structure */
typedef struct config {
  /* Versions info */
  char test_id[10];
  char kernel_version[10];
  char build_version[24];
  char board_name[56];

  /* Parameters */
  short int power;
  short int voltage;
  short int temperature;
  short int frequency;
  short int utilization;
  int log_interval;
  char log_mode[64];
  char output_path[256];
  /* Node path for parameters */
  char frequency_curr_node[256];
  char temperature_node[256];
  char power_node[256];
  char gpu_rail[64];
  char voltage_node[256];
  char voltage_address[24];
  char utilization_node[256];

  FILE *freqFile;
  FILE *tempFile;
  FILE *powFile;
  FILE *utilFile;

} config_t;


/* enum contains the gpu load type info */
typedef enum {
  INVALID = -1,
  BLANK,         /* Capture parameter in BLANK */
  CPU,           /* Generate the maximum CPU load */
  SHADER,        /* Set the SHADER */
  TILES,         /* Generate TILES load */
  STRESS,        /* Generate STRESS load */
  THROTTLE,      /* throttling the load */
  USAGE,         /* HIGH usage of gpu  */
  POWER,         /* Max power drawn   */
  INSTALL,       /* Install of Android APP   */
  IDLE           /* IDLE GPU */

}gpuload_type_t;

/* Structure to hold the mapping between in_load and ROW */
typedef struct {
    int in_load;
    int in_row;
} LoadMapping;

/* Function to print configured values */
void print_config();
/* Function to parse config file and store value in config_t structure member */
int config_parser(const char *filename, config_t *config);
/* Function create a full path for output file */
void create_output_file_path(void);
/* Function create a log file based on config log_mode */
int create_test_log_file(void);
/* Function fetch the command line argument and validate */
int parse_validate_optarguments(int argc, char *argv[]);
/* Thread function for parameter capturing */
void monitor_gpu_parameter (ThLoader* th );
/* Thread function for kpi monitoring */
void capture_gpu_performance (ThLoader* th );
/* Function to run stress test for GPU */
void run_gpu_stress(void);
/* Function use to get and validate version and build info */
bool validate_kernel_and_build_version();
int verify_parameter_output();

/**
 * Function Name: gpu_001_usage
 * Description:   Program usage description
 */
void gpu_001_usage(const char *program_name);

/**
 * Function Name: print_banner
 * Description:   Function to print test info as banner format.
 */
void print_banner() ;

#endif
