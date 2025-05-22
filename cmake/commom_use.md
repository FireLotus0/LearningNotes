### CMake常用命令

1. 根据目标系统定义宏
	- CMAKE_SYSTEM_NAME 是一个 CMake 内置变量，表示目标系统的名称
		- 它由 CMake 自动设置，值通常为目标操作系统的名称，例如：
			- Linux
			- Windows
			- MacOS
		- 可以用它来检测当前项目运行在哪个操作系统上
	- MATCHES 是 CMake 的一个条件操作符，用来检查变量的值是否匹配指定的正则表达式
	- add_definitions() 是 CMake 用于为编译器添加预处理器定义的命令
		- -D<macro> 是编译器的选项，用于在预处理阶段定义宏。
		- -DGLES 相当于在代码中添加了 #define GLES。
		- 在代码中，开发者通常使用宏定义来控制条件编译
		```c
			#ifdef GLES
			// 针对 OpenGL ES 的代码
			#else
			// 针对其他图形 API 的代码
			#endif
		```
	- add_definitions 是较早的 CMake 命令，现代 CMake 推荐使用 target_compile_definitions，它可以更细粒度地控制目标
	```cmake
		if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    		target_compile_definitions(my_target PRIVATE GLES)
		endif()
	```
```c
if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    add_definitions(-DGLES)
endif()
```

2. 针对 Windows 平台，进行一些特定的编译设置，包括启用 UTF-8 编码支持和为 Release 模式添加调试信息
```cmake
if (WIN32)
    #编译使用utf8文件
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
        # 为Release添加调试信息
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
        set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
    endif ()
endif()
```
	- WIN32 是 CMake 的内建变量，用于判断目标平台是否是 Windows
		- 如果当前系统是 Windows（包括 32 位和 64 位），条件为真，执行后续代码
		- /utf-8 是 MSVC 的一个选项，用于将源文件的默认编码设置为 UTF-8
		- 它会确保编译器正确地解析 UTF-8 编码的源文件，避免因编码问题导致的编译错误（如中文字符乱码）。
	- "$<$<C_COMPILER_ID:MSVC>:/utf-8>" 和 "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>" 使用了 CMake 的 生成表达式（Generator Expressions）。
		- <$<C_COMPILER_ID:MSVC>:/utf-8> 的意思是：如果 C 编译器是 MSVC，则添加 /utf-8 选项
		- <$<CXX_COMPILER_ID:MSVC>:/utf-8> 的意思是：如果 C++ 编译器是 MSVC，则添加 /utf-8 选项。
		- 这种写法使得代码更加灵活，仅在 MSVC 编译器上生效，不会影响其他平台。
	- 为 Release 模式添加调试信息
		- 作用：
			- 如果当前构建类型是 Release，就为 Release 模式添加调试相关的编译和链接选项。
			- CMAKE_BUILD_TYPE：表示当前的构建类型，常见值有 Debug、Release、RelWithDebInfo 等。
			- set(CMAKE_CXX_FLAGS_RELEASE ...)：
				- 将 /Zi 添加到 Release 模式的 C++ 编译选项中
				- /Zi：告诉 MSVC 编译器生成调试符号（Program Database，PDB 文件），用于调试优化后的代码。
			- set(CMAKE_EXE_LINKER_FLAGS_RELEASE ...)：
				- 为 Release 模式的链接器添加选项
				- /DEBUG：生成调试符号文件（PDB 文件）
				- /OPT:REF：移除未使用的函数和数据，优化生成的可执行文件大小。
				- /OPT:ICF：合并相同的函数和数据，进一步优化大小。
	- 在 Release 模式下默认不生成调试信息，但有时需要为优化后的代码进行调试（如定位性能问题或确认 Release 的行为），这段代码可以启用调试符号，同时保持 Release 的优化特性。
	- 优化写法：  
	```cmake
		if (WIN32)
    			target_compile_options(my_target PRIVATE 
      			  "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>"
    		)
    		if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
        		target_compile_options(my_target PRIVATE /Zi)
        		target_link_options(my_target PRIVATE /DEBUG /OPT:REF /OPT:ICF)
    		endif ()
		endif ()
	```


