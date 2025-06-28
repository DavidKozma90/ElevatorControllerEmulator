# Raylib-Quickstart
A simple cross platform template for setting up a project with the bleeding edge raylib code.
Works with C or C++.

## Supported Platforms
Quickstart supports the main 3 desktop platforms:
* Windows
* Linux
* MacOS

# Naming projects
Do not name your game project 'raylib', it will conflict with the raylib library.

# VSCode Users (all platforms)
*Note* You must have a compiler toolchain installed in addition to vscode.

* Download the quickstart
* Rename the folder to your game name
* Open the folder in VSCode
* Run the build task ( CTRL+SHIFT+B or F5 )
* You are good to go

# Windows Users
There are two compiler toolchains available for windows, MinGW-W64 (a free compiler using GCC), and Microsoft Visual Studio
## Using MinGW-W64
* Double click the `build-MinGW-W64.bat` file
* CD into the folder in your terminal
* run `make`
* You are good to go

### Note on MinGW-64 versions
Make sure you have a modern version of MinGW-W64 (not mingw).
The best place to get it is from the W64devkit from
https://github.com/skeeto/w64devkit/releases
or the version installed with the raylib installer
#### If you have installed raylib from the installer
Make sure you have added the path

`C:\raylib\w64devkit\bin`

To your path environment variable so that the compiler that came with raylib can be found.

DO NOT INSTALL ANOTHER MinGW-W64 from another source such as msys2, you don't need it.

## Microsoft Visual Studio
* Run `build-VisualStudio2022.bat`
* double click the `.sln` file that is generated
* develop your game
* you are good to go

# Linux Users
* CD into the build folder
* run `./premake5 gmake2`
* CD back to the root
* run `make`
* you are good to go

# MacOS Users
* CD into the build folder
* run `./premake5.osx gmake2`
* CD back to the root
* run `make`
* you are good to go

# Output files
The built code will be in the bin dir

# Working directories and the resources folder
The example uses a utility function from `path_utils.h` that will find the resources dir and set it as the current working directory. This is very useful when starting out. If you wish to manage your own working directory you can simply remove the call to the function and the header.

# Changing to C++
Simply rename `src/main.c` to `src/main.cpp` and re-run the steps above and do a clean build.

# Using your own code
Simply remove `src/main.c` and replace it with your code, and re-run the steps above and do a clean build.

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


