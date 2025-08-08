/*
 * Copyright (c) 2024-25 akumrao@yahoo.com
 *
 * FileName:      app.h
 * Description:   File have code related with android app install process
 *
 * Author:        Arvind Umrao <aumrao@google.com>
 */

#ifndef APP_H
#define APP_H

#define TAG "TC_GPU_001"
#define ETAG TAG, "[%s:%d] "
#define FL __FILE__,__LINE__

#include "monitor.h"

/*Checks if a given application package is installed on the system.*/
int is_app_installed(const char *package_name, bool *is_installed);

/*Install App*/
int install_app(const char *apk);

#endif
