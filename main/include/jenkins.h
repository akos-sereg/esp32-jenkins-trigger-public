#ifndef __jenkins_h_included__
#define __jenkins_h_included__

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#define JENKINS_ON_HTTPS

/* Jenkins instance when using HTTP */
#define WEB_SERVER                  CONFIG_WEB_SERVER
#define WEB_SERVER_URL              CONFIG_WEB_SERVER_URL
#define WEB_PORT                    CONFIG_WEB_PORT
#define WEB_PORT_HTTPS              CONFIG_WEB_PORT_HTTPS

/*
    Configure jenkins jobs for protected switches.
    Jenkins job name usually available in url: http://<jenkins>/job/<job-name>
*/
#define JENKINS_JOB_1   CONFIG_JENKINS_JOB_1
#define JENKINS_JOB_2   CONFIG_JENKINS_JOB_2
#define JENKINS_JOB_3   CONFIG_JENKINS_JOB_3

/*
    Base64 encoded "BASIC" authentication header: should be base64(user:apikey)
*/
#define JENKINS_AUTH	CONFIG_JENKINS_AUTH

/*
    Number of attempts we are trying to fetch Job Number

    After starting a Build, the task will be in the Queue, so the build itself wont start immediately. We are
    trying to request the Job number (assuming that the job is started) this many times. Between each attempts,
    we wait DELAY_BEFORE_RETRY seconds.

    If we fail to get the Job Number, we consider the build as "failed".
*/
#define RETRY_JOB_NUMBER	5

/*
    Number of seconds between Job Number fetching attempts.
*/
#define DELAY_BEFORE_RETRY	5

extern int IS_BUILD_FAILED;
extern int IS_BUILD_IN_PROGRESS;
extern int IS_BUILD_SUCCEEDED;
extern int JOB_NUMBER;

void trigger_build(int jenkinsJob);
void get_job_number();
void jenkins_check_status();

#endif

