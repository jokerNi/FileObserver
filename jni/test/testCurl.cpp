#include <curl/curl.h>
#include "MyLog.h"

int testCurl()
{
  CURL *curl;
  CURLcode res;
 
  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "http://www.baidu.com");
    /* example.com is redirected, so we tell libcurl to follow redirection */ 
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
 
    /* Perform the request, res will get the return code */ 
    res = curl_easy_perform(curl);
    /* Check for errors */ 
    if(res != CURLE_OK)
    {
      XLOG("curl_easy_perform() failed: %s", curl_easy_strerror(res));
    }
    else
    {
      long retcode = 0;
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &retcode);
      XLOG("curl_easy_perform() success, response code=%d", retcode);
    }
 
    /* always cleanup */ 
    curl_easy_cleanup(curl);
  }
  return 0;
}
}