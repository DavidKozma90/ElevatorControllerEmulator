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

    encoded |= ((uint16_t)(instruction->jump_addr)      & JUMP_ADDR_MASK);
    encoded |= ((uint16_t)(instruction->req_move_up)    << 8);
    encoded |= ((uint16_t)(instruction->req_move_down)  << 9);
    encoded |= ((uint16_t)(instruction->req_door_state) << 10);
    encoded |= ((uint16_t)(instruction->req_reset)      << 11);
    encoded |= ((uint16_t)(instruction->cond_sel)       << 12);
    encoded |= ((uint16_t)(instruction->cond_inv)       << 15);

    return encoded;
}
