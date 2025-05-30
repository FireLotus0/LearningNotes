#include <arpa/inet.h>
#include <errno.h>
#include <error.h>
#include <fstream>
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <chrono>
#include <iomanip>

#define FILE_SZ (1024 * 1024 * 1024)
#define FRAME_SZ (10000 * 32768)


void createTestFile() {

    char buff[4096];
    std::fstream fileStream("test_data.txt", std::ios::out | std::ios::binary | std::ios::app);
    std::size_t sz = 0;
    while (sz < FILE_SZ) {
        memset(buff, 1, sizeof(buff));
        fileStream.write(buff, 4096);
        sz += 4096;
    }
    fileStream.flush();
    std::cout << "create file finished!" << std::endl;
}

std::ostream& printTime() {
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    auto milliseconds = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()) % 1000;
    std::ios::fmtflags tmpFlags(std::cout.flags());
    std::cout << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S") << "." << std::setw(3) << std::setfill('0') << milliseconds.count();
    std::cout.setf(tmpFlags);
    std::cout << ": ";
    return std::cout;
}

int main() {

    createTestFile();

    int listenFd = -1, clientFd = -1;
    struct sockaddr_in serverAddr;

    listenFd = socket(AF_INET, SOCK_STREAM, 0);

    if (listenFd == -1) {
        std::cout << "create listen  socket failed" << std::endl;
        return 1;
    }

    int opt = 1;
    setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(12345);

    if ((bind(listenFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)) {
        std::cout << "bind addr failed: " << strerror(errno) << std::endl;
        close(listenFd);
        return 1;
    } else {
        std::cout << "bind finish..." << std::endl;
    }

    if ((listen(listenFd, 10)) == -1) {
        std::cout << "listen failed: " << strerror(errno) << std::endl;
        close(listenFd);
        return 1;
    }
    std::cout << "start server";
    if ((clientFd = accept(listenFd, (struct sockaddr *)NULL, NULL)) == -1) {
        std::cout << "accept failed: " << strerror(errno) << std::endl;
        close(listenFd);
        return 1;
    } else {
        std::cout << "client connect success!" << std::endl;
    }
    std::fstream file("test_data.txt", std::ios::in | std::ios::binary);
    if (!file) {
        std::cout << "open file failed!" << std::endl;
        close(clientFd);
        close(listenFd);
        return 1;
    }

    char buffer[FRAME_SZ];
    std::size_t totalLen = 0;
    while (file.read(buffer, sizeof(buffer))) {
        auto len = send(clientFd, buffer, file.gcount(), 0);
        if(len < 0) {
            std::cout << "send error: " << strerror(errno) << std::endl;
        } else {
            totalLen += len;
            printTime() << "once send bytes: " << len << " total send: " << (totalLen / (1024 * 1024)) << " MB" << std::endl;
        }
    }

    close(clientFd);
    close(listenFd);

    std::cout << "file send finished";
    return 0;
}
