#include "std.h"
#include "global_var.h"

int flag_con = 0;

// int DLCS()
// {
//     int max = 0;
//     int decision = 0;
//     for (int i = 1; i <= literal_count; ++i)
//     {
//         if (as_info[i].assign_type == FREE)
//         {
//             int x = 0, y = 0, r = 0;
//             for (int j = 0; j < LitsInfo[i][1].num_count; ++j)
//             {

//                 int curlit = LitsInfo[i][1].cl_list[j];
//                 x += 1 - ClInfo[curlit].is_sat;
//             }
//             for (int j = 0; j < LitsInfo[i][0].num_count; ++j)
//             {
//                 int curlit = LitsInfo[i][0].cl_list[j];
//                 y += 1 - ClInfo[curlit].is_sat;
//             }
//             r = x + y;
//             if (r > max)
//             {
//                 max = r;
//                 if (x >= y)
//                 {
//                     decision = i;
//                 }
//                 else
//                 {
//                     decision = -i;
//                 }
//             }
//         }
//     }
//     return decision;
// }

int get_minlen()
{
    int minlen = maxlen;
    if (minlen == 2)
        return 2;
    for (int i = 1; i <= literal_count; ++i)
    {
        if (as_info[i].assign_type == FREE)
        {
            for (int j = 0; j < 2; ++j)
            {
                for (int t = 0; t < LitsInfo[i][j].num_count; ++t)
                {
                    int curcl = LitsInfo[i][j].cl_list[t];
                    if (!ClInfo[curcl].is_sat && ClInfo[curcl].cur_len < minlen)
                    {
                        minlen = ClInfo[curcl].cur_len;
                        if (minlen == 2)
                            return 2;
                    }
                }
            }
        }
    }
    return minlen;
}

void cal_ds(int l, int s, int *x, int *y)
{
    *x = *y = 0;
    for (int j = 0; j < LitsInfo[l][1].num_count; ++j)
    {
        int curcl = LitsInfo[l][1].cl_list[j];
        if (ClInfo[curcl].cur_len == s)
        {
            *x += 1 - ClInfo[curcl].is_sat;
        }
    }
    for (int j = 0; j < LitsInfo[l][0].num_count; ++j)
    {
        int curcl = LitsInfo[l][0].cl_list[j];
        if (ClInfo[curcl].cur_len == s)
        {
            *y += 1 - ClInfo[curcl].is_sat;
        }
    }
}

int MOM()
{
    int max = 0, x = 0, y = 0;
    int rlit = 0;
    for (int i = 1; i <= literal_count; ++i)
    {
        if (as_info[i].assign_type == FREE)
        {
            int s = get_minlen();
            cal_ds(i, s, &x, &y);
            int inter = (x + 1) * (y + 1);
            if (inter > max)
            {
                if (x >= y)
                {
                    rlit = i;
                }
                else
                {
                    rlit = -i;
                }
                max = inter;
            }
        }
    }
    return rlit;
}

void setvar(int var)
{
    int ind = abs(var);
    int vl = var > 0 ? 1 : 0;

    int *cor_cl_list_1 = (LitsInfo[ind][vl].cl_list);
    for (int i = 0; i < LitsInfo[ind][vl].num_count; i++)
    {
        int Clindex = cor_cl_list_1[i];
        if (ClInfo[Clindex].is_sat == 1)
            continue;
        ClInfo[Clindex].is_sat = 1;
        clchanges = (cchange_info *)realloc(clchanges, sizeof(cchange_info) * (ch_ind + 1));
        clchanges[ch_ind++].cl_ind = Clindex;
        r_clauses--;
        dep_change[depth][1]++;
    }

    vl = !vl; //对该变元的反面进行操作

    int *cor_cl_list_0 = LitsInfo[ind][vl].cl_list;
    for (int j = 0; j < LitsInfo[ind][vl].num_count; j++)
    {
        int Clindex = cor_cl_list_0[j];
        if (ClInfo[Clindex].is_sat == 1)
            continue;
        clchanges = (cchange_info *)realloc(clchanges, sizeof(cchange_info) * (ch_ind + 1));
        clchanges[ch_ind].cl_ind = Clindex;
        clchanges[ch_ind++].lit_locincl = LitsInfo[ind][vl].cl_locs[Clindex];
        if (ClInfo[Clindex].or_len <= 32)
        {
            ClInfo[Clindex].bin_num -= 1 << LitsInfo[ind][vl].cl_locs[Clindex];
        }

        ClInfo[Clindex].cur_len--;
        dep_change[depth][0]++;
        if (ClInfo[Clindex].cur_len == 1)
        {
            int unit_lit = 0;
            if (ClInfo[Clindex].or_len <= 32)
            {
                int unit_loc = (int)log2(ClInfo[Clindex].bin_num); //计算单子句中的单变元下标
                unit_lit = ClInfo[Clindex].lit_list[unit_loc];     //取出单子句变元
            }
            else
            {
                for (int k = 0; k < ClInfo[Clindex].or_len; ++j)
                {
                    int lit = ClInfo[Clindex].lit_list[k];
                    if (as_info[abs(lit)].assign_type == FREE)
                    {
                        unit_lit = lit;
                        break;
                    }
                }
            }
            if (LitsInfo[abs(unit_lit)][unit_lit > 0 ? 0 : 1].is_unit == 1)
            {
                flag_con = 1;
            }
            else if (LitsInfo[abs(unit_lit)][unit_lit > 0 ? 1 : 0].is_unit == 0)
            {
                LitsInfo[abs(unit_lit)][unit_lit > 0 ? 1 : 0].is_unit = 1;
                ClInfo[Clindex].unit_lit = unit_lit;
                ucl_st[n_ucl++] = unit_lit;
            }
        }
    }
    depth++;
}

