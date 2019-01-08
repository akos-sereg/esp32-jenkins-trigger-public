#include "include/jenkins.h"
#include "include/networking.h"
#include "include/led-displays.h"
#include "include/http.h"
#include "include/https.h"
#include "include/utils.h"

int IS_BUILD_IN_PROGRESS = 0;
int IS_BUILD_FAILED = 0;
int IS_BUILD_SUCCEEDED = 0;
int JOB_NUMBER = 0;
int QUEUE_NUMBER = 0;
int JENKINS_JOB_INDEX = 0;
char request[512];

static const char *REQUEST_1_PATH = "job/" JENKINS_JOB_1 "/build";
static const char *REQUEST_JENKINS_JOB_1 = "POST /job/" JENKINS_JOB_1 "/build HTTP/1.0\r\n"
    "Host: "WEB_SERVER"\r\n"
    "User-Agent: esp-idf/1.0 esp32\r\n"
    "Authorization: Basic "JENKINS_AUTH"\r\n"
    "\r\n";

static const char *REQUEST_2_PATH = "job/" JENKINS_JOB_2 "/build";
static const char *REQUEST_JENKINS_JOB_2 = "POST /job/" JENKINS_JOB_2 "/build HTTP/1.0\r\n"
    "Host: "WEB_SERVER"\r\n"
    "User-Agent: esp-idf/1.0 esp32\r\n"
    "Authorization: Basic "JENKINS_AUTH"\r\n"
    "\r\n";

static const char *REQUEST_3_PATH = "job/" JENKINS_JOB_3 "/build";
static const char *REQUEST_JENKINS_JOB_3 = "POST /job/" JENKINS_JOB_3 "/build HTTP/1.0\r\n"
    "Host: "WEB_SERVER"\r\n"
    "User-Agent: esp-idf/1.0 esp32\r\n"
    "Authorization: Basic "JENKINS_AUTH"\r\n"
    "\r\n";

int get_queue_number(char *payload) {
    char *location_header;
    char queue_number[10];
    int i;
    int queue_number_length = 0;

    ESP_LOGI(TAG, "get_queue_number: looking for queue number in response payload: '%s'", payload);
    location_header = get_line(payload, "Location:");
    if (location_header == NULL) {
        ESP_LOGI(TAG, "get_queue_number: location header not found, aborting");
        return -1;
    }

    ESP_LOGI(TAG, "get_queue_number: location header line is '%s'", location_header);

    for (i = strlen(location_header) - 10; i != strlen(location_header); i++) {
        if (location_header[i] >= 48 && location_header[i] <= 57) {
            queue_number[queue_number_length++] = location_header[i];
        }
    }

    queue_number[queue_number_length] = '\0';

    ESP_LOGI(TAG, "get_queue_number: returning queue string to int: '%s'", queue_number);
    return atoi(queue_number);
}

int get_job_number_from_payload(char *response_payload) {
    char numberBuffer[16];
    int numberBufferChar = 0;
    int i;
    char *pfound = strstr(response_payload, "number\":");

    if (pfound == NULL) {
	    return -1;
    }

    int dposfound = (int) (pfound - response_payload) + 8;
    for (i=dposfound; i!=dposfound+16; i++) {
        if (i >= strlen(response_payload)) {
            return -1;
        }

        ESP_LOGI(TAG, "-> %c", response_payload[i]);

        if (response_payload[i] >= 48 && response_payload[i] <= 57) {
            numberBuffer[numberBufferChar] = response_payload[i];
            numberBufferChar++;
        }
        else {
            break;
        }
    }

    numberBuffer[numberBufferChar] = '\0';
    return atoi(numberBuffer);
}


void get_job_number()
{
    int retry_count = 0;
    char request_url[256];

    JOB_NUMBER = -1;

    if (QUEUE_NUMBER == -1) {
        ESP_LOGI(TAG, "get_job_number: unable to get job number as queue number is not set");
        return;
    }

    sprintf(request_url, "/queue/item/%d/api/json", QUEUE_NUMBER);
    sprintf(request, "GET %s HTTP/1.0\r\nHost: %s\r\nUser-Agent: esp-idf/1.0 esp32\r\nAuthorization: Basic %s\r\n\r\n",
	request_url,
	WEB_SERVER,
	JENKINS_AUTH);

    while (JOB_NUMBER == -1) {
        ESP_LOGI(TAG, "get_job_number: attempting to request for job number, attempt count: %d/%d ...", (retry_count + 1), RETRY_JOB_NUMBER);

#ifdef JENKINS_ON_HTTPS
        do_https_request(request);
#else
        do_http_request(request);
#endif

        JOB_NUMBER = get_job_number_from_payload(response_payload);

        if (JOB_NUMBER == -1) {
            ESP_LOGI(TAG, "get_job_number: could not retrieve job number, still in queue, waiting 5 seconds and re-trying");
        }
        else {
            ESP_LOGI(TAG, "get_job_number: job number resolved as '%d'", JOB_NUMBER);
        }

        retry_count++;
        if (retry_count > RETRY_JOB_NUMBER && JOB_NUMBER == -1) {
            IS_BUILD_FAILED = 1;
            IS_BUILD_IN_PROGRESS = 0;
            IS_BUILD_SUCCEEDED = 0;
            ESP_LOGI(TAG, "get_job_number: giving up after 3 attempts, could not fetch job number, job is still waiting in the queue");
            break;
        }

        vTaskDelay((DELAY_BEFORE_RETRY * 1000) / portTICK_RATE_MS);
    }

    if (JOB_NUMBER != -1) {
	    ESP_LOGI(TAG, "get_job_number: job number fetched: '%d'", JOB_NUMBER);
    }
}

