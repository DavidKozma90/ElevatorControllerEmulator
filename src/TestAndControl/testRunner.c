#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "commonHeader.h"
#include "PublicAPI/seqnet.h"
#include "PublicAPI/condsel.h"
#include "Utils/instructionEncoder.h"
#include "Utils/customAssert.h"

#define MAX_CYCLES 50
#define MAX_TESTS  10

/* Global simulation state */
static uint8_t elevatorPos = 0;
static uint8_t callFloor = 0;
static bool callActive = true;
static CondSel_In condition = {0};
static SeqNet_Out output = {0};

/* -------------- Test Infrastructure -------------- */

typedef void (*TestFunc)(void);

typedef struct 
{
    const char* name;
    TestFunc function;
} TestCase;

static TestCase testRegistry[MAX_TESTS];
static uint32_t testCount = 0;

static void registerTest(const char* name, TestFunc function) 
{
    assert(testCount < MAX_TESTS);
    testRegistry[testCount++] = (TestCase){name, function};
}

static void runAllTests() 
{
    printf("=== Running %d Test(s) ===\n\n", testCount);
    for (int i = 0; i < testCount; ++i) 
    {
        printf("-> [%d/%d] %s\n", i + 1, testCount, testRegistry[i].name);
        testRegistry[i].function();
    }
    printf("SUCCESS: All tests completed.\n\n");
}

/* -------------- Setup / Teardown / Helpers -------------- */

void setup(int start_pos, int target_floor) 
{
    SeqNet_init();
    LoadProgram_Default();
    elevatorPos = start_pos;
    callFloor = target_floor;
    callActive = true;
    condition.door_open = true;
    condition.door_closed = false;
    condition.call_pending_below = false;
    condition.call_pending_same = false;
    condition.call_pending_above = false;
    condition.call_pending_below = false;
    condition.call_pending_same = false;
    condition.call_pending_above = false;
    output.jump_addr = 0;
    output.req_move_up = false;
    output.req_move_down = false;
    output.req_door_state = DOOR_OPEN;
    output.req_reset = false;
    output.cond_sel = 0;
    output.cond_inv = false;
    printf("=== Test Setup ===\n");
    printf("   Initial floor: %d\n", elevatorPos);
    printf("   Target floor: %d\n", callFloor);
}

void teardown() 
{
    printf("   Final floor: %d\n\n", elevatorPos);
}

void ASSERT_ELEVATOR_REACHES_FLOOR(int expected_floor) 
{
    CUSTOM_ASSERT((elevatorPos == expected_floor),"Test Fail: Elevator did not reach expected floor!");
}

void ASSERT_DOOR_IS_OPEN(void) 
{
    CUSTOM_ASSERT(condition.door_open, "Test Fail: Door should be open!");
}

void ASSERT_NO_MOVEMENT(void) 
{
    CUSTOM_ASSERT((!output.req_move_up && !output.req_move_down), "Test Fail: Elevator should not be moving!");
}

void ASSERT_PC_IS_WITHIN_BOUNDS(void) 
{
    CUSTOM_ASSERT((GetProgramCounter() < GetProgramSize()), "Test Fail: Program counter is out of bounds!");
}

/* -------------- Test Cases -------------- */

void test_move_down_single_call() 
{
    int pc_before = 0;
    int pc_after = 0;
    bool cond_result = false;

    setup(5, 1);

    for (int cycle = 0; cycle < MAX_CYCLES; ++cycle) 
    {
        if (callActive) 
        {
            condition.call_pending_same  = (elevatorPos == callFloor);
            condition.call_pending_below = (elevatorPos > callFloor);
            condition.call_pending_above = (elevatorPos < callFloor);
        }
        else 
        {
            condition.call_pending_same = false;
            condition.call_pending_below = false;
            condition.call_pending_above = false;
        }

        if (output.req_reset) 
        {
            condition.call_pending_same = false;
            condition.call_pending_above = false;
            condition.call_pending_below = false;
            callActive = false;
        }

        pc_before = GetProgramCounter();
        uint16_t raw_instr = GetProgMemAtPC(pc_before);
        SeqNet_Out instr = DecodeInstruction(raw_instr);

        cond_result = CondSel_calc(instr.cond_inv, instr.cond_sel, condition);
        output = SeqNet_loop(cond_result);
        pc_after = GetProgramCounter();

        LOG("Cycle %2d | PC: %2d → %2d | MoveUp: %d | MoveDown: %d | Door: %s | Reset: %d | Floor: %d\n",
               cycle, pc_before, pc_after, output.req_move_up, output.req_move_down,
               output.req_door_state ? "OPEN" : "CLOSED", output.req_reset, elevatorPos);

        // Simulate movement
        if (output.req_move_down && condition.call_pending_below && (elevatorPos >= 0))
        {
            elevatorPos--;
        }
        else if (output.req_move_up && condition.call_pending_above && (elevatorPos <= 5))
        {
            elevatorPos++;
        }

        /* Simulate door state */
        if (output.req_door_state)
        {
            condition.door_open = true;
            condition.door_closed = false;
        }
        else
        {
            condition.door_open = false;
            condition.door_closed = true;
        }

        ASSERT_PC_IS_WITHIN_BOUNDS();
    }

    ASSERT_ELEVATOR_REACHES_FLOOR(1);
    ASSERT_DOOR_IS_OPEN();
    teardown();
}

