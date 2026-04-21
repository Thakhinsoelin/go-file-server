#pragma once
#include <Windows.h>
#include <string>

#include "nlohmannJson.hpp"
struct FsResponse {
	bool isDir;
	bool isFile;
	std::string path;

	std::wstring GetWidePath() const {
		if (path.empty()) return L"";

		// Calculate how much space we need
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, &path[0], (int)path.size(), NULL, 0);
		std::wstring wstrTo(size_needed, 0);

		// Do the actual conversion
		MultiByteToWideChar(CP_UTF8, 0, &path[0], (int)path.size(), &wstrTo[0], size_needed);
		return wstrTo;
	}
};

// This helper tells the library how to map JSON keys to your struct
inline void from_json(const nlohmann::json& j, FsResponse& f) {
	j.at("IsDir").get_to(f.isDir);
	j.at("IsFile").get_to(f.isFile);
	j.at("Path").get_to(f.path);
}
