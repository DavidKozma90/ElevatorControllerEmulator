#include "commonHeader.h"
#include "PublicAPI/seqnet.h"
#include "PublicAPI/condsel.h"
#include "Utils/instructionEncoder.h"
#include "Utils/customAssert.h"

#include <stdio.h>
#include <stdbool.h>

void testSimpleCalls(uint8_t elevator_pos, uint8_t call_floor)
{
    CondSel_In cond = {0};
    SeqNet_Out out = {0};
    bool cond_result;
    int pc_before = 0;
    int pc_after = 0;
    bool call_active = true;

    cond.door_closed = true;
    cond.door_open = false;

    for (int cycle = 0; cycle < 50; ++cycle)
    {
        if (call_active) 
        {
            cond.call_pending_same  = (elevator_pos == call_floor);
            cond.call_pending_below = (elevator_pos > call_floor);
            cond.call_pending_above = (elevator_pos < call_floor);
        }
        else 
        {
            cond.call_pending_same = false;
            cond.call_pending_below = false;
            cond.call_pending_above = false;
        }

        if (out.req_reset) 
        {
            cond.call_pending_same = false;
            cond.call_pending_above = false;
            cond.call_pending_below = false;
            call_active = false;
        }

        pc_before = GetProgramCounter();
        uint16_t raw_instr = GetProgMemAtPC(pc_before);
        SeqNet_Out instr = DecodeInstruction(raw_instr);

        cond_result = CondSel_calc(instr.cond_inv, instr.cond_sel, cond);
        out = SeqNet_loop(cond_result);
        pc_after = GetProgramCounter();

        printf("Cycle %2d | PC: %2d → %2d | MoveUp: %d | MoveDown: %d | Door: %s | Reset: %d | Floor: %d\n",
               cycle, pc_before, pc_after, out.req_move_up, out.req_move_down,
               out.req_door_state ? "OPEN" : "CLOSED", out.req_reset, elevator_pos);

        // Simulate movement
        if (out.req_move_down && cond.call_pending_below && (elevator_pos >= 0))
        {
               elevator_pos--;
        }
           else if (out.req_move_up && cond.call_pending_above && (elevator_pos <= 5))
           {
            elevator_pos++;
        }

        /* Simulate door state */
        if (out.req_door_state)
        {
            cond.door_open = true;
            cond.door_closed = false;
        }
        else
        {
            cond.door_open = false;
            cond.door_closed = true;
        }
    }
}

int main ()
{
#if 0
    // Load the elevator control program
    LoadProgram_Default();

    testSimpleCalls(5, 1);
    testSimpleCalls(1, 5);
    testSimpleCalls(2, 2);
#endif

    LoadProgram_Default();
    RunValidationTests();

    return 0;

}
