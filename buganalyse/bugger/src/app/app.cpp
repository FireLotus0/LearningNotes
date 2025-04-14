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

    scpSessions.push_back(sessionManager->createSession(SessionType::SCP, "lyf", "rootlyf", "139.9.189.48").second);
//    scpSessions.push_back(sessionManager->createSession(SessionType::SHELL, "root", "Xykj20160315", "192.168.1.159").second);
//    scpSessions.push_back(sessionManager->createSession(SessionType::SHELL, "root", "Xykj20160315", "192.168.1.159").second);
//    scpSessions.push_back(sessionManager->createSession(SessionType::SHELL, "root", "Xykj20160315", "192.168.1.159").second);

//    scpSessions.push_back(sessionManager->createSession(SessionType::SCP, "root", "Xykj20160315", "192.168.1.159").second);
//    scpSessions.push_back(sessionManager->createSession(SessionType::SCP, "root", "Xykj20160315", "192.168.1.159").second);
//    scpSessions.push_back(sessionManager->createSession(SessionType::SCP, "root", "Xykj20160315", "192.168.1.159").second);
//    scpSessions.push_back(sessionManager->createSession(SessionType::SCP, "lyf", "rootlyf", "139.9.189.48").second);

    QList<QString> localFiles = {"E:\\tmp\\err.txt", "E:\\tmp\\info.txt", "E:\\tmp\\tmp.py", "E:\\tmp\\testSteps.txt"};
    QList<QString> remoteFiles = {"/home/lyf/err.txt", "/home/lyf/info.txt", "/home/lyf/tmp.py", "/home/lyf/testSteps.txt"};



    sessionManager->scpTransfer(scpSessions[0], localFiles[0], remoteFiles[0], true);

}

void App::on_btn_quit_clicked() {
    this->close();
}
