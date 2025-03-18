# 命令示例
- [静态库和动态库的区别](#静态库和动态库的区别)
- [库的版本](#库的版本)
- [生成库](#生成库)
    - [install](#install)
- [链接库](#库的链接)
    - [根据配置链接](#根据配置链接)

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
- 其他属性：
    - DESTINATION: 绝对路径或者相对路径(结合CMAKE_INSTALL_PREFIX)
    - CNFIGURATION: 指定配置下进行安装，例如DEBUG,RELEASE，参考缓存变量CMAKE_BUILD_TYPE
    - COMPONENT, OPTIONAL
 
### 库的链接
#### 根据配置链接
```
target_link_library(myTarget debug libDebug release libRelease)
```