void test_move_up_single_call() 
{
    uint8_t pc_before = 0;
    uint8_t pc_after = 0;
    bool cond_result = false;

    setup(1, 5);

    for (int cycle = 0; cycle < MAX_CYCLES; ++cycle) 
    {
        if (callActive) 
        {
            condition.call_pending_same  = (elevatorPos == callFloor);
            condition.call_pending_below = (elevatorPos > callFloor);
            condition.call_pending_above = (elevatorPos < callFloor);
        }
        else 
        {
            condition.call_pending_same = false;
            condition.call_pending_below = false;
            condition.call_pending_above = false;
        }

        if (output.req_reset) 
        {
            condition.call_pending_same = false;
            condition.call_pending_above = false;
            condition.call_pending_below = false;
            callActive = false;
        }

        pc_before = GetProgramCounter();
        uint16_t raw_instr = GetProgMemAtPC(pc_before);
        SeqNet_Out instr = DecodeInstruction(raw_instr);

        cond_result = CondSel_calc(instr.cond_inv, instr.cond_sel, condition);
        output = SeqNet_loop(cond_result);
        pc_after = GetProgramCounter();

        LOG("Cycle %2d | PC: %2d → %2d | MoveUp: %d | MoveDown: %d | Door: %s | Reset: %d | Floor: %d\n",
               cycle, pc_before, pc_after, output.req_move_up, output.req_move_down,
               output.req_door_state ? "OPEN" : "CLOSED", output.req_reset, elevatorPos);

        /* Simulate movement */
        if (output.req_move_down && condition.call_pending_below && (elevatorPos >= 0))
        {
            elevatorPos--;
        }
        else if (output.req_move_up && condition.call_pending_above && (elevatorPos <= 5))
        {
            elevatorPos++;
        }

        /* Simulate door state */
        if (output.req_door_state)
        {
            condition.door_open = true;
            condition.door_closed = false;
        }
        else
        {
            condition.door_open = false;
            condition.door_closed = true;
        }

        ASSERT_PC_IS_WITHIN_BOUNDS();
    }

    ASSERT_ELEVATOR_REACHES_FLOOR(5);
    ASSERT_DOOR_IS_OPEN();
    teardown();
}

void test_same_floor_call_handled() 
{
    uint8_t pc_before = 0;
    uint8_t pc_after = 0;
    bool cond_result = false;

    setup(2, 2);

    for (int cycle = 0; cycle < MAX_CYCLES; ++cycle) 
    {
        if (callActive) 
        {
            condition.call_pending_same  = (elevatorPos == callFloor);
            condition.call_pending_below = (elevatorPos > callFloor);
            condition.call_pending_above = (elevatorPos < callFloor);
        }
        else 
        {
            condition.call_pending_same = false;
            condition.call_pending_below = false;
            condition.call_pending_above = false;
        }

        if (output.req_reset) 
        {
            condition.call_pending_same = false;
            condition.call_pending_above = false;
            condition.call_pending_below = false;
            callActive = false;
        }

        pc_before = GetProgramCounter();
        uint16_t raw_instr = GetProgMemAtPC(pc_before);
        SeqNet_Out instr = DecodeInstruction(raw_instr);

        cond_result = CondSel_calc(instr.cond_inv, instr.cond_sel, condition);
        output = SeqNet_loop(cond_result);
        pc_after = GetProgramCounter();

        LOG("Cycle %2d | PC: %2d → %2d | MoveUp: %d | MoveDown: %d | Door: %s | Reset: %d | Floor: %d\n",
               cycle, pc_before, pc_after, output.req_move_up, output.req_move_down,
               output.req_door_state ? "OPEN" : "CLOSED", output.req_reset, elevatorPos);

        /* Simulate movement */
        if (output.req_move_down && condition.call_pending_below && (elevatorPos >= 0))
        {
            elevatorPos--;
        }
        else if (output.req_move_up && condition.call_pending_above && (elevatorPos <= 5))
        {
            elevatorPos++;
        }

        /* Simulate door state */
        if (output.req_door_state)
        {
            condition.door_open = true;
            condition.door_closed = false;
        }
        else
        {
            condition.door_open = false;
            condition.door_closed = true;
        }

        ASSERT_NO_MOVEMENT();
        ASSERT_PC_IS_WITHIN_BOUNDS();
    }

    ASSERT_ELEVATOR_REACHES_FLOOR(2);
    ASSERT_DOOR_IS_OPEN();
    teardown();
}

