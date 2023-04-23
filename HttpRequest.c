#include "HttpRequest.h"


BOOL request_get_status(HINTERNET hRequest, LPDWORD out) {
    DWORD dwSize = sizeof(DWORD);
    
    // Get HTTP status code
    if (!WinHttpQueryHeaders(
            hRequest, 
            WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, 
            NULL,
            out, 
            &dwSize,
            NULL
    )) {
        fputs("Error: failed to query response status code\n", stderr);
        *out = 0;
        return FALSE;
    }
    
    return TRUE;
}

BOOL request_get_reason(HINTERNET hRequest, LPWSTR* out) {
    DWORD dwSize = 0;

    WinHttpQueryHeaders(
            hRequest, 
            WINHTTP_QUERY_STATUS_TEXT, 
            NULL,
            WINHTTP_NO_OUTPUT_BUFFER,
            &dwSize,
            NULL
    );

    *out = calloc(dwSize + 1, sizeof(WCHAR));

    
    if (!WinHttpQueryHeaders(
            hRequest, 
            WINHTTP_QUERY_STATUS_TEXT, 
            NULL,
            *out,
            &dwSize,
            NULL
    )) {    
        fputs("Error: failed to query response status reason", stderr);
        free(*out);
        *out = NULL;
        return FALSE;
    }
    return TRUE;
}

BOOL request_get_version_info(HINTERNET hRequest, LPHTTP_VERSION_INFO out) {
    DWORD version_info_size = sizeof(HTTP_VERSION_INFO);
    
    if (!WinHttpQueryOption(
        hRequest,
        WINHTTP_OPTION_HTTP_VERSION,
        out,
        &version_info_size
    )) {
        fputs("Error: failed to query version info", stderr);
        return FALSE;
    }

    return TRUE;
}

BOOL request_get_scheme(HINTERNET hRequest, LPWSTR* out) {
    LPWSTR szUrl = NULL;
    DWORD dwSize = 0;

    WinHttpQueryOption(
        hRequest,
        WINHTTP_OPTION_URL,
        WINHTTP_NO_OUTPUT_BUFFER,
        &dwSize
    );

    szUrl = calloc(dwSize + 1, sizeof(WCHAR));

    if (!WinHttpQueryOption(
        hRequest,
        WINHTTP_OPTION_URL,
        szUrl,
        &dwSize
    )) {
        fputs("Error: failed to query url", stderr);
        free(szUrl);
        *out = NULL;
        return FALSE;
    }

    LPWSTR tok = wcsstr(szUrl, L"://");
    if (!tok) {
        fputs("Error: failed to find `://` inside url", stderr);
        free(szUrl);
        *out = NULL;
        return FALSE;
    }
    
    DWORD size = (DWORD)(tok - szUrl);
    *out = calloc(size + 1, sizeof(WCHAR));
    wcsncpy(*out, szUrl, size);
    free(szUrl);
    return TRUE;
}

BOOL request_get_headers(HINTERNET hRequest, LPWSTR* out) {
    DWORD dwSize = 0;

    // Read the response headers
    WinHttpQueryHeaders(
        hRequest,
        WINHTTP_QUERY_RAW_HEADERS_CRLF, 
        WINHTTP_HEADER_NAME_BY_INDEX, 
        WINHTTP_NO_OUTPUT_BUFFER,
        &dwSize, 
        WINHTTP_NO_HEADER_INDEX
    );
    
    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
        fputs("Error: failed to query response headers size\n", stderr);
        return FALSE;
    }

    *out = calloc(dwSize + 1, sizeof(WCHAR));
    if (!WinHttpQueryHeaders(
        hRequest,
        WINHTTP_QUERY_RAW_HEADERS_CRLF,
        WINHTTP_HEADER_NAME_BY_INDEX,
        *out,
        &dwSize,
        WINHTTP_NO_HEADER_INDEX
    )) {
        fputs("Error: failed to query response headers\n", stderr);
        free(*out);
        *out = NULL;
        return FALSE;
    }

    return TRUE;
}

BOOL request_get_content(HINTERNET hRequest, LPSTR* out){
    DWORD content_length = 0;
    DWORD content_read = 0;

    // Read the response content
    if (!WinHttpQueryDataAvailable(hRequest, &content_length)) {
        fputs("Error: failed to query response content length", stderr);
        return FALSE;
    }

    if (content_length > MAX_RESPONSE_SIZE) {
        fputs("Error: response content is too large", stderr);
        return FALSE;
    }

    *out = calloc(content_length + 1, sizeof(CHAR));

    DWORD bytes_read = 0;
    while (content_read < content_length) {
        if (!WinHttpReadData(
            hRequest,
            (*out) + content_read,
            content_length - content_read,
            &bytes_read
        )) {
            fputs("Error: failed to read response content", stderr);
            free(*out);
            *out = NULL;
            return FALSE;
        }

        content_read += bytes_read;
    }
    
    return TRUE;
}