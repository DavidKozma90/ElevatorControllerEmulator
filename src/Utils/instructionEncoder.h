#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "seqnet.h"

static inline uint16_t EncodeInstruction(const SeqNet_Out* instruction)
{
    uint16_t encoded = 0;

    encoded |= ((uint16_t)(instruction->jump_addr)      & 0x00FFU); // bits 0–7
    encoded |= ((uint16_t)(instruction->req_move_up)    << 8);      // bit 8
    encoded |= ((uint16_t)(instruction->req_move_down)  << 9);      // bit 9
    encoded |= ((uint16_t)(instruction->req_door_state) << 10);     // bit 10
    encoded |= ((uint16_t)(instruction->req_reset)      << 11);     // bit 11
    encoded |= ((uint16_t)(instruction->cond_sel)       << 12);     // bits 12–14
    encoded |= ((uint16_t)(instruction->cond_inv)       << 15);     // bit 15

    return encoded;
}
