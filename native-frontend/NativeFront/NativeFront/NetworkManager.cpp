
#include <Windows.h>

#include "NetworkManager.h"

NetworkManager::NetworkManager(const std::wstring& ip4addr, int port)
    :m_IPV4Address(ip4addr), m_Port(port)
{
}

NetworkManager::~NetworkManager()
{
    WinHttpCloseHandle(m_hConnect);
    WinHttpCloseHandle(m_hSession);
}

void NetworkManager::Init()
{
    m_hSession = WinHttpOpen(L"NativeFileServer/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);
    // This must be rewritten to be configurable
    m_hConnect = WinHttpConnect(m_hSession, m_IPV4Address.c_str(), m_Port, 0);
}

void NetworkManager::Init(const std::wstring& ip4addr, int port)
{
    m_IPV4Address = ip4addr;
    m_Port = port;

    m_hSession = WinHttpOpen(L"NativeFileServer/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);
    // This must be rewritten to be configurable
    m_hConnect = WinHttpConnect(m_hSession, m_IPV4Address.c_str(), m_Port, 0);
}

std::string NetworkManager::RequestFileList()
{
    DWORD dwSize = 0;
    DWORD dwDownloaded = 0;
    LPSTR pszOutBuffer;
    std::string responseData;

    
    HINTERNET hRequest = WinHttpOpenRequest(m_hConnect, L"GET", L"/list/all-file",
        NULL, WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES, 0);

    if (!hRequest)
    {
        return "";
    }
    if (WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
        WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
        if (WinHttpReceiveResponse(hRequest, NULL)) {
            // 5. Read the data into a string
            do {
                dwSize = 0;
                WinHttpQueryDataAvailable(hRequest, &dwSize);
                pszOutBuffer = new char[dwSize + 1];
                if (WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded)) {
                    pszOutBuffer[dwSize] = '\0';
                    responseData += pszOutBuffer;
                }
                delete[] pszOutBuffer;
            } while (dwSize > 0);
        }
    }

    // 6. Cleanup handles
    WinHttpCloseHandle(hRequest);
    
	//OutputDebugStringA(responseData.c_str());
    return responseData;
}
