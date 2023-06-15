#include "client.h"

HTTPSession CreateHTTPSession(LPCWSTR user_agent) {
    HTTPSession session = {
        WinHttpOpen(
            user_agent, 
            WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, 
            WINHTTP_NO_PROXY_NAME, 
            WINHTTP_NO_PROXY_BYPASS, 
            0
        )
    };
    return session;
}

void CloseHTTPSession(HTTPSession* session) {
    if (!session || !session->hSession) return;
    if (session->hRequest
    ) {
        WinHttpCloseHandle(session->hRequest);
        session->hRequest = NULL;
    }
    if (session->hConnection) {
        DisconnectHTTPSession(session);
    }
    WinHttpCloseHandle(session->hSession);
}

void ConnectHTTPSession(HTTPSession* session, LPCWSTR host, INTERNET_PORT port) {
    if (!session || !host) return;
    session->hConnection = WinHttpConnect(session->hSession, host, port, 0);
    if (session->hConnection) {
        session->bSecure = FALSE;
        if (port == INTERNET_DEFAULT_HTTPS_PORT) {
            session->bSecure = TRUE;
        }
    }
}

void DisconnectHTTPSession(HTTPSession* session) {
    if (!session || !session->hConnection) return;
    WinHttpCloseHandle(session->hConnection);
    session->hConnection = NULL;
}

BOOL SendHTTPRequest(HTTPSession* session, LPCWSTR method, LPCWSTR path) {
    HINTERNET hRequest = 
        WinHttpOpenRequest(
            session->hConnection, 
            method, 
            path,
            NULL,
            WINHTTP_NO_REFERER,
            WINHTTP_DEFAULT_ACCEPT_TYPES,
            session->bSecure ? WINHTTP_FLAG_SECURE : 0
    );

    if (!hRequest) {
        fputs("Error: failed to open request handler", stderr);
        return FALSE;
    }

    session->hRequest = hRequest;
    
    if (!WinHttpSendRequest(
        hRequest,
        WINHTTP_NO_ADDITIONAL_HEADERS,
        0,
        WINHTTP_NO_REQUEST_DATA,
        0,
        0,
        0
    )) {
        fputs("Error: failed to send request", stderr);
        return FALSE;
    }
    return TRUE;
}

BOOL ReceiveHTTPResponse(HTTPSession* session, HTTPResponse* out) {
    if (!WinHttpReceiveResponse(session->hRequest, NULL)) {
        fputs("Error: failed to receive response", stderr);
        return FALSE;
    }

    if (!ReadHttpResponse(session->hRequest, out)) return FALSE;

    // close/release request object
    if (!WinHttpCloseHandle(session->hRequest)) {
        fputs("Error: failed to close the request", stderr);
        session->hRequest = INVALID_HANDLE_VALUE;
        return FALSE;
    }
    return TRUE;
}
