#include "PublicAPI/seqnet.h"
#include <stdio.h>

#define PROG_MEM_SIZE 256U

uint16_t ProgMem[PROG_MEM_SIZE];
uint8_t  PC;

const uint16_t JUMP_ADDR_MASK      = 0x00FFU;
const uint16_t REQ_MOVE_UP_MASK    = 0x0100U;
const uint16_t REQ_MOVE_DOWN_MASK  = 0x0200U;
const uint16_t REQ_DOOR_STATE_MASK = 0x0400U;
const uint16_t REQ_CALL_RESET_MASK = 0x0800U;
const uint16_t COND_SELECT_MASK    = 0x7000U;
const uint16_t COND_INVERT_MASK    = 0x8000U;

const uint16_t COND_SELECT_SHIFT = 12U;

void SeqNet_init(void)
{
    /* Set all instructions to zero */
    for (uint16_t i = 0; i < PROG_MEM_SIZE; i++)
    {
        ProgMem[i] = 0x00;
    }
    
    /* Program counter reset */
    PC = 0x00;
}

SeqNet_Out SeqNet_loop(const bool condition_active)
{
    SeqNet_Out output = {0};
    uint16_t instruction = ProgMem[PC];

    output.jump_addr      = (uint8_t)(instruction & JUMP_ADDR_MASK);                          /* Bits 0-7 */
    output.req_move_up    = ((instruction & REQ_MOVE_UP_MASK) != 0U) ? true : false;          /* Bit 8 */
    output.req_move_down  = ((instruction & REQ_MOVE_DOWN_MASK) != 0U) ? true : false;        /* Bit 9 */
    output.req_door_state = ((instruction & REQ_DOOR_STATE_MASK) != 0U) ? true : false;       /* Bit 10 */
    output.req_reset      = ((instruction & REQ_CALL_RESET_MASK) != 0U) ? true : false;       /* Bit 11 */
    output.cond_sel       = (uint8_t)((instruction & COND_SELECT_MASK) >> COND_SELECT_SHIFT); /* Bits 12-14 */
    output.cond_inv       = ((instruction & COND_INVERT_MASK) != 0U) ? true : false;          /* Bit 15 */

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
