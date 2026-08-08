#pragma once

#include <filesystem>
#include <optional>
#include <string>

#if defined(_WIN32)
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
    #include <commdlg.h>
#else
    #include <cstdio>
    #include <array>
    #include <memory>
#endif

namespace fs = std::filesystem;

class FileSelector {
public:
    /**
     * @brief 唤起系统文件选择窗口
     * @param filter_description 过滤器描述（例如 "Text Files"）
     * @param filter_pattern 扩展名匹配模式（例如 "*.txt" 或 "*.*"）
     * @return std::optional<fs::path> 用户选中时返回文件路径，取消/失败时返回 std::nullopt
     */
    static std::optional<fs::path> open_file(const std::string& filter_description = "All Files", 
                                            const std::string& filter_pattern = "*.*") {
#if defined(_WIN32)
        // ------------------ Windows 实现 ------------------
        wchar_t szFile[MAX_PATH] = { 0 };

        // 构造 Windows 要求的 Filter 字符串 (以 \0 分隔，以 \0\0 结尾)
        std::wstring wdesc(filter_description.begin(), filter_description.end());
        std::wstring wpattern(filter_pattern.begin(), filter_pattern.end());
        std::wstring filter = wdesc + L" (" + wpattern + L")" + L'\0' + wpattern + L'\0' + L'\0';

        OPENFILENAMEW ofn = { 0 };
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);
        ofn.lpstrFilter = filter.c_str();
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

        if (GetOpenFileNameW(&ofn) == TRUE) {
            return fs::path(szFile);
        }
        return std::nullopt;

#elif defined(__APPLE__)
        // ------------------ macOS 实现 (AppleScript) ------------------
        std::string cmd = "osascript -e 'POSIX path of (choose file with prompt \"Select a file\")' 2>/dev/null";
        std::string result = exec_cmd(cmd.c_str());
        
        // 移除末尾换行符
        while (!result.empty() && (result.back() == '\n' || result.back() == '\r')) {
            result.pop_back();
        }

        if (!result.empty()) {
            return fs::path(result);
        }
        return std::nullopt;

#else
        // ------------------ Linux 实现 (Zenity / Kdialog) ------------------
        // 优先尝试 zenity，没有则尝试 kdialog
        std::string cmd = "zenity --file-selection --title=\"Select File\" 2>/dev/null";
        std::string result = exec_cmd(cmd.c_str());

        if (result.empty()) {
            cmd = "kdialog --getopenfilename . 2>/dev/null";
            result = exec_cmd(cmd.c_str());
        }

        // 移除末尾换行符
        while (!result.empty() && (result.back() == '\n' || result.back() == '\r')) {
            result.pop_back();
        }

        if (!result.empty()) {
            return fs::path(result);
        }
        return std::nullopt;
#endif
    }

private:
#if !defined(_WIN32)
    // 辅助函数：用于在 POSIX 环境下运行 shell 命令并捕获其标准输出
    static std::string exec_cmd(const char* cmd) {
        std::array<char, 256> buffer;
        std::string result;
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
        if (!pipe) return "";
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result += buffer.data();
        }
        return result;
    }
#endif
};