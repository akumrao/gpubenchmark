/*
 * Copyright (c) 2024-25 akumrao@yahoo.com
 *
 * FileName:      gpu_common.c
 * Description:   File have common code wrapper implementation to gpu.
 */

#include "gpu_common.h"
#include "common.h"
/* Include for Android logging */
#include <android/log.h>

#define TAG "GPU_COMMON"
#define ETAG TAG, "[%s:%d] "
#define FL __FILE__,__LINE__

#define DEBUGFS_MOUNT_POINT "/sys/kernel/debug"

/**
 * Function:        int execute_version_command(const char *command, char *cmd_output, size_t cmd_output_size)
 * Description:     Executes a shell command to fetch the build version and stores the output.
 * Parameters:      command:        const char*  - The shell command to execute.
 *                  cmd_output:     char*        - Buffer to store the command output.
 *                  cmd_output_size: size_t      - Size of the output buffer.
 * Returns:         SUCCESS (0)  - If the command executes successfully.
 *                  FAILURE (-1) - If an error occurs during execution.
 */

int execute_version_command(const char *command, char *cmd_output, size_t cmd_output_size) {
  FILE *fp = popen(command, "r");
  if (fp == NULL) {
    slog_message(LOG_ERR,  ETAG "Failed: to run build version command: %s", FL, command);
    return FAILURE;
  }

  if (fgets(cmd_output, cmd_output_size, fp) == NULL) {
    slog_message(LOG_ERR,  ETAG "Failed to read output from command: %s", FL, command);
    strncpy(cmd_output, "Unknown", cmd_output_size);
  } else {
    size_t len = strcspn(cmd_output, "\n");
    if (len < cmd_output_size - 1) {
      cmd_output[len] = '\0';
    } else {
        /* Truncate if necessary */
        cmd_output[cmd_output_size - 1] = '\0';
    }
  }

  if (pclose(fp) == -1) {
    slog_message(LOG_ERR,  ETAG "Failed: Error closing command stream for: %s", FL, command);
    return FAILURE;
  }
  return SUCCESS;
}


/**
 * Function:        void initialize_parameter_threshold(parameter_threshold_t *th_data)
 * Description:     Initializes the parameter threshold values for GPU metrics, setting
 *                  minimum values for idle cases and maximum values for maximum load.
 * Parameters:      th_data: parameter_threshold_t* - Pointer to the structure where the
 *                                                   threshold values will be stored.
 * Returns:         void - This function does not return a value.
 */

void initialize_parameter_threshold(parameter_threshold_t *th_data) {
  /* Min value for parameter is on Idle case value */
  th_data->power.min = GPU_MIN_POWER;
  th_data->temp.min  = GPU_MIN_TEMP;
  th_data->freq.min  = GPU_MIN_FREQ;
  th_data->volt.min  = GPU_MIN_VOLT;
  th_data->util.min  = GPU_MIN_UTIL;

  /* Max value for parameter will be on Max Load */
  th_data->power.max = GPU_MAX_POWER;
  th_data->temp.max  = GPU_MAX_TEMP;
  th_data->freq.max  = GPU_MAX_FREQ;
  th_data->volt.max  = GPU_MAX_VOLT;
  th_data->util.max  = GPU_MAX_UTIL;
}


/**
 * Function:        bool validate_gpu_temperature(double temp)
 * Description:     Validates whether the given GPU temperature falls within the
 *                  predefined threshold range (min and max). Logs a message indicating
 *                  whether the temperature value is within range or not.
 * Parameters:      temp: double - The GPU temperature value to validate.
 * Returns:         bool - TRUE if the temperature value is within the threshold range,
 *                         FALSE otherwise.
 */

bool validate_gpu_temperature(double temp)
{
  double min = threshold_data.temp.min;
  double max = threshold_data.temp.max;

  bool ret = (temp >= min) && (temp <= max);
  if (ret == TRUE) {
    slog_message(LOG_DEBUG,  TAG, "Temperature: %.2f is within the range %.2f to %.2f", temp, min, max);
  } else {
    slog_message(LOG_DEBUG,  ETAG "Temperature: %.2f is not within the range %.2f to %.2f", FL, temp, min, max);
  }
  return ret;
}

/**
 * Function:        bool validate_gpu_power(double power)
 * Description:     Validates whether the given GPU power consumption falls within the
 *                  predefined threshold range (min and max). Logs a message indicating
 *                  whether the power value is within range or not.
 * Parameters:      power: double - The GPU power consumption value to validate.
 * Returns:         bool - TRUE if the power value is within the threshold range,
 *                  FALSE otherwise.
 */

