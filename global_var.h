#ifndef GLOBAL_VAR_H
#define GLOBAL_VAR_H

#define CHOSEN 10 //表示该变元是由决策出来的
#define IMPLIED -10  //表示该变元是由于BCP而赋值的
#define FREE 11  //用于初始化


#define LEGAL_DIG 20
#define ILLEGAL_DIG -20
#define FREE_DIG 15

typedef struct Literal_Info  //变元信息
{
    int num_count;  //计数器
    int is_unit;
    int *cl_list; //对应的子句下标储存的数组
    int *cl_locs; //处在对应子句的第几个
} Literal_Info;


typedef struct Clause_Info  //子句信息
{
    int *lit_list;  //变元数组
    int or_len;    //记录子句的原始变元数
    int cur_len;   //记录子句的现在变元数，每遇到一个false的变元减一
    int bin_num;  //用二进制记录子句的复制情况，用于计算单子句变元的位置
    int is_sat;      //2表示尚未决定状态，1表示已经满足，0表示不满足
    int unit_lit; //子句当前的单变元，若该子句不是单子句，则为0
}Clause_Info;



typedef struct lit_assign_info
{
    int assign_type;
    int assign_val;
    int dig_stat;
}lassign_info;   //记录当前赋值（即发生变化）的变元信息


typedef struct cl_change_info
{
    int cl_ind;  //当句子非false时，记录
    int lit_locincl;  //只有当变元为假时才记录，记录该变元在cl_ind对应子句中的位置
}cchange_info;


extern Clause_Info* ClInfo;
extern Literal_Info** LitsInfo;
extern lassign_info *as_info;
extern cchange_info* clchanges;
extern int* ucl_st;
extern int **dep_change;
extern int ch_ind;
extern int literal_count;
extern int clause_count;
extern char buff[128];
extern int n_clauses;
extern int r_clauses;
extern int n_ucl;
extern int depth;
extern int n;
extern int **sudoku;
extern char address[200];
extern char filename[100];
extern char opfile[200];
extern FILE* opfp;
extern bool dpll();
extern void cnfparser(char *addr);


#endif

