#pragma once

#include "libusb.h"
#include "usbdevice.h"
#include <qstring.h>
/* 获取端点描述符
 * note: 每一个端点都有一个端点描述符，端点描述符会提供端点的方向、传输类型和数据包的最大尺寸 等信息
 * 一个具体的端点只能属于四种传输模式中的一种，通常把工作在什么模式下的端点就叫做什么端点，比如控制端点，批量端点
*  BYTE bLength;            // 设备描述符的字节数大小，为0x7
   BYTE bDescriptorType;    // 描述符类型编号，为0x05
   BYTE bEndpointAddress;   // 端点地址及输入输出属性:[0:3]:端点号，低速设备(Low-speed devices)最多拥有3个端点号(0–2)，其他设备则最多拥有16个端点号(0–15)
                                - [4:6]:预留  [7]:端点方向 0=OUT 1=IN
   BYTE bmAttribute;        // 端点的传输类型属性:[0:1]:传输类型 00=控制 01=同步 10=批量 11=中断
                                - 如果是同步端点：
                                    - [2:3]:同步类型 00=无同步  01=异步  10==自适应  11=同步
                                    - [4:5]:用途类型 00=数据端点  01=反馈端点  10=隐式反馈端点  11=保留端点
    ·                           - 如果是增强型超高速中断端点(Enhanced SuperSpeed interrupt endpoint)， Bits 5…2表示使用类型(Usage Type)
                                    - [2:3]:保留
                                    - [4:5]:00=周期  01=通知  10=11=保留
                                - 如果是其他端点：[2:5]预留，设置为0
   WORD wMaxPacketSize;     // 每个端点描述符都规定了该端点所支持的最大数据包长 (wMaxPacketSize)，主机每次发送数据包，都不能超过端点的最大包长
                            - 在USB2.0
                                - [0:10]:最大数据包大小，范围0–1024
                                - [13:15]:保留，0
                                - 如果是高速(high-speed)等时(isochronous)和中断(interrupt)端点
                                    - [11:12]:每个微帧(microframe)所支持的额外事务数目 00=每个微帧1个事务 01=每个微帧2个事务 10=每个微帧3个事务 11=保留
                                - 如果是其他传输速度与传输类型端点
                                    - [11:12]:未使用
                            - 在USB1.x协议中
                                - [0:10]: 范围是0–1023
                                - [11:12]: 保留 0
                                - [13:15]: 保留 0
                            - 在USB3.x协议中，对于增强型超高速传输(Enhanced SuperSpeed)
                                - [0:10]
                                    - 对于控制端点(control endpoints)，这个字段必须设为512
                                    - 对于批量端点(bulk endpoint)，这个字段必须设为1024
                                    - 对于中断(interrupt)和等时端点(isochronous endpoints)，
                                        这个字段的值的大小决定于超高速端点伙伴描述符(SuperSpeed Endpoint Companion Descriptor)中bMaxBurst 的值,
                                        如果bMaxBurst 大于0，那么这个字段的值为1024。如果bMaxBurst 等于0，那么对于等时端点 (isochronous endpoint)，
                                        取值范围是0-1024，对于中断端点(interrupt endpoint)，取值范围是1-1024
   BYTE bInterval;          // 中断端点主机查询端点的时间间隔
*/

/* 接口描述符
 * note:每个接口都有一个接口描述符，接口描述符会记录端点数量等信息，接口描述符中也包含USB类别的信息
*  BYTE bLength;            // 设备描述符的字节数大小，为0x09
   BYTE bDescriptorType;    // 描述符类型编号，为0x04
   BYTE bInterfaceNumber;   // 表示该接口的编号，如果一个配置具有多个接口，每个接口的编号都不相同，从0开始依次递增对一个配置的接口进行编号
   BYTE bAlternateSetting;  // 接口的备用编号，很少用到，设置为0
   BYTE bNumEndpoints;      // 表示该接口的端点数（不包括0端点）。
   BYTE bInterfaceClass;    // 接口类型 由USB协会规定。
   BYTE bInterfaceSubClass; // 接口子类型 由USB协会规定。
   BYTE bInterfaceProtocol; // 接口所遵循的协议 由USB协会规定。
   BYTE iInterface;         // 描述该接口的字符串索引值 如果为0，表示没有字符串。
*/