void test_call_repressed_during_door_open() 
{
    bool repressed = false;
    uint8_t pc_before = 0;
    uint8_t pc_after = 0;
    bool cond_result = false;

    setup(2, 2);

    for (int cycle = 0; cycle < MAX_CYCLES; ++cycle) 
    {
        if (callActive) 
        {
            condition.call_pending_same  = (elevatorPos == callFloor);
            condition.call_pending_below = (elevatorPos > callFloor);
            condition.call_pending_above = (elevatorPos < callFloor);
        }
        else 
        {
            condition.call_pending_same = false;
            condition.call_pending_below = false;
            condition.call_pending_above = false;
        }

        if (cycle == 15) 
        {
            condition.call_pending_same = true;
            callActive = true;
            repressed = true;
        }

        if (output.req_reset) 
        {
            condition.call_pending_same = false;
            condition.call_pending_above = false;
            condition.call_pending_below = false;
            callActive = false;
        }

        pc_before = GetProgramCounter();
        uint16_t raw_instr = GetProgMemAtPC(pc_before);
        SeqNet_Out instr = DecodeInstruction(raw_instr);

        cond_result = CondSel_calc(instr.cond_inv, instr.cond_sel, condition);
        output = SeqNet_loop(cond_result);
        pc_after = GetProgramCounter();

        LOG("Cycle %2d | PC: %2d → %2d | MoveUp: %d | MoveDown: %d | Door: %s | Reset: %d | Floor: %d\n",
               cycle, pc_before, pc_after, output.req_move_up, output.req_move_down,
               output.req_door_state ? "OPEN" : "CLOSED", output.req_reset, elevatorPos);

        /* Simulate movement */
        if (output.req_move_down && condition.call_pending_below && (elevatorPos >= 0))
        {
            elevatorPos--;
        }
        else if (output.req_move_up && condition.call_pending_above && (elevatorPos <= 5))
        {
            elevatorPos++;
        }

        /* Simulate door state */
        if (output.req_door_state)
        {
            condition.door_open = true;
            condition.door_closed = false;
        }
        else
        {
            condition.door_open = false;
            condition.door_closed = true;
        }

        ASSERT_NO_MOVEMENT();
        ASSERT_PC_IS_WITHIN_BOUNDS();

        condition.call_pending_same  = (elevatorPos == callFloor);


        /* Simulate door state */
        if (output.req_door_state)
        {
            condition.door_open = true;
            condition.door_closed = false;
        }
        else
        {
            condition.door_open = false;
            condition.door_closed = true;
        }

        ASSERT_NO_MOVEMENT();
        if ((cycle > 25) && repressed) 
        {
            ASSERT_DOOR_IS_OPEN();
        }
    }

    ASSERT_ELEVATOR_REACHES_FLOOR(2);
    ASSERT_DOOR_IS_OPEN();
    teardown();
}

void test_idle_no_calls() 
{
    uint8_t pc_before = 0;
    uint8_t pc_after = 0;
    bool cond_result = false;

    setup(3, 3);
    callActive = false;

    for (int cycle = 0; cycle < MAX_CYCLES; ++cycle) 
    {
        condition.call_pending_same = false;
        condition.call_pending_below = false;
        condition.call_pending_above = false;

        pc_before = GetProgramCounter();
        uint16_t raw_instr = GetProgMemAtPC(pc_before);
        SeqNet_Out instr = DecodeInstruction(raw_instr);

        cond_result = CondSel_calc(instr.cond_inv, instr.cond_sel, condition);
        output = SeqNet_loop(cond_result);
        pc_after = GetProgramCounter();

        LOG("Cycle %2d | PC: %2d → %2d | MoveUp: %d | MoveDown: %d | Door: %s | Reset: %d | Floor: %d\n",
               cycle, pc_before, pc_after, output.req_move_up, output.req_move_down,
               output.req_door_state ? "OPEN" : "CLOSED", output.req_reset, elevatorPos);

        /* Simulate door state */
        if (output.req_door_state)
        {
            condition.door_open = true;
            condition.door_closed = false;
        }
        else
        {
            condition.door_open = false;
            condition.door_closed = true;
        }

        ASSERT_NO_MOVEMENT();

        CUSTOM_ASSERT((pc_after == 0 || pc_after == 1), "FSM did not stay in idle loop.");

        ASSERT_DOOR_IS_OPEN();

        ASSERT_PC_IS_WITHIN_BOUNDS();
    }

    ASSERT_ELEVATOR_REACHES_FLOOR(3);
    ASSERT_DOOR_IS_OPEN();
    teardown();
}

