#include "include/https.h"
#include "include/jenkins.h"
#include "include/networking.h"

esp_tls_cfg_t global_cfg;
int global_cfg_set = 0;

void do_https_request(const char *request) {

    int ret, len;
    char buf[128];
    struct esp_tls *tls;

    charCount = 0;

    ESP_LOGI(TAG, "do_https_request: initializing ...");

    if (!global_cfg_set) {
        esp_tls_cfg_t cfg = {
            .cacert_pem_buf  = server_root_cert_pem_start,
            .cacert_pem_bytes = server_root_cert_pem_end - server_root_cert_pem_start,
        };

        global_cfg = cfg;
        global_cfg_set = 1;
    }

    ESP_LOGI(TAG, "do_https_request: connecting");

    tls = esp_tls_conn_http_new(WEB_SERVER_URL, &global_cfg);

    if(tls != NULL) {
        ESP_LOGI(TAG, "Connection established...");
    } else {
        ESP_LOGE(TAG, "Connection failed...");
        goto exit;
    }

    size_t written_bytes = 0;
    do {
        ret = esp_tls_conn_write(tls,
                                 request + written_bytes, 
                                 strlen(request) - written_bytes);
        if (ret >= 0) {
            ESP_LOGI(TAG, "%d bytes written", ret);
            written_bytes += ret;
        } else if (ret != MBEDTLS_ERR_SSL_WANT_READ  && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            ESP_LOGE(TAG, "esp_tls_conn_write  returned 0x%x", ret);
            goto exit;
        }
    } while(written_bytes < strlen(request));

    ESP_LOGI(TAG, "Reading HTTP response...");

    do
    {
        len = sizeof(buf) - 1;
        bzero(buf, sizeof(buf));
        ret = esp_tls_conn_read(tls, (char *)buf, len);

        if(ret == MBEDTLS_ERR_SSL_WANT_WRITE  || ret == MBEDTLS_ERR_SSL_WANT_READ)
            continue;

        if(ret < 0)
        {
            ESP_LOGE(TAG, "esp_tls_conn_read  returned -0x%x", -ret);
            break;
        }

        if(ret == 0)
        {
            ESP_LOGI(TAG, "connection closed");
            break;
        }

        len = ret;
        ESP_LOGD(TAG, "%d bytes read", len);

        for(int i = 0; i < len; i++) {
            putchar(buf[i]);
	        response_payload[charCount] = buf[i];
	        charCount++;
        }
    } while(1);

    exit:
	ESP_LOGI(TAG, "do_https_request: complete");
	esp_tls_conn_delete(tls);
}

