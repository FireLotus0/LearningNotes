#include "src/utils/session/channelsession.h"
#include "src/utils/session/scpsession.h"

#include <filesystem>

int main()
{
    TASK_EXECUTOR.start();
//    ChannelSession channelSession("root", "Xykj20160315", "192.168.1.159");
//    channelSession.addCmdTask("ls");
    std::string curPath =  std::filesystem::current_path().string() + "/test_file.txt";

    ScpSession scpSession("root", "Xykj20160315", "192.168.1.159");
    scpSession.addScpTask("/root/rote_file.txt", curPath, false);

    std::this_thread::sleep_for(std::chrono::seconds(10));
    TASK_EXECUTOR.stop();
    return 0;
}