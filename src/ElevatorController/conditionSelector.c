#include "commonHeader.h"
#include "PublicAPI/condsel.h"
#include "Utils/customAssert.h"

/** Calculates the result of the condition selector based on the parameters.
 * @param[in] invert  Return value is inverted.
 * @param[in] index   Index of the value to select (@see documentation for details).
 * @param[in] values  External input values to select.
 * @return Resturns with the selected value or the negated value of it.
 */  
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
            ASSERT_ERROR("ERROR: Reserved condition selector index used");
            break;
        case CONDSEL_FIXED_ZERO:
            /* Always returns false before inversion (allows unconditional jump via invert=true) */
            condition = false;
            break;
        default:
            ASSERT_ERROR("ERROR: Invalid condition selector index");
            break;
    }

    condition = invert ? !condition : condition;

    return condition;
}
