#include "commonHeader.h"
#include "PublicAPI/seqnet.h"
#include "Utils/instructionCoders.h"

#define PROG_MEM_SIZE 256U

/** @brief Sequential network program memory.
  * Note: needs to be initialized before use.
  */
static uint16_t ProgMem[PROG_MEM_SIZE];

/** @brief Program counter for the sequential network.
  * Note: needs to be initialized before use.
  */
static volatile uint8_t PC = 0U;

/** @brief Size of the program in the sequential network.
  * Note: needs to be initialized before use.
  */
static uint8_t ProgramSize = 0U;

/** @brief Initializes the sequential network internal state.
  * Note: needs to be called only once at startup
  */
void SeqNet_init(void)
{
    for (uint16_t i = 0; i < PROG_MEM_SIZE; i++)
    {
        ProgMem[i] = 0x00;
    }
    
    PC = 0x00;
    ProgramSize = 0U;
}

/** Steps the sequential network to the next state.
  * @param[in] condition_active  True, if the selected condition value is active (or inactive if inversion is activate)
  * @return Returns with the new instruction values (@see SeqNet_Out).
  */
SeqNet_Out SeqNet_loop(const bool condition_active)
{
    SeqNet_Out output = {0};
    uint16_t instruction = ProgMem[PC];

    output = DecodeInstruction(instruction);

    if(condition_active)
    {
        PC = output.jump_addr;
    }
    else
    {
        PC = (uint8_t)(((uint16_t)PC + 1U) % PROG_MEM_SIZE);
    }

    return output;
}

/** @brief Loads the default program into the sequential network program memory.
  * Note: needs to be called only once at startup
  * (SC) -> Safety-Critical conditional steps
  */