3. 为什么需要 /utf-8？
	- 默认情况下，MSVC 编译器会根据系统的区域设置来推测源文件的编码（通常是 ANSI 编码）。
	- 如果源文件包含非 ASCII 字符（如中文注释、特殊符号），而文件使用 UTF-8 编码，可能会导致编译错误或乱码。
	- 添加 /utf-8 选项可以强制 MSVC 编译器将源文件解释为 UTF-8 编码，避免这些问题。

4. 为什么 Release 模式需要调试信息？
	- 在生产环境中，Release 模式的代码通常是经过优化的，运行速度更快，文件更小，但缺乏调试信息。
	- 通过添加 /Zi 和 /DEBUG，可以在 Release 模式下生成调试符号文件（PDB 文件），用于调试优化后的代码。

5. 指定C++版本，如果版本不满足，cmake会报错
```cmake
set(CMAKE_CXX_STANDARD_REQUIRED on)
```

6. 这个指令的作用是告诉 CMake 在编译时包含当前目录的头文件。这意味着，如果当前目录包含头文件，CMake 会自动将当前目录添加到编译器的搜索路径中，方便你在源文件中直接引用头文件而不需要使用相对路径
```cmake
set(CMAKE_INCLUDE_CURRENT_DIR on)

#更现代的写法
target_include_directories(<target> PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
```
在现代 CMake 中，通常也会使用 CMakeLists.txt 中的 include_directories 或 target_include_directories 来显式地定义包含目录。尽管 CMAKE_INCLUDE_CURRENT_DIR 仍然可用，但使用 target_include_directories 是更推荐的做法

7. 添加带值的宏定义：
```cmake
 add_compile_definitions(ROOT_PROJECT_NAME="${PROJECT_NAME}")
```
- add_compile_definitions 指令用于向编译器添加预处理器定义。它通常用于定义在源码中需要使用的宏，以便在编译时影响代码的条件编译在这个特定的例子中，ROOT_PROJECT_NAME 被定义为当前项目的名称（由 ${PROJECT_NAME} 变量获取）。这意味着在项目的代码中，您可以使用 ROOT_PROJECT_NAME 这个宏来获取项目名称，例如用于调试、日志记录或其他需要特定项目名称的功能。

8. include() 函数：CMake 的 include() 函数用于引入其他 CMake 脚本文件或模块。在调用 include() 时，CMake 会读取并执行指定文件中的所有指令
- 如果 src.cmake 中定义了一些函数或宏，你可以在当前的 CMakeLists.txt 中调用它们
```cmake 
include(${CMAKE_SOURCE_DIR}/modules/singleinstance/src.cmake)

# 假设 src.cmake 中定义了一个函数 `configure_single_instance`
configure_single_instance()
```

9. 包含 CPM 模块:
```cmake
# 这行代码包含了 CPM（CMake Package Manager）模块的 CMake 脚本。CPM 允许你在 CMake 项目中轻松地添加和管理外部库或依赖。
include(${CMAKE_SOURCE_DIR}/cpm/CPM_0.40.2.cmake)

CPMAddPackage(
        NAME Dev-Tools
        GIT_REPOSITORY git@192.168.1.159:qt-dev/dev-tools.git
        GIT_TAG v1.0.5
        DOWNLOAD_ONLY YES
        SOURCE_DIR "${CMAKE_SOURCE_DIR}/dev-tools")

```
- CPMAddPackage 是 CPM 提供的一个函数，用于添加外部依赖。
- NAME Dev-Tools：指定包的名称为 "Dev-Tools"。
- GIT_REPOSITORY：指定 Git 仓库的地址，这里是一个本地网络地址。
- GIT_TAG v1.0.5：指定要下载的 Git 标签，表示版本。
- DOWNLOAD_ONLY YES：表示只下载包，而不构建它。
- SOURCE_DIR "${CMAKE_SOURCE_DIR}/dev-tools"：指向下载的源代码存放的目录。

