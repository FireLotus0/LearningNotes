#pragma once

#include <qobject.h>
#include <qlocalsocket.h>
#include <qlocalserver.h>

class SingleInstanceControl : public QObject {
    Q_OBJECT

public:
    explicit SingleInstanceControl(QObject *parent = nullptr);

    int listen();

signals:
    void anotherInstanceCreated();

private:
    QLocalServer* localServer;
};