void LoadProgram_Default(void)
{
    SeqNet_Out instr = {0};

    ProgramSize = 0U;

    /* PC = 0: Check for pending call → jump to 2 if any call exists */
    instr.jump_addr      = 2U;
    instr.cond_sel       = CONDSEL_CALL_PENDING_ANY;
    instr.cond_inv       = false;
    instr.req_move_up    = false;
    instr.req_move_down  = false;
    instr.req_door_state = DOOR_OPEN;
    instr.req_reset      = false;
    ProgMem[0] = EncodeInstruction(&instr);
    ProgramSize++;

    /* PC = 1: Always jump to 0 (idle loop) */
    instr.jump_addr      = 0U;
    instr.cond_sel       = CONDSEL_FIXED_ZERO;
    instr.cond_inv       = true;
    instr.req_move_up    = false;
    instr.req_move_down  = false;
    instr.req_door_state = DOOR_OPEN;
    instr.req_reset      = false;
    ProgMem[1] = EncodeInstruction(&instr);
    ProgramSize++;

    /* PC = 2: Request door close */
    instr.jump_addr      = 3U;
    instr.cond_sel       = CONDSEL_FIXED_ZERO;
    instr.cond_inv       = false;
    instr.req_move_up    = false;
    instr.req_move_down  = false;
    instr.req_door_state = DOOR_CLOSED;
    instr.req_reset      = false;
    ProgMem[2] = EncodeInstruction(&instr);
    ProgramSize++;

    /* (SC) PC = 3: Wait until door is closed (self-loop) */
    instr.jump_addr      = 3U;
    instr.cond_sel       = CONDSEL_DOOR_CLOSED;
    instr.cond_inv       = true;
    instr.req_move_up    = false;
    instr.req_move_down  = false;
    instr.req_door_state = DOOR_CLOSED;
    instr.req_reset      = false;
    ProgMem[3] = EncodeInstruction(&instr);
    ProgramSize++;

    /* PC = 4: If call is below, jump to move down loop (PC = 8) */
    instr.jump_addr       = 8U;
    instr.cond_sel        = CONDSEL_CALL_PENDING_BELOW;
    instr.cond_inv        = false;
    instr.req_move_up     = false;
    instr.req_move_down   = false;
    instr.req_door_state  = DOOR_CLOSED;
    instr.req_reset       = false;
    ProgMem[4] = EncodeInstruction(&instr);
    ProgramSize++;

    /* PC = 5: If call is above, jump to move up loop (PC = 11) */
    instr.jump_addr       = 11U;
    instr.cond_sel        = CONDSEL_CALL_PENDING_ABOVE;
    instr.cond_inv        = false;
    instr.req_move_up     = false;
    instr.req_move_down   = false;
    instr.req_door_state  = DOOR_CLOSED;
    instr.req_reset       = false;
    ProgMem[5] = EncodeInstruction(&instr);
    ProgramSize++;

    /* PC = 6: If call on same floor, jump to PC = 13 */
    instr.jump_addr       = 13U;
    instr.cond_sel        = CONDSEL_CALL_PENDING_SAME;
    instr.cond_inv        = false;
    instr.req_move_up     = false;
    instr.req_move_down   = false;
    instr.req_door_state  = DOOR_CLOSED;
    instr.req_reset       = false;
    ProgMem[6] = EncodeInstruction(&instr);
    ProgramSize++;

    /* (SC) PC = 7: Loop back to PC = 4 if no same-floor call - recheck direction */
    instr.jump_addr       = 4U;
    instr.cond_sel        = CONDSEL_FIXED_ZERO;
    instr.cond_inv        = true;
    instr.req_move_up     = false;
    instr.req_move_down   = false;
    instr.req_door_state  = DOOR_CLOSED;
    instr.req_reset       = false;
    ProgMem[7] = EncodeInstruction(&instr);
    ProgramSize++;

    /* PC = 8: Request move down */
    instr.jump_addr       = 9U;
    instr.cond_sel        = CONDSEL_FIXED_ZERO;
    instr.cond_inv        = false;
    instr.req_move_up     = false;
    instr.req_move_down   = true;
    instr.req_door_state  = DOOR_CLOSED;
    instr.req_reset       = false;
    ProgMem[8] = EncodeInstruction(&instr);
    ProgramSize++;

    /* (SC) PC = 9: Wait until same-floor call is detected (floor reached) - move down */
    instr.jump_addr       = 9U;
    instr.cond_sel        = CONDSEL_CALL_PENDING_SAME;
    instr.cond_inv        = true;
    instr.req_move_up     = false;
    instr.req_move_down   = true;
    instr.req_door_state  = DOOR_CLOSED;
    instr.req_reset       = false;
    ProgMem[9] = EncodeInstruction(&instr);
    ProgramSize++;

    /* PC = 10: Target floor reached → stop moving, jump to door open logic */
    instr.jump_addr       = 14U;
    instr.cond_sel        = CONDSEL_FIXED_ZERO;
    instr.cond_inv        = true;
    instr.req_move_up     = false;
    instr.req_move_down   = false;
    instr.req_door_state  = DOOR_CLOSED;
    instr.req_reset       = false;
    ProgMem[10] = EncodeInstruction(&instr);
    ProgramSize++;

    /* PC = 11: Request move up */
    instr.jump_addr       = 12U;
    instr.cond_sel        = CONDSEL_FIXED_ZERO;
    instr.cond_inv        = false;
    instr.req_move_up     = true;
    instr.req_move_down   = false;
    instr.req_door_state  = DOOR_CLOSED;
    instr.req_reset       = false;
    ProgMem[11] = EncodeInstruction(&instr);
    ProgramSize++;

    /* (SC) PC = 12: Wait until same-floor call is detected (floor reached) - move up */
    instr.jump_addr       = 12U;
    instr.cond_sel        = CONDSEL_CALL_PENDING_SAME;
    instr.cond_inv        = true;
    instr.req_move_up     = true;
    instr.req_move_down   = false;
    instr.req_door_state  = DOOR_CLOSED;
    instr.req_reset       = false;
    ProgMem[12] = EncodeInstruction(&instr);
    ProgramSize++;

    /* PC = 13: Target floor reached → stop moving, jump to door open logic */
    instr.jump_addr       = 14U;
    instr.cond_sel        = CONDSEL_FIXED_ZERO;
    instr.cond_inv        = true;
    instr.req_move_up     = false;
    instr.req_move_down   = false;
    instr.req_door_state  = DOOR_CLOSED;
    instr.req_reset       = false;
    ProgMem[13] = EncodeInstruction(&instr);
    ProgramSize++;

    /* PC = 14: Request door open */
    instr.jump_addr       = 15U;
    instr.cond_sel        = CONDSEL_FIXED_ZERO;
    instr.cond_inv        = false; 
    instr.req_move_up     = false;
    instr.req_move_down   = false;
    instr.req_door_state  = DOOR_OPEN;
    instr.req_reset       = false;
    ProgMem[14] = EncodeInstruction(&instr);
    ProgramSize++;

    /* (SC) PC = 15: Wait until door is fully open */
    instr.jump_addr       = 15U;
    instr.cond_sel        = CONDSEL_DOOR_OPEN;
    instr.cond_inv        = true;
    instr.req_move_up     = false;
    instr.req_move_down   = false;
    instr.req_door_state  = DOOR_OPEN;
    instr.req_reset       = false;
    ProgMem[15] = EncodeInstruction(&instr);
    ProgramSize++;

    /* (SC) PC = 16: Clear pending call, then jump back to idle */
    instr.jump_addr       = 0U;
    instr.cond_sel        = CONDSEL_FIXED_ZERO;
    instr.cond_inv        = true;
    instr.req_move_up     = false;
    instr.req_move_down   = false;
    instr.req_door_state  = DOOR_OPEN;
    instr.req_reset       = true;
    ProgMem[16] = EncodeInstruction(&instr);
    ProgramSize++;
}

void PrintProgMem(void)
{
    printf("Program Memory:\n");
    for (uint8_t i = 0; i < ProgramSize; i++)
    {
        SeqNet_Out instr = DecodeInstruction(ProgMem[i]);
        printf("PC: %2d | Jump: %2d | MoveUp: %d | MoveDown: %d | Door: %s | Reset: %d | CondSel: %d | CondInv: %d | InstrHex: 0x%04X\n",
               i, instr.jump_addr, instr.req_move_up, instr.req_move_down,
               instr.req_door_state ? "OPEN" : "CLOSED", instr.req_reset,
               instr.cond_sel, instr.cond_inv, ProgMem[i]);
    }
}

uint8_t GetProgramSize(void)
{
    return ProgramSize;
}

uint8_t GetProgramCounter(void)
{
    return PC;
}

uint16_t GetProgMemAtPC(uint8_t program_counter)
{
    return ProgMem[program_counter];
}
