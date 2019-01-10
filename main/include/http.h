#ifndef __http_h_included__
#define __http_h_included__

#define REQUEST_TIMEOUT_IN_SECONDS	5

/**
  Executes HTTP request, puts response into response_payload global variable
*/
extern void do_http_request(const char *request);

#endif
