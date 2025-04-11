#include "app.h"
#include "src/widgets/sessioninfo/fileviewer.h"

#include <qtimer.h>
#include <qstyleoption.h>
#include <qpainter.h>
#include <qdebug.h>
#include <qgraphicseffect.h>

App::App(QWidget *parent)
        : WindowFramelessWidget<QWidget>(parent)
{
    ui.setupUi(this);
    setMoveAreaWidget(ui.title);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_DeleteOnClose, false);

    sessionManager = new SessionManager(this);
}

void App::initUi() {
    ui.vTag->init(CustomBtnGroup::VERTICAL, QColor("#f7f6e7"), QColor("#c2e9fb"), {":/res/res/session.png", ":/res/res/search_dev.png", ":/res/res/setting.png"}, true);

    auto fileViewer = new FileViewer(this);
    ui.sessionInfo->addWidget(fileViewer);
}

void App::showEvent(QShowEvent *event) {
    initUi();
    QWidget::showEvent(event);

    scpSessions.push_back(sessionManager->createSession(SessionType::SHELL, "root", "Xykj20160315", "192.168.1.159").second);
//    scpSessions.push_back(sessionManager->createSession(SessionType::SHELL, "root", "Xykj20160315", "192.168.1.159").second);
//    scpSessions.push_back(sessionManager->createSession(SessionType::SHELL, "root", "Xykj20160315", "192.168.1.159").second);
//    scpSessions.push_back(sessionManager->createSession(SessionType::SHELL, "root", "Xykj20160315", "192.168.1.159").second);

//    scpSessions.push_back(sessionManager->createSession(SessionType::SCP, "root", "Xykj20160315", "192.168.1.159").second);
//    scpSessions.push_back(sessionManager->createSession(SessionType::SCP, "root", "Xykj20160315", "192.168.1.159").second);
//    scpSessions.push_back(sessionManager->createSession(SessionType::SCP, "root", "Xykj20160315", "192.168.1.159").second);
    scpSessions.push_back(sessionManager->createSession(SessionType::SCP, "root", "Xykj20160315", "192.168.1.159").second);

    QList<QString> localFiles = {"E:\\test\\err.txt", "E:\\test\\info.txt", "E:\\test\\test.py", "E:\\test\\testSteps.txt"};
    QList<QString> remoteFiles = {"/root/err.txt", "/root/info.txt", "/root/test.py", "/root/testSteps.txt"};


    for(int i = 0; i < 2; i++) {
        QTimer::singleShot((i + 1) * 2000, [&, localFiles, remoteFiles, i]{
            sessionManager->scpTransfer(scpSessions[i], localFiles[i], remoteFiles[i], true);
        });
    }
}

void App::on_btn_quit_clicked() {
    this->close();
}
