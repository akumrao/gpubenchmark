/*
 * Copyright (c) 2024-25 akumrao@yahoo.com
 *
 * FileName:      gpu_parameter.c
 * Description:   File have common code wrapper implementation for
 *                getting frequency, temperature, voltage and power for gpu.
 *
 * Author:        Arvind Umrao <aumrao@google.com>
 */

#ifndef GPU_PARAMETER_H
#define GPU_PARAMETER_H

/* Get the temp value from the node and convert into readable form */
double get_gpu_temperature(char *node);

/* Get the node type for given temperature node */
void get_temperature_type(char *node);

/* Function to get the current frequency value for gpu */
void get_gpu_frequency(char *freqNodePath, char *cfreq);

/* Function to get the converted voltage value for given rail node and address */
bool get_gpu_voltage(char *node, char *pmic_addr, double *volt );

/* To get rail energy value from the node for given rail */
int get_rail_power(char *node, char *rail, power_data_point_t* power_data);

/* Function to get the utilization of GPU */
int get_gpu_utilization(const char *node, const char *board );

#endif
