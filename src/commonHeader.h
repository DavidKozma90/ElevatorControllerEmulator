#pragma once

/* STD Library headers are define here */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define PROG_MEM_SIZE 256U

#define DOOR_CLOSED false
#define DOOR_OPEN true 

#define ENABLE_LOGGING 1 /* Set to 0 to disable detailed logging */

#if(ENABLE_LOGGING == 1)
    #define LOG(fmt, ...) \
        printf("[LOG] " fmt, ##__VA_ARGS__)
#else
    #define LOG(fmt, ...)
#endif
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

static const uint16_t JUMP_ADDR_MASK      = 0x00FFU;
static const uint16_t REQ_MOVE_UP_MASK    = 0x0100U;
static const uint16_t REQ_MOVE_DOWN_MASK  = 0x0200U;
static const uint16_t REQ_DOOR_STATE_MASK = 0x0400U;
static const uint16_t REQ_CALL_RESET_MASK = 0x0800U;
static const uint16_t COND_SELECT_MASK    = 0x7000U;
static const uint16_t COND_INVERT_MASK    = 0x8000U;

static const uint16_t REQ_MOVE_UP_SHIFT    = 8U;
static const uint16_t REQ_MOVE_DOWN_SHIFT  = 9U;
static const uint16_t REQ_DOOR_STATE_SHIFT = 10U;
static const uint16_t REQ_CALL_RESET_SHIFT = 11U;
static const uint16_t COND_SELECT_SHIFT    = 12U;
static const uint16_t COND_INVERT_SHIFT    = 15U;

extern uint8_t GetProgramCounter(void);
extern void LoadProgram_Default(void);
extern uint8_t GetProgramSize(void);
extern void RunValidationTests(void);
extern uint16_t GetProgMemAtPC(uint8_t program_counter);
extern void printProgMem(void);
extern void TestSimpleCalls(uint8_t elevator_pos, uint8_t call_floor);
