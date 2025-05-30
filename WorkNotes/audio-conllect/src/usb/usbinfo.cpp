#include "usbinfo.h"
#include <qdebug.h>

namespace UsbInfo {
QString getPrefix(InfoLevel infoLevel) {
    switch((int)infoLevel) {
        case 0: return "";
        case 1: return QString(1, '\t');
        case 2: return QString(2, '\t');
        case 3: return QString(3, '\t');
        case 4: return QString(4, '\t');
        case 5: return QString(5, '\t');
        case 6: return QString(6, '\t');
    }
    return "";
}

void printDevSpeed(int speed) {
    QString res;
    switch(speed) {
        case 0: res = "SUPPORT SPEED: LIBUSB_SPEED_UNKNOWN";break;
        case 1: res = "SUPPORT SPEED: LIBUSB_SPEED_LOW";break;
        case 2: res = "SUPPORT SPEED: LIBUSB_SPEED_FULL";break;
        case 3: res = "SUPPORT SPEED: LIBUSB_SPEED_HIGH";break;
        case 4: res = "SUPPORT SPEED: LIBUSB_SPEED_SUPER";break;
        case 5: res = "SUPPORT SPEED: LIBUSB_SPEED_SUPER_PLUS";break;
        default: res = "SUPPORT SPEED:  NOT SUPPORTED!";break;
    }
    qDebug() << res;
}

QString getStringFromCArray(unsigned char *arr, size_t length) {
    size_t validLength = 0;
    for (auto i = 0; i < length; i++) {
        if (arr[i] == '\0') {
            break;
        }
        validLength++;
    }
    return QString::fromStdString(std::string(reinterpret_cast<const char *>(arr), validLength));
}

QString getTransferType(int index) {
    switch(index) {
        case 0: return "Control Transfer";
        case 1: return "Sync Transfer";
        case 2: return "Bulk Transfer";
        case 3: return "Interrupt Transfer";
    }
    return "";
}

void printPointInfo(const libusb_endpoint_descriptor *endpointDescriptor, UsbDevice* usbDevice, bool configMatched) {
    auto prefix = getPrefix(UsbInfo::POINT_CONTENT);
    auto type = (endpointDescriptor->bEndpointAddress & 0x80) == 0 ? IOType::WRITE : IOType::READ;
    qDebug().noquote() << prefix << QString("Point Number%2: %1").arg(int(endpointDescriptor->bEndpointAddress & 0x0F)).arg(endpointDescriptor->bEndpointAddress);
    qDebug().noquote() << prefix << QString("Transfer Direction: %1").arg((endpointDescriptor->bEndpointAddress & 0x80) == 0 ? "OUT" : "IN");
    qDebug().noquote() << prefix << QString("Transfer Type: %1").arg(getTransferType(int(endpointDescriptor->bmAttributes & 3)));
    qDebug().noquote() << prefix << QString("Max Packet Size: %1").arg((int)endpointDescriptor->wMaxPacketSize);
    qDebug().noquote() << prefix << QString("Query Interval: %1").arg((int)endpointDescriptor->bInterval);
    if(configMatched) {
        usbDevice->insertEndPoint(uint8_t(endpointDescriptor->bEndpointAddress), libusb_transfer_type(endpointDescriptor->bmAttributes & 3), type);
    }
}

void printInterfaceInfo(const libusb_interface_descriptor *interfaceDescriptor, int cfgIndex,  UsbDevice* usbDevice) {
    int interfaceIndex = 0;
    auto interPrefix = getPrefix(UsbInfo::INTERFACE_CONTENT);
    do {
        qDebug().noquote() << interPrefix << QString("Interface Index: %1").arg((int)interfaceDescriptor->bInterfaceNumber);
        qDebug().noquote() << interPrefix << QString("Alt Interface: %1").arg((int)interfaceDescriptor->bAlternateSetting);
        qDebug().noquote() << interPrefix << QString("Point Count(exclude point 0): %1").arg((int)interfaceDescriptor->bNumEndpoints);
        // 遍历端点
        bool matched = usbDevice->interfaceRequired(cfgIndex, (int)interfaceDescriptor->bInterfaceNumber);
        for (int k = 0; k < interfaceDescriptor->bNumEndpoints; k++) {
            qDebug().noquote() << getPrefix(UsbInfo::POINT_DESC) << "Point Descriptor: ";
            printPointInfo(&interfaceDescriptor->endpoint[k], usbDevice, matched);
        }
        interfaceIndex++;
    } while (interfaceIndex < interfaceDescriptor->bAlternateSetting);
    // 添加端点0
//    usbDevice->insertEndPoint(0, LIBUSB_TRANSFER_TYPE_CONTROL);
}

void printInterfaceInfo(const libusb_interface *interfaceDescriptor, int cfgIndex, UsbDevice* usbDevice) {
    for (int interfaceIndex = 0; interfaceIndex < interfaceDescriptor->num_altsetting; interfaceIndex++) {
        printInterfaceInfo(&interfaceDescriptor->altsetting[interfaceIndex], cfgIndex, usbDevice);
    }
}

void printCfgInfo(libusb_device* device, int count, UsbDevice* usbDevice) {
    QString cfgPrefix = getPrefix(UsbInfo::DEVICE_DESC);
    QString cfgContent = getPrefix(UsbInfo::CONFIG_DESC);
    auto cfgDesc = cfgPrefix + QString("Config Descriptor: ");
    qDebug().noquote() << cfgDesc;
    for (int configIndex = 0; configIndex < count; configIndex++) {
        libusb_config_descriptor *config;
        if(libusb_get_config_descriptor(device, configIndex, &config) == 0) {
            cfgContent = cfgContent + QString("Config Index: %1").arg(configIndex);
            auto interNums = cfgContent + QString("Config Count: %1").arg((int)config->bNumInterfaces);
            auto interDesc =  getPrefix(UsbInfo::INTERFACE_DESC) + QString("Config Descriptor: ");
            qDebug().noquote() << cfgContent << Qt::flush;
            qDebug().noquote() << interNums;
            qDebug().noquote() << interDesc;
            for (int i = 0; i < config->bNumInterfaces; i++) {
                printInterfaceInfo(&config->interface[i], configIndex, usbDevice);
            }
        }
        libusb_free_config_descriptor(config);
    }
}

void printDevDesc(libusb_device *device, libusb_device_handle* deviceHandle, const libusb_device_descriptor& deviceDescriptor, UsbDevice* usbDevice) {
    QString res = "USB Device[";
    if (deviceHandle != nullptr) {
        unsigned char tmpStr[256];
        memset(&tmpStr, '\0', 256);
        QString tmp;
        if (deviceDescriptor.iManufacturer) {
            if (libusb_get_string_descriptor_ascii(deviceHandle, deviceDescriptor.iManufacturer, tmpStr, 256)) {
                tmp = getStringFromCArray(tmpStr, 256) + " ";
                res += tmp;
                memset(&tmpStr, '\0', 256);
            }
        }
        if (deviceDescriptor.iProduct) {
            if (libusb_get_string_descriptor_ascii(deviceHandle, deviceDescriptor.iProduct, tmpStr, 256)) {
                tmp = getStringFromCArray(tmpStr, 256) + " ";
                res += tmp;
                memset(&tmpStr, '\0', 256);
            }
        }
        if (deviceDescriptor.iSerialNumber) {
            if (libusb_get_string_descriptor_ascii(deviceHandle, deviceDescriptor.iSerialNumber, tmpStr, 256)) {
                tmp = getStringFromCArray(tmpStr, 256) + " ";
                res += tmp;
                memset(&tmpStr, '\0', 256);
            }
        }
        res += "]:";
        qDebug().noquote() << res;
        printDevSpeed(libusb_get_device_speed(device));
        QList<QString> devDesc = {QString("USB Protocol Version(%3): %1.%2").arg(int(deviceDescriptor.bcdUSB & 0xFF00) >> 8).arg(int(deviceDescriptor.bcdUSB & 0xFF)).arg(deviceDescriptor.bcdUSB),
                                    QString("PID:%1").arg((int)deviceDescriptor.idProduct),  QString("VID:%1").arg((int)deviceDescriptor.idVendor),
                                    QString("Available Config: %1").arg((int)deviceDescriptor.bNumConfigurations)};
        auto prefix =  getPrefix(UsbInfo::DEVICE_DESC);
        for(const auto& str : devDesc) {
            qDebug().noquote() << prefix << str;
        }
        printCfgInfo(device, deviceDescriptor.bNumConfigurations, usbDevice);
}}

void resolveUsbInfo(UsbDevice* usbDevice, uint16_t& pid, uint16_t& vid, uint16_t& cid) {
    auto deviceHandle = usbDevice->getDeviceHandle();
    auto device = libusb_get_device(deviceHandle);
    libusb_device_descriptor deviceDescriptor;
    Q_ASSERT(device != nullptr && deviceHandle != nullptr);
    auto ret = libusb_get_device_descriptor(device, &deviceDescriptor);
    if (ret < 0) {
        qWarning() << "Resolve Usb Information Failed!!!";
    } else {
        pid = deviceDescriptor.idProduct;
        vid = deviceDescriptor.idVendor;
        cid = deviceDescriptor.bDeviceClass;
        printDevDesc(device, deviceHandle, deviceDescriptor, usbDevice);
    }
}
}