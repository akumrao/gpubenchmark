/*
 * Copyright (c) 2024-25 akumrao@yahoo.com
 *
 * FileName:      gpu_common.c
 * Description:   File have common code wrapper implementation to gpu.
 */

#ifndef _GPU_COMMON_H_
#define _GPU_COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <sys/mount.h>
#include <dirent.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <stdarg.h>
#include <sys/utsname.h>
#include <stdbool.h>
#include "threadload.h"

#define GPU_MIN_POWER 0.0
#define GPU_MIN_TEMP  30.0
#define GPU_MIN_FREQ  19800000
#define GPU_MIN_VOLT  0.55
#define GPU_MIN_UTIL  0

#define GPU_MAX_POWER 4000.0
#define GPU_MAX_TEMP  90.0
#define GPU_MAX_FREQ  1094000000
#define GPU_MAX_VOLT  1.0
#define GPU_MAX_UTIL  100



/* Represent the pass and fail counts for a specific parameter */
typedef struct {
  int pass;  /* Number of successful measurements (pass) */
  int fail;  /* Number of failed measurements */
} parameter_status_count;

/* Represent the parameter threshold range */
typedef struct {
    double min;
    double max;
} parameter_threshold;

/* Represent thresholds for different parameter */
typedef struct {
  parameter_threshold     power;
  parameter_status_count  power_status; /* Power pass/fail counts */
  parameter_threshold     temp;
  parameter_status_count  temp_status; /* Temperature pass/fail counts */
  parameter_threshold     freq;
  parameter_status_count  freq_status; /* Frequency pass/fail counts */
  parameter_threshold     volt;
  parameter_status_count  volt_status; /* Voltage pass/fail counts */
  parameter_threshold     util;
  parameter_status_count  util_status; /* Temperature pass/fail counts */
} parameter_threshold_t;

/* Structure to hold power rail data */
typedef struct {
  int channel;
  long int timestamp;
  char rail_name[20];
  long int energy_value;
} power_data_point_t;

extern parameter_threshold_t threshold_data;


/* logging based on the log_type */
void log_message(int log_lvl, FILE *fp, const char *tag, const char *fmt, ...);
/* Function use to mount debugfs */
void mount_debugfs();
/* Function use to execute command */
int execute_version_command(const char *command, char *cmd_output, size_t cmd_output_size);
/* Function to set the log type */
int set_log_type(const char *config_value);
/* Function to initialize the threshold data */
void initialize_parameter_threshold(parameter_threshold_t *th_data);
/* Function to validate the temperature */
bool validate_gpu_temperature(double temp);
/* Function to validate the power */
bool validate_gpu_power(double power);
/* Function to validate the voltage */
bool validate_gpu_voltage(double volt);
/* Function to validate the frequency */
bool validate_gpu_frequency(double freq);
/* Function to validate the Utilization */
bool validate_gpu_utilization(double temp);
/* Function to check the adbd process is running */
int check_adbd_process();
/* Function used to checks, if the program is running as the root */
int check_for_root();
/* Function used to capture logcat log */
void redirect_logcat_to_file(ThLoader* th);


#endif
