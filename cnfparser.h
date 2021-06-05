#include "std.h"
#include "global_var.h"

char buff[128];
int literal_count;
int clause_count;


Clause_Info *ClInfo = NULL;     //用来存放子句信息的数组
Literal_Info **LitsInfo = NULL; //用来存放变元信息的数组
lassign_info *as_info = NULL;
cchange_info *clchanges = NULL; //当子句性质发生改变时，储存改变的信息
int ch_ind = 0;          //cchanges的下标，越后发生的改变，ch_ind越大
int *ucl_st;             //储存单子句变元
int n_clauses = 0;       //原始句子总数
int r_clauses = 0;       //现存句子总数
int n_ucl = 0;           //单子句个数
int depth = 1;           //当前分支递归深度,一旦对某一个变元进行set，则深度加1
int **dep_change = NULL; //记录与深度相关的改变数
int *con_cls = NULL;
int maxlen = 0;
FILE *adfp = NULL;


void scanfile(char *addr)
{
    adfp = fopen(addr, "r"); //打开文件
    char lit;
    while (true)
    {
        fscanf(adfp,"%c", &lit);
        // 若是p则记录变元数及子句数
        if (lit == 'p')
        {
            fgets(buff, 5, adfp);
            fscanf(adfp,"%d", &literal_count);
            fscanf(adfp,"%d", &clause_count);
            break;
        }
        else
            fgets(buff, 128, adfp);
    }
}

void init_C_L_info(void)
{
    ClInfo = (Clause_Info *)malloc(sizeof(Clause_Info) * (clause_count + 1));
    LitsInfo = (Literal_Info **)malloc(sizeof(Literal_Info *) * (literal_count + 1));
    as_info = (lassign_info *)malloc(sizeof(lassign_info) * (literal_count + 1));
    ucl_st = (int *)malloc(sizeof(int) * (literal_count));
    dep_change = (int **)malloc(sizeof(int *) * (literal_count + 1));
    con_cls = (int *)malloc(sizeof(int) * (clause_count + 1));
    n_clauses = r_clauses = clause_count;

    for (int k = 0; k <= clause_count; ++k)
    {
        ClInfo[k].lit_list = (int *)malloc(sizeof(int) * 60);
        ClInfo[k].or_len = ClInfo[k].cur_len = 0;
        ClInfo[k].is_sat = 0;
        ClInfo[k].unit_lit = 0;
        ClInfo[k].bin_num = 0;
    }

    for (int i = 0; i <= literal_count + 1; ++i)
    {
        as_info[i].assign_type = FREE;
        as_info[i].assign_val = -1;
        dep_change[i] = (int *)malloc(sizeof(int) * 2);
        LitsInfo[i] = (Literal_Info *)malloc(sizeof(Literal_Info) * 2);
        for (int j = 0; j < 2; ++j)
        {
            dep_change[i][j] = 0;
            LitsInfo[i][j].is_unit = 0;
            LitsInfo[i][j].num_count = 0;
            LitsInfo[i][j].cl_list = NULL;
            LitsInfo[i][j].cl_locs = (int *)malloc(sizeof(int) * clause_count + 1);
            for (int u = 0; u <= clause_count; u++)
            {
                LitsInfo[i][j].cl_locs[u] = -1;
            }
        }
    }
}

void Create_clind_list(int var, int Cl_index, int loc)
{
    if (var > 0) //构建子句下标链表
    {
        int cur = LitsInfo[var][1].num_count;
        LitsInfo[var][1].cl_list = (int *)realloc(LitsInfo[var][1].cl_list, sizeof(int) * (LitsInfo[var][1].num_count + 1));
        LitsInfo[var][1].cl_list[cur] = Cl_index;
        LitsInfo[var][1].num_count++;
        LitsInfo[var][1].cl_locs[Cl_index] = loc;
    }
    else
    {
        int cur = LitsInfo[-var][0].num_count;
        LitsInfo[-var][0].cl_list = (int *)realloc(LitsInfo[-var][0].cl_list, sizeof(int) * (LitsInfo[-var][0].num_count + 1));
        LitsInfo[-var][0].cl_list[cur] = Cl_index;
        LitsInfo[-var][0].num_count++;
        LitsInfo[-var][0].cl_locs[Cl_index] = loc;
    }
}

bool check(int var, int clindex, int loc)
{
    for (int i = 0; i < loc; ++i)
    {
        if (ClInfo[clindex].lit_list[i] == var)
        {
            return true;
        }
    }
    return false;
}
void CreateLitslist(int Cl_index) //一次读取一行
{
    int var = 0;
    int Cl_len = 0;
    for (; fscanf(adfp,"%d", &var) && var != 0; ++Cl_len)
    {
        if (check(var, Cl_index, Cl_len))
        {
            continue;
        }
        ClInfo[Cl_index].lit_list[Cl_len] = var;
        Create_clind_list(var, Cl_index, Cl_len);
    }
    ClInfo[Cl_index].lit_list[Cl_len] = 0;
    ClInfo[Cl_index].or_len = ClInfo[Cl_index].cur_len = Cl_len;
    if (ClInfo[Cl_index].cur_len <= 32)
    {
        ClInfo[Cl_index].bin_num = (((1 << (Cl_len - 1)) - 1) << 1) + 1;
    }
    if (Cl_len > maxlen)
        maxlen = Cl_len;

    if (Cl_len == 1)
    {
        int unit_var = ClInfo[Cl_index].lit_list[0];
        ClInfo[Cl_index].unit_lit = unit_var;
        LitsInfo[abs(unit_var)][unit_var > 0 ? 1 : 0].is_unit = 1;
        ucl_st[n_ucl++] = unit_var;
    }
}

void gen_C_L_list(void)
{
    for (int i = 1; i <= clause_count; ++i)
    {
        CreateLitslist(i);
    }
    for (int j = 1; j <= literal_count; ++j)
    {
        for (int k = 0; k < 2; ++k)
        {
            int cur = LitsInfo[j][k].num_count;
            LitsInfo[j][k].cl_list = (int *)realloc(LitsInfo[j][k].cl_list, sizeof(int) * (LitsInfo[j][k].num_count + 1));
            LitsInfo[j][k].cl_list[cur] = 0;
        }
    }
}

void cnfparser(char *addr)
{
    scanfile(addr);
    init_C_L_info();
    gen_C_L_list();
    fclose(adfp);
}