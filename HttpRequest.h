#pragma once
#include "pch.h"

#define MAX_RESPONSE_SIZE 1024*1024

BOOL request_get_status(HINTERNET hRequest, LPDWORD out);
BOOL request_get_reason(HINTERNET hRequest, LPWSTR* out);
BOOL request_get_version_info(HINTERNET hRequest, LPHTTP_VERSION_INFO out);
BOOL request_get_scheme(HINTERNET hRequest, LPWSTR* out);
BOOL request_get_headers(HINTERNET hRequest, LPWSTR* out);
BOOL request_get_content(HINTERNET hRequest, LPSTR* out);