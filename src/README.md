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

- **Utils/instructionCoders.h**  
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

This structure separates core logic, utilities, public APIs, and tests for clarity and maintainability.

---
---

# Elevator Control Program Overview

## Instruction Table (ProgMem - 'Firmware')

| PC  | Jump To | Condition               | Invert | Move Up | Move Down | Door     | Reset | Purpose                                           |
|-----|---------|--------------------------|--------|----------|-------------|----------|--------|---------------------------------------------------|
| 0   | 2       | CALL_PENDING_ANY         | false  | ❌       | ❌          | OPEN     | ❌     | Check for any pending calls                       |
| 1   | 0       | FIXED_ZERO               | true   | ❌       | ❌          | OPEN     | ❌     | Idle loop                                         |
| 2   | 3       | FIXED_ZERO               | false  | ❌       | ❌          | CLOSED   | ❌     | Start door closing                                |
| 3   | 3       | DOOR_CLOSED              | true   | ❌       | ❌          | CLOSED   | ❌     | (SC) Wait until door is fully closed                   |
| 4   | 8       | CALL_PENDING_BELOW       | false  | ❌       | ❌          | CLOSED   | ❌     | If call is below → move down                      |
| 5   | 11      | CALL_PENDING_ABOVE       | false  | ❌       | ❌          | CLOSED   | ❌     | If call is above → move up                        |
| 6   | 13      | CALL_PENDING_SAME        | false  | ❌       | ❌          | CLOSED   | ❌     | If on same floor → handle call immediately        |
| 7   | 4       | FIXED_ZERO               | true   | ❌       | ❌          | CLOSED   | ❌     | (SC) Loop back if no call matched                      |
| 8   | 9       | FIXED_ZERO               | false  | ❌       | ✅          | CLOSED   | ❌     | Start moving down                                 |
| 9   | 9       | CALL_PENDING_SAME        | true   | ❌       | ✅          | CLOSED   | ❌     | (SC) Wait until call at current floor is detected      |
| 10  | 14      | FIXED_ZERO               | true   | ❌       | ❌          | CLOSED   | ❌     | Arrived → stop movement, open door next           |
| 11  | 12      | FIXED_ZERO               | false  | ✅       | ❌          | CLOSED   | ❌     | Start moving up                                   |
| 12  | 12      | CALL_PENDING_SAME        | true   | ✅       | ❌          | CLOSED   | ❌     | (SC) Wait until call at current floor is detected      |
| 13  | 14      | FIXED_ZERO               | true   | ❌       | ❌          | CLOSED   | ❌     | Stop movement, open door next                     |
| 14  | 15      | FIXED_ZERO               | false  | ❌       | ❌          | OPEN     | ❌     | Open the door                                     |
| 15  | 15      | DOOR_OPEN                | true   | ❌       | ❌          | OPEN     | ❌     | (SC) Wait until door is fully open                     |
| 16  | 0       | FIXED_ZERO               | true   | ❌       | ❌          | OPEN     | ✅     | (SC) Clear the call and return to idle                 |

##### Note: (SC) means safety critical instructions
---

## 📋 Program Behavior Summary

This elevator control logic follows a simple state-machine-based loop to handle pending calls:

1. **Idle Monitoring (PC 0–1):**
   - Constantly checks if there's a pending call. If not, stays in an idle loop.

2. **Call Handling Start (PC 2–3):**
   - Closes the door if a call is received.
   - Waits for confirmation that the door is closed.

3. **Call Direction Decision (PC 4–7):**
   - Determines whether to go up, down, or service a call on the current floor.
   - Loops back if no direction is valid yet.

4. **Movement State (PC 8–13):**
   - Initiates up/down movement and waits at each floor for a matching call.
   - Once the target is reached, stops moving and proceeds to open the door.

5. **Door Opening and Servicing (PC 14–16):**
   - Opens the door and waits until it's fully open.
   - Resets the call flags and goes back to idle.

