#pragma once
#include <windows.h>
#include <string>
#include <vector>
#include <shellapi.h>     // For SHGetFileInfo
#include <commctrl.h>
#include <commoncontrols.h> // For IImageList and SHIL_JUMBO
#include <shlobj.h>       // For IID_IImageList
#include <shlwapi.h>      // For Path helpers if needed

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shlwapi.lib")

#include "FileData.h"

struct FileItem {
    FsResponse data;
    RECT Rect;
    bool isHovered = false;
    bool isSelected = false;
    int iIconIndex = -1;

    

    void m_LoadIcon() {
        std::wstring wPath = data.GetWidePath();
        SHFILEINFOW sfi = { 0 };

        // Use SHGFI_SYSICONINDEX to get the handle to the system image list
        UINT_PTR hr = SHGetFileInfoW(wPath.c_str(),
            data.isDir ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL,
            &sfi, sizeof(sfi), SHGFI_SYSICONINDEX | SHGFI_LARGEICON | SHGFI_USEFILEATTRIBUTES);

        if (hr) {
            iIconIndex = sfi.iIcon;
        }
    }

    void Draw(HDC hdc, HFONT hFont) {
        // 1. Prepare Colors and Background
        COLORREF bgColor = isSelected ? RGB(0, 120, 215) : (isHovered ? RGB(50, 50, 50) : RGB(30, 30, 30));
        HBRUSH hBrush = CreateSolidBrush(bgColor);

        // IMPORTANT: Select both brush and null pen for a clean rounded rect
        HGDIOBJ hOldBrush = SelectObject(hdc, hBrush);
        HPEN hOldPen = (HPEN)SelectObject(hdc, GetStockObject(NULL_PEN));

        RoundRect(hdc, Rect.left, Rect.top, Rect.right, Rect.bottom, 10, 10);

        // 2. Draw High-Quality Icon
        if (iIconIndex != -1) {
            IImageList* piml = nullptr;
            // SHIL_JUMBO is 256x256, SHIL_EXTRALARGE is 48x48/64x64 depending on DPI
            if (SUCCEEDED(SHGetImageList(SHIL_JUMBO, IID_IImageList, (void**)&piml))) {
                int iconSize = 80; // Standard Explorer "Large" size
                int x = Rect.left + ((Rect.right - Rect.left) - iconSize) / 2;
                int y = Rect.top + 5;
				IMAGELISTDRAWPARAMS drawParams = { 0 };
                drawParams.cbSize = sizeof(IMAGELISTDRAWPARAMS);
                drawParams.himl = reinterpret_cast<HIMAGELIST>(piml); // Cast IImageList to HIMAGELIST
                drawParams.i = iIconIndex;
                drawParams.hdcDst = hdc;
                drawParams.x = x;
                drawParams.y = y;
                drawParams.cx = iconSize; // 0 uses the default size of the image in the list
                drawParams.cy = iconSize;
                drawParams.xBitmap = 0;
                drawParams.yBitmap = 0;
                drawParams.fStyle = ILD_TRANSPARENT | ILD_SCALE;
                drawParams.fState = ILS_NORMAL;
                drawParams.dwRop = SRCCOPY;
                // Draw the icon from the system list
                piml->Draw(&drawParams);
                piml->Release();
            }
        }

        // 3. Setup Text Properties
        SelectObject(hdc, hFont);
        SetTextColor(hdc, RGB(255, 255, 255));
        SetBkMode(hdc, TRANSPARENT);

        // 4. String Preparation (Safe Trimming)
        std::wstring displayPath = data.GetWidePath();
        
        bool hasDataPrefix = false;
        if (displayPath.length() >= 5) {
            std::wstring prefix = displayPath.substr(0, 5);
            if (prefix == L"data/" || prefix == L"data\\") {
                hasDataPrefix = true;
            }
        }

        if (hasDataPrefix) {
            displayPath.erase(0, 5);
        }

        // 5. Text Layout Math
        // We calculate the text area based on the bottom 30-40% of the card
        RECT textRect = { Rect.left + 5, Rect.top + 90, Rect.right - 5, Rect.bottom - 5 };

        UINT textFlags = DT_CENTER | DT_EDITCONTROL | DT_NOPREFIX;

        if (!isSelected) {
            // Unselected: Single line with ellipsis (...)
            textFlags |= DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS;
        }
        else {
            // Selected: Multi-line wrapping
            textFlags |= DT_WORDBREAK;
        }

        DrawTextW(hdc, displayPath.c_str(), -1, &textRect, textFlags);

        // 6. Cleanup GDI Objects
        SelectObject(hdc, hOldBrush);
        SelectObject(hdc, hOldPen);
        DeleteObject(hBrush);
    }
};
