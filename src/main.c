# include<stdio.h>
# include"cpu/register.h"
# include"cpu/mmu.h"
# include"disk/elf.h"
# include"memory/dram.h"
# include"memory/instruction.h"


int main() {
    init_handler_table();

    reg.rax = 0x12340000;
    reg.rbx = 0x0;
    reg.rcx = 0x400500;
    reg.rdx = 0xabcd;
    reg.rsi = 0x7fffffffe2c8;
    reg.rdi = 0x1;
    reg.rbp = 0x7fffffffe1e0;
    reg.rsp = 0x7fffffffe1c0;

    reg.rip = (uint64_t)&(program[11]);


    write64bits_dram(va2pa(0x7fffffffe1e0), 0x00000000);  // rbp
    write64bits_dram(va2pa(0x7fffffffe1d8), 0x12340000);
    write64bits_dram(va2pa(0x7fffffffe1d0), 0x0000abcd);
    write64bits_dram(va2pa(0x7fffffffe1c8), 0x00400400);
    write64bits_dram(va2pa(0x7fffffffe1c0), 0x00400500);  // rsp
    print_register();
    print_stack();

    // run inst
    for (int i = 0; i < 15; ++i) {
        instruction_cycle();
        print_register();
        print_stack();
    }

    // varify：验证我们的状态机是否实现的正确
    // 若正确，则寄存器和对应内存中的值（状态）应该和 gdb 中显示的是一样的
    int match = 1;
    match &= (reg.rax == 0x1234abcd);
    match &= (reg.rbx == 0x0);
    match &= (reg.rcx == 0x400500);
    match &= (reg.rdx == 0x12340000);
    match &= (reg.rsi == 0xabcd);
    match &= (reg.rdi == 0x12340000);
    match &= (reg.rbp == 0x7fffffffe1e0);
    match &= (reg.rsp == 0x7fffffffe1c0);

    if (match == 1) {
        printf("register match\n");
    }
    else {
        printf("register not match\n");
    }

    match &= (read64bits_dram(va2pa(0x7fffffffe1e0)) == 0x00000000);  // rbp
    match &= (read64bits_dram(va2pa(0x7fffffffe1d8)) == 0x12340000);
    match &= (read64bits_dram(va2pa(0x7fffffffe1d0)) == 0x0000abcd);
    match &= (read64bits_dram(va2pa(0x7fffffffe1c8)) == 0x1234abcd);
    match &= (read64bits_dram(va2pa(0x7fffffffe1c0)) == 0x00400500);  // rsp 

    if (match == 1) {
        printf("memory match\n");
    }
    else {
        printf("memory not match\n");
    }

    return 0;
}