#include "unittest.h"

#include <qtest.h>

UnitTest::UnitTest(QObject *parent)
        : QObject(parent) {
}

void UnitTest::initTestCase() {

}

void UnitTest::cleanup() {

}

void UnitTest::cleanupTestCase() {

}

void UnitTest::simpleUnitTest() {
    int a = 1 + 2;
    QCOMPARE(a, 3);
}
