# DataGenerator

（本工具目前只在 Windows 下进行了测试）

用于批量生成 OI/ACM 竞赛题目测试数据的交互式终端工具。

给定一个随机数据生成器和一个标准程序，DataGenerator 会自动完成以下工作：

1. 运行数据生成器，生成测试点输入。
2. 将输入交给标准程序，生成对应输出。
3. 按测试点编号保存为同名的 `.in` 和 `.ans` 文件。

## 功能特点

- 支持 Windows、macOS 和 Linux。
- 使用系统文件选择器选择数据生成器和标准程序。
- 使用系统文件夹选择器指定输出目录。
- 可设置连续的测试点编号范围，例如 `1` 到 `10`。
- 生成过程中逐个显示测试点状态，失败时报告对应编号。

## 程序约定

数据生成器和标准程序都必须是可执行文件，并通过标准流工作：

- 数据生成器：不需要输入，从标准输出打印一组测试数据。
- 标准程序：从标准输入读取一组测试数据，并将答案打印到标准输出。

例如，选择编号范围 `1` 到 `3`、输出目录为 `./data` 后，程序会生成：

```text
data/
	1.in
	1.ans
	2.in
	2.ans
	3.in
	3.ans
```

如果目标目录中已经存在同名文件，生成时会覆盖这些文件。程序不会限制单个测试点的运行时间，也不会捕获程序的标准错误输出。

## 构建

项目使用 CMake，要求支持 C++23 的编译器。

### Windows

在项目根目录执行：

```powershell
cmake -S . -B build
cmake --build build --config Release
```

可执行文件通常位于：

```text
build/Release/datagenerator.exe
```

使用单配置生成器时，也可能位于 `build/datagenerator.exe`。

### macOS / Linux

```bash
cmake -S . -B build
cmake --build build
./build/datagenerator
```

Linux 的文件和文件夹选择功能需要安装 `zenity` 或 `kdialog`。例如 Debian/Ubuntu：

```bash
sudo apt install zenity
```

## 使用流程

启动 `datagenerator` 后，在主菜单中依次选择：

1. **Select input data generation program**：选择数据生成器。
2. **Select standard program**：选择标准程序或正确答案程序。
3. **Set the range of data sets ID**：输入起始和结束编号，编号范围包含两端。
4. **Select the generation directory**：选择 `.in/.out` 文件的保存目录。
5. **Generate**：开始生成测试数据。
6. **Exit**：退出程序。

生成完成后，可以修改数据生成器中的数据范围，再次设置新的编号范围继续生成。

## 项目语言标准

项目使用 **C++23**，源码中使用了 `std::expected`、`std::format` 和 `std::filesystem` 等标准库特性。请使用较新的 GCC、Clang 或 MSVC 编译器。

## 项目结构

```text
.
├── CMakeLists.txt
├── README.md
└── src/
		├── main.cpp
		├── data_generator.hpp
		├── file_selector.hpp
		├── directory_selector.hpp
		└── console_utils.hpp
```

