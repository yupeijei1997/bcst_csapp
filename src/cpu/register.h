# ifndef reg_guard
# define reg_guard

# include<stdio.h>
# include<stdint.h>


/* 寄存器
其值表示立即数 / 虚拟内存地址
*/
typedef struct REG_STRUCT {
    union {
        struct {
            uint8_t al;
            uint8_t ah;
        };
        uint16_t ax;
        uint32_t eax;
        uint64_t rax;
    };
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rbp;
    uint64_t rsp;

    uint64_t rip;  // 程序计数器PC，指向当前运行指令的虚拟地址
} reg_t;


reg_t reg;

# endif
