#include "HttpResponse.h"

#include "HttpRequest.h"

BOOL ReadHttpResponse(HINTERNET hRequest, HTTPResponse* out) {
    return 
        request_get_status(hRequest, &out->status) &&
        request_get_reason(hRequest, &out->reason) && 
        request_get_version_info(hRequest, &out->version_info) &&
        request_get_headers(hRequest, &out->headers) && 
        request_get_content(hRequest, &out->content) && 
        request_get_scheme(hRequest, &out->scheme);
}

void FreeResponse(HTTPResponse* response) {
    if (!response) return;

    if (response->reason) free(response->reason);
    if (response->headers) free(response->headers);
    if (response->content) free(response->content);
    if (response->scheme) free(response->scheme);
}

void PrintResponse(HTTPResponse response) {
    wprintf(L"Scheme: %S\n", response.scheme);
    wprintf(L"Version: %lu.%lu\n", response.version_info.dwMajorVersion, response.version_info.dwMinorVersion);
    wprintf(L"Status: %lu\n", response.status);
    wprintf(L"Reason: %S\n", response.reason);
    puts("\n");
    wprintf(L"Headers: %S\n", response.headers);
    wprintf(L"Content: %s\n", response.content);
}