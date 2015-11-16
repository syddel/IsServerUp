// ------------------------------------------------------------------------------------------------
// --
// -- IsServerUp - test servers are up and returning HTTP 200 response codes.
// --
// -- Author: syddel (a.turfer@gmail.com)
// -- Date:   16th November 2015
// --
// -- Notes:  Uses the curl library. This was written to replace a CasperJS test that
// --         checks for 200 HTTP response codes, as this is faster and uses less resources 
// --         (no need to instantiate a browser, be it a headless browser or otherwise).
// --
// --         A 'reference' server (a domain/server IP used to determine if there is an
// --         available network) should be provided as the last parameter. For example:
// --         IsServerUp server1.com server2.com reference_server.com
// ------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <curl/curl.h>

const int REFERENCE_SERVER_FAILURE  = 0;
const int PASSED                    = 0;
const int FAILED                    = 1;
const int UNDEFINED_HTTP_CODE       = 1;
const int CURL_ERROR                = 2;
const int UNKNOWN_ERROR             = 4;

// ------------------------------------------------------------------------------------------------
// Output the specified CURL error.
// ------------------------------------------------------------------------------------------------
void output_error(const char *str, CURLcode res)
{
  fprintf(stderr, "%s error: %s\n", str, curl_easy_strerror(res));
}

// ------------------------------------------------------------------------------------------------
// Perform the CURL operation on the given URL.
// Sets http_response_code.
// Returns: CURL error code.
// ------------------------------------------------------------------------------------------------
CURLcode check_website(const char *url, unsigned long &http_response_code)
{
  CURL *curl;
  CURLcode retval;
  
  curl = curl_easy_init();

  if (curl)
  {
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
  }

  retval = curl_easy_perform(curl);

  if (retval == CURLE_OK)
    retval = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_response_code);

  curl_easy_cleanup(curl);

  return retval;
}

// ------------------------------------------------------------------------------------------------
// Check the availability of the supplied reference server.
// ------------------------------------------------------------------------------------------------
int check_ref_server(const int argc, const char **argv)
{
  bool ref_server_failure = false;
  unsigned long http_response_code = 0;
  CURLcode retval;


  retval = check_website(argv[argc], http_response_code);
  fprintf(stderr, "\nReference server response code: %lu\n\n", http_response_code);

  if (retval != CURLE_OK)
  {
    fprintf(stderr, "CURL ERROR for reference server: %s\n", argv[argc]);
    output_error("CURL ERROR: ", retval);
    ref_server_failure = true;
  }

  return ref_server_failure;
}

// ------------------------------------------------------------------------------------------------
// Check the URLs provided each return a 200.
// ------------------------------------------------------------------------------------------------
bool check_servers(const int argc, const char **argv)
{
  bool retcode = true;
  unsigned long http_response_code = 0;

  for (int i = 1; i < argc - 1; i++)
  {
    check_website(argv[i], http_response_code);
    fprintf(stderr, "Check %d: (%lu) %s\n", i, http_response_code, argv[i]);
    if (http_response_code != 200)
      retcode = false;
  }

  return retcode;
}

// ------------------------------------------------------------------------------------------------
// Program entry point.
// Usage: ./checkserver servertocheck.com [refserver1.com] [refserver2.com] ...
// ------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  if (argc < 3) 
  {
    printf("Incorrect number of parameters.\n\nUsage: IsServerUp server1 [server2 server3 serverX...] ref_server\n");
    printf("\nExample: IsServerUp domain1.com domain2.com www.google.co.uk\n\n");
    printf("In the above example, google.co.uk will be checked to determine if there is a network connection.\n"
           "If there is, domain1.com and domain2.com will be checked to see if they return a 200.\n\n");
    return 1;
  }
  
  //
  // Check the (optional) reference servers. If one or more is not contactable,
  // do not continue with the test (as results will be unreliable).
  //
  if (check_ref_server(argc - 1, (const char**)argv))
  {
    fprintf(stderr, "ABORTING. Reference server check failed, therefore network considered unreliable.");
    return REFERENCE_SERVER_FAILURE;
  }

  //
  // Check the list of servers provided.
  //
  if (check_servers(argc, (const char**)argv))
  {
    fprintf(stderr, "\nTEST PASSED!\n");
    return PASSED;
  }
  else
  {
    fprintf(stderr, "\nTEST FAILED!\n");
    return FAILED;
  }
}
