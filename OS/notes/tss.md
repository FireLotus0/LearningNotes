- TSS全称为任务状态段，是指在操作系统进程管理的过程中，进程切换时的任务现场信息。  
X86体系从硬件上支持任务间的切换。为此目的，它增设了一个新段:任务状态段(TSS)，它和数据段、代码段样也是一种段，记录了任务的状态信息。
与其它段一样，TSS也有描述它的结构:TSS描述符表，它记录了一个TSS的信息，同时还有一个TR寄存器它指向当前任务的TSS。任务切换的时候，CPU会将原寄存器的内容写出到相应的TSS，同时将新TSS的内容填到寄存器中，这样就实现了任务的切换，  
- TSS在任务切换过程中起着重要作用，通过它实现任务的挂起和恢复。所谓任务切换是指挂起当前正在执行的任务，恢复或启动执行另一个任务。Linux任务切换是通过switch to这个宏来实现的，它利用长跳指令，当长跳指令的操作数是TSS描述符的时候，就会引起CPU的任务的切换，此时，CPU将所有寄存器的状态保存到当前任务寄存器TR所指向的TSS段中，然后利用长跳指令的操作数(TSS描述符)找到新任务的TSS段，并将其中的内容填写到各个寄存器中，最后，将新任务的TSS选择符更新到TR中。这样系统就开始运行新切换的任务了。由此可见，通过在TSS中保存任务现场各寄存器状态的完整映象，实现了任务的切换。task struct中的tss成员就是记录TSS段内容的。当进程被切换前，该进程用tss struct保存处理器的所有寄存器的当前值。当进程重新执行时，CPU利用tss恢复寄存器状态  
  

- tss的作用举例：保存不同特权级别下任务所使用的寄存器，特别重要的是esp，因为比如中断后，涉及特权级切换时(一个任务切换)，首先要切换栈，这个栈显然是内核栈，那么如何找到该栈的地址呢，这需要从tss段中得到，这样后续的执行才有所依托(在x86机器上，c语言的函数调用是通过栈实现的)。只要涉及地特权环到高特权环的任务切换，都需要找到高特权环对应的栈，因此需要esp2，esp1，esp0起码三个esp，然而linux只使用esp0。  
- tss是什么：tss是一个段，段是x86的概念，在保护模式下，段选择符参与寻址，段选择符在段寄存
器中，而tss段则在tr寄存器中。  
- intel的建议：为每一个进程准备一个独立的tss段，进程切换的时候切换tr寄存器使之指向该进程对应
的tss段，然后在任务切换时(比如涉及特权级切换的中断)使用该段保留所有的寄存器。  
- linux的做法：
    1. linux没有为每一个进程都准备一个tss段，而是每一个cpu使用一个tss段，tr寄存器保存该段。进程
切换时，只更新唯一tss段中的esp0字段到新进程的内核栈。  
    2. linux的tss段中只使用esp0和iomap等字段，不用它来保存寄存器，在一个用户进程被中断进入
ring0的时候，tss中取出esp0，然后切到esp0，其它的寄存器则保存在esp0指示的内核栈上而不保存在tss中。  
    3. 结果，linux中每一个cpu只有一个tss段，tr寄存器永远指向它。符合x86处理器的使用规范，但不遵循intel的建议，这样的后果是开销更小了，因为不必切换tr寄存器了。  
```cpp
linux的实现：
1.定义tss：
struct tss_struct init_tss[NR_CPUS] __cacheline_aligned = { [0 ... NR_CPUS-1] = INIT_TSS };
(arch/i386/kernel/init_task.c)
INIT_TSS定义为：
#define INIT_TSS { /
 .esp0 = sizeof(init_stack) + (long)&init_stack, /
 .ss0 = __KERNEL_DS, /
 .esp1 = sizeof(init_tss[0]) + (long)&init_tss[0], /
 .ss1 = __KERNEL_CS, /
 .ldt = GDT_ENTRY_LDT, /
 .io_bitmap_base = INVALID_IO_BITMAP_OFFSET, /
 .io_bitmap = { [ 0 ... IO_BITMAP_LONGS] = ~0 }, /
}
2.初始化tss：
struct tss_struct * t = init_tss + cpu;
...
load_esp0(t, thread);
set_tss_desc(cpu,t);
cpu_gdt_table[cpu][GDT_ENTRY_TSS].b &= 0xfffffdff;
load_TR_desc();
相关函数或者宏为：
#define load_TR_desc() __asm__ __volatile__("ltr %%ax"::"a" (GDT_ENTRY_TSS*8))
static inline void __set_tss_desc(unsigned int cpu, unsigned int entry, void *addr)
{
 _set_tssldt_desc(&cpu_gdt_table[cpu][entry], (int)addr,
 offsetof(struct tss_struct, __cacheline_filler) - 1, 0x89);
}
#define set_tss_desc(cpu,addr) __set_tss_desc(cpu, GDT_ENTRY_TSS, addr)
经过上述的初始化，tr永远指向唯一的tss段，然而tss段中的esp0以及iomap却是不断随着进程切换
而变化的。
3.进程切换时切换全局唯一tss段中的esp0以及iomap即可：
在__switch_to中：
struct tss_struct *tss = init_tss + cpu;
...

load_esp0(tss, next);
```
此时如果进程在用户态被中断，机器切到ring0，从tr中取出唯一的tss段，找到它的esp0，将堆栈切
过去即可，然后把所有的其它寄存器都保存在tss当前的esp0指示的内核也就是ring0的堆栈上。 从
而改变了tss的esp0。