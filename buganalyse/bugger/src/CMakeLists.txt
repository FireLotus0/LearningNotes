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
    widgets/frame/windowframelesshelper.h
    widgets/frame/windowframelesshelper.cpp

    dialog/info/infodlg.h
    dialog/info/infodlg.cpp
    dialog/login/logindlg.h
    dialog/login/logindlg.cpp

    logger/logger.h

    utils/sessionmanager/sessionmanager.h
    utils/sessionmanager/sessionmanager.cpp

    utils/utils.h
    utils/utils.cpp
    utils/macros.h

    utils/task/taskentity.h
    utils/task/taskentity.cpp
    utils/task/taskexecutor.h
    utils/task/taskexecutor.cpp

    utils/session/session.h
    utils/session/session.cpp
    utils/session/channelsession.h
    utils/session/channelsession.cpp
    utils/session/scpsession.h
    utils/session/scpsession.cpp
    utils/session/sftpsession.h
    utils/session/sftpsession.cpp
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