10. include_directories 是用来指定头文件搜索路径的命令，可以用于简化源文件中的头文件引用。现代 CMake 项目建议使用 target_include_directories，因为它提供了更好的作用域控制和可维护性。
- 作用域：include_directories 添加的路径会影响后续的所有目标。如果希望只对特定目标生效，可以使用 target_include_directories 命令，这是更现代且推荐的做法。
```cmake 
add_library(MyLibrary src/mylibrary.cpp)
target_include_directories(MyLibrary PRIVATE ${CMAKE_SOURCE_DIR}/include)
```
- 多个目录：可以同时指定多个目录，例如：
```cmake
include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src)

```

11. add_subdirectory 是 CMake 中一个非常重要的命令，用于将子目录中的 CMakeLists.txt 文件包含到当前构建中。通过使用这个命令，可以将大型项目分解为多个子模块或子项目，使得构建过程更加清晰和结构化。
```cmake
add_subdirectory(<directory> [binary_dir] [EXCLUDE_FROM_ALL])
```
- <directory>：子目录的路径，其中包含一个 CMakeLists.txt 文件。
- [binary_dir]（可选）：指定子目录的构建目录。如果没有提供，CMake 默认会在与源目录相同的位置创建构建目录。
- [EXCLUDE_FROM_ALL]（可选）：如果指定该选项，则该子目录中的目标不会被默认构建，除非显式要求。
```cmake
/my_project
    CMakeLists.txt
    /lib
        CMakeLists.txt
    /app
        CMakeLists.txt
```
- 在项目的根目录（my_project）中的 CMakeLists.txt 文件中，可以使用 add_subdirectory 来包含子目录：
```cmake
cmake_minimum_required(VERSION 3.10)
project(MyProject)

add_subdirectory(lib)
add_subdirectory(app)

```

12. find_package(): 是 CMake 中用于查找和配置外部包的命令,这个命令用于查找已安装的包（在这里是 Qt）。它会设置相应的变量，帮助你找到需要的库和头文件。
```cmake
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Gui REQUIRED)
```
- 这是我们要查找的包的名称。在这个例子中，我们是在寻找 Qt 库
- NAMES Qt6 Qt5：这一行指定了我们希望查找的包的候选名称。CMake 会首先尝试查找 Qt6，如果找不到，则会尝试 Qt5。这对于支持不同版本的 Qt 非常有用。
- 
```cmake
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS
            Core
            Gui
            Widgets
            Network
            Sql
            OpenGL
            REQUIRED)
```
- ${QT_VERSION_MAJOR} 是一个变量，通常在 CMake 文件中先通过 find_package(Qt REQUIRED) 设置。它指定了你要查找的 Qt 版本的主版本号
- COMPONENTS: 后面跟着的是你希望查找的 Qt 组件列表。每个组件都对应 Qt 的一个模块

13. set(CMAKE_CXX_FLAGS "-Wl,--copy-dt-needed-entries")
- CMAKE_CXX_FLAGS 是用于设置 C++ 编译器标志的变量。
- -Wl,--copy-dt-needed-entries 是一个链接器选项。在 GNU 系统下，它指示链接器复制依赖库的必要条目。这在使用某些动态库时可能是必要的，以确保所有需要的符号在最终的可执行文件中可用。

14. add_definitions(-Wno-attributes)
- -Wno-attributes 是一个 GCC 和 Clang 编译器的选项，它告诉编译器在编译时忽略有关属性的警告。这可能在某些情况下是有用的，比如在使用了不太标准的代码或库时。

