# include"cpu/mmu.h"
# include"cpu/register.h"
# include"memory/dram.h"
# include"memory/instruction.h"


static uint64_t decode_od(od_t od) {
    /*
    备注：该函数是一个静态函数，不需要在头文件中声明
    */
    if (od.type == IMM) {
        return *((uint64_t *)&od.imm);  // int64_t -> uint64_t
    }
    else if (od.type == REG) {
        /*
            例如 reg.rsp 寄存器，pmm[0xabcd] = 0x1，其物理地址为0xabcd，存储的值为0x1，
            这里0x1可能是要用的整数，也可能是虚拟内存地址。
            虽然根据寄存器寻址的概念来看，应该返回0x1，但为了后面操作这个 rsp 这个寄存器 
            我们应该返回其物理地址 0xabcd 才行，否则无法对其进行操作了
            例如 mov rax rbx，把 rax 中的值放到 rbx 中，若返回的是0x1(rax中的值) 0x2(rbx中的值)
            那么我们还需要先 &(0x2) 获得 rbx 的地址，再将 0x1 存储进该地址中
            这里，我认为也可以，但作者没这么实现
        */
        return (uint64_t)od.reg1;
    }
    else {
        uint64_t vaddr = 0;  // 虚拟内存地址
        
        if (od.type == MM_IMM) {
            vaddr = od.imm;
        }
        else if (od.type == MM_REG) {
            vaddr = *(od.reg1);
        }
        else if (od.type == MM_IMM_REG) {
            vaddr = od.imm + *(od.reg1);
        }
        else if (od.type == MM_REG1_REG2) {
            vaddr = *(od.reg1) + *(od.reg2);
        }
        else if (od.type == MM_IMM_REG1_REG2) {
            vaddr = od.imm + *(od.reg1) + *(od.reg2);
        }
        else if (od.type == MM_REG2_S) {
            vaddr = (*(od.reg2)) * od.scal;
        }
        else if (od.type == MM_IMM_REG2_S) {
            vaddr = od.imm + (*(od.reg2)) * od.scal;
        }
        else if (od.type == MM_REG1_REG2_S) {
            vaddr = *(od.reg1) + (*(od.reg2)) * od.scal;
        }
        else if (od.type == MM_IMM_REG1_REG2_S) {
            vaddr = od.imm + *(od.reg1) + (*(od.reg2)) * od.scal;
        }

        return va2pa(vaddr);
    }
}


void init_handler_table() {
    /* 指令执行函数注册
    */

    // mov_reg_reg_handler 需要在头文件中声明，否则编译器会找不到。
    // 原因：该.c文件引入头文件以后，声明会在该代码的前面，那么编译器就可以找到；若不声明，显然编译器找不到。
    // 或者你不在头文件中声明，就直接在该代码的前面实现 mov_reg_reg_handler() 也可以。
    handler_table[mov_reg_reg] = &mov_reg_reg_handler;
    handler_table[call] = &call_handler;
    handler_table[add_reg_reg] = &add_reg_reg_handler;
}

void instruction_cycle() {
    inst_t *instr = (inst_t *)reg.rip;

    uint64_t src = decode_od(instr->src);
    uint64_t dst = decode_od(instr->dst);

    handler_t handler = handler_table[instr->op];
    handler(src, dst);
    printf("    %s\n", instr->code);
}

void mov_reg_reg_handler(uint64_t src, uint64_t dst) {
    /* 把一个寄存器中的值复制到另一个寄存器中
    params:
        src: 寄存器地址
        dst: 寄存器地址
    */
    *(uint64_t *)dst = *(uint64_t *)src;
    reg.rip += sizeof(inst_t);
}

void call_handler(uint64_t src, uint64_t dst) {
    /* 调用函数，只是调用，还未执行
    params:
        src: 被调函数的起始地址，是一个立即数
        dst: NULL
    */

   // 栈向下扩张，用来存储返回地址
   reg.rsp -= 8;

   // 返回地址写入 rsp 指向的内存中
   write64bits_dram(
       va2pa(reg.rsp),
       reg.rip + sizeof(inst_t)
   );

   reg.rip = src;
}

void add_reg_reg_handler(uint64_t src, uint64_t dst) {
    /* 把源寄存器中的值加到目标寄存器中
    params:
        src: 寄存器地址
        dst: 寄存器地址
    */
    *(uint64_t *)dst += *(uint64_t *)src;
    reg.rip += sizeof(inst_t);
}
