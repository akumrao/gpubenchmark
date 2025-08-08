/*
 * Copyright (c) 2024-25 akumrao@yahoo.com
 *
 * FileName:      app.h
 * Description:   File have code related with android app install process
 *
 * Author:        Arvind Umrao <aumrao@google.com>
 */

#include <stdio.h>
#include "common.h"
#include "app.h"

#define TAG "TC_GPU_001"
#define ETAG TAG, "[%s:%d] "


/**
 * Function:        int is_app_installed(const char *package_name, bool *is_installed)
 * Description:     Checks if a given application package is installed on the system.
 * Parameters:      package_name: const char* - Name of the package to check.
 *                  is_installed: bool*       - Pointer to a boolean value that will be
 *                                              set to true if the package is installed,
 *                                              otherwise false.
 * Returns:         SUCCESS (0)  - If the check is performed successfully.
 *                  FAILURE (-1) - If an error occurs (e.g., invalid arguments or command failure).
 */

int is_app_installed(const char *package_name, bool *is_installed) {
    if (package_name == NULL || is_installed == NULL) {
        slog_message(LOG_ERR,  ETAG "Invalid arguments provided..", FL);
        return FAILURE;
    }

    char command[256];
    snprintf(command, sizeof(command), "pm list packages | grep %s", package_name);

    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        slog_message(LOG_ERR,  ETAG "popen failed: %s", FL, command);
        return FAILURE;
    }

    char result[1024];
    *is_installed = false;

    /* Read the output of the command */
    while (fgets(result, sizeof(result), fp) != NULL) {
        if (strstr(result, package_name) != NULL) {
            *is_installed = true;
            break;
        }
    }

    pclose(fp);
    return SUCCESS;
}

/**
 * Function:        int install_app(const char *apk)
 * Description:     Installs an application package (APK) using the package manager (pm).
 * Parameters:      apk: const char*  - Path to the APK file to be installed.
 * Returns:         SUCCESS (0)  - If the installation is successful.
 *                  FAILURE (-1) - If an error occurs or the APK path is invalid.
 */

int install_app(const char *apk) {
    if (apk == NULL) {
        fprintf(stderr, "Invalid APK path provided.\n");
        slog_message(LOG_ERR,  ETAG "App installation failed.", FL);
        return FAILURE;
    }

    char command[256];
    snprintf(command, sizeof(command), "pm install -t %s/%s", TEMP_PATH, apk);

    int result = system(command);
    if (result != 0) {
        fprintf(stderr, "Failed to install app from %s.\n", apk);

        return FAILURE;
    }

    return SUCCESS;
}