/* 获取配置描述符
 * note: 每种配置都有一个配置描述符，该描述符会提供特定设备配置的信息，如接口数量、 设备由总线供电还是自供电、 设备能否启动一个远程唤醒以及设备功耗
*   BYTE bLength;               // 配置描述符的字节数大小，固定为9字节
    BYTE bDescriptorType;       // 描述符类型编号，为0x02
    WORD wTotalLength;          // 整个配置描述符集合的总长度。包括配置描述符，接口描述符，端点描述符和类特殊描述符（如果有）
    BYTE bNumInterface;         // 定义了在该指定配置中接口总数。最小为 1 个接口，通常功能单一的设备只有一个接口（如鼠标），而复合设备具有多个接口（如音频设备）
    BYTE bConfigurationValue;   // 表示该配置的值，设置配置请求时会发送一个配置值，如果某个配置的bConfigurationValue和它相匹配，就表示该配置被激活
    BYTE iConfiguration;        // 描述该配置的字符串的索引，如果该值为0，表示没有字符串来描述它。
    BYTE bmAttribute;           // 定义了 USB 设备的一些特性。 位7保留，必须设置为1，位6表示供电方式，设置为 0表示设备由总线供电，
                                // 设置为 1 表示设备自供电。位5表示是否支持远程唤醒，设置为 1表示支持远程唤醒，设置为 0 表示不支持远程唤醒。位4~位0设置为0
    BYTE MaxPower;              // ，表示设备需要从总线获取的最大电流量，以 2 mA 为单位。如需要200mA的最大电流，则该字段的值为100
*/

/* 设备描述符
*  uint8_t  bDeviceClass;         // 设备类别。由USB协会规定，对于大多数标准的USB设备，通常设置为0，如果bDeviceClass为0xFF，表示是厂商自定义的设备类
   uint8_t  bDeviceSubClass;      // 设备子类别，由USB协会规定，当bDeviceClass为0时，bDeviceSubClass也必须为0
   uint16_t idVendor;             // 供应商 ID，由USB协会分配
   uint16_t idProduct;            // 产品 ID，由厂家自己决定
   uint8_t bDeviceProtocol;       // 设备所使用的协议，协议代码由USB协会规定。bDeviceProtocol必须要结合设备类和设备子类联合使用才有意义，因此当类代码为0时，bDeviceProtocol也必须为0。
   uint16_t bcdDevice;            // usb版本号
   uint8_t  iManufacturer;        // 描述制造商的字符串索引,如果存在字符串描述符，这些变量应该指向其索引位置。如果没有任何字符串，那么应该将零值填充到各个字段内
   uint8_t  iProduct;             // 描述产品的字符串索引
   uint8_t  iSerialNumber;        // 描述序列号的字符串索引
   uint8_t  bNumConfigurations;   // 可能的配置数量
*/

namespace UsbInfo {
    enum InfoLevel {
        DEVICE_DESC = 0,
        CONFIG_DESC,
        CONFIG_CONTENT,
        INTERFACE_DESC,
        INTERFACE_CONTENT,
        POINT_DESC,
        POINT_CONTENT,
    };
    QString getPrefix(InfoLevel infoLevel);

    QString getStringFromCArray(unsigned char *arr, size_t length);

    QString getTransferType(int index);

    void printDevSpeed(int speed);

    void printCfgInfo(libusb_device* device, int count, UsbDevice* usbDevice);

    void printInterfaceInfo(const libusb_interface_descriptor *interfaceDescriptor, int cfgIndex,  UsbDevice* usbDevice);

    void printInterfaceInfo(const libusb_interface *interfaceDescriptor, int cfgIndex, UsbDevice* usbDevice);

    void printPointInfo(const libusb_endpoint_descriptor *endpointDescriptor, UsbDevice* usbDevice, bool configMatched);

    void printDevDesc(libusb_device *device, libusb_device_handle* deviceHandle, const libusb_device_descriptor& deviceDescriptor, UsbDevice* usbDevice);

    void resolveUsbInfo(UsbDevice* usbDevice, uint16_t & pid, uint16_t& vid, uint16_t &cid);
}