bool validate_gpu_power(double power) {
  double min = threshold_data.power.min;
  double max = threshold_data.power.max;

  bool ret = (power >= min) && (power <= max);
  if (ret == TRUE) {
    slog_message(LOG_DEBUG, TAG, "GPU Power: %.2f is within the range %.2f to %.2f", power, min, max);
  } else {
    slog_message(LOG_DEBUG,  ETAG "GPU Power: %.2f is not within the range %.2f to %.2f", FL, power, min, max);
  }
  return ret;
}

/**
 * Function:        bool validate_gpu_voltage(double volt)
 * Description:     Validates whether the given GPU voltage falls within the predefined
 *                  threshold range (min and max). Logs a message indicating whether the
 *                  voltage is within range or not.
 * Parameters:      volt: double - The GPU voltage value to validate.
 * Returns:         bool - TRUE if the voltage is within the threshold range,
 *                         FALSE otherwise.
 */

bool validate_gpu_voltage(double volt) {
  double min = threshold_data.volt.min;
  double max = threshold_data.volt.max;

  bool ret = (volt >= min) && (volt <= max);
  if (ret == TRUE) {
    slog_message(LOG_DEBUG,  TAG, "GPU Voltage: %.2f is within the range %.2f to %.2f", volt, min, max);
  } else {
    slog_message(LOG_DEBUG,  ETAG "GPU Voltage: %.2f is not within the range %.2f to %.2f", FL, volt, min, max);
  }
  return ret;
}

/**
 * Function:        bool validate_gpu_frequency(double freq)
 * Description:     Validates whether the given GPU frequency falls within the predefined
 *                  threshold range (min and max). Logs a message indicating whether the
 *                  frequency is within range or not.
 * Parameters:      freq: double - The GPU frequency value to validate.
 * Returns:         bool - TRUE if the frequency is within the threshold range,
 *                         FALSE otherwise.
 */

bool validate_gpu_frequency(double freq) {
  double min = threshold_data.freq.min;
  double max = threshold_data.freq.max;

  bool ret = (freq >= min) && (freq <= max);
  if (ret == TRUE) {
    slog_message(LOG_DEBUG,  TAG, "Frequency: %.0f is within the range %.0f to %.0f", freq, min, max);
  } else {
    slog_message(LOG_DEBUG,  ETAG "Frequency: %.0f is not within the range %.0f to %.0f", FL, freq, min, max);
  }
  return ret;
}

/**
 * Function:        bool validate_gpu_utilization(double util)
 * Description:     Validates whether the given GPU utilization value falls within the
 *                  predefined threshold range (min and max). Logs a message indicating
 *                  whether the utilization is within range or not.
 * Parameters:      util: double - The GPU utilization value to validate.
 * Returns:         bool - TRUE if the utilization is within the threshold range,
 *                  FALSE otherwise.
 */

bool validate_gpu_utilization(double util) {
  double min = threshold_data.util.min;
  double max = threshold_data.util.max;

  bool ret = (util >= min) && (util <= max);
  if (ret == TRUE) {
    slog_message(LOG_DEBUG,  TAG, "GPU Utilization: %.2f is within the range %.2f to %.2f", util, min, max);
  } else {
    slog_message(LOG_DEBUG,  ETAG "GPU Utilization: %.2f is not within the range %.2f to %.2f", FL, util, min, max);
  }
  return ret;
}

/**
 * Function:        int mountpoint_is_mounted(const char* path)
 * Description:     Checks if a given mount point is already mounted by using the statfs()
 *                  system call. It verifies whether the filesystem type matches DEBUGFS_MAGIC.
 * Parameters:      path: const char* - The path of the mount point to check.
 * Returns:         int - SUCCESS if the directory is mounted,
 *                        FAILURE if it is not mounted or an error occurs.
 */

int mountpoint_is_mounted(const char* path) {
  struct statfs stat;
  if (statfs(path, &stat) == -1) {
    perror("statfs");
    return FAILURE; /* Error occurred, assume not mounted */
  }
  /* Check if the filesystem type is debugfs */
  if (stat.f_type == DEBUGFS_MAGIC) {
    return SUCCESS; /* Directory is mounted */
  }
  return FAILURE; /* Directory is not mounted */
}

/**
 * Function:        void mount_debugfs()
 * Description:     Checks if the debugfs filesystem is already mounted. If not, it attempts
 *                  to mount it at the predefined DEBUGFS_MOUNT_POINT. Logs messages
 *                  indicating whether debugfs was mounted successfully or was already mounted.
 * Parameters:      None.
 * Returns:         void - This function does not return a value. It exits with FAILURE if
 *                         the mount operation fails.
 */

void mount_debugfs() {
  if (mountpoint_is_mounted(DEBUGFS_MOUNT_POINT)) {
    /* Attempt to mount debugfs */
    if (mount("none", DEBUGFS_MOUNT_POINT, "debugfs", 0, NULL) == -1) {
      perror("mount");
      exit(FAILURE);
    }
    slog_message(LOG_INFO,   TAG, "debugfs mounted successfully.");
  } else {
    slog_message(LOG_DEBUG,  TAG, "debugfs is already mounted");
  }
}

