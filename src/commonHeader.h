#pragma once

#include <stdint.h>
#include <stdbool.h>

#define PROG_MEM_SIZE 256U

typedef enum 
{
    CONDSEL_CALL_PENDING_ANY   = 0,  /* There is an active call below or same or above the elevator current level */
    CONDSEL_CALL_PENDING_BELOW = 1,  /* There is an active call below the elevator current level */
    CONDSEL_CALL_PENDING_SAME  = 2,  /* There is an active call on the same level of the elevator */
    CONDSEL_CALL_PENDING_ABOVE = 3,  /* There is an active call above the elevator current level */
    CONDSEL_DOOR_CLOSED        = 4,  /* Door is closed and locked */
    CONDSEL_DOOR_OPEN          = 5,  /* Door is fully opened */
    CONDSEL_RESERVED           = 6,  /* Reserved / Not in use */
    CONDSEL_FIXED_ZERO         = 7   /* Fixed value of zero (false) */
}CondSelIndex_e;

extern uint16_t ProgMem[];

extern const uint16_t JUMP_ADDR_MASK;
extern const uint16_t REQ_MOVE_UP_MASK;
extern const uint16_t REQ_MOVE_DOWN_MASK;
extern const uint16_t REQ_DOOR_STATE_MASK;
extern const uint16_t REQ_CALL_RESET_MASK;
extern const uint16_t COND_SELECT_MASK;
extern const uint16_t COND_INVERT_MASK;

extern const uint16_t COND_SELECT_SHIFT;