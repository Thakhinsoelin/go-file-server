// what tf is these things
#pragma comment(lib, "comctl32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#include "MainWindow.h"

#include "NetworkManager.h"

#include <thread>

// a really shitty way to do but whatever
#define ID_UPLOAD_BUTTON 1001
#define ID_REFRESH_BUTTON 1002
#define WM_DATA_READY (WM_USER + 1)

// Helper functions area
BOOL CALLBACK SetFontCallback(HWND hwnd, LPARAM lParam) {
    SendMessage(hwnd, WM_SETFONT, (WPARAM)lParam, TRUE);
    return TRUE;
}
// This function is called by Windows for every font it finds that matches our criteria
int CALLBACK AppearanceCallback(const LOGFONT* lpelfe, const TEXTMETRIC* lpntme, DWORD FontType, LPARAM lParam) {
    bool* found = reinterpret_cast<bool*>(lParam);
    *found = true;
    return 0; // Stop searching once we find at least one
}

MainWindow::MainWindow()
{
    client.Init(L"192.168.100.9", 3000);
    m_browser.width = 110;
    m_browser.height = 140;
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {
        bool fontExists = false;
        LOGFONT lf = { 0 };
        lf.lfCharSet = DEFAULT_CHARSET;
        wcscpy_s(lf.lfFaceName, L"Pyidaungsu"); // The name you are looking for

        HDC hdc = GetDC(m_hwnd);
        EnumFontFamiliesEx(hdc, &lf, (FONTENUMPROC)AppearanceCallback, (LPARAM)&fontExists, 0);
        ReleaseDC(m_hwnd, hdc);

        if (!fontExists)
        {
            if (AddFontResourceEx(L"Font/Pyidaungsu-2.5.3_Regular.ttf", FR_PRIVATE, NULL) > 0) {
				fontExists = true;
			}
			else {
				MessageBox(m_hwnd, L"Failed to load Pyidaungsu font. Please ensure the font file is in the correct location.", L"Font Load Error", MB_OK | MB_ICONERROR);
            }
        }

        bool ofontExists = false;
        LOGFONT lff = { 0 };
        lff.lfCharSet = DEFAULT_CHARSET;
        wcscpy_s(lff.lfFaceName, L"Open Sans"); // The name you are looking for

        HDC hdcc = GetDC(m_hwnd);
        EnumFontFamiliesEx(hdcc, &lff, (FONTENUMPROC)AppearanceCallback, (LPARAM)&ofontExists, 0);
        ReleaseDC(m_hwnd, hdcc);

        if (!ofontExists)
        {
            if (AddFontResourceEx(L"Font/OpenSans-Medium.ttf", FR_PRIVATE, NULL) > 0) {
                ofontExists = true;
            }
            else {
                MessageBox(m_hwnd, L"Failed to load Opensans font. Please ensure the font file is in the correct location.", L"Font Load Error", MB_OK | MB_ICONERROR);
            }
        }

        // This might become problem for machines that don't have
        // pyidaungsu installed. 
		m_hFont = CreateFontW(
			16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
			DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
			CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Pyidaungsu"
		);

        m_hLabelFont = CreateFontW(
            32, 0, 0, 0,           // Height (32px), Width
            FW_BOLD,               // Weight: Bold
            FALSE, FALSE, FALSE,   // Italic, Underline, Strikeout
            DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY,
            VARIABLE_PITCH | FF_SWISS,
            L"Pyidaungsu"            // Typeface name
        );

        m_hButtonFont = CreateFontW(
            32, 0, 0, 0,           // Height (32px), Width
            FW_MEDIUM,               // Weight: Bold
            FALSE, FALSE, FALSE,   // Italic, Underline, Strikeout
            DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY,
            VARIABLE_PITCH | FF_SWISS,
            L"Open Sans"            // Typeface name
        );

        //m_hTitleLabel = CreateWindowExW(
        //    0, L"STATIC", L"စိုးသီဟဖိုင်ဆာဗာ",
        //    WS_CHILD | WS_VISIBLE | SS_LEFT,
        //    10, 10, 200, 30, // x, y, width, height
        //    m_hwnd, NULL, GetModuleHandle(NULL), NULL
        //);

        m_hUploadBtn = CreateWindowEx(
            0, L"BUTTON", L"Upload",
            WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
            220, 10, 80, 30,          // Positioned to the right of the label
            m_hwnd, (HMENU)ID_UPLOAD_BUTTON, GetModuleHandle(NULL), NULL
        );

        CreateWindowEx(0, L"BUTTON", L"Refresh", WS_CHILD | WS_VISIBLE,
            320, 10, 80, 30, m_hwnd, (HMENU)ID_REFRESH_BUTTON, NULL, NULL);

        if (m_hTitleLabel) {
			SendMessage(m_hTitleLabel, WM_SETFONT, (WPARAM)m_hLabelFont, TRUE);
        }

        if (m_hUploadBtn)
        {
			SendMessage(m_hUploadBtn, WM_SETFONT, (WPARAM)m_hButtonFont, TRUE);
        }

        fillResponseBuffer();
        if (!m_ResponseBuffer.empty())
        {
			OutputDebugStringA(m_ResponseBuffer.c_str());
        }
        //EnumChildWindows(m_hwnd, SetFontCallback, (LPARAM)m_hFont);
      
        return 0;
    }

    case WM_DRAWITEM:
    {
        LPDRAWITEMSTRUCT pDIS = (LPDRAWITEMSTRUCT)lParam;
        if (pDIS->CtlID == ID_UPLOAD_BUTTON)
        {
            HDC hdc = pDIS->hDC;
            RECT rc = pDIS->rcItem;
            int width = rc.right - rc.left;
            int height = rc.bottom - rc.top;

			HGDIOBJ hOldFont = SelectObject(hdc, m_hButtonFont);
            // 1. Create the Green Brush
            HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
            HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

            // 2. Remove the default border
            HPEN hOldPen = (HPEN)SelectObject(hdc, GetStockObject(NULL_PEN));

            // 3. Draw the Pill/Oval
            // Last two params (height, height) make it a perfect circle on the sides
            RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, height, height);

            // 4. Draw the Pyidaungsu Text
            SelectObject(hdc, m_hButtonFont);
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(0, 0, 0)); // Black text

            // Center the text in the pill
            DrawText(hdc, L"Upload", -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            // 5. Cleanup
            SelectObject(hdc, hOldBrush);
            SelectObject(hdc, hOldPen);
			SelectObject(hdc, hOldFont);
            DeleteObject(hBrush);
            return TRUE;
        }
        break;
    }

	case WM_CTLCOLORBTN:
	{
		HDC hdcButton = (HDC)wParam;
		HWND hWndButton = (HWND)lParam;
		SetBkMode(hdcButton, TRANSPARENT);
		// Set Text to White
		SetTextColor(hdcButton, RGB(255, 255, 255));
		// Return the brush for the background
		return (LRESULT)GetStockObject(NULL_BRUSH);
	}

    case WM_CTLCOLORSTATIC:
    {
        HDC hdcStatic = (HDC)wParam;
        HWND hWndStatic = (HWND)lParam;

		SetBkMode(hdcStatic, TRANSPARENT);
        
        SetTextColor(hdcStatic, RGB(0, 0, 255));

        //// Set Background to Dark
        SetBkColor(hdcStatic, RGB(0, 255, 0));

        // Return the brush for the background
        return (LRESULT)GetStockObject(NULL_BRUSH);
    }

    case WM_DESTROY:
    {
        if (m_hFont) DeleteObject(m_hFont);
        PostQuitMessage(0);
        return 0;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(m_hwnd, &ps);

        RECT rc;
        GetClientRect(m_hwnd, &rc);
        int width = rc.right - rc.left;
        int height = rc.bottom - rc.top;

        // 1. Create Buffer
        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP memBitmap = CreateCompatibleBitmap(hdc, width, height);
        HGDIOBJ hOldBitmap = SelectObject(memDC, memBitmap);

        // 2. Draw Backgrounds to Buffer
        HBRUSH hBodyBrush = CreateSolidBrush(RGB(16, 16, 16));
        HBRUSH hGreenBrush = CreateSolidBrush(RGB(96, 219, 98));

        // Paint Body
        FillRect(memDC, &rc, hBodyBrush);

        // Paint Header (The Nav Bar)
        RECT headerRect = rc;
        headerRect.bottom = 60;
        FillRect(memDC, &headerRect, hGreenBrush);

        SetBkMode(memDC, TRANSPARENT);
        SetTextColor(memDC, RGB(0, 0, 255)); // Your blue color
        HGDIOBJ hOldFont = SelectObject(memDC, m_hLabelFont);
        RECT titleRect = { 10, 10, 300, 50 };
        DrawTextW(memDC, L"စိုးသီဟဖိုင်ဆာဗာ", -1, &titleRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        SelectObject(memDC, hOldFont);

        // 3. Clipping & Content
        HRGN hClipRgn = CreateRectRgn(0, 60, rc.right, rc.bottom);
        SelectClipRgn(memDC, hClipRgn);

        m_browser.DrawAll(memDC, m_hFont, rc.bottom);

        // 4. Finalizing
        SelectClipRgn(memDC, NULL);
        DeleteObject(hClipRgn);

        // One single "slam" to the screen
        BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);

        // 5. Cleanup
        SelectObject(memDC, hOldBitmap);
        DeleteObject(memBitmap);
        DeleteDC(memDC);
        DeleteObject(hGreenBrush);
        DeleteObject(hBodyBrush);

        EndPaint(m_hwnd, &ps);
        return 0;
    }
    case WM_MOUSEWHEEL:
    {
        int delta = GET_WHEEL_DELTA_WPARAM(wParam);
        // Scroll speed: 40 pixels per notch
        m_browser.m_scrollY -= (delta / WHEEL_DELTA) * 40;

        // Bounds checking
        if (m_browser.m_scrollY < 0) m_browser.m_scrollY = 0;

        RECT rc;
        GetClientRect(m_hwnd, &rc);
        int maxScroll = m_browser.m_contentHeight - rc.bottom;
        if (maxScroll < 0) maxScroll = 0;
        if (m_browser.m_scrollY > maxScroll) m_browser.m_scrollY = maxScroll;

        InvalidateRect(m_hwnd, NULL, FALSE);
        return 0;
    }

    case WM_ERASEBKGND:
        // Return 1 to tell Windows we handled the erasing ourselves.
        // This prevents the white flicker.
        return 1;
    case WM_SIZE:
    {
		// Clanker's code for responsive design in Win32. It's a bit manual, but it works!
        int width = LOWORD(lParam);
        int height = HIWORD(lParam);

        // 1. Calculate the 'Flex' positions you already have
        int labelWidth = (int)(width * 0.3);
        int btnWidth = 200;
        int btnHeight = 40;
        int btnX = width / 2 - btnWidth / 2 - 20;
        int btnY = 10;

        // 2. Position the controls
        MoveWindow(m_hTitleLabel, 10, 10, labelWidth, 40, FALSE);
        MoveWindow(m_hUploadBtn, btnX, btnY, btnWidth, btnHeight, FALSE);

        m_browser.RecalculateLayout(width, 80);
        // 3. APPLY ROUNDED EDGES
        // Parameters: Left, Top, Right, Bottom, Width of Ellipse, Height of Ellipse
        // To make it a perfect OVAL/PILL: Set the last two numbers to the button's height.
        // HRGN hRgn = CreateRoundRectRgn(0, 0, btnWidth, btnHeight, btnHeight, btnHeight);

        // IMPORTANT: Once you call SetWindowRgn, the OS takes ownership of the HRGN.
        // Do NOT call DeleteObject(hRgn) yourself!
        // SetWindowRgn(m_hUploadBtn, hRgn, TRUE);
		InvalidateRect(m_hwnd, NULL, FALSE); // Redraw the window
        return 0;
    }
    case WM_LBUTTONDOWN: 
    {
		int mx = LOWORD(lParam);
		int my = HIWORD(lParam);
		int hitIndex = m_browser.HitTest(mx, my);
        if (hitIndex != -1) {
            MessageBox(m_hwnd, m_browser.m_items[hitIndex].data.GetWidePath().c_str(), L"File Clicked", MB_OK);
        }
    }
    case WM_DATA_READY:
        // THIS is where the data is actually ready to be used
        if (!m_FileList.empty()) {
            RECT itemRect = { 20, 80, 20 + 120, 80 + 120 };
			m_browser.UpdateData(m_FileList, itemRect);

            RECT rc;
            GetClientRect(m_hwnd, &rc);
            m_browser.RecalculateLayout(rc.right, 80); // 80 is below your green header

        }
        return 0;
    case WM_COMMAND:
        if (LOWORD(wParam) == ID_UPLOAD_BUTTON)
        {
			MessageBox(m_hwnd, L"Upload button clicked!", L"Upload Box", MB_OK);
        }
        return 0;
    default:
        return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
    }
    return TRUE;
}

void MainWindow::fillResponseBuffer()
{
    HWND hNotifyWnd = m_hwnd;
    std::thread([this, hNotifyWnd]() {
        std::string rawJson = client.RequestFileList();
		OutputDebugStringA(rawJson.c_str());
        if (!rawJson.empty()) {
            
            try {
                auto j = nlohmann::json::parse(rawJson);

                // This one line converts the entire JSON array 
                this->m_FileList = j.get<std::vector<FsResponse>>();

                
                PostMessage(hNotifyWnd, WM_DATA_READY, 0, 0);
            }
            catch (const std::exception& e) {
                OutputDebugStringA("JSON Parse Error: ");
                OutputDebugStringA(e.what());
                OutputDebugStringA("\n");
            }
        }
        else {
            OutputDebugString(L"Server connection lost mid-request\n");
        }

    }).detach();
}
