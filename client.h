#pragma once
#include "pch.h"
#include "HttpResponse.h"

typedef struct {
    HINTERNET hSession;
    HINTERNET hConnection;
    HINTERNET hRequest;
    BOOL bSecure;
} HTTPSession;

HTTPSession CreateHTTPSession(LPCWSTR user_agent);
void CloseHTTPSession(HTTPSession* session);

void ConnectHTTPSession(HTTPSession* session, LPCWSTR host, INTERNET_PORT port);
void DisconnectHTTPSession(HTTPSession* session);

BOOL SendHTTPRequest(HTTPSession* session, LPCWSTR method, LPCWSTR path);
BOOL ReceiveHTTPResponse(HTTPSession* session, HTTPResponse* out);