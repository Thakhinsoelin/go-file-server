#pragma once

#include <string>
#include <rpc.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")


class NetworkManager
{
public:
	NetworkManager() = default;
	NetworkManager(const std::wstring& ip4addr, int port);
	~NetworkManager();

	void Init();
	void Init(const std::wstring& ip4addr, int port);

	std::string RequestFileList();
private:
	std::wstring m_IPV4Address;
	int m_Port;
	HINTERNET m_hSession;
	HINTERNET m_hConnect;

public:
	
};