---

## 🛠️ What This Program Is Useful For

- **Simulated elevator control** in a test or embedded system environment.
- Servicing **one call at a time**, with door management.
- Useful as a **baseline FSM design** for elevator logic before layering in more complex features like:
  - Call queueing,
  - Direction prioritization,
  - Timeout handling,
  - Emergency stop conditions.

---
---

# ElevatorControllerEmulator Coding Guidelines

This section outlines the coding standards and best practices for contributing to the ElevatorControllerEmulator project. These guidelines are based on the current codebase in the `src/` folder and the project's safety considerations.

---

## 1. **File and Directory Structure**

- Organize code by functionality:
  - **ElevatorController/**: Core logic (state machine, condition selector)
  - **PublicAPI/**: Public-facing headers and data structures (given)
  - **TestAndControl/**: Test framework and validation logic
  - **Utils/**: Utility headers (assertions, instruction encoding/decoding)
  - **src/**: Entry point (`main.c`), common headers, and documentation

---

## 2. **Naming Conventions**

- **Files**: Use `pascalCase` where applicable.
- **Types**: Use `CamelCase` with `_t` or `_e` suffix for structs and enums.
- **Functions**: Use `CamelCase` for global and `pascalCase` to static function definitions.
- **Varriables**: Use `CamelCase` for global and static and `snake_case` to local variable definitions.
- **Constants/Macros**: Use all uppercase with underscores (e.g., `PROG_MEM_SIZE`, `ENABLE_LOGGING`).

---

## 3. **Header Files**

- Use `#pragma once` include guard at the top of every header file.
- Include only what is necessary.
- Provide clear documentation for each function and data structure.

---

## 4. **Function and Variable Declarations**

- **Function Prototypes**: Declare all external functions in headers.
- **Static Functions**: Use `static` for internal linkage where possible.
- **Variable Scope**: Minimize global variables; prefer function scope or `static` file scope.

---

## 5. **Code Formatting**

- **Indentation**: Use 4 spaces per indentation level.
- **Braces**: Place opening braces on the same line as the statement.
- **Spacing**: Use spaces around operators and after commas.
- **Line Length**: Limit lines to 100 characters where possible.
- **Comments**: Use Doxygen-style comments for functions and modules.

---

## 6. **Safety and Robustness**

- **Assertions**: Use `CUSTOM_ASSERT` and `ASSERT_ERROR` macros for critical checks.
- **Input Validation**: Always validate user and API inputs (see `main.c` for input range checks).
- **Error Handling**: Fail fast on unrecoverable errors; provide clear error messages.
- **Safety-Critical Logic**: Clearly mark safety-critical sections (see comments in `sequentialNetwork.c`).
- **Testing**: Testing is done with Unit and Characteristic Testing in mind. It testes use cases along with program overflow etc. for embedded programming mindset.

---

## 7. **Testing**

- Place all test and validation code in `TestAndControl/`.
- Register and run tests using the provided test framework (`testRunner.c`).
- Use assertions to verify expected behavior and document test intent.

---

## 8. **Documentation**

- Document all modules, functions, and data structures.
- Maintain up-to-date README files in each directory for structure and usage.
- Use tables and diagrams where helpful (see `src/README.md` for instruction table).

---

## 9. **General Best Practices**

- Prefer explicit over implicit behavior.
- Avoid magic numbers; use named constants.
- Keep functions short and focused on a single task.
- Refactor duplicated code into reusable functions or macros.

---
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

---
---

## AI usage disclaimer
Since the assignment allowed the usage of AI, for transparency reasons here are the tasks it was used for (ChatGPT-4.1 and GitHub Copilot):
* Helping with the concept of the tasks
* Code review and code check for coding mistakes
* Documentation structure and safety concern suggestions based on prompts

Project framework, general coding design, manual testing and checking of AI decisions were done solely by the author
