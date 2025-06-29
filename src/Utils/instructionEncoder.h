#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "commonHeader.h"
#include "PublicAPI/seqnet.h"

/* Helper method to encode a SeqNet_Out instruction into a 16-bit value.
 * The encoding is done as follows:
 * - Bits 0–7: jump address
 * - Bit 8: request to move the elevator upwards
 * - Bit 9: request to move the elevator downwards
 * - Bit 10: target door state (0: closed, 1: open)
 * - Bit 11: request to clear the pending bit from active call memory for the current floor
 * - Bits 12–14: condition select index
 * - Bit 15: activates the inversion of the value of the selected condition value
 */
static inline uint16_t EncodeInstruction(const SeqNet_Out* instruction)
{
    uint16_t encoded = 0;

    encoded |= ((uint16_t)(instruction->jump_addr)       & JUMP_ADDR_MASK);
    encoded |= ((uint16_t)(instruction->req_move_up & 0x01)    << REQ_MOVE_UP_SHIFT);
    encoded |= ((uint16_t)(instruction->req_move_down & 0x01)  << REQ_MOVE_DOWN_SHIFT);
    encoded |= ((uint16_t)(instruction->req_door_state & 0x01) << REQ_DOOR_STATE_SHIFT);
    encoded |= ((uint16_t)(instruction->req_reset & 0x01)      << REQ_CALL_RESET_SHIFT);
    encoded |= ((uint16_t)(instruction->cond_sel & 0x07)       << COND_SELECT_SHIFT);
    encoded |= ((uint16_t)(instruction->cond_inv & 0x01)       << COND_INVERT_SHIFT);

    return encoded;
}

/* Helper method to decode a 16-bit encoded instruction into a SeqNet_Out structure. */
static inline SeqNet_Out DecodeInstruction(const uint16_t encoded)
{
    SeqNet_Out instruction = {0};

    instruction.jump_addr      = (uint8_t)(encoded & JUMP_ADDR_MASK);                          /* Bits 0-7 */
    instruction.req_move_up    = ((encoded & REQ_MOVE_UP_MASK) != 0U) ? true : false;          /* Bit 8 */
    instruction.req_move_down  = ((encoded & REQ_MOVE_DOWN_MASK) != 0U) ? true : false;        /* Bit 9 */
    instruction.req_door_state = ((encoded & REQ_DOOR_STATE_MASK) != 0U) ? true : false;       /* Bit 10 */
    instruction.req_reset      = ((encoded & REQ_CALL_RESET_MASK) != 0U) ? true : false;       /* Bit 11 */
    instruction.cond_sel       = (uint8_t)((encoded & COND_SELECT_MASK) >> COND_SELECT_SHIFT); /* Bits 12-14 */
    instruction.cond_inv       = ((encoded & COND_INVERT_MASK) != 0U) ? true : false;          /* Bit 15 */

    return instruction;
}
