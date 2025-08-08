/*
 * Copyright (c) 2024-25 akumrao@yahoo.com
 *
 * FileName:      gpu_parameter.c
 * Description:   File have common code wrapper implementation for
 *                getting frequency, temperature, voltage and power for gpu.
 *
 * Author:        Arvind Umrao <aumrao@google.com>
 */

#include "gpu_common.h"
#include "common.h"

#define TAG "GPU_PARAMETER"
#define ETAG TAG, "[%s:%d] "
#define FL __FILE__,__LINE__

/**
 * Function:        void get_gpu_frequency(char *freqNodePath, char *cfreq)
 * Description:     Reads the current GPU frequency from the specified sysfs node and
 *                  stores it as a string in the provided buffer.
 * Parameters:      freqNodePath: char* - The path to the GPU frequency node in the sysfs filesystem.
 *                  cfreq: char* - A buffer to store the retrieved GPU frequency value as a string.
 * Returns:         void - This function does not return a value.
 */

void get_gpu_frequency(char *freqNodePath, char *cfreq)
{
  FILE *freqFile = fopen(freqNodePath, "r");
  if (!freqFile) {
      slog_message(LOG_ERR,  ETAG "failed: Error to fetch the node: %s", FL, freqNodePath);
  }
  fscanf(freqFile, "%s", cfreq);
  fclose(freqFile);

}

/**
 * Function:        double get_gpu_temperature(char *node)
 * Description:     Reads the GPU temperature from the specified sysfs node and converts
 *                  the raw value to degrees Celsius. The function reads the temperature
 *                  as a string, converts it to a double, and scales it appropriately.
 * Parameters:      node: char* - The path to the GPU temperature node in the sysfs filesystem.
 * Returns:         double - The GPU temperature in degrees Celsius, or -1.0 if an error occurs.
 */

double get_gpu_temperature(char *node)
{
  char temp[32];

  FILE *temp_file = fopen(node, "r");
  if (temp_file == NULL) {
      slog_message(LOG_ERR,  ETAG"Failed: to open temperature node", FL);
    return -1.0;
  }

  /* Read the temperature from the file */
  
  fread(temp,1, 32, temp_file);
  
  fclose(temp_file);

  /* Handle potential conversion errors from atoi */
  double temperature = strtod(temp, NULL);
  return temperature / 1000.0;
}

/**
 * Function:        bool get_raw_voltage(char *node, char *pmic_addr, char *volt_buff)
 * Description:     Reads the raw voltage data from the specified PMIC node by first writing
 *                  the PMIC address to the node and then reading the corresponding voltage
 *                  value into the provided buffer.
 * Parameters:      node: char* - The path to the PMIC voltage node in the sysfs filesystem.
 *                  pmic_addr: char* - The address of the PMIC from which voltage data is requested.
 *                  volt_buff: char* - A buffer to store the retrieved voltage value as a string.
 * Returns:         bool - true if the voltage data is successfully read, false otherwise.
 */

bool get_raw_voltage(char *node, char *pmic_addr, char *volt_buff)
{
  FILE *pmicWfp, *pmicRfp;
  int write_result, read_result;

  /* Open the PMIC node for writing */
  pmicWfp = fopen(node, "w");
  if (pmicWfp == NULL) {
    perror("Error opening PMIC node for writing");
    return false;
  }

  write_result = fprintf(pmicWfp, "%s", pmic_addr);
  if (write_result < 0) {
    perror("Error writing to PMIC node");
    fclose(pmicWfp);
    return false;
  }

  fclose(pmicWfp);

  /* Open the PMIC node for reading */
  pmicRfp = fopen(node, "r");
  if (pmicRfp == NULL) {
    perror("Error opening PMIC node for reading");
    return false;
  }

  read_result = fscanf(pmicRfp, "%[^\n]%*c", volt_buff);
  if (read_result < 0) {
    perror("Error reading from PMIC node");
    fclose(pmicRfp);
    return false;
  }

  fclose(pmicRfp);
  return true;
}

/**
 * Function:        bool get_gpu_voltage(char *node, char *pmic_addr, double *volt)
 * Description:     Retrieves the GPU voltage by reading raw voltage data from the specified
 *                  power management IC (PMIC) address and calculating the voltage using a
 *                  predefined formula.
 * Parameters:      node: char* - The path to the voltage node in the sysfs filesystem.
 *                  pmic_addr: char* - The address of the PMIC from which voltage data is read.
 *                  volt: double* - A pointer to a double where the calculated voltage will be stored.
 * Returns:         bool - true if the voltage is successfully retrieved and calculated,
 *                         false otherwise.
 */

bool get_gpu_voltage(char *node, char *pmic_addr, double *volt)
{
  int value;
  char volt_buff[64] = {0};

  if( get_raw_voltage(node, pmic_addr, volt_buff)) {

      sscanf(volt_buff, "%x", &value);
      /* Get the voltage calculation formula from pmic data sheet for specific rail */
      *volt = (0.24 + ((value * 5.0) / 1000));
      return true;
  }

  return false;
}

