#pragma once
#include "FileData.h"
#include "FileItem.h"
#include "NetworkManager.h"

#include <vector>
// FileBrowser.h
class FileBrowser {
public:
    int width = 0;
    int height = 0;

    int m_scrollOffset = 0; // Future-proofing for when you have 100+ files
    int m_scrollY = 0;      // Current scroll position
    int m_contentHeight = 0; // Total height of all items combined
    std::vector<FileItem> m_items;
    // Takes raw JSON, parses it, and populates m_items
    void UpdateData(const std::vector<FsResponse>& responses, RECT itemSize);

    // This is the "Brain": Calculates where every item should sit
    // Called during WM_SIZE
    void RecalculateLayout(int windowWidth, int startY) {
        int padding = 20;
        int itemW = width;
        int itemH = height;
        int x = 20;
        int y = startY;

        for (auto& item : m_items) {
            // Store the "World" position (without scroll)
            item.Rect = { x, y, x + itemW, y + itemH };

            x += itemW + padding;
            if (x + itemW > windowWidth - 20) {
                x = 20;
                y += itemH + padding;
            }
        }
        // Save the total height so we know when to stop scrolling
        m_contentHeight = y + itemH + padding;
    }

    // Called during WM_LBUTTONDOWN
    int HitTest(int mx, int my) {
        for (int i = 0; i < (int)m_items.size(); i++) {
            m_items[i].isSelected = false;
            RECT scrollRect = m_items[i].Rect;
            OffsetRect(&scrollRect, 0, -m_scrollY);
            if (PtInRect(&scrollRect, { mx, my })) {
                m_items[i].isSelected = true;
                return i;
            }
        }
        return -1;
    }

    void DrawAll(HDC hdc, HFONT hFont, int viewBottom) {
        for (auto& item : m_items) {
            // Apply the scroll offset only during drawing
            RECT drawRect = item.Rect;
            OffsetRect(&drawRect, 0, -m_scrollY);

            // Optimization: Only draw if the item is actually visible on screen
            if (drawRect.bottom > 60 && drawRect.top < viewBottom) {
                // Temporarily swap the rect to draw it in the scrolled position
                RECT original = item.Rect;
                item.Rect = drawRect;
                item.Draw(hdc, hFont);
                item.Rect = original;
            }
        }
    }
};

