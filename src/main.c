#include "commonHeader.h"
#include "PublicAPI/seqnet.h"
#include "PublicAPI/condsel.h"
#include "Utils/instructionCoders.h"
#include "Utils/customAssert.h"

static void printMenu(void) 
{
    printf("\nElevator Controller Emulator - Menu\n");
    printf("1. Run simple example simulation \n");
    printf("2. Load and display default program\n");
    printf("3. Print program memory \n");
    printf("4. Run validation tests\n");
    printf("x. Exit\n");
    printf("Select an option (and press Enter): ");
}

int main()
{
    char input[16];

    SeqNet_init();
    LoadProgram_Default();

    while (1) 
    {
        printMenu();
        if (!fgets(input, sizeof(input), stdin)) 
        {
            continue;
        }

        switch (input[0]) 
        {
            case '1':   
                int elevator_pos = 0, call_floor = 0;
                printf("Enter elevator starting position (0-5): ");
                if (scanf("%d", &elevator_pos) != 1 || elevator_pos < 0 || elevator_pos > 5) 
                {
                    printf("Invalid input. Please enter a number between 0 and 5.\n");
                    int c; while ((c = getchar()) != '\n' && c != EOF);
                    break;
                }

                printf("Enter destination floor (0-5): ");
                if (scanf("%d", &call_floor) != 1 || call_floor < 0 || call_floor > 5) 
                {
                    printf("Invalid input. Please enter a number between 0 and 5.\n");
                    int c; while ((c = getchar()) != '\n' && c != EOF);
                    break;
                }
                int c; while ((c = getchar()) != '\n' && c != EOF);
                TestSimpleCalls((uint8_t)elevator_pos, (uint8_t)call_floor);
                break;
            case '2':
                LoadProgram_Default();
                printf("Default program loaded.\n");
                printProgMem();
                break;
            case '3':
                printProgMem();
                break;
            case '4':
                RunValidationTests();
                break;
            case 'x':
            case 'X':
                printf("Exiting...\n");
                return 0;
            default:
                printf("Unknown option. Please try again.\n");
        }
    }
}
