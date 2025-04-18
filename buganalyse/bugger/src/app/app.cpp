#include "app.h"
#include "src/widgets/sessioninfo/fileviewer.h"
#include "dialog/login/logindlg.h"
#include "pages/commands/commandpage.h"

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
}

void App::initUi() {
    ui.vTag->init(CustomBtnGroup::VERTICAL, QColor("#D3D3D3"), QColor("#FFF5E0"), {":/res/res/session.png", ":/res/res/search_dev.png", ":/res/res/setting.png"}, true);
    QList<QString> localFiles = {"E:\\tmp\\err.txt", "E:\\tmp\\info.txt", "E:\\tmp\\tmp.py", "E:\\tmp\\testSteps.txt"};
    QList<QString> remoteFiles = {"/home/lyf/err.txt", "/home/lyf/info.txt", "/home/lyf/tmp.py", "/home/lyf/testSteps.txt"};

//    SMPTR->scpTransfer(scpSessions[0], localFiles[0], remoteFiles[0], true);
//    auto fileViewer = new FileViewer(this);
//    ui.sessionInfo->addWidget(fileViewer);
}

void App::showEvent(QShowEvent *event) {
    initUi();
    QWidget::showEvent(event);
}

void App::on_btn_quit_clicked() {
    this->close();
}

void App::on_btn_ssh_clicked() {
    LoginDlg dlg(SessionType::SHELL);
    connect(&dlg, &LoginDlg::sessionCreated, this, &App::onNewConnCreated);
    dlg.exec();
    repaint();
    ui.btn_ssh->setChecked(false);
    ui.btn_sftp->setChecked(false);
    ui.btn_scp->setChecked(false);
}

void App::onNewConnCreated(int type, unsigned int id, const QString &sessionName) {
    switch(type) {
        case SessionType::SHELL:
        {
            ui.sessionContent->addWidget(new CommandPage(id));
            break;
        }
        default:
            break;
    }
}