15. file(GLOB_RECURSE ...) 是 CMake 中用于查找文件的命令。它可以根据给定的模式递归地搜索目录，并将符合条件的文件路径存储到变量中。 
```cmake
file(GLOB_RECURSE GRAPHIC_SRC ${CMAKE_CURRENT_SOURCE_DIR}/graphicview/*.h
                              ${CMAKE_CURRENT_SOURCE_DIR}/graphicview/*.cpp)
```
- 递归查找: 在 graphicview 目录及其子目录中查找所有的 .h 和 .cpp 文件
- 存储结果: 将找到的所有文件路径存储在变量 GRAPHIC_SRC 中，以供后续使用（例如添加到可执行文件或库中）
- 语法:
	- file(GLOB_RECURSE <variable> <globbing expressions>)
	- <variable>: 这是用于存储找到的文件列表的变量名。在你的示例中是 GRAPHIC_SRC
	- <globbing expressions>: 一个或多个 glob 模式，用于匹配文件名。例如，*.h 匹配所有以 .h 结尾的文件
- 使用 file(GLOB_RECURSE ...) 可能会使得 CMakeLists.txt 文件中的文件变化不够显式，且不利于增量构建（CMake 可能不会检测到新添加的文件而不重新运行配置）。因此，推荐的现代写法是显式列出源文件，或者使用其他方式来处理文件，尤其是在大型项目中。
	- 尽管在小项目中使用 GLOB 可以简化代码，但在较大项目中，建议显式列出文件，减少潜在的构建问题
```cmake
set(GRAPHIC_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/graphicview/file1.h
    ${CMAKE_CURRENT_SOURCE_DIR}/graphicview/file1.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graphicview/file2.h
    ${CMAKE_CURRENT_SOURCE_DIR}/graphicview/file2.cpp
)
```

16. 使用 CMake 的自动化功能来管理文件:
	1. 使用 CMake 的自定义命令和目标:
		- 可以使用 Python、Shell 或其他脚本语言编写一个简单的脚本，扫描指定目录，生成 CMake 文件或文本文件，包含所有源文件的列表。
		```python
			import os
			import sys

			def find_source_files(directory):
    			source_files = []
    			for dirpath, _, filenames in os.walk(directory):
        			for filename in filenames:
            			if filename.endswith(('.cpp', '.h')):
                			source_files.append(os.path.join(dirpath, filename))
    			return source_files

			if __name__ == "__main__":
    			directory = sys.argv[1]
    			source_files = find_source_files(directory)
    			with open('sources.cmake', 'w') as f:
        			f.write('set(SOURCE_FILES\n')
        			for file in source_files:
            			f.write(f'    {file}\n')
        			f.write(')\n')

		```
		- 在你的 CMakeLists.txt 中，使用 execute_process 调用这个脚本，并包含生成的 CMake 文件:
		```cmake
			cmake_minimum_required(VERSION 3.5)
			project(MyProject)

			# 调用外部脚本生成源文件列表
			execute_process(COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/generate_sources.py ${CMAKE_CURRENT_SOURCE_DIR}/graphicview
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                  RESULT_VARIABLE result)

			if(NOT result EQUAL 0)
    			message(FATAL_ERROR "Failed to generate source files")
			endif()

			# 包含生成的 CMake 文件
			include(sources.cmake)

			# 创建可执行文件
			add_executable(MyExecutable ${SOURCE_FILES})

		```
	2. 使用 CMake 的 file(GLOB ...) 结合 CMake 变量
	3. 使用 CMake 的 configure_file 和 file(WRITE):如果需要在构建时动态生成某些文件，可以使用 configure_file 或 file(WRITE) 来创建包含源文件列表的文件
	```cmake
		cmake_minimum_required(VERSION 3.5)

		project(MyProject)

		# 生成源文件列表
		set(SOURCE_FILES "")
		file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/graphicview/*.h ${CMAKE_CURRENT_SOURCE_DIR}/graphicview/*.cpp)
		foreach(src ${SOURCES})
    		set(SOURCE_FILES ${SOURCE_FILES} ${src})
		endforeach()

		# 创建可执行文件
		add_executable(MyExecutable ${SOURCE_FILES})

	```
	4. 使用 CMake 的 file(REMOVE_IF_EXISTS) 和 file(APPEND):在某些复杂情况下，你可能需要清理某些文件或根据条件动态生成文件。例如，可以在构建开始时删除旧的文件列表，生成新的文件列表。


