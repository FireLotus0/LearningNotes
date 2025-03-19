# 命令示例
- [静态库和动态库的区别](#静态库和动态库的区别)
- [库的版本](#库的版本)
- [生成库](#生成库)
    - [install](#install)
- [链接库](#库的链接)
    - [查找库和头文件](#查找库和头文件)
    - [根据配置链接](#根据配置链接)
- [安装依赖库](#安装依赖库)
- [目标的导入导出](#目标的导入导出)
    - [导入](#导入)
    - [导出](#导出)
- [高级命令](#高级命令)
    - [手动添加依赖关系](#手动添加依赖关系)

### 库的版本
- VERSION:库版本
- SOVERSION:API版本

### 静态库和动态库的区别
- 动态库在运行时链接，静态库在编译时链接
- 动态库节省磁盘和内存，通常使用target_link_library链接，动态库的依赖库也需要写出；相反，静态库链接时不需要写出依赖库
- 动态库及其依赖作为一个原子单元，在链接时，即使未使用该符号，仍然会链接动态库及其依赖；静态库中每一个obj对象文件作为一个原子单元，只有使用到才会链接
- windows上动态库通常存放在bin目录下，和可执行文件一样，对应安装选项RUNTIME DESTINATION bin;动态库的导入库与静态库一样，通常在lib/MyProject下面，对应安装选项，ARCHIVE DESTINATION lib/MyProject
- Linux系统上，动态库不需要导入库
- 优缺点：  
    - 动态库节省存储，减少编译链接的时间，动态库修改时接口未改变情况下，不需要修改程序，但是静态库修改后，程序需要重新编译链接
    - 静态库不依赖外部环境，容易打包发布

### 生成库
#### install
- 安装什么：
    - TARGETS:二进制文件，库，程序
        - RUNTIME:
            - 可执行程序，add_executable
        - LIBRARY:
            - add_library：指定MODULE选项，.dll .so
            - add_library: 指定SHARED选项, .so
            - add_library: 指定SHARED选项，.dll
        - ARCHIVE:
            - windows上动态库的导入库：.lib
            - 静态库: .lib .a
        - 如果Targetsw是库，则可以指定选项:
            - NAMELINK_ONLY
            - NAMELINK_SKIP
            - 未指定，则默认都安装
        ```
        # 库的安装位置因链接器的行为在不同平台上不同而有所区分
        # linux上动态库通常安装在lib，windows上动态库安装在bin，和程序在一个目录
        # 导入库和静态库一样在lib/MyProject
        # 在不手动区分平台的情况下，一个命令中完成并自动区分示例：
        install(TARGETS MySharedLib
            RUNTIME bin
            LIBRARY lib
            ARCHIVE lib/MyProject
        )
        ```
    - FILE:普通文件，例如头文件，程序数据文件
    - DIRECTORY:目录树，例如资源文件
    - PROGRAM:例如shell脚本，和FILE的区别是默认的权限不同
    - SCRIPTS:用户提供的cmake脚本，在pre-install或post-install执行
    - CODE:字符串形式提供的cmake命令
        ```
        install(CODE "message(\"Installing MyPrject\")")
        ```
- 其他属性：
    - DESTINATION: 绝对路径或者相对路径(结合CMAKE_INSTALL_PREFIX)
    - CNFIGURATION: 指定配置下进行安装，例如DEBUG,RELEASE，参考缓存变量CMAKE_BUILD_TYPE
    - COMPONENT, OPTIONAL

 
### 库的链接
#### 查找库和头文件
常用命令：find_library, find_parth

#### 根据配置链接
```
target_link_library(myTarget debug libDebug release libRelease)
```

### 安装依赖库
- get_prerequisites(): 在GetPrerequisites.cmake中，使用平台本地工具分析程序依赖库，windows(dumpbin), linux(ldd)
- fixup_budle()： 在BundleUtilities.cmake中，可以创建一个install tree
1. FixBundle.cmake.in: cmake.in 文件 是 CMake 的一种模板文件，允许使用变量占位符来生成配置文件,
    @ONLY 关键字表示仅替换 @VAR@ 格式的占位符，而不替换 ${VAR} 格式的占位符
    ```
    # 配置一个可以在安装执行时调用的cmake脚本，里面包含BundleUtilities.cmake
    include(BundleUtilities)

    set(bundle "${CMAKE_INSTALL_PREFIX}/MyExecutable@CMAKE_EXECUTABLE_SUFFIX@")
    # 列出不能被依赖分析得到的库路径
    set(other_libs "")
    # 设置依赖库查找目录
    set(dirs "@LIBRARY_OUTPUT_PATH@")

    fixup_bundle("${bundle}"  "${other_libs}"  "${dirs}")
    ```
2. 
    ```
    install(TARGETS MyExecutable DESTINATION bin)

    # 安装系统库，例如MSVC运行时库
    include(InstallRequiredSystemLibraries)
    # 安装第三方库
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/FixBundle.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/FixBundle.cmake
        @ONLY
    )

    install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/FixxBundle.cmake)
    ```

### 目标的导入导出
#### 导入
使用IMPORTED选项，不会生成构建文件，仅仅师是对目标的引用
```
add_executable(other_program IMPORTED)
set_property(TARGET other_program PROPERTY
    IMPORTED_LOCATION "path/other_program"
)
# 使用
add_custom_command(OUTPUT generated.c COMMAND other_program generated.c)

# linux上导入库
add_library(unix_lib IMPORTED)
set_property(TARGET unix_lib PROPERTY
    IMPORTED_LOCATION "path/unix_lib.a"
)
# windows上导入
add_library(win_lib IMPORTED)
set_property(TARGET win_lib PROPERTY
    IMPORTED_LOCATION "path/win_lib.dll"
    IMPOORTED_IMPLIB "path/win_lib.lib"
)

# 若存在多个配置
add_library(foo IMPORTED)
set_property(TARGET foo PROPERTY
    IMPORTED_LOCATION_DEBUG ""
    IMPORTED_LOCATION_RELEASE ""
)

```

#### 导出
- 导入目标时，缺点是需要知道目标在磁盘上路径位置，不方便，因此，导出目标就是用于帮助简化导入操作。install(TARGETS)和install(EXPORT)一起使用。下面脚本将会安装两个文件  
    - < path >/lib/MyProjetc/generators/generator
    - < path >/lib/MyProject/MyProject-targets.cmake
    ```
    add_executable(generator generator.c)
    install(TARGETS generator DESTINATION lib/MyProject/generator EXPORT MyProject-targets)
    install(EXPORT MyProject-targets DESTINATION lib/MyProject)
    ```
    外部导入时只需要include("path/MyProject-targets.cmake")
- 合并导出
    ```
    # A/CMakeLists.txt
    add_excutable(a_exe a.c)
    install(TARGETS a_exe DESTINATION bin
        EXPORT MyProject-targets
    )

    # B/CMakeLists.txt
    add_library(b_lib b.c)
    install(TARGETS b_lib DESTINATION lib
        EXPORT MyProject-targets
    )

    # Main CMakeLists.txt
    add_subdirectory(A)
    add_subdirectory(B)
    install(EXPORT MyProject-targets DESTINATION lib/MyProject)
    ```
- 在不安装的情况下，直接从构建目录下导入目标，用于跨平台编译
    ```
    add_executable(generator generator.c)
    export(TARGETS genertor FILE MyProject-exports.cmake)
    ```

### 高级命令
#### 手动添加依赖关系
    - add_dependencies
    - include_regular_expression