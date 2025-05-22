## USB问题记录

- 查看usb被哪个进程占用：  
![](find_proc.png)  

- 查看usb设备所加载的驱动：  
![Alt text](find_driver.png)

- 创建udev规则，禁止内核驱动绑定设备
```cpp
SUBSYSTEM=="usb", ATTRS{idVendor}=="0a81", ATTRS{idProduct}=="0701", MODE="0660", GROUP="plugdev", RUN="/bin/sh -c 'echo -n $id:1.0 > /sys/bus/usb/drivers/usbhid/unbind'"
// udevadm control --relload-rules
// udevadm trigger

```
