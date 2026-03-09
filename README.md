# GDQ 2026 HeartGold & SoulSilver Arbitrary Code Execution Demo

This repository contains the source code, build environment and installation details for the Arbitrary Code Execution (ACE) demo showcased live at GDQ 2026 for Pokémon HeartGold and SoulSilver on the Nintendo DS.

## Overview

The demo exploits a vulnerability in the Wonder Card system to achieve Arbitrary Code Execution. By crafting a specific Wonder Card and manipulating game data, we can execute custom code on the Nintendo DS hardware.

## Technical Details

### Exploit Chain

1. **Wonder Card Exploit**: Obtain a Wonder Card with the 'accessory' type and 'seal' subtype. Invalid seal IDs (either too large or negative due to signed integer handling) bypass bounds checks, allowing arbitrary writes in memory. The arbitrary write is limited to incrementing individual bytes by one (acting as receiving +1 seal). The increment is capped at values less than 100 (the maximum seal count).

2. **NPC Event ID Manipulation**: Increment the higher byte of an NPC's event ID, (+0x0100) causing an out-of-bounds read from the script offset buffer. This causes the offset to be read from leftover pokemon party data allocated during a battle, which can be controlled by the player through RNG manipulation or ingame trades.

3. **Bootstrap Pokémon**: Control the battle data through a specially crafted Pokémon, jumping to the data field of the original Wonder Card exploit. Note that during this the execution context is script execution, not code execution.

4. **Payload Unpacking**: The initial 100-byte payload in the Wonder Card transfers the script execution to code execution. This is done by using arbtrary write commands to hack/create a script command that executes custom code from the remainder of the Wonder Card. This command then unpacks a larger 73188 bytes payload stored in box data. Box data is normally encrypted, but we bypass this by setting the 'encryption' flag on each Pokémon, we can copy all other data of each pokémon to puzzle together a
binary stored in unused memory. This binary is in fact a number of seperate functions and assets, which is defined
through the [/project/packager.yaml](./project/packager.yaml) config.

5. **Soft Reset Hook**: It then continues to hack the OSi_DoResetSystem function in ITCM. This function is responsible for the soft reset functionality. It first copies ROM to RAM again, and then boots into main. This would ordinarily cause
code execution to no longer persist, since any code you stored would be overwritten during this.

    This is why we opt to put a hook in OSi_DoResetSystem that jumps to unused memory. In here we then manage the function calls that would be executed by OSi_DoResetSystem itself. This allows the following chain:
    1. Copy ROM to RAM and reinitalize. This does **not** touch unused memory.
    2. Stack pointer and payload remain in unused memory
    3. Set a new hook in the MIi_UncompressBackwards function, which runs at boot to **again** copy ROM to RAM
    4. Boot into main
    5. Once MIi_UncompressBackwards runs, install several custom hook, including in CARDi_ReadCArd
    6. CARDi_ReadCard runs on any overlay load, and installs hook if the loaded overlay matches a target

6. **Communication Error**: To trigger a soft reset unconspicously, return code execution to script execution by simply
returning, as all code execution was in essence a custom script command. Since all the above happened instantaniously,
we can now run a script to open up a shop, and follow it by a another command to run the communication error screen. This calls OSi_DoResetSystem if the player presses 'A' to resume gameplay.

After all this, the game reboots with full controll over any functions.

## Repository Features

This repository provides a build environment for ARMv5te targets (Nintendo DS architecture) using a docker container.
It contains all code required to build the entire demo, along with a simple one-click install script.

Here's a quick list of some utilities provided by the framework:
- **Hooking system**: Put a hook on any function, overlay or not.
- **Payloads**: Compile binaries or assets, up to 78K bytes in size.
- **Converters**: Tools to convert images and text to in-game formats.
- **Packager**: Script that combines binaries according to [/project/packager.yaml](./project/packager.yaml), automatically handling payload allocation according to the linker, and calls converts on the fly.It then converts it to data that can
be installed ingame as pokemon box data, to be unpacked by the initial payload.
- **Installer**: Lua script compatible with BizHawk's melonDS emulation core for one-click installation of the exploit chain, including the Wonder Card and bootstrap Pokémon. It installs both the packed box pokemon data as well as the unpacked variant to exactly where the initial payload would unpack in unused memory, allowing debugging on the fly.

## Prerequisites

- Docker
- Git (for cloning the repository)

## Build Instructions

1. Clone the repository:
   ```bash
   git clone <repository-url>
   cd GDQ2026-ACE-DEMO-HGSS
   ```

2. Start the Docker environment:
   ```bash
   ./run_docker.sh -b
   ./run_docker.sh -x
   ```

3. Run the build script:
   ```bash
   cd project
   ./build.sh
   ```

   The build script accepts an optional game identifier (defaults to `POKEMON_SS_IPGE_01`):
   ```bash
   ./build.sh [GAME_ID]
   ```

   Available game IDs correspond to symbol files in [/project/tools/symbols/](./project/tools/symbols/).
   Currently only English SoulSilver has been tested. English HeartGold may work, but not tested.
   Other versions can be added by providing a matching symbol map, and specifying it as target in the build script.

## Installation

After building, use the Lua installer script in an BizHawk with the melonDS core with a running, matching game revision.
The script automatically installs:
- Bootstrap Pokémon
- Exploit Wonder Card
- Packed binary data in box storage
  Unpacked binary data in unused memory (for debugging, lost on save reset)

This enables one-click setup of the ACE demo in the emulator.

# Running the demo

Have a surf or fishing encounter on the left side of CherryGrove City.
Immediately enter the PokeMarket. Refrain from opening any menus, talking to any NPC, or leaving the area.
You should be met with the Deliveryman. Talk to him. You obtain the ' ' gift.
Talk to the bottom shop employee.
Close the shop window.

If everything went well, you'll be met with a communication error, and can press A to continue.
From here, simply follow along the story as done in the showcase of the demo!

NOTE:
At the ruins of Alphs the cutscene in front of the house won't trigger unless you are NOT on a bike!

## Customization

**To add a custom hook/payload**:
- Add a hook in [project/src/hooks](./project/src/hooks/)
- Add the code the hook shoud execute in [project/src/payloads](./project/src/payloads/)
- Use SET_HOOK(DIRECTORY_NAME_OF_HOOK) in [/project/src/payloads/MIi_UncompressBackward/main.c](./project/src/payloads/MIi_UncompressBackward/main.c) A preprocessor step creates the hook by grabbing the address specified in the linker and assembled data of the hook.
- Adjust packaging configuration in [/project/packager.yaml](./project/packager.yaml) to add the payload.

**To add scripts or events**:
- Scripts and events are compatible with the binary format output by the DSPRE romhacking tool.
- Simply take the binary and add it under [/project/scripts/](./project/scripts/) or [project/eventdata](./project/eventdata/) under a directory named after the corresponding archive id.
- Any script or event added here will automatically be added/overwrite the one in the game
- Prefix with an underscore to prevent it from being compiled along

**To support other versions**:
- Add new symbol files to [/project/tools/symbols/](./project/tools/symbols/) for additional game versions.

## License

See `LICENSE.txt` for details.

