#include "singleinstancecontrol.h"

#include <qcoreapplication.h>
#include <qdebug.h>

SingleInstanceControl::SingleInstanceControl(QObject *parent)
    : QObject(parent)
{
}

int SingleInstanceControl::listen() {
    QLocalSocket localSocket;
    localSocket.connectToServer(QCoreApplication::applicationName());
    if (localSocket.waitForConnected(1000)) {
        return -1;
    }

    localServer = new QLocalServer(this);
    QLocalServer::removeServer(QCoreApplication::applicationName());
    if (!localServer->listen(QCoreApplication::applicationName())) {
        qDebug() << localServer->errorString();
        return 0;
    }

    connect(localServer, &QLocalServer::newConnection, this, [&] {
        auto socket = localServer->nextPendingConnection();
        connect(socket, &QLocalSocket::disconnected, socket, &QLocalSocket::deleteLater);
        emit anotherInstanceCreated();
    });

    return 1;
}