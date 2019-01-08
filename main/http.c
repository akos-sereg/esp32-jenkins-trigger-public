#include "include/http.h"
#include "include/jenkins.h"
#include "include/networking.h"
#include "include/led-displays.h"

const struct addrinfo hints = {
    .ai_family = AF_INET,
    .ai_socktype = SOCK_STREAM,
};
struct addrinfo *res;
struct in_addr *addr;
int address_retrieved = 0;

void do_http_request(const char *request)
{
    int s, r;
    char recv_buf[64];

    charCount = 0;
    xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
                        false, true, portMAX_DELAY);
    ESP_LOGI(TAG, "Connected to AP");

    int err = 0;

    if (address_retrieved != 1) {
	err = getaddrinfo(WEB_SERVER, WEB_PORT, &hints, &res);
	ESP_LOGI(TAG, "Got address info");

	if(err != 0 || res == NULL) {
    	    ESP_LOGE(TAG, "DNS lookup failed err=%d res=%p", err, res);
	    IS_BUILD_FAILED = 1;
	    IS_BUILD_IN_PROGRESS = 0;
	    refresh_led_screen();
    	    return;
	}

	addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
	ESP_LOGI(TAG, "DNS lookup succeeded. IP=%s", inet_ntoa(*addr));
	address_retrieved = 1;
    }

    err = getaddrinfo(WEB_SERVER, WEB_PORT, &hints, &res);
    ESP_LOGI(TAG, "Got address info");

    if(err != 0 || res == NULL) {
        ESP_LOGE(TAG, "DNS lookup failed err=%d res=%p", err, res);
	IS_BUILD_FAILED = 1;
	IS_BUILD_IN_PROGRESS = 0;
	refresh_led_screen();
        return;
    }

    addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
    ESP_LOGI(TAG, "DNS lookup succeeded. IP=%s", inet_ntoa(*addr));

    s = socket(res->ai_family, res->ai_socktype, 0);
    if(s < 0) {
        ESP_LOGE(TAG, "... Failed to allocate socket.");
        freeaddrinfo(res);
	IS_BUILD_FAILED = 1;
	IS_BUILD_IN_PROGRESS = 0;
	refresh_led_screen();
	return;
    }

    ESP_LOGI(TAG, "... allocated socket");

    if(connect(s, res->ai_addr, res->ai_addrlen) != 0) {
        ESP_LOGE(TAG, "... socket connect failed errno=%d", errno);
        close(s);
        freeaddrinfo(res);
	IS_BUILD_IN_PROGRESS = 0;
	IS_BUILD_FAILED = 1;
	refresh_led_screen();
	return;
    }

    ESP_LOGI(TAG, "... connected");
    freeaddrinfo(res);

    ESP_LOGI(TAG, "Sending request: %s", request);

    if (write(s,
	request,
	strlen(request)) < 0) {
        ESP_LOGE(TAG, "... socket send failed");
        close(s);
	IS_BUILD_FAILED = 1;
	IS_BUILD_IN_PROGRESS = 0;
	refresh_led_screen();
	return;
    }
    ESP_LOGI(TAG, "... socket send success");

    struct timeval receiving_timeout;
    receiving_timeout.tv_sec = REQUEST_TIMEOUT_IN_SECONDS;
    receiving_timeout.tv_usec = 0;
    if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout,
            sizeof(receiving_timeout)) < 0) {
        ESP_LOGE(TAG, "... failed to set socket receiving timeout");
        close(s);
	IS_BUILD_FAILED = 1;
	IS_BUILD_IN_PROGRESS = 0;
	refresh_led_screen();
	return;
    }
    ESP_LOGI(TAG, "... set socket receiving timeout success");

    do {
        bzero(recv_buf, sizeof(recv_buf));
        r = read(s, recv_buf, sizeof(recv_buf)-1);
        for(int i = 0; i < r; i++) {
	    if (charCount < sizeof(response_payload)) {
		response_payload[charCount++] = recv_buf[i];
	    }
        }
    } while(r > 0);

    response_payload[charCount] = '\0';

    ESP_LOGI(TAG, "... done reading from socket. Last read return=%d errno=%d\r\n", r, errno);
    ESP_LOGI(TAG, "Payload: %s\r\n", response_payload);
    close(s);
}