void trigger_build(int jenkinsJob)
{
    char request_url[256];
    const char *request_payload;

    QUEUE_NUMBER = -1;
    JOB_NUMBER = -1;
    JENKINS_JOB_INDEX = jenkinsJob;

    IS_BUILD_FAILED = 0;
    IS_BUILD_SUCCEEDED = 0;
    IS_BUILD_IN_PROGRESS = 1;
    refresh_led_screen();

    ESP_LOGI(TAG, "trigger_build: assembling request payload and url");

    switch(jenkinsJob) {
        case 1:
            request_payload = REQUEST_JENKINS_JOB_1;
            sprintf(request_url, "/%s", REQUEST_1_PATH);
            break;
        case 2:
            request_payload = REQUEST_JENKINS_JOB_2;
            sprintf(request_url, "/%s", REQUEST_2_PATH);
            break;
        case 3:
            request_payload = REQUEST_JENKINS_JOB_3;
            sprintf(request_url, "/%s", REQUEST_3_PATH);
            break;
        default:
            ESP_LOGI(TAG, "Unknown Jenkins job index: %d (should be 1, 2 or 3)", jenkinsJob);
            return;
            break;
    }

    ESP_LOGI(TAG, "trigger_build: sending http request");

#ifdef JENKINS_ON_HTTPS
    do_https_request(request_payload);
#else
    do_http_request(request_payload);
#endif

    ESP_LOGI(TAG, "trigger_build: http request sent, response received");

    char *response_status_created = strstr(response_payload, "201 Created");
    ESP_LOGI(TAG, "trigger_build: detecting response status");

    if (response_status_created == NULL) {
        IS_BUILD_FAILED = 1;
        IS_BUILD_SUCCEEDED = 0;
        IS_BUILD_IN_PROGRESS = 0;
        ESP_LOGI(TAG, "trigger_build: considering build-failed because we could not create Jenkins job");
    } else {
        IS_BUILD_FAILED = 0;
        IS_BUILD_SUCCEEDED = 0;
        IS_BUILD_IN_PROGRESS = 1;

        ESP_LOGI(TAG, "trigger_build: fetching queue number");
        QUEUE_NUMBER = get_queue_number(response_payload);
        ESP_LOGI(TAG, "trigger_build: Queue number is %d", QUEUE_NUMBER);
    }

    get_job_number();
    refresh_led_screen();
}

void jenkins_check_status() {

    char request_url[256];

    if (JOB_NUMBER == -1) {
        ESP_LOGI(TAG, "jenkins_check_status: ignoring status check, job number is not set yet. probably job is still in queue.");
        return;
    }

    ESP_LOGI(TAG, "jenkins_check_status: checking job status %d", JOB_NUMBER);

    switch(JENKINS_JOB_INDEX) {
        case 1:
            sprintf(request_url, "/job/%s/%d/api/json", JENKINS_JOB_1, JOB_NUMBER);
            break;
        case 2:
            sprintf(request_url, "/job/%s/%d/api/json", JENKINS_JOB_2, JOB_NUMBER);
            break;
        case 3:
            sprintf(request_url, "/job/%s/%d/api/json", JENKINS_JOB_3, JOB_NUMBER);
            break;
        default:
            ESP_LOGI(TAG, "Unknown Jenkins job index: %d (should be 1, 2 or 3)", JENKINS_JOB_INDEX);
            return;
            break;
    }

    sprintf(request, "GET %s HTTP/1.0\r\nHost: %s\r\nUser-Agent: esp-idf/1.0 esp32\r\nAuthorization: Basic %s\r\n\r\n",
        request_url,
        WEB_SERVER,
        JENKINS_AUTH);

    ESP_LOGI(TAG, "jenkins_check_status: sending request");

#ifdef JENKINS_ON_HTTPS
    do_https_request(request);
#else
    do_http_request(request);
#endif

    char *status_in_progress = strstr(response_payload, "\"result\":null");
    char *status_success = strstr(response_payload, "\"result\":\"SUCCESS\"");
    char *status_failed = strstr(response_payload, "\"result\":\"FAIL");

    if (status_in_progress != NULL) {
        ESP_LOGI(TAG, "Jenkins Job %d is IN PROGRESS", JOB_NUMBER);
        return;
    }

    if (status_success != NULL) {
        IS_BUILD_SUCCEEDED = 1;
        IS_BUILD_IN_PROGRESS = 0;
        IS_BUILD_FAILED = 0;
        refresh_led_screen();
        ESP_LOGI(TAG, "Jenkins Job %d SUCCEEDED", JOB_NUMBER);
        return;
    }

    if (status_failed != NULL) {
        IS_BUILD_SUCCEEDED = 0;
        IS_BUILD_IN_PROGRESS = 0;
        IS_BUILD_FAILED = 1;
        refresh_led_screen();
        ESP_LOGI(TAG, "Jenkins Job %d FAILED", JOB_NUMBER);
    }
}
