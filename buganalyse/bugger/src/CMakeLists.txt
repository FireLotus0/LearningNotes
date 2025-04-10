cmake_minimum_required(VERSION 3.2)
project(BugAnalyse)

set(SRC
    main.cpp
    app/app.h
    app/app.cpp
    app.qrc

    widgets/custombtngroup.h
    widgets/custombtngroup.cpp
    widgets/sessioninfo/fileviewer.h
    widgets/sessioninfo/fileviewer.cpp


    dialog/info/infodlg.h
    dialog/info/infodlg.cpp

    utils/frame/windowframelesshelper.h
    utils/frame/windowframelesshelper.cpp

    #    utils/ssh/sessionmanager.h
    #    utils/ssh/sessionmanager.cpp
    #    utils/ssh/session.h
    #    utils/ssh/session.cpp
    #    utils/ssh/taskexecutor.h
    #    utils/ssh/taskexecutor.cpp

    utils/task/macros.h
    utils/task/session.h
    utils/task/session.cpp
    utils/task/utils.h
    utils/task/utils.cpp
    utils/task/taskentity.h
    utils/task/taskentity.cpp
    utils/task/taskexecutor.h
    utils/task/taskexecutor.cpp
    utils/task/channelsession.h
    utils/task/channelsession.cpp
    utils/task/scpsession.h
    utils/task/scpsession.cpp
)

add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Network
    Qt5::Concurrent
    libssh2::libssh2_shared
    ws2_32
)