void test_new_call_during_movement() 
{
    bool second_call_triggered = false;
    uint8_t pc_before = 0;
    uint8_t pc_after = 0;
    bool cond_result = false;

    // Start from floor 1, original target is floor 4
    setup(1, 4);

    int second_call_floor = 2;
    bool second_call_active = false;

    for (int cycle = 0; cycle < MAX_CYCLES; ++cycle) 
    {
        // Trigger second call while elevator is moving (e.g., after 5 cycles)
        if (cycle == 5) 
        {
            second_call_active = true;
            second_call_triggered = true;
        }

        // Update condition flags based on current floor and active calls
        condition.call_pending_same =
            (callActive && elevatorPos == callFloor) ||
            (second_call_active && elevatorPos == second_call_floor);

        condition.call_pending_below =
            (callActive && elevatorPos > callFloor) ||
            (second_call_active && elevatorPos > second_call_floor);

        condition.call_pending_above =
            (callActive && elevatorPos < callFloor) ||
            (second_call_active && elevatorPos < second_call_floor);

        // Clear calls on reset
        if (output.req_reset) 
        {
            if (elevatorPos == callFloor) callActive = false;
            if (elevatorPos == second_call_floor) second_call_active = false;
            if (!callActive && !second_call_active)
            {
                break;
            }
        }

        pc_before = GetProgramCounter();
        uint16_t raw_instr = GetProgMemAtPC(pc_before);
        SeqNet_Out instr = DecodeInstruction(raw_instr);

        cond_result = CondSel_calc(instr.cond_inv, instr.cond_sel, condition);
        output = SeqNet_loop(cond_result);
        pc_after = GetProgramCounter();

        LOG("Cycle %2d | PC: %2d → %2d | MoveUp: %d | MoveDown: %d | Door: %s | Reset: %d | Floor: %d\n",
            cycle, pc_before, pc_after, output.req_move_up, output.req_move_down,
            output.req_door_state ? "OPEN" : "CLOSED", output.req_reset, elevatorPos);

        // Simulate movement
        if (output.req_move_down && condition.call_pending_below && elevatorPos > 0)
            elevatorPos--;
        else if (output.req_move_up && condition.call_pending_above && elevatorPos < 5)
            elevatorPos++;

        /* Simulate door state */
        if (output.req_door_state)
        {
            condition.door_open = true;
            condition.door_closed = false;
        }
        else
        {
            condition.door_open = false;
            condition.door_closed = true;
        }

        ASSERT_PC_IS_WITHIN_BOUNDS();
    }

    // Final assertions
    CUSTOM_ASSERT(!callActive, "Test Fail: Initial call not completed.");
    CUSTOM_ASSERT(!second_call_active, "Test Fail: New call not completed.");
    ASSERT_DOOR_IS_OPEN();
    CUSTOM_ASSERT((elevatorPos == callFloor || elevatorPos == second_call_floor),
        "Test Fail: Elevator did not stop at expected floors.");
    CUSTOM_ASSERT(second_call_triggered, "Test Fail: Second call was not triggered.");

    teardown();
}


void RunValidationTests(void)
{
    registerTest("Move Down to Target", test_move_down_single_call);
    registerTest("Move Up to Target", test_move_up_single_call);
    registerTest("Handle Same Floor Call", test_same_floor_call_handled);
    registerTest("Re-Press Call During Door Open", test_call_repressed_during_door_open);
    registerTest("Idle Loop with No Calls", test_idle_no_calls);
    registerTest("New Call During Movement", test_new_call_during_movement);

    runAllTests();
}

// -------------- Main Entry Point --------------
#if 0
int main() {
    register_test("Move Down to Target", test_move_down_single_call);
    register_test("Move Up to Target", test_move_up_single_call);
    register_test("Handle Same Floor Call", test_same_floor_call_handled);
    register_test("Re-Press Call During Door Open", test_call_repressed_during_door_open);

    run_all_tests();
    return 0;
}
#endif