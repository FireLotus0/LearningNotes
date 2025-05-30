#include "libusb.h"
#include "usbmanager.h"
#include <qdebug.h>


class Test : public QObject {
    Q_OBJECT
public:
    Test(QObject* parent = nullptr) : QObject(parent) {}

public slots:

};


int main()
{



    libusb_exit(nullptr);
}