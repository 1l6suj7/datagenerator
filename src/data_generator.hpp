#pragma once

#include <iostream>
#include <filesystem>
#include <string>
#include <cstdlib>
#include <expected>

namespace fs = std::filesystem;

class DataGenerator {
public:
    /**
     * @brief 批量生成测试数据
     * @param gen_exe 数据生成器可执行文件路径 (产生 1 组随机数据到 stdout)
     * @param std_exe 标程/正确答案可执行文件路径 (从 stdin 读入，输出到 stdout)
     * @param start_idx 起始编号 (inclusive)
     * @param end_idx 结束编号 (inclusive)
     * @param output_dir 数据保存目录 (默认为当前目录)
     * @return bool 是否全部成功生成
     */
    static std::expected<void, std::string> generate(const fs::path& gen_exe, 
                         const fs::path& std_exe, 
                         int start_idx, 
                         int end_idx, 
                         const fs::path& output_dir = "./") {
        
        // 检查可执行文件是否存在
        if (!fs::exists(gen_exe)) {
            return std::unexpected(std::format("Data generator doesn't exist: {}", gen_exe.string()));
        }
        if (!fs::exists(std_exe)) {
            return std::unexpected(std::format("Stardard program doesn't exist: {}", std_exe.string()));
        }

        // 确保输出目录存在
        if (!output_dir.empty() && !fs::exists(output_dir)) {
            fs::create_directories(output_dir);
        }

        std::cout << std::format("=== Start to generate test data [{}, {}] ===\n", start_idx, end_idx);

        for (int i = start_idx; i <= end_idx; ++i) {
            fs::path in_file  = output_dir / (std::to_string(i) + ".in");
            fs::path out_file = output_dir / (std::to_string(i) + ".ans");

            std::cout << std::format("Generating testcase #{} ...", i) << std::flush;

            // 1. 生成输入文件: gen_exe > X.in
            int gen_ret = exec_redirect(gen_exe, "", in_file);
            if (gen_ret != 0) {
                std::cout << "[Failed]" << std::endl;
                return std::unexpected(std::format("The generator failed at testcase #{}", i));
            }

            // 2. 生成输出文件: std_exe < X.in > X.out
            int std_ret = exec_redirect(std_exe, in_file, out_file);
            if (std_ret != 0) {
                std::cout << "[Failed]" << std::endl;
                return std::unexpected(std::format("The standard program failed at Testcase #{}", i));
            }

            std::cout << "[Succeeded]" << std::endl;
        }

        std::cout << "=== All test data has been generated! ===" << std::endl;
        return {};
    }

private:
    static int exec_redirect(const fs::path& exe_path, 
                      const fs::path& in_path = "", 
                      const fs::path& out_path = "") {
            
        fs::path abs_exe = fs::absolute(exe_path);
        std::string exe_str = abs_exe.string();
        std::string in_str  = in_path.empty()  ? "" : fs::absolute(in_path).string();
        std::string out_str = out_path.empty() ? "" : fs::absolute(out_path).string();

        #if defined(_WIN32)
            std::string inner_cmd = "\"" + exe_str + "\"";
            
            if (!in_str.empty()) {
                inner_cmd += " < \"" + in_str + "\"";
            }
            if (!out_str.empty()) {
                inner_cmd += " > \"" + out_str + "\"";
            }

            std::string final_cmd = "cmd /c \"\"" + inner_cmd + "\"\"";

        #else
            std::string final_cmd = "\"" + exe_str + "\"";

            if (!in_str.empty()) {
                final_cmd += " < \"" + in_str + "\"";
            }
            if (!out_str.empty()) {
                final_cmd += " > \"" + out_str + "\"";
            }
        #endif

        return std::system(final_cmd.c_str());
    }
};