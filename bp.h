#include "std.h"
#include "global_var.h"

int n; //阶数
int **sudoku = NULL;
int **row = NULL;
int **col = NULL;
int unit_cl_count = 0;
int add_var = 0;
FILE *sdkfp = NULL;
int dig_count = 0; //记录挖洞以后生成的数独中已复制的个数
bool sudoku_ini()
{
    scanf("%d", &n);
    while (n != 8 && n != 6 && n != 4)
    {
        printf("\nInvalid order bound, please enter again:");
        scanf("%d", &n);
    }
    if (n == 8)
        unit_cl_count = 12;
    else if (n == 6)
        unit_cl_count = 8;
    else if (n == 4)
        unit_cl_count = 3;
    if (n % 2)
        return false;
    sudoku = (int **)malloc(sizeof(int *) * (n + 1));
    row = (int **)malloc(sizeof(int *) * (n + 1));
    col = (int **)malloc(sizeof(int *) * (n + 1));
    for (int i = 0; i <= n; ++i)
    {
        sudoku[i] = (int *)malloc(sizeof(int) * (n + 1));
        row[i] = (int *)malloc(sizeof(int) * 2);
        col[i] = (int *)malloc(sizeof(int) * 2);
        for (int j = 0; j <= n; ++j)
        {
            sudoku[i][j] = -1;
        }
        for (int k = 0; k < 2; ++k)
        {
            row[i][k] = 0;
            col[i][k] = 0;
        }
    }
    return true;
}

int comb(int a, int b)
{
    int cab = 1;
    for (int k = 1; k <= b; ++k)
    {
        cab = (cab * (a - b + k)) / k;
    }
    return cab;
}

