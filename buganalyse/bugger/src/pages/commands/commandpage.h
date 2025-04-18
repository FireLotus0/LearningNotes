#pragma once

#include "ui_commandpage.h"
#include "utils/sessionmanager/sessionmanager.h"
#include <qwidget.h>

class CommandPage : public QWidget {
    Q_OBJECT
public:
    explicit CommandPage(unsigned int sessionId, QWidget *parent = nullptr);

    void execute(const QString& cmd);

    void clearScreen();

private slots:
    void onCommandFinished(bool success, const QString& result);

private:
    void init();

private:
    Ui::CommandPage ui;
    QString prefix = "";
    unsigned int id;
    ChannelSession* session;
    bool prefixValid = false;
};