/**
 * Function:        int create_test_dir(char *path)
 * Description:     Checks if a directory exists at the specified path. If it does not exist,
 *                  the function attempts to create it with the specified permissions (0755).
 *                  Logs messages indicating whether the directory was created or already exists.
 * Parameters:      path: char* - The path of the directory to check or create.
 * Returns:         int - 0 if the directory exists or is successfully created,
 *                        1 if an error occurs.
 */

int create_test_dir (char *path) {
  /* Check if directory exists */
  struct stat s;
  if (stat(path, &s) == -1) {
    if (errno == ENOENT) {
      /* Directory doesn't exist, create it */
      if (mkdir(path, 0755) == -1) {
        perror("mkdir");
        return 1;
      }
      slog_message(LOG_INFO,  TAG, "Directory '%s' created successfully.", path);
    } else {
      perror("stat");
      return 1;
    }
  } else {
    /* Directory exists */
    slog_message(LOG_DEBUG,  ETAG "Directory '%s' already exists.", FL, path);
  }
  return 0;
}

/**
 * Function:        int check_adbd_process()
 * Description:     Checks if the 'adbd' process is running by executing the "ps | grep adbd"
 *                  command. It searches for 'adbd' in the process list to determine if an
 *                  external ADB shell is active.
 * Parameters:      None.
 * Returns:         int - 1 if the 'adbd' process is found, 0 if it is not found.
 */

int check_adbd_process() {
  /* Check if the 'adbd' process is running */
  FILE *fp = popen("ps | grep adbd", "r");
  if (fp == NULL) {
    slog_message(LOG_ERR,  ETAG "Failed: executing ps command to check adbd shell ");
    return 0;
  }
  char result[256];
  while (fgets(result, sizeof(result), fp) != NULL) {
    if (strstr(result, "adbd") != NULL) {
      fclose(fp);
      /* 'adbd' process is found, in an external ADB shell */
      return 1;  
    }
  }
  fclose(fp);
  return 0;  /* No 'adbd' process found */
}

/**
 * Function:        int clear_logcat_buffer()
 * Description:     Clears the Android logcat buffer by executing the "logcat -c" command
 *                  using popen(). This ensures that previous logs are removed before
 *                  capturing new log messages.
 * Parameters:      None.
 * Returns:         int - SUCCESS if the logcat buffer is cleared successfully,
 *                        FAILURE if an error occurs.
 */

int clear_logcat_buffer() {
  FILE *logcat = popen("logcat -c", "r");
  if (logcat == NULL) {
    slog_message(LOG_ERR,  ETAG "Failed to clear the logcat buffer using popen",FL);
    return FAILURE;
  }
  fclose(logcat);
  return SUCCESS;
}

/**
 * Function:        void redirect_logcat_to_file(ThLoader* th)
 * Description:     Captures logcat output and writes it to a file. It first clears
 *                  the logcat buffer, then starts the logcat process using popen(),
 *                  and continuously reads and writes log messages while the thread
 *                  is running.
 * Parameters:      th: ThLoader* - A pointer to the thread loader structure that
 *                                  contains the log file and a flag to control execution.
 * Returns:         void - This function does not return a value.
 */

void redirect_logcat_to_file(ThLoader* th) {
  /* Clear the logcat buffer before capturing logs */
  if (clear_logcat_buffer() != 0) {
    return;
  }

  /* Use popen() to start the logcat process and redirect, output to a pipe */
  FILE *logcat = popen("logcat", "r");
  if (logcat == NULL) {
    slog_message(LOG_ERR,  ETAG "Failed: running logcat",FL);
    return;
  }

  char buffer[BUFFER_SIZE] = {0};

  /* Read logcat output and write to the file */
    while (atomic_load_explicit(&th->keeprunning, memory_order_relaxed)){
    /* Read a line of output from logcat */
    if (fgets(buffer, sizeof(buffer), logcat) != NULL) {
      /* Write the logcat output to the file */
      fprintf(th->thread_log, "%s", buffer);
    }
  }

  /* Close the logcat process and the output file */
  pclose(logcat);
}

/**
 * Function:        int check_for_root()
 * Description:     Checks if the program is running with root privileges by verifying
 *                  the effective user ID. Logs a message indicating whether the test
 *                  is running as root or not.
 * Parameters:      None.
 * Returns:         int - SUCCESS if running as root, FAILURE otherwise.
 */

int check_for_root() {
  int ret = SUCCESS;
  /* Check if effective user ID is 0 (root) */
  if (geteuid() == 0) {
    slog_message(LOG_INFO,  ETAG "Test Running as root...", FL);
  } else {
    slog_message(LOG_ERR,  ETAG "Failed: Test Not Running as root", FL);
    ret = FAILURE;
  }
  return ret;
}