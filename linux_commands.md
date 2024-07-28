```cpp
//查看软中断次数
cat /proc/softirqs

//查看硬中断次数
cat /proc/interrupts

//查看软中断变化率
watch -d cat /proc/softirqs

//软中断是在内核线程上运行的，查看内核线程，通常内核线程的名字由中括号括起来，并且看不到参数
ps -aux | grep softirq



```