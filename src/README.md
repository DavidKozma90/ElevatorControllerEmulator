# ElevatorControllerEmulator Project Structure

This project emulates an elevator controller, including its logic, test framework, and supporting utilities. Below is a summary of the main directories and files, with a brief description of each.

## Key Source Files

### Top-Level

- **main.c**  
  Entry point for the emulator. Runs validation tests and simple call simulations.

- **commonHeader.h**  
  Common definitions, macros, enums, and external variable declarations used throughout the project.

---

### Elevator Controller Logic

- **ElevatorController/conditionSelector.c**  
  Implements the condition selector logic, mapping condition indices to elevator state inputs.

- **ElevatorController/sequentialNetwork.c**  
  Implements the sequential network (state machine) that interprets program memory and controls elevator actions. Handles program memory, program counter, and instruction execution.

---

### Utilities

- **Utils/instructionEncoder.h**  
  Provides functions to encode and decode elevator instructions to/from 16-bit values.

- **Utils/customAssert.h**  
  Custom assertion macros for error handling and debugging.

---

### Public API

- **PublicAPI/seqnet.h**  
  Defines the `SeqNet_Out` structure and API for the sequential network module.

- **PublicAPI/condsel.h**  
  Defines the `CondSel_In` structure and API for the condition selector module.

---

### Test and Validation

- **TestAndControl/testRunner.c**  
  Contains the test framework and a suite of validation tests for elevator behavior (movement, door logic, call handling, etc.).

---

### Build and Configuration

- **build/premake5.lua**  
  Build configuration script for generating project files and managing build settings.

- **.vscode/settings.json**  
  VS Code workspace settings for file associations and exclusions.

---

## Additional Files

- **README.md**  
  Project overview, build instructions, and safety considerations.

---

This structure separates core logic, utilities, public APIs, and tests for clarity and maintainability.

---

# Safety Considerations

This section outlines key safety concerns related to the elevator controller design and identifies how each is handled or mitigated in the current firmware. Where appropriate, recommended improvements or external system responsibilities are also noted.

---

## 1. Shaft Endpoint Protection

- **Concern:** Moving down from floor 0 or up beyond the top floor.
- **Current Handling:** ❌ Not enforceable in firmware due to API limitations.
- **Mitigation:** Must be handled via hardware (limit switches).
- **Recommendation:** Extend `CondSel_In` to include `at_floor0`, `at_top_floor` for safe movement gating.

---

## 2. Door Interlock Enforcement

- **Concern:** Moving while door is open or unlocked.
- **Current Handling:** ✅ Movement commands only issued if `door_closed == true`.
- **Mitigation:** Built into instruction sequence logic.
- **Recommendation:** Ensure mechanical door lock relay also wired in real system.

---

## 3. Door Obstruction / Reopen

- **Concern:** Door closes while someone is in the doorway or a new request arrives.
- **Current Handling:** ⚠️ Reopen support added via check for `call_pending_same` during door-close step.
- **Limitation:** No physical obstruction sensor present in the API.
- **Recommendation:** Add optional obstruction detection input or use door-close timeout.

---

## 4. Emergency Stop / Fault Recovery

- **Concern:** Hardware failure, watchdog timeout, or system fault.
- **Current Handling:** ❌ Not handled in current logic.
- **Recommendation:** Extend API with fault output, implement watchdog or safe-mode behavior.

---

## 5. Output Clamping and Exclusivity

- **Concern:** `move_up` and `move_down` outputs simultaneously active.
- **Current Handling:** ✅ Firmware ensures only one motion bit is ever set per instruction.
- **Recommendation:** Add hardware relay interlock for redundancy.

---

## 6. Power Loss and Reboot Safety

- **Concern:** Elevator resumes motion on reboot or stops mid-shaft.
- **Current Handling:** ✅ On startup, system resets to idle and waits for a call.
- **Recommendation:** Ensure motor is off at startup and position detection is reliable.

---

## 7. Multiple Simultaneous Calls

- **Concern:** Multiple requests issued; controller must choose direction.
- **Current Handling:** ✅ Firmware prioritizes: below → above → same floor.
- **Limitation:** No direction memory; controller may reverse mid-path.
- **Recommendation:** Add direction memory for smarter scheduling if needed.

---

## 8. API Safety Limitations

- **Concern:** Controller cannot reason about physical limits or position.
- **Current Handling:** ⚠️ Missing inputs (e.g. current floor, top/bottom indicators).
- **Recommendation:** Extend API with discrete position data or one-hot floor inputs.

---

## Summary Table

| Safety Concern         | Handled? | Notes / Recommendations                            |
|------------------------|----------|----------------------------------------------------|
| Shaft end movement     | ❌        | Needs position input or hardware interlock         |
| Door interlock         | ✅        | Software logic enforced                            |
| Door re-open           | ⚠️        | Covered via call repeat; no obstruction detection  |
| Emergency stop         | ❌        | Watchdog or manual stop not implemented            |
| Output exclusivity     | ✅        | Handled in firmware; redundant relay suggested     |
| Power-on reset         | ✅        | Defaults to safe idle state                        |
| Direction prioritization| ✅       | Simple logic, no movement memory                   |
| API safety gaps        | ⚠️        | Extension required for full safety coverage        |


