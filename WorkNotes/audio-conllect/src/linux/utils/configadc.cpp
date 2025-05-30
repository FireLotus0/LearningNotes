
#include "configadc.h"

void Xil_Out32(unsigned long Addr, unsigned long Value)
{
    int fd;
    void *map_base, *virt_addr;

    if((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1) FATAL;
    //printf("Xil_Out32 /dev/mem opened.\n");

    /* Map one page */
    map_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, Addr & ~MAP_MASK);
    if(map_base == (void *) -1) FATAL;
    //printf("Xil_Out32 Memory mapped at address %p.\n", map_base);

    virt_addr = map_base + (Addr & MAP_MASK);
    //printf("Xil_Out32 virt_addr = %p\n", virt_addr);  // ��ӡ virt_addr ��ֵ
    *((unsigned int *) virt_addr) = Value;
    //printf("xil_out32_l: %x\n", (Value & 0xffff));

    if(munmap(map_base, MAP_SIZE) == -1) FATAL;
    close(fd);
}

unsigned long Xil_In32(unsigned long Addr)
{
    int fd;
    void *map_base, *virt_addr;
    unsigned long read_result;

    if((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1) FATAL;
    //printf("Xil_In32 /dev/mem opened.\n");

    /* Map one page */
    map_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, Addr & ~MAP_MASK);
    if(map_base == (void *) -1) FATAL;
    //printf("Xil_In32 Memory mapped at address %p.\n", map_base);

    virt_addr = map_base + (Addr & MAP_MASK);
    //printf("Xil_In32 virt_addr = %p\n", virt_addr);  // ��ӡ virt_addr ��ֵ
    read_result = *((unsigned int *) virt_addr);

    if(munmap(map_base, MAP_SIZE) == -1) FATAL;

    close(fd);

    return read_result;
}

// AD4630
unsigned int ad4630_reg_write(unsigned int data)
{
    unsigned int rev;

    Xil_Out32(XPAR_BRAM_0_BASEADDR + 0x4, data);
    rev = Xil_In32(XPAR_BRAM_0_BASEADDR + 0xC);
    while(rev != 0)
    {
        rev = Xil_In32(XPAR_BRAM_0_BASEADDR + 0xC);
    }

    return 1;
}

unsigned int ad4630_reg_read(unsigned int addr)
{
    unsigned int rev;

    Xil_Out32(XPAR_BRAM_0_BASEADDR + 0x8, addr);
    rev = Xil_In32(XPAR_BRAM_0_BASEADDR + 0xC);
    while(rev != 0)
    {
        rev = Xil_In32(XPAR_BRAM_0_BASEADDR + 0xC);
    }

    rev = Xil_In32(XPAR_BRAM_0_BASEADDR + 0x10);

    return rev;
}
