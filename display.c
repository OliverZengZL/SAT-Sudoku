#include "std.h"
#include "bp.h"
#include "solver.h"
#include "global_var.h"

int main()
{
    int choice = 0;
    printf("\n1.SAT Solver\n");
    printf("2.Binary Puzzle\n");

    printf("\nPlease enter the function number (1/2):");
    scanf("%d", &choice);
    while (choice != 1 && choice != 2 && choice != 3)
    {
        printf("\nInvalid choice, please enter again (1/2):");
        scanf("%d", &choice);
    }
    if (choice == 1)
    {
        solver();
    }
    else
    {
        BinaryPuzzle();
    }
}
