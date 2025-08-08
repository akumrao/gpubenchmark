/*
 * Copyright (c) 2024-25 akumrao@yahoo.com
 *
 * FileName:      threadload.h
 * Description:   Json config file

 * Author:        Arvind Umrao <aumrao@google.com>
 *                Rajanee Kumbhar <rajaneek@google.com>
 */

#ifndef JSON_H
#define JSON_H

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif


 /* Edit JSON configuration file, add new node to json */
EXTERNC int editJsonConfig(const char *key , const char *value );

/* Initializes and save JSON configuration file for GPU stress testing */
EXTERNC void initJsonConfig(const char *modeName, int stress_time, int log_interval,  char *tmptNode);


#undef EXTERNC

#endif

