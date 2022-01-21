# ifndef inst_guard
# define inst_guard

# include<stdio.h>
# include<stdint.h>


# define NUM_INSTRTYPE 30

/*解析操作数：根据操作数类型获取操作数
Rarams:
    od: 操作数
Returns:
    立即数/寄存器中存储的整数/寄存器中存储的虚拟内存地址
*/

// 指令类型
typedef enum OP {
    mov_reg_reg,     // 0
    mov_reg_mem,     // 1
    mov_mem_reg,     // 2
    push_reg,        // 3
    pop_reg,         // 4
    call,            // 5
    ret,             // 6
    add_reg_reg      // 7
} op_t;

// 操作数类型
typedef enum OD_TYPE {     //                       操作数值       
    EMPTY,                 // 
    IMM,                   // 立即数寻址            Imm
    REG,                   // 寄存器寻址            R[ra]
    MM_IMM,                // 绝对寻址              M[Imm]
    MM_REG,                // 间接寻址              M[R[ra]]
    MM_IMM_REG,            // （基址 + 偏移量）寻址 M[Imm + R[rb]]
    MM_REG1_REG2,          // 变址寻址              M[R[rb] + R[ri]]
    MM_IMM_REG1_REG2,      // 变址寻址              M[Imm + R[rb] + R[ri]]
    MM_REG2_S,             // 比例变址寻址          M[R[ri] * s]
    MM_IMM_REG2_S,         // 比例变址寻址          M[Imm + R[ri] * s]
    MM_REG1_REG2_S,        // 比例变址寻址          M[R[rb] + R[ri] * s]
    MM_IMM_REG1_REG2_S     // 比例变址寻址          M[Imm + R[rb] + R[ri] * s]
} od_type_t;

// 操作数
typedef struct OD {
    od_type_t type;

    int64_t imm;     // 立即数
    int64_t scal;    // 比例因子
    uint64_t *reg1;  // 基址寄存器的地址
    uint64_t *reg2;  // 变址寄存器的地址
} od_t;

// 指令
typedef struct INSTRUCT_STRUCT {
    op_t op;         // 指令类型
    od_t src;        // 源操作数
    od_t dst;        // 目的操作数

    char code[100];  // 指令字符串
} inst_t;

typedef void (*handler_t)(uint64_t, uint64_t);

handler_t handler_table[NUM_INSTRTYPE];

void init_handler_table();

void instruction_cycle();

void mov_reg_reg_handler(uint64_t src, uint64_t dst);

void mov_reg_mem_handler(uint64_t src, uint64_t dst);

void push_reg_handler(uint64_t src, uint64_t dst);

void pop_reg_handler(uint64_t src, uint64_t dst);

void call_handler(uint64_t src, uint64_t dst);

void add_reg_reg_handler(uint64_t src, uint64_t dst);

# endif
