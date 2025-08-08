/*
 * Copyright (c) 2024-25 akumrao@yahoo.com
 *
 * Filename:        gpu_001.c
 * TestCase:        GPU_001
 * TestCase Name:   GPU Stress Test
 * Description:     The GPU stress test aims to push the GPU to its limits,
 *                  evaluating its performance, stability, and error handling
 *                  under heavy workloads.  Ensuring the GPU can
 *                  handle demanding applications while maintaining stability.
 * Test Steps:
 *                  It performs the following tasks:
 *                  1. Initializes configuration by parsing the config file.
 *                  2. Validate command line argument.
 *                  3. Sets up logging for the test results.
 *                  4. Checks for root permissions.
 *                  5. Retrieves kernel and build version information.
 *                  6. Initializes parameter thresholds and starts parameter
 *                     capture threads if required.
 *                  7. Executes the GPU stress test.
 *                  8. Verifies stress test outputs and logged error.
 *                  9. Cleans up and finalizes test results.
 *
 * Author:          Arvind Umrao <aumrao@google.com> and Team
 */

#include "gpu_common.h"
#include "common.h"
#include "monitor.h"
#include "json.h"
#include "app.h"
#include "slogger.h"
#include "spwan.h"
#include "condwait.h"
#include "threadload.h"
#include <stdbool.h>

/* This variable hold Android package name */
const char *package_name = "org.gpu.glload.debug";
/* Variable to hold the path to APK*/
const char *apk = "android-arm64-v8a-debug.apk";
/* Global variable to store  time value */
char *duration = "60";
int stress_time = 60;

/* This variable holds the configuration file parameters and their values */
config_t config_parameter;

/* Default logging level for console output */
int console_loglevel = LOG_INFO;
/* Variable holds type of log have to be used */
log_type_t log_type;
/* Variable to store load type */
gpuload_type_t load_type = TILES;



parameter_threshold_t threshold_data;

//const char *stress_binary = "stress-ng";  /* This variable hold stressor binary value */ //[SA]


Condwait condwait = (Condwait){ condwait_int, condwait_wait, condwait_signal, condwait_stop };

void intHandler(int sig)
{ // can be called asynchronously

    condwait.signal(&condwait);

    printf("Caught signal: %d\n", sig );

}

/**
 * Function:      main
 * Description:   Main routine for cpu stress test.
 * param[in]:     argc: stand for argument count.
 * param[in]:     argv: hold the user provided arguments.
 */

