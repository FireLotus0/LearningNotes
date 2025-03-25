#pragma once

#include "session.h"
#include <qobject.h>

class SessionManager : public QObject {
Q_OBJECT
public:
    static SessionManager &getInstance();

signals:
    void errorOccurred(const QString& str);

private:
    explicit SessionManager(QObject *parent = nullptr);

    ~SessionManager();

    bool isWSValid() const { return wsValid; }

private:
    bool wsValid = false;
};
