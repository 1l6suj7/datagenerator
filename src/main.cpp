#include <filesystem>
#include <format>
#include <iostream>
#include <functional>
#include <optional>
#include <ostream>
#include <thread>
#include <vector>
#include <chrono>
#include "file_selector.hpp"
#include "data_generator.hpp"
#include "directory_selector.hpp"
#include "console_utils.hpp"

namespace fs = std::filesystem;
using namespace std::chrono_literals;

void clear_screen();
void main_page();
void input_selection_page();
void output_selection_page();
void data_sets_number_setting_page();
void gen_dir_selection_page();
void generation_page();

int page_no = 0;
std::vector<std::function<void()>> pages({
    main_page, input_selection_page, output_selection_page, data_sets_number_setting_page, 
    gen_dir_selection_page, generation_page});
bool input_already;
bool std_already;
bool no_already;
fs::path input_prog_path;
fs::path std_prog_path;
fs::path gen_dir_path = "./";
std::pair<int, int> data_no_range;

void main_page() {
    int num = 0;
    clear_screen();
    std::cout << "1. Select input data generation program.\n";
    if (input_already) {
        std::cout << std::format("   (Current path: {})\n", input_prog_path.string());
    }
    std::cout << "\n2. Select standard program.\n";
    if (std_already) {
        std::cout << std::format("   (Current path: {})\n", std_prog_path.string());
    }
    std::cout << "\n3. Set the range of data sets ID.\n";
    if (no_already) {
        std::cout << std::format("   (Current ID range: [{}, {}])\n", data_no_range.first, data_no_range.second);
    }
    std::cout << "\n4. Select the generation directory.\n";
    std::cout << std::format("   (Current direction: {})\n", gen_dir_path.string());
    std::cout << "\n5. Generate.\n";
    std::cout << "\n6. Exit\n";
    while (num < 1 || num > 6) {
        std::cout << "Please enter the option number: ";
        std::cin >> num;
    }
    page_no = num;
}

void input_selection_page() {
    FileSelector fsl;
    std::optional<fs::path> ret;
    clear_screen();
    ret = fsl.open_file();
    if (ret == std::nullopt) goto fail;
    input_prog_path = ret.value();
    input_already = true;
    page_no = 0;
    return;
fail:
    std::cout << "File selection failed!\n";
    page_no = 0;
    pause_any_key();
}

void output_selection_page() {
    FileSelector fsl;
    std::optional<fs::path> ret;
    clear_screen();
    ret = fsl.open_file();
    if (ret == std::nullopt) {
        goto fail;
    }
    std_prog_path = ret.value();
    std_already = true;
    page_no = 0;
    return;
fail:
    std::cout << "File selection failed!\n" << std::flush;
    page_no = 0;
    pause_any_key();
}

void data_sets_number_setting_page() {
    int l = -1, r = -1;
    clear_screen();
    std::cout << "Now you should enter the data ID range (inclusive).\n";
    while (l < 0) {
        std::cout << "Please enter the starting number (must be a non-negative integer):\n";
        std::cin >> l;
    }
    while (r < 0) {
        std::cout << "Please enter the ending number (must be a non-negative integer):\n";
        std::cin >> r;
    }
    if (l > r) {
        std::cout << "Not a valid range!\n" << std::flush;
        page_no = 0;
        pause_any_key();
        return;
    }
    data_no_range = std::pair(l, r);
    no_already = true;
    page_no = 0;
}

void gen_dir_selection_page() {
    DirectorySelector ds;
    std::optional<fs::path> ret;
    clear_screen();
    ret = ds.open_directory();
    if (ret == std::nullopt) {
        goto fail;
    }
    gen_dir_path = ret.value();
    page_no = 0;
    return;
fail:
    std::cout << "Directory selection failed!\n";
    page_no = 0;
    pause_any_key();
}

void generation_page() {
    DataGenerator gen;
    std::expected<void, std::string> result;
    clear_screen();
    if (!input_already) {
        std::cout << "You should first pick a program that generates input data.\n" << std::flush;
        goto fail;
    }
    if (!std_already) {
        std::cout << "You should first pick a program that generates output data.\n" << std::flush;
        goto fail;
    }
    if (!no_already) {
        std::cout << "You should set the data number range first.\n" << std::flush;
        goto fail;
    }
    result = gen.generate(input_prog_path, std_prog_path, data_no_range.first, data_no_range.second, gen_dir_path);
    if (!result.has_value()) {
        std::cout << result.error() << std::endl;
        goto fail;
    }
    page_no = 0;
fail:
    page_no = 0;
    pause_any_key();
    return;
}

int main() {
    std::cout << "Data generator launched successfully!\nVersion:1.0\n" << std::flush;
    std::this_thread::sleep_for(1000ms);
    while (true) {
        pages[page_no]();
        if (page_no == 6) break;
    }
    return 0;
}