#pragma once

#include <iostream>

#if defined(_WIN32) || defined(_WIN64)
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
    #include <conio.h>
#else
    #include <termios.h>
    #include <unistd.h>
#endif

inline void clear_screen() {
#if defined(_WIN32) || defined(_WIN64)
    // 优先尝试开启 ANSI 支持；如果失败则降级使用 Win32 API 填空格
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (GetConsoleMode(hOut, &dwMode)) {
        SetConsoleMode(hOut, dwMode | 0x0004); // ENABLE_VIRTUAL_TERMINAL_PROCESSING
    }
    std::cout << "\033[2J\033[1;1H" << std::flush;
#else
    // POSIX 平台直接输出 ANSI 转义码
    std::cout << "\033[2J\033[1;1H" << std::flush;
#endif
}

inline char getch_silent() {
#if defined(_WIN32) || defined(_WIN64)
    return static_cast<char>(_getch());
#else
    char ch = 0;
    termios oldt{}, newt{};
    // 获取当前终端属性
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    // 禁用行缓冲区 (ICANON) 和本地回显 (ECHO)
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    // 读取单个字符
    read(STDIN_FILENO, &ch, 1);

    // 恢复原始终端设置
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
#endif
}

// 提示并等待按任意键
inline void pause_any_key(const std::string& prompt = "Press any key to go back...") {
    std::cout << prompt << std::flush;
    getch_silent();
    std::cout << std::endl;
}