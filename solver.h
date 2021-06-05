#include "std.h"
#include "global_var.h"
#include "cnfparser.h"
#include "dpll.h"

int flag_dpll = 0;
char address[200];
char filename[100];
char opfile[200];
FILE *opfp = NULL;

void filename_confirm()
{
    char *lp = "./";
    char *op = "_op.res";
    printf("\nPlease enter the filename:");
    scanf("%s", filename);
    strcat(address, lp);
    strcat(address, filename);
    strcat(opfile, lp);
    strcat(opfile, filename);
    strcat(opfile, op);
}

void solver()
{
    printf("\nSAT Solver!\n");
    filename_confirm();
    cnfparser(address);
    clock_t start, finish;
    start = clock();
    flag_dpll = dpll();
    finish = clock();
    double ts = (double) (finish - start) / CLOCKS_PER_SEC * 1000;
    opfp = fopen(opfile, "w+");
    if (flag_dpll)
    {
        fprintf(opfp, "s 1\n\n");
        fprintf(opfp, "v\n");
        for (int i = 1; i <= literal_count; ++i)
        {
            if (as_info[i].assign_val == 1)
            {
                fprintf(opfp, " %d", i);
            }
            else if (as_info[i].assign_val == 0)
            {
                fprintf(opfp, " %d", -i);
            }
            else
                continue;
            if (i % 10 == 0)
                fprintf(opfp, "\n");
        }
        if (literal_count % 10)
            fprintf(opfp, "\n");
        fprintf(opfp, "\nt %d\n", (int) ts );
    }
    else
    {
        fprintf(opfp, "s 0\n");
        fprintf(opfp, "\nt %d\n", (int) ts );
    }
    fclose(opfp);
    printf("\nSolver Done!\n");
}