void resetvar(int var) //撤销对var的赋值及操作
{
    depth--; //返回上一层
    int ind = abs(var);
    //撤销赋值为假的操作

    while (dep_change[depth][0] > 0)
    {
        ch_ind--;
        int Clindex = clchanges[ch_ind].cl_ind;
        int lit_loc = clchanges[ch_ind].lit_locincl;

        if (ClInfo[Clindex].cur_len == 1)
        {
            int unit_lit = ClInfo[Clindex].unit_lit;
            ClInfo[Clindex].unit_lit = 0;
            LitsInfo[abs(unit_lit)][unit_lit > 0 ? 1 : 0].is_unit = 0;
        }
        ClInfo[Clindex].cur_len++;
        if (ClInfo[Clindex].or_len <= 32)
        {
            ClInfo[Clindex].bin_num += 1 << lit_loc;
        }
        dep_change[depth][0]--;
    }

    //撤销赋值为真的操作
    while (dep_change[depth][1] > 0)
    {
        ch_ind--;
        int Clindex = clchanges[ch_ind].cl_ind;
        r_clauses++;
        ClInfo[Clindex].is_sat = 0;
        dep_change[depth][1]--;
    }
}

bool dpll()
{

    int level_n_ucl = 0; //用来记录bcp中赋值的变元
    int *level_ucl_st = NULL;

    int level_n_pl = 0; //用来记录纯文字
    int *level_pl_st = NULL;

    while (true)
    {
        if (flag_con == 1) //如果发生冲突，则开始回溯
        {
            while (level_n_ucl)
            {
                level_n_ucl--;
                int re_ucl = level_ucl_st[level_n_ucl];
                resetvar(re_ucl);
                // con_cls[con_ind++] = LitsInfo[abs(re_ucl)][re_ucl > 0 ? 1 : 0].ante_cl;
                as_info[abs(re_ucl)].assign_type = FREE;
                as_info[abs(re_ucl)].assign_val = -1;
            }
            n_ucl = 0;
            flag_con = 0;
            free(level_ucl_st);
            return false;
        }
        else if (n_ucl > 0)
        {
            int unit_var = ucl_st[--n_ucl];
            level_ucl_st = (int *)realloc(level_ucl_st, sizeof(int) * (level_n_ucl + 1));
            level_ucl_st[level_n_ucl] = unit_var;
            level_n_ucl++;
            as_info[abs(unit_var)].assign_type = IMPLIED;
            as_info[abs(unit_var)].assign_val = unit_var > 0 ? 1 : 0;
            setvar(unit_var);
        }
        else
            break;
    }

    for (int i = 1; i <= literal_count; ++i) //赋值纯文字
    {
        int pl = 0;
        int t_c = 0;
        int f_c = 0;
        if (as_info[i].assign_type == FREE)
        {
            for (int j = 0; j < LitsInfo[abs(i)][1].num_count; ++j)
            {
                int Clindex = LitsInfo[abs(i)][1].cl_list[j];
                t_c += 1 - ClInfo[Clindex].is_sat;
            }
            for (int j = 0; j < LitsInfo[abs(i)][0].num_count; ++j)
            {
                int Clindex = LitsInfo[abs(i)][0].cl_list[j];
                f_c += 1 - ClInfo[Clindex].is_sat;
            }
            if (t_c && (f_c == 0))
                pl = i;
            if ((t_c == 0) && f_c)
                pl = -i;
            if (pl != 0)
            {
                level_pl_st = (int *)realloc(level_pl_st, sizeof(int) * (level_n_pl + 1));
                level_pl_st[level_n_pl] = pl;
                level_n_pl++;
                as_info[abs(pl)].assign_type = IMPLIED;
                as_info[abs(pl)].assign_val = pl > 0 ? 1 : 0;
                setvar(pl);
            }
        }
    }

    if (r_clauses == 0) //如果子句全部满足，则返回true
        return true;

    int chosen_var = MOM(); //选择变元

    as_info[abs(chosen_var)].assign_type = CHOSEN;
    as_info[abs(chosen_var)].assign_val = chosen_var > 0 ? 1 : 0;
    setvar(chosen_var);
    if (dpll())
        return true;
    resetvar(chosen_var);

    as_info[abs(chosen_var)].assign_type = IMPLIED;
    as_info[abs(chosen_var)].assign_val = -chosen_var > 0 ? 1 : 0;
    setvar(-chosen_var);
    if (dpll())
        return true;
    resetvar(-chosen_var);
    as_info[abs(chosen_var)].assign_type = FREE;
    as_info[abs(chosen_var)].assign_val = -1;

    while (level_n_pl) //对本层纯文字赋值变元撤销操作
    {
        level_n_pl--;
        int re_pl = level_pl_st[level_n_pl];
        resetvar(re_pl);
        as_info[abs(re_pl)].assign_type = FREE;
        as_info[abs(re_pl)].assign_val = -1;
    }
    free(level_pl_st);

    while (level_n_ucl) //对本层剩下的bcp赋值变元撤销操作
    {
        level_n_ucl--;
        int re_ucl = level_ucl_st[level_n_ucl];
        resetvar(re_ucl);
        as_info[abs(re_ucl)].assign_type = FREE;
        as_info[abs(re_ucl)].assign_val = -1;
    }

    n_ucl = 0;
    flag_con = 0;
    free(level_ucl_st);
    return false;
}
