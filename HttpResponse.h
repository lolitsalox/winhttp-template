#pragma once
#include "pch.h"

typedef struct HTTPResponse {
    DWORD status;
    LPWSTR reason;
    HTTP_VERSION_INFO version_info;
    LPWSTR scheme;
    LPWSTR headers;
    LPSTR content;
} HTTPResponse;

BOOL ReadHttpResponse(HINTERNET hRequest, HTTPResponse* out);
void FreeResponse(HTTPResponse* response);
void PrintResponse(HTTPResponse response);