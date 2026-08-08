#pragma once

#include <filesystem>
#include <optional>
#include <string>

#if defined(_WIN32)
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
    #include <shobjidl.h> // 使用现代 Win32 IFileDialog 接口
#else
    #include <cstdio>
    #include <array>
    #include <memory>
#endif

namespace fs = std::filesystem;

class DirectorySelector {
public:
    /**
     * @brief 唤起系统文件夹选择窗口
     * @param title 提示框标题
     * @return std::optional<fs::path> 用户选中时返回文件夹路径，取消/失败时返回 std::nullopt
     */
    static std::optional<fs::path> open_directory(const std::string& title = "Select Directory") {
#if defined(_WIN32)
        // ------------------ Windows 实现 (IFileDialog) ------------------
        HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        bool co_initialized = SUCCEEDED(hr);

        IFileDialog* pfd = NULL;
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
        
        if (FAILED(hr)) {
            if (co_initialized) CoUninitialize();
            return std::nullopt;
        }

        // 设置为选择文件夹模式 (FOS_PICKFOLDERS)
        DWORD dwOptions;
        if (SUCCEEDED(pfd->GetOptions(&dwOptions))) {
            pfd->SetOptions(dwOptions | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM);
        }

        // 设置窗口标题
        std::wstring wtitle(title.begin(), title.end());
        pfd->SetTitle(wtitle.c_str());

        // 显示对话框
        hr = pfd->Show(NULL);
        if (FAILED(hr)) { // 用户取消或关闭
            pfd->Release();
            if (co_initialized) CoUninitialize();
            return std::nullopt;
        }

        // 获取选中的文件夹路径
        IShellItem* psi = NULL;
        hr = pfd->GetResult(&psi);
        if (FAILED(hr)) {
            pfd->Release();
            if (co_initialized) CoUninitialize();
            return std::nullopt;
        }

        PWSTR pszPath = NULL;
        hr = psi->GetDisplayName(SIGDN_FILESYSPATH, &pszPath);
        std::optional<fs::path> result = std::nullopt;

        if (SUCCEEDED(hr) && pszPath != NULL) {
            result = fs::path(pszPath);
            CoTaskMemFree(pszPath);
        }

        psi->Release();
        pfd->Release();
        if (co_initialized) CoUninitialize();

        return result;

#elif defined(__APPLE__)
        // ------------------ macOS 实现 (AppleScript) ------------------
        std::string cmd = "osascript -e 'POSIX path of (choose folder with prompt \"" + title + "\")' 2>/dev/null";
        std::string result = exec_cmd(cmd.c_str());

        while (!result.empty() && (result.back() == '\n' || result.back() == '\r')) {
            result.pop_back();
        }

        if (!result.empty()) {
            return fs::path(result);
        }
        return std::nullopt;

#else
        // ------------------ Linux 实现 (Zenity / Kdialog) ------------------
        std::string cmd = "zenity --file-selection --directory --title=\"" + title + "\" 2>/dev/null";
        std::string result = exec_cmd(cmd.c_str());

        if (result.empty()) {
            cmd = "kdialog --getexistingdirectory . 2>/dev/null";
            result = exec_cmd(cmd.c_str());
        }

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