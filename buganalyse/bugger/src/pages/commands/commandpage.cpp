#include "commandpage.h"

CommandPage::CommandPage(unsigned int sessionId, QWidget *parent)
    : id(sessionId)
{
    ui.setupUi(this);
    init();
}

void CommandPage::onCommandFinished(bool success, const QString &result) {
    ui.textEdit->append(prefix + result.trimmed());
}

void CommandPage::init() {
    session = SMPTR->getSession<ChannelSession>(id);
    Q_ASSERT(session);
    connect(SMPTR, &SessionManager::sigCommandFinished, this, &CommandPage::onCommandFinished);
    prefix = "[" + QString::fromStdString(session->getUserIp()) + "]: ";

    SMPTR->executeShellCmd(id, "ls");
    SMPTR->executeShellCmd(id, "ls");
    SMPTR->executeShellCmd(id, "ls");
    SMPTR->executeShellCmd(id, "pwd");

}

void CommandPage::clearScreen() {
    ui.textEdit->clear();
    if(!prefix.isEmpty()) {
       ui.textEdit->append(prefix);
    }
}