void constrainI(FILE *fp)
{
    for (int i = 1; i <= n; ++i)
    {
        for (int j = 1; j <= n - 2; ++j)
        {
            int l1 = (i - 1) * n + j;
            int l2 = (i - 1) * n + j + 1;
            int l3 = (i - 1) * n + j + 2;
            fprintf(fp, "%d %d %d %d\n", l1, l2, l3, 0);
            fprintf(fp, "%d %d %d %d\n", -l1, -l2, -l3, 0);
        }
    }
    for (int j = 1; j <= n; ++j)
    {
        for (int i = 1; i <= n - 2; ++i)
        {
            int l1 = (i - 1) * n + j;
            int l2 = (i + 1 - 1) * n + j;
            int l3 = (i + 2 - 1) * n + j;
            fprintf(fp, "%d %d %d %d\n", l1, l2, l3, 0);
            fprintf(fp, "%d %d %d %d\n", -l1, -l2, -l3, 0);
        }
    }
}
void constrainII_row(int row, FILE *fp)
{
    int m = n / 2 + 1;
    int j = 0;
    int *rowar = (int *)malloc(sizeof(int) * n);
    for (int k = 1; k <= n; ++k)
    {
        rowar[k - 1] = (row - 1) * n + k;
    }
    int *a = (int *)malloc(sizeof(int) * m);
    for (int i = 0; i < m; ++i)
    {
        a[i] = i + 1;
    }
    for (j = m - 1; a[0] <= (n - m + 1);)
    {
        for (; a[m - 1] <= n; a[m - 1]++)
        {
            for (int cur = 0; cur < m; cur++)
            {
                if (cur == m - 1)
                    fprintf(fp, "%d %d\n", rowar[a[cur] - 1], 0);
                else
                {
                    fprintf(fp, "%d ", rowar[a[cur] - 1]);
                }
            }
            for (int cur = 0; cur < m; cur++)
            {
                if (cur == m - 1)
                    fprintf(fp, "%d %d\n", -rowar[a[cur] - 1], 0);
                else
                {
                    fprintf(fp, "%d ", -rowar[a[cur] - 1]);
                }
            }
        }
        for (j = m - 2; j >= 0; j--)
        {
            ++a[j];
            if (a[j] <= (j - m + n + 1))
                break;
        }
        for (j++; j > 0 && j < m; j++)
        {
            a[j] = a[j - 1] + 1;
        }
    }
    free(a);
    free(rowar);
}
void constrainII_col(int col, FILE *fp)
{
    int m = n / 2 + 1;
    int *colar = (int *)malloc(sizeof(int) * n);
    for (int k = 1; k <= n; ++k)
    {
        colar[k - 1] = (k - 1) * n + col;
    }
    int *a = (int *)malloc(sizeof(int) * m);
    for (int i = 0; i < m; ++i)
    {
        a[i] = i + 1;
    }
    for (int j = m - 1; a[0] <= (n - m + 1);)
    {
        for (; a[m - 1] <= n; a[m - 1]++)
        {
            for (int cur = 0; cur < m; cur++)
            {
                if (cur == m - 1)
                    fprintf(fp, "%d %d\n", colar[a[cur] - 1], 0);
                else
                    fprintf(fp, "%d ", colar[a[cur] - 1]);
            }
            for (int cur = 0; cur < m; cur++)
            {
                if (cur == m - 1)
                    fprintf(fp, "%d %d\n", -colar[a[cur] - 1], 0);
                else
                    fprintf(fp, "%d ", -colar[a[cur] - 1]);
            }
        }
        for (j = m - 2; j >= 0; j--)
        {
            ++a[j];
            if (a[j] <= (j - m + n + 1))
                break;
        }
        for (++j; j > 0 && j < m; ++j)
        {
            a[j] = a[j - 1] + 1;
        }
    }
    free(a);
    free(colar);
}
void constrainIII_row(FILE *fp)
{
    for (int i = 1; i <= n; ++i)
    {
        for (int j = i + 1; j <= n; ++j)
        {
            int *arr = (int *)malloc(sizeof(int) * n);
            int arindex = 0;
            for (int k = 1; k <= n; ++k)
            {
                int l1 = (i - 1) * n + k;
                int l2 = (j - 1) * n + k;
                int l3 = add_var + 1;
                int l4 = add_var + 2;
                int l5 = add_var + 3;
                arr[arindex++] = l5;
                add_var += 3;
                fprintf(fp, "%d %d %d\n", l1, -l3, 0);
                fprintf(fp, "%d %d %d\n", l2, -l3, 0);
                fprintf(fp, "%d %d %d %d\n", -l1, -l2, l3, 0);
                fprintf(fp, "%d %d %d\n", -l1, -l4, 0);
                fprintf(fp, "%d %d %d\n", -l2, -l4, 0);
                fprintf(fp, "%d %d %d %d\n", l1, l2, l4, 0);
                fprintf(fp, "%d %d %d\n", l3, -l5, 0);
                fprintf(fp, "%d %d %d\n", l4, -l5, 0);
                fprintf(fp, "%d %d %d %d\n", -l3, -l4, l5, 0);
            }
            int l0 = add_var + 1;
            add_var++;
            fprintf(fp, "%d ", -l0);
            for (int u = 0; u < arindex; u++)
            {
                if (u == arindex - 1)
                    fprintf(fp, "%d %d\n", -arr[u], 0);
                else
                    fprintf(fp, "%d ", -arr[u]);
            }
            for (int u = 0; u < arindex; u++)
            {
                fprintf(fp, "%d %d %d\n", arr[u], l0, 0);
            }
        }
    }
}
void constrainIII_col(FILE *fp)
{
    for (int i = 1; i <= n; ++i)
    {
        for (int j = i + 1; j <= n; ++j)
        {
            int *arr = (int *)malloc(sizeof(int) * n);
            int arindex = 0;
            for (int k = 1; k <= n; ++k)
            {
                int l1 = (k - 1) * n + i;
                int l2 = (k - 1) * n + j;
                int l3 = add_var + 1;
                int l4 = add_var + 2;
                int l5 = add_var + 3;
                arr[arindex++] = l5;
                add_var += 3;
                fprintf(fp, "%d %d %d\n", l1, -l3, 0);
                fprintf(fp, "%d %d %d\n", l2, -l3, 0);
                fprintf(fp, "%d %d %d %d\n", -l1, -l2, l3, 0);
                fprintf(fp, "%d %d %d\n", -l1, -l4, 0);
                fprintf(fp, "%d %d %d\n", -l2, -l4, 0);
                fprintf(fp, "%d %d %d %d\n", l1, l2, l4, 0);
                fprintf(fp, "%d %d %d\n", -l3, l5, 0);
                fprintf(fp, "%d %d %d\n", -l4, l5, 0);
                fprintf(fp, "%d %d %d %d\n", l3, l4, -l5, 0);
            }
            int l0 = add_var + 1;
            add_var++;
            fprintf(fp, "%d ", -l0);
            for (int u = 0; u < arindex; u++)
            {
                if (u == arindex - 1)
                    fprintf(fp, "%d %d\n", -arr[u], 0);
                else
                    fprintf(fp, "%d ", -arr[u]);
            }
            for (int u = 0; u < arindex; u++)
            {
                fprintf(fp, "%d %d %d\n", arr[u], l0, 0);
            }
        }
    }
}
bool r_check(int i, int j, int val) //行检查
{
    if (j == 1)
    {
        if (sudoku[i][j + 1] == sudoku[i][j + 2] && sudoku[i][j + 1] == val)
        {
            return false;
        }
    }
    else if (j == n)
    {
        if (sudoku[i][j - 1] == sudoku[i][j - 2] && sudoku[i][j - 1] == val)
        {
            return false;
        }
    }
    else if (j == 2)
    {
        if (sudoku[i][j - 1] == sudoku[i][j + 1] && sudoku[i][j - 1] == val)
        {
            return false;
        }
        else if (sudoku[i][j + 1] == sudoku[i][j + 2] && sudoku[i][j + 1] == val)
        {
            return false;
        }
    }
    else if (j == n - 1)
    {
        if (sudoku[i][j - 1] == sudoku[i][j + 1] && sudoku[i][j - 1] == val)
        {
            return false;
        }
        else if (sudoku[i][j - 1] == sudoku[i][j - 2] && sudoku[i][j - 1] == val)
        {
            return false;
        }
    }
    else
    {
        if (sudoku[i][j - 1] == sudoku[i][j + 1] && sudoku[i][j - 1] == val)
        {
            return false;
        }
        else if (sudoku[i][j - 1] == sudoku[i][j - 2] && sudoku[i][j - 1] == val)
        {
            return false;
        }
        else if (sudoku[i][j + 1] == sudoku[i][j + 2] && sudoku[i][j + 1] == val)
        {
            return false;
        }
    }
    return true;
}

bool c_check(int i, int j, int val) //列检查
{
    if (i == 1)
    {
        if (sudoku[i + 1][j] == sudoku[i + 2][j] && sudoku[i + 1][j] == val)
        {
            return false;
        }
    }
    else if (i == n)
    {
        if (sudoku[i - 1][j] == sudoku[i - 2][j] && sudoku[i - 1][j] == val)
        {
            return false;
        }
    }
    else if (i == 2)
    {
        if (sudoku[i - 1][j] == sudoku[i + 1][j] && sudoku[i - 1][j] == val)
        {
            return false;
        }
        else if (sudoku[i + 1][j] == sudoku[i + 2][j] && sudoku[i + 1][j] == val)
        {
            return false;
        }
    }
    else if (i == n - 1)
    {
        if (sudoku[i - 1][j] == sudoku[i + 1][j] && sudoku[i - 1][j] == val)
        {
            return false;
        }
        else if (sudoku[i - 1][j] == sudoku[i][j - 2] && sudoku[i - 1][j] == val)
        {
            return false;
        }
    }
    else
    {
        if (sudoku[i - 1][j] == sudoku[i + 1][j] && sudoku[i - 1][j] == val)
        {
            return false;
        }
        else if (sudoku[i - 1][j] == sudoku[i - 2][j] && sudoku[i - 1][j] == val)
        {
            return false;
        }
        else if (sudoku[i + 1][j] == sudoku[i + 2][j] && sudoku[i + 1][j] == val)
        {
            return false;
        }
    }
    return true;
}

void origin_sdk()
{
    for (int i = 1; i <= n; ++i)
    {
        for (int j = 1; j <= n; ++j)
        {
            if (sudoku[i][j] == -1)
                continue;
            else if (sudoku[i][j] == 1)
            {
                fprintf(sdkfp, "%d %d\n", (i - 1) * n + j, 0);
            }
            else
            {
                fprintf(sdkfp, "%d %d\n", -((i - 1) * n + j), 0);
            }
        }
    }
}
void test_term()
{
    sdkfp = fopen("./Sudoku.cnf", "w+");
    int m = n / 2;
    int lit_count = n * n + (3 * n + 1) * comb(n, 2) * 2;
    int cl_count_1 = n * (n - 2) * 4;
    int cl_count_2 = 4 * n * comb(n, m + 1);
    int cl_count_3 = 2 * comb(n, 2) * (10 * n + 1);
    int cl_count = cl_count_1 + cl_count_2 + cl_count_3 + unit_cl_count;
    fprintf(sdkfp, "p cnf %d %d\n", lit_count, cl_count);
    origin_sdk();

    constrainI(sdkfp);
    for (int i = 1; i <= n; ++i)
    {
        constrainII_row(i, sdkfp);
        constrainII_col(i, sdkfp);
    }
    add_var = n * n;
    constrainIII_row(sdkfp);
    constrainIII_col(sdkfp);
    fclose(sdkfp);
}

bool lasvegas(int pn) //生成终盘
{
    int i = 0, j = 0, val = 0;
    srand(time(NULL));
    while (pn)
    {
        i = rand() % n + 1;
        j = rand() % n + 1;
        if (sudoku[i][j] == -1)
        {
            val = rand() % 2;
            if (row[i][val] < n / 2 && col[j][val] < n / 2 && r_check(i, j, val) && c_check(i, j, val))
            {
                sudoku[i][j] = val;
                row[i][val]++;
                col[j][val]++;
                pn--;
            }
        }
    }

    test_term();

    depth = 1;
    ch_ind = 0;
    n_ucl = 0;
    ClInfo = NULL;
    LitsInfo = NULL;
    as_info = NULL;
    dep_change = NULL;
    cnfparser("./Sudoku.cnf");
    if (dpll())
    {
        return true;
    }
    return false;
}

void dighole()
{
    int m = n / 2;
    int dlit_count = n * n + (3 * n + 1) * comb(n, 2) * 2;
    int dcl_count_1 = n * (n - 2) * 4;
    int dcl_count_2 = 4 * n * comb(n, m + 1);
    int dcl_count_3 = 2 * comb(n, 2) * (10 * n + 1);
    int dcl_count = dcl_count_1 + dcl_count_2 + dcl_count_3;
    int leg_dit_cnt = 1;
    lassign_info *dig_info = (lassign_info *)malloc(sizeof(lassign_info) * (dlit_count + 1));
    for (int i = 1; i <= n; ++i)
    {
        for (int j = 1; j <= n; ++j)
        {
            dig_info[(i - 1) * n + j].assign_val = as_info[(i - 1) * n + j].assign_val;
            dig_info[(i - 1) * n + j].dig_stat = as_info[(i - 1) * n + j].dig_stat;
        }
    }
    dig_info[1].dig_stat = LEGAL_DIG;
    for (int i = 1; i <= n; ++i)
    {
        for (int j = 1; j <= n; ++j)
        {
            if (dig_info[(i - 1) * n + j].dig_stat == FREE_DIG)
            {
                FILE *digfp = fopen("./digtest.cnf", "w+");
                fprintf(digfp, "p cnf %d %d\n", dlit_count, dcl_count + n * n - leg_dit_cnt);

                for (int k = 1; k <= n; ++k)
                {
                    for (int u = 1; u <= n; ++u)
                    {
                        if (((k - 1) * n + u != (i - 1) * n + j) && dig_info[(k - 1) * n + u].dig_stat != LEGAL_DIG)
                        {
                            if (dig_info[(k - 1) * n + u].assign_val == 1)
                                fprintf(digfp, "%d %d\n", (k - 1) * n + u, 0);
                            else if (dig_info[(k - 1) * n + u].assign_val == 0)
                                fprintf(digfp, "%d %d\n", -((k - 1) * n + u), 0);
                        }
                    }
                }
                if (dig_info[(i - 1) * n + j].assign_val == 1)
                {
                    fprintf(digfp, "%d %d\n", -((i - 1) * n + j), 0);
                }
                else if (dig_info[(i - 1) * n + j].assign_val == 0)
                {
                    fprintf(digfp, "%d %d\n", (i - 1) * n + j, 0);
                }

                constrainI(digfp);
                for (int i = 1; i <= n; ++i)
                {
                    constrainII_row(i, digfp);
                    constrainII_col(i, digfp);
                }
                add_var = n * n;
                constrainIII_row(digfp);
                constrainIII_col(digfp);
                fclose(digfp);

                depth = 1;
                ch_ind = 0;
                n_ucl = 0;
                ClInfo = NULL;
                LitsInfo = NULL;
                as_info = NULL;
                dep_change = NULL;
                cnfparser("./digtest.cnf");
                if (dpll())
                {
                    dig_info[(i - 1) * n + j].dig_stat = ILLEGAL_DIG;
                    continue;
                }
                else
                {
                    dig_info[(i - 1) * n + j].dig_stat = LEGAL_DIG;
                    leg_dit_cnt++;
                }
            }
        }
    }
    for (int i = 1; i <= n; ++i)
    {
        for (int j = 1; j <= n; ++j)
        {
            as_info[(i - 1) * n + j].assign_val = dig_info[(i - 1) * n + j].assign_val;
            as_info[(i - 1) * n + j].dig_stat = dig_info[(i - 1) * n + j].dig_stat;
        }
    }
}
void gen_sudoku()
{
    int pn = unit_cl_count;
    while (!lasvegas(pn))
        ;
    for (int i = 1; i <= n; ++i)
    {
        for (int j = 1; j <= n; ++j)
        {
            as_info[(i - 1) * n + j].dig_stat = FREE_DIG;
        }
    }
    dighole();
}
void map_sudoku()
{
    for (int i = 1; i <= n; ++i)
    {
        for (int j = 1; j <= n; ++j)
        {
            if (as_info[(i - 1) * n + j].dig_stat == LEGAL_DIG)
            {
                dig_count++;
                sudoku[i][j] = -1;
            }
            else
            {
                if (as_info[(i - 1) * n + j].assign_val == 1)
                {
                    sudoku[i][j] = 1;
                }
                else if (as_info[(i - 1) * n + j].assign_val == 0)
                {
                    sudoku[i][j] = 0;
                }
            }
        }
    }
}

bool check_answer()
{
    for (int i = 1; i <= n; ++i)
    {
        for (int j = 1; j <= n; ++j)
        {
            if (sudoku[i][j] != as_info[(i - 1) * n + j].assign_val)
            {
                return false;
            }
        }
    }
    return true;
}
void present_sudoku() //打印数独
{
    printf("\n");
    for (int u = 1; u <= n; ++u)
    {
        if (u < n)
        {
            printf("|———");
        }
        else
        {
            printf("|———|\n");
        }
    }
    for (int i = 1; i <= n; ++i)
    {
        for (int j = 1; j <= n; ++j)
        {
            if (j == 1)
            {
                if (sudoku[i][j] == -1)
                {
                    printf("|   |");
                }
                else
                {

                    printf("| %d |", sudoku[i][j]);
                }
            }
            else
            {
                if (sudoku[i][j] == -1)
                {
                    printf("   |");
                }
                else
                {

                    printf(" %d |", sudoku[i][j]);
                }
            }
        }
        printf("\n");
        for (int k = 1; k <= n; ++k)
        {
            if (k < n)
            {
                printf("|———");
            }
            else
            {
                printf("|———|\n");
            }
        }
    }
    printf("\n");
}
void present_answer() //打印答案
{
    printf("\n");
    for (int u = 1; u <= n; ++u)
    {
        if (u < n)
        {
            printf("|———");
        }
        else
        {
            printf("|———|\n");
        }
    }
    for (int i = 1; i <= n; ++i)
    {
        for (int j = 1; j <= n; ++j)
        {
            if (j == 1)
            {
                printf("| %d |", as_info[(i - 1) * n + j].assign_val);
            }
            else
            {
                printf(" %d |", as_info[(i - 1) * n + j].assign_val);
            }
        }
        printf("\n");
        for (int k = 1; k <= n; ++k)
        {
            if (k < n)
            {
                printf("|———");
            }
            else
            {
                printf("|———|\n");
            }
        }
    }
    printf("\n");
}
void BinaryPuzzle()
{
    char answer_flag, pro_flag = 'Y';
    int fin_flag;
    int end_choice;
    int r, c, value;
    printf("\nBinary Puzzle!\n");
    printf("\nPlease enter the order of Sudoku (4/6/8):");
    sudoku_ini();
    printf("Sudoku Initializing...\n");
    gen_sudoku();
restart:
    map_sudoku();
    fin_flag = dig_count;
    present_sudoku();
    printf("\nP : Proceed\n");
    printf("\nR : Restart\n");
    printf("\nQ : Quit\n");
    printf("\nYour Choice (P/R/Q) :");
    getchar();
    scanf("%c", &pro_flag);
    while (pro_flag != 'P' && pro_flag != 'p' && pro_flag != '\n' && pro_flag != 'Q' && pro_flag != 'q' && pro_flag != 'R' && pro_flag != 'r')
    {
        printf("Invalide decision! Please enter again (P/R/Q):");
        getchar();
        scanf("%c", &pro_flag);
    }
    if (pro_flag == 'R' || pro_flag == 'r')
    {
        goto restart;
    }
    while (pro_flag == 'P' || pro_flag == 'p' || pro_flag == '\n' || pro_flag == 'R' || pro_flag == 'r')
    {
        if (pro_flag == 'R' || pro_flag == 'r')
        {
            goto restart;
        }
        else if (pro_flag == 'P' || pro_flag == 'p' || pro_flag == '\n')
        {
            printf("\nPlease enter the coordinate and value:\n");
            printf("r (1~%d):", n);
            scanf("%d", &r);
            printf("c (1~%d):", n);
            scanf("%d", &c);
            printf("value (0/1):");
            scanf("%d", &value);
            if (sudoku[r][c] == -1 && (value == 0 || value == 1))
            {
                sudoku[r][c] = value;
            }
            else
            {
                printf("Access denied! Please enter again.\n");
                continue;
            }
            present_sudoku();
            fin_flag--;
        }

        if (fin_flag)
        {
            printf("\nYour Choice (P/R/Q) :");
            getchar();
            scanf("%c", &pro_flag);
            while (pro_flag != 'P' && pro_flag != 'p' && pro_flag != '\n' && pro_flag != 'Q' && pro_flag != 'q' && pro_flag != 'R' && pro_flag != 'r')
            {
                printf("Invalide decision! Please enter again (P/R/Q):");
                getchar();
                scanf("%c", &pro_flag);
            }
        }
        else
            break;
    }
    int o_flag = check_answer();
    if (o_flag)
    {
        printf("Congratulations! You find the right answer.\n");
    }
    else
    {
        if (pro_flag == 'P' || pro_flag == 'p' || pro_flag == '\n')
        {
            printf("Sorry, your answer is wrong.\n");
        }
        printf("Do you wanna check the answer ? (Y/N):");
        getchar();
        scanf("%c", &answer_flag);
        if (answer_flag == 'Y' || answer_flag == 'y' || answer_flag == '\n')
        {
            present_answer();
        }
    }
    printf("Done!\n");
}