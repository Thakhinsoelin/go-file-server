#pragma once
#include <windows.h>

#include <string>
#include <vector>

#include "BaseWindow.h"
#include "NetworkManager.h"

#include "nlohmannJson.hpp"
#include "FileData.h"
#include "FileBrowser.h"
class MainWindow : public BaseWindow<MainWindow>
{
public:
	MainWindow();
	PCWSTR ClassName() const { return L"MainWindow"; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	HWND m_hListView = nullptr;

	HFONT m_hFont = nullptr;
	
	HFONT m_hLabelFont = nullptr;
	HFONT m_hButtonFont = nullptr;



	HBRUSH m_hLabelBackBrush;

	HWND m_hTitleLabel;
	HWND m_hUploadBtn;
	
	std::wstring m_CurrentDir;

	NetworkManager client;
	std::string m_ResponseBuffer;
private:
	RECT buttonRect;
	void fillResponseBuffer();
private:
	std::vector<FsResponse> m_FileList;
	FileBrowser m_browser;
	FileItem test;
};