int main(int argc, char *argv[])
{

    print_banner();

    signal(SIGINT, intHandler);
    signal(SIGTSTP, intHandler);
    signal(SIGTERM, intHandler);
    signal(SIGHUP, intHandler);

    condwait.init(&condwait);

    

    /* Parse the configuration file */
    if (config_parser(CONFIG_FILE_NAME, &config_parameter) != 0) {
        log_message(LOG_ERR, NULL,  ETAG "Failed: GPU_001 Config file parser", FL);
         return FAILURE;
    }



    //Slogger slogger = (Slogger){   slog_start, slog_stop, "/data/local/tmp/GPU001.log" };
    Slogger slogger = (Slogger){   slog_start, slog_stop, "", "GPU001.log" };
    strcpy(slogger.logPath,config_parameter.output_path);
    slogger.start(&slogger);



     /* Check if the test is running with root permission */
    if (check_for_root(TAG) != SUCCESS) {
        return FAILURE;
    }


    /* Parse and validate optional command line arguments */
    if (parse_validate_optarguments(argc, argv) != 0) {
        gpu_001_usage(argv[0]);
         return FAILURE;
    }

    if(load_type == INSTALL) {
        bool is_installed = false;
        #if 0
        if (is_app_installed(package_name, &is_installed) != SUCCESS) {
            log_message(LOG_ERR, NULL, ETAG "Error checking if app is installed.", FL);
            return FAILURE;
        }
        #endif

        if (!is_installed) {
            if (install_app(apk) != SUCCESS) {
                log_message(LOG_ERR, NULL, ETAG "App installation failed.", FL);
                return FAILURE;
            } else {
                system("am start -n org.gpu.glload.debug/org.gpu.glload.MainActivity");
                initJsonConfig("tiles", stress_time, config_parameter.log_interval,
                               config_parameter.temperature_node);

                log_message(LOG_INFO, NULL, TAG,
                            "App installation done, please re run the test script to complete the test");

                return SUCCESS;
            }
        }
    }

    /* Function to print configured values */
    print_config();

    /* Check the kernel and build version */
    if (!validate_kernel_and_build_version()) {
        slog_message(LOG_WARN,  ETAG "Failed: version check, version info does not match", FL);
        //return FAILURE;
    }


    /* Initialize parameter threshold for specific load mode */
    initialize_parameter_threshold(&threshold_data);

    slog_message(LOG_INFO,  TAG, "GPU Test Started");

    ThLoader thParameter= (ThLoader){ thload_start, monitor_gpu_parameter, thload_stop, 0, "", "gpu001_parameter_output.txt"  };
    strcpy(thParameter.logPath,config_parameter.output_path);
    thParameter.start(&thParameter);

//    ThLoader thPerf= (ThLoader){ thload_start, capture_gpu_performance, thload_stop, 0, "", "gpu001_kpi_output.txt"  };
//    strcpy(thPerf.logPath,config_parameter.output_path);
//    thPerf.start(&thPerf);

    /* Check the test running from adb external or internal shell */
    if (check_adbd_process()) {
        printf("*** Test Running as External ADB shell (host) ***\n");
    } else {
        printf( "*** Test Running as Internal ADB shell (device) ***\n");
    }

    ThLoader thlogcat = (ThLoader){ thload_start, redirect_logcat_to_file, thload_stop, 0, "" , "gpu001_logcat_output.txt"  };
      strcpy(thlogcat.logPath,config_parameter.output_path);
    /* When logcat is configured in config file, dump logcat output into a file */
    if (log_type == LOG_TYPE_LOGCAT) {
        thlogcat.start(&thlogcat);
    }

    Exec thexecMp;
    char* arg_list[] = {  "/data/local/tmp/gpu001/mpstat",  "-P",   "ALL", "1", "-intv", "200", NULL  };

    if(load_type < USAGE ) {
        thexecMp = (Exec){ exec_start, exec_run, exec_stop,  "/data/local/tmp/gpu001/", "gpu001_cpuusages.txt", arg_list};
        strcpy(thexecMp.logPath,config_parameter.output_path);
        thexecMp.start(&thexecMp);
    }


    /* data/local/tmp/cpu001/stress-ng --cpu 8 --cpu-method all --timeout 60 --vm 8 --vm-bytes 100% --vm-method flip --temp-path /data/local/tmp --metrics-brief -v --verify --timestamp */

    Exec thStressng;
    char* arg_sressng[] = {  "/data/local/tmp/gpu001/stress-ng",  "--cpu", "8", "--cpu-method", "all", "--timeout", "50", "--vm", "8", "--vm-bytes", "100%", "--vm-method", "flip", "--temp-path", "/data/local/tmp", "--metrics-brief", "-v", "--verify", "--timestamp", NULL  };
    if(load_type == CPU) {
        thStressng = (Exec) {exec_start, exec_run, exec_stop, "/data/local/tmp/gpu001/",
                             "gpu001_stressng.txt", arg_sressng};
        strcpy(thStressng.logPath, config_parameter.output_path);
        thStressng.start(&thStressng);
    }

    if(load_type != IDLE)
    {
        system("am force-stop org.gpu.glload.debug");
        char am_command[512];
        snprintf(am_command, sizeof(am_command),
                 "am start -n org.gpu.glload.debug/org.gpu.glload.MainActivity "
                 "-a android.intent.action.MAIN -e TYPE LOW -e DURATION %d -e ROW %d",
                 stress_time, 1);

        int status = system(am_command);
        if(status)
            slog_message(LOG_ERR,  ETAG "Command execution failed to launch GPULoad APP", FL);
    }


#if 0
   run_gpu_stress();


   /* Verifying the parameter output */
   verify_parameter_output();

   /* Verifying the stress-ng output log */
   if (strcmp(stress_binary, "stress-ng") == 0) {
       if (verify_stress_output() != 0) {
           slog_message(LOG_ERR,  ETAG "GPU Clock Switch test output verification failed!!!", FL);
           exit(EXIT_FAILURE);
       }
   }

   if (log_type == LOG_TYPE_LOGCAT) {
       fclose(logcat_fp);
   }


   ThLoader tloaderArr[NOOFTHREADLOAD];

   for(int n=0 ; n < NOOFTHREADLOAD ; ++n ) {
       tloaderArr[n] = (ThLoader){ thload_start, thload_run, thload_stop, 1 , "/data/local/tmp/"  };
       tloaderArr[n].start(&tloaderArr[n]);
   }

 #endif

    int sigStatus = condwait.wait(&condwait, stress_time, 10);

    thParameter.stop(&thParameter);
//    thPerf.stop(&thPerf);

    if(load_type < USAGE ) {
        thexecMp.stop(&thexecMp);
    }
    if(load_type == CPU)
    thStressng.stop(&thStressng);

    if(!sigStatus) {
        slog_message(LOG_WARN, TAG,"Caught signal");
    }
    else
    {
        slog_message(LOG_INFO,  TAG, "Parameter logs: gpu001_parameter_output.txt");
        slog_message(LOG_INFO,  TAG, "KPI logs: gpu_kpi*.txt");
        slog_message(LOG_INFO,  TAG, "Results log: gpu_result.log");
        slog_message(LOG_INFO,  TAG, "Error logs: GPU001.log");
        if(load_type < USAGE ) {
            slog_message(LOG_INFO,  TAG, "CPU usages: cpu_tc_001_usages.txt");
        }
        if(load_type == CPU) {
            slog_message(LOG_INFO,  TAG, "CPU Load:gpu001_stressng.txt");
        }
        thlogcat.stop(&thlogcat);
    }
   
    slog_message(LOG_INFO,  TAG, "GFU001 Completed!!!");
    
    slogger.stop(&slogger);
    condwait.stop(&condwait);
    if(!sigStatus) {
        system("am force-stop org.gpu.glload.debug");
    }

    exit(EXIT_SUCCESS);
}
