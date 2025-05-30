#pragma once

#include <qobject.h>


class UnitTest : public QObject
{
    Q_OBJECT
public:
    explicit UnitTest(QObject *parent = nullptr);

private slots:
    void initTestCase();

    void simpleUnitTest();

    void cleanup();

    void cleanupTestCase();
};
