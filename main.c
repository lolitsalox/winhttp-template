#include "client.h"
#include <assert.h>

int main(int argc, char** argv) {
    HTTPSession session = CreateHTTPSession(L"A WinHTTP Example Program/1.0");
    if (!session.hSession) {
        printf("Error %lu in WinHttpOpen.\n", GetLastError());
        return 1;
    }

    if (argc <= 2) {
        printf("Usage: <program.exe> <url> <path>\nexample: a.exe api.myip.com /\n");
        return 1;
    }

    PSTR url = argv[1], path = argv[2];

    size_t url_wide_length = MultiByteToWideChar(CP_UTF8, 0, url , -1, NULL, 0);
    size_t path_wide_length = MultiByteToWideChar(CP_UTF8, 0, path , -1, NULL, 0);
    
    PWSTR url_wide = malloc(sizeof(WCHAR) * url_wide_length);
    assert(url_wide != NULL && "Coudln't allocate for url_wide");
    
    PWSTR path_wide = malloc(sizeof(WCHAR) * path_wide_length);
    assert(path_wide != NULL && "Coudln't allocate for path_wide");
    
    MultiByteToWideChar(CP_UTF8, 0, url, -1, url_wide, url_wide_length);
    MultiByteToWideChar(CP_UTF8, 0, path, -1, path_wide, path_wide_length);

    ConnectHTTPSession(&session, url_wide, INTERNET_DEFAULT_HTTPS_PORT);
    free(url_wide);

    if (!SendHTTPRequest(&session, L"GET", path_wide)) {
        CloseHTTPSession(&session);
        return 1;
    }
    free(path_wide);

    HTTPResponse res;
    if (!ReceiveHTTPResponse(&session, &res)) {
        FreeResponse(&res);
        CloseHTTPSession(&session);
        return 1;
    }

    PrintResponse(res);
    
    FreeResponse(&res);
    CloseHTTPSession(&session);
    return 0;
}