/**
 * Function:        int parse_line(const char *line, power_data_point_t *data_point)
 * Description:     Parses a line of text and extracts power-related data, including
 *                  channel number, timestamp, rail name, and energy value. The extracted
 *                  data is stored in the provided power_data_point_t structure.
 * Parameters:      line: const char* - A string containing the line to be parsed.
 *                  data_point: power_data_point_t* - A pointer to a structure where the
 *                                                    extracted data will be stored.
 * Returns:         int - SUCCESS if parsing is successful, FAILURE otherwise.
 */

int parse_line(const char *line, power_data_point_t *data_point) {
  /* Format specifier for sscanf */
  const char *format_spec = "CH%d(T=%ld)[%[^]]], %ld";

  /* Check for expected format pattern ->  CH42(T=176090328)[S2S_VDD_GPU], 168620176 */

  if (sscanf(line, format_spec, &data_point->channel, &data_point->timestamp, &data_point->rail_name, &data_point->energy_value) != 4) {
    return FAILURE;
  }
  return SUCCESS;
}

/**
 * Function:        int get_rail_power(char *node, char *rail, power_data_point_t* power_data)
 * Description:     Reads power data from the specified node file and extracts information
 *                  for the given power rail. It parses each line and checks if the rail name
 *                  matches the specified rail.
 * Parameters:      node: char* - The path to the power node file in the sysfs filesystem.
 *                  rail: char* - The name of the power rail to search for in the file.
 *                  power_data: power_data_point_t* - A pointer to a structure where the
 *                                                    extracted power data will be stored.
 * Returns:         int - SUCCESS if the rail's power data is found and extracted,
 *                        FAILURE if not found or an error occurs.
 */

int get_rail_power(char *node, char *rail, power_data_point_t* power_data)
{
  FILE *file = fopen(node, "r");
  if (file == NULL) {
      slog_message(LOG_ERR,  ETAG "Failed: to open power node file: %s", FL, strerror(errno));
      return false;
  }

  char line[BUFFER_SIZE];
  if(fgets(line, BUFFER_SIZE, file) != NULL) {}

  while (fgets(line, BUFFER_SIZE, file) != NULL) {
    if (parse_line(line, power_data) != 0) {
      fprintf(stderr, "Error parsing line: %s\n", line);
      fclose(file);
      return FAILURE;
    }
    if (strcmp(power_data->rail_name, rail) == 0) {
      fclose(file);
      return SUCCESS;
    }
  }
  fclose(file);
  return FAILURE;
}

/**
 * Function:        int get_gpu_utilization(const char *node, const char *board)
 * Description:     Reads and returns the GPU utilization percentage from the specified
 *                  sysfs node. It searches for the keyword "GPU Utilisation:" in the file
 *                  and extracts the corresponding value. For the "JUMA" board, it reads
 *                  the first 31 bytes directly.
 * Parameters:      node: const char* - The path to the GPU utilization node in the sysfs filesystem.
 *                  board: const char* - The name of the board to determine specific handling logic.
 * Returns:         int - The extracted GPU utilization percentage, or -1 if an error occurs
 *                        or if the value is not found.
 */

int get_gpu_utilization(const char *node , const char *board ) {
    char buffer[BUFFER_SIZE];
    const char *keyword = "GPU Utilisation:";
    int utilization = -1; // Default value if not found

    if (node == NULL) {
      return -1;
    }

    FILE *file = fopen(node, "r");
    if (file == NULL) {
        slog_message(LOG_ERR, ETAG"Failed: to open GPU Utilisation node", FL);
      return -1;
    }

    if(!strcmp( board,  "JUMAPRO" ))
    {
        fread(buffer,1, 31, file);
        utilization = strtod(buffer, NULL);
        fclose(file);
        return utilization;
    }

    /* Read each line from the file */
    while (fgets(buffer, sizeof(buffer), file)) {
        /* Check if the line contains the keyword */
        if (strstr(buffer, keyword)) {
            /* Find the position of the keyword in the line */
            char *start = strstr(buffer, keyword);
            if (start) {
                /* Move the pointer to the position after the keyword */
                start += strlen(keyword);

                /* Skip any whitespace */
                while (*start == ' ' || *start == '\t' || *start == ':') {
                    start++;
                }

                /* Extract the numeric value */
                utilization = strtod(start, NULL);
                break;
            }
        }
    }

    fclose(file);
    return utilization;
}

/**
 * Function:        void get_temperature_type(char *node)
 * Description:     Retrieves and logs the type of temperature sensor based on the provided
 *                  sysfs node path. It constructs the path to the "type" file associated
 *                  with the temperature node and reads its content.
 * Parameters:      node: char* - The path to the temperature sensor node in the sysfs filesystem.
 * Returns:         void - This function does not return a value. It logs the temperature
 *                         sensor type if the "type" file is successfully read.
 */

void get_temperature_type(char *node) {
  char temp[10];
  /* To print node type of temperature */
  char type_path[100];
  char *last_slash = strrchr(node, '/');
  strncpy(type_path, node, last_slash - node);
  type_path[last_slash - node] = '\0';
  strcat(type_path, "/type");

  FILE *type = fopen(type_path, "r");
  if ( type && fgets(temp, sizeof(temp), type) != NULL) {
    temp[strcspn(temp, "\n")] = '\0';
    slog_message(LOG_INFO,  TAG, "Temperature node type is %s", temp);
  }
  if(type)
  fclose(type);
}
