#include "PublicAPI/condsel.h"
#include "Utils/customAssert.h"

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

bool CondSel_calc(const bool invert, const uint8_t index, const CondSel_In values)
{
    bool condition = false;

    switch ((const CondSelIndex_e)index)
    {
        case CONDSEL_CALL_PENDING_ANY:
            condition = values.call_pending_below || values.call_pending_same || values.call_pending_above;
            break;
        case CONDSEL_CALL_PENDING_BELOW:
            condition = values.call_pending_below;
            break;
        case CONDSEL_CALL_PENDING_SAME:
            condition = values.call_pending_same;
            break;
        case CONDSEL_CALL_PENDING_ABOVE:
            condition = values.call_pending_above;
            break;
        case CONDSEL_DOOR_CLOSED:
            condition = values.door_closed;
            break;
        case CONDSEL_DOOR_OPEN:
            condition = values.door_open;
            break;
        case CONDSEL_RESERVED:
            ASSERT_FATAL_ERROR("ERROR: Reserved condition selector index used");
            break;
        case CONDSEL_FIXED_ZERO:
            condition = false;
            break;
        default:
            ASSERT_FATAL_ERROR("ERROR: Invalid condition selector index");
            break;
    }

    condition = invert ? !condition : condition;

    return condition;
}
