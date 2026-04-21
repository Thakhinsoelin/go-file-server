#include "FileBrowser.h"

void FileBrowser::UpdateData(const std::vector<FsResponse>& responses, RECT itemSize)
{
	m_items.clear();
	for (const auto& resp : responses) {
		FileItem item;
		item.Rect = itemSize;
		item.data = resp;
		item.m_LoadIcon(); // Load the icon index for this file
		m_items.push_back(item);
	}
}
