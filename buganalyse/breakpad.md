[原理](#原理)

### 原理  
![breakpad.png]()
#### 概述 
- 对于应用程序，通常希望在启动过程中尽可能早地安装处理程序。使用 Breakpad 来监控自身库的开发者可能希望在库加载时安装 Breakpad 处理程序，或者可能只想在调用库时安装处理程序。
- 处理器可以通过捕获异常或由应用程序本身请求来触发生成转储。后一种情况在调试断言或其他开发者希望了解程序如何进入特定非崩溃状态的情况下可能很有用。在生成转储后，处理器调用用户指定的回调函数。回调函数可以收集有关程序状态的额外数据，退出程序，启动崩溃报告应用程序，或执行其他任务。允许通过回调函数来决定此功能可以保持灵活性。  
- 整个客户端系统的规范示例可能是，一个监控的应用程序将其链接到处理程序库，从其主函数中安装 Breakpad 处理程序，并提供一个回调以启动一个小型崩溃报告程序。崩溃报告程序将链接到发送器库，并在启动时发送崩溃转储。由于从崩溃进程中执行任何大量工作固有的不可靠性，建议为此功能使用单独的进程。
#### 异常处理程序安装  
异常处理机制的安装方式在不同操作系统之间有所不同。在 Windows 上，这只是一个相对简单的问题，只需调用一次注册顶级异常过滤器回调函数。在大多数类 Unix 系统（如 Linux）上，进程通过信号的传递来得知异常，因此异常处理器的形式是一个信号处理器。在 Mac OS X 上捕获异常的原生机制需要大量的代码来设置一个 Mach 端口，将其识别为异常端口，并分配一个线程来监听该端口的异常。正如异常处理器的准备方式不同一样，它们被调用的方式也不同。在 Windows 和大多数类 Unix 系统上，处理器是在引发异常的线程上被调用的。在 Mac OS X 上，监听异常端口的线程会被通知已发生异常。Breakpad 处理器库的不同实现方式在每个平台上以适当的方式执行这些任务，同时在每个平台上暴露出类似的接口。

一个 ***Breakpad 处理器*** 体现在一个 ***ExceptionHandler*** 对象中。它是一个 C++对象， ExceptionHandler 可以创建为局部变量，允许它们在函数调用和返回时安装和移除。这为开发者提供了一种仅监控应用程序部分以检测崩溃的可能方法。

#### 过滤器回调
当处理线程开始处理异常时，它会调用一个可选的用户定义的过滤器回调函数，该函数负责判断 Breakpad 的处理程序是否应该继续处理该异常。此机制提供给了库或插件代码的好处，这些代码的开发者可能对它们模块之外但宿主其代码的进程发生的崩溃报告不感兴趣。如果过滤器回调指示它对异常不感兴趣，Breakpad 处理程序会安排将其传递给任何先前安装的处理程序。

#### Dump Generation
假设过滤器回调函数批准（或不存在），处理程序在处理程序安装时由应用程序开发者指定的目录中写入转储，使用先前生成的唯一标识符以避免名称冲突。转储生成的机制在不同平台之间也有所不同，但通常，该过程涉及枚举每个执行线程，并捕获其状态，包括处理器上下文和其堆栈区域的活跃部分。转储还包括已加载到应用程序中的代码模块列表，以及指示哪个线程生成了异常或请求了转储。为了避免在此过程中分配内存，转储在磁盘上就地写入。

#### 发送库
Breakpad 发送库提供了一个函数，用于将崩溃报告发送到崩溃服务器。它接受崩溃服务器的 URL、将随 dump 一起发送的键值参数映射以及 dump 文件的路径。每个键值参数和 dump 文件都作为 multipart HTTP POST 请求的独立部分发送到指定的 URL，使用平台本地的 HTTP 功能。在 Linux 上，使用 libcurl 执行此功能，因为它是该平台上最接近标准 HTTP 库的东西。
