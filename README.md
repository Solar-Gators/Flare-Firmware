TODO: put videos that explain workflow
TODO: note about needing to do make clean after messing with ioc

# Solar Gators Flare Firmware

- **CMake** + **Ninja** build system
- Shared **resources** (HAL, CMSIS, FreeRTOS, toolchains, Drivers)
- **pre-commit** hook enforcing **clang-format**
- **Unified** intellisense for all firmware subfolders using root cmakelists.txt

---

## 🧩 CLion Setup (You can install arm-none-eabi with a package manager as well)

You can also build and debug projects directly in **JetBrains CLion** using the official **ARM GNU Toolchain** from Arm Developer.

1. **Install CLion and ARM GNU Toolchain**
- **Download CLion**:  
  [https://www.jetbrains.com/clion/](https://www.jetbrains.com/clion/)
- **Download ARM GNU Toolchain (non-EABI)**:  
  [https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)
- During installing, a popup will ask to add to PATH, make sure to check that box.

2. **Open the Project in CLion**
1. Launch CLion and open the **Flare-Firmware** repository root.
2. Go to **File → Settings → Build, Execution, Deployment → CMake**.
3. Enable the existing **Debug** and **Release** build presets that are already configured in the repository.
4. Click **Apply** and then reload your cmake (this should be a button somewhere) to configure the environment.

Once configured, you can select any project inside the `firmware/` subfolders as the active CMake target and build it using the top-right build dropdown in CLion.

## 🚀 Quick Start (Windows) (Can be done on linux type OS too using whatever package manager you have and regular terminal)

1. **Install MSYS2**  
   Download: https://www.msys2.org/  
   Open an `MSYS2 Mingw64` terminal (Start menu).
   All the following commands need to be ran in an MSYS2 Mingw64 (Unix like) terminal

2. **Install git in MSYS MINGW64 terminal**
    ```sh
    pacman -S git
    ```

3. **Clone this repo in the MSYS MINGW64 terminal**
    ```sh
    git clone https://github.com/Solar-Gators/Flare-Firmware.git
    cd Flare-Firmware
    ```

4. **Install build tools in MINGW64 terminal**
    ```sh
    pacman -Syu         # Update system, restart if prompted
    pacman -S --needed \
      mingw-w64-x86_64-python-pre-commit \
      mingw-w64-x86_64-arm-none-eabi-gcc \
      mingw-w64-x86_64-cmake \
      ninja \
      git \
      make \
      clang
    ```
5. **.vscode/settings.json for windows**
    - If on Windows and did the MSYS2 setup, make a path to a file called .vscode/settings.json in the repo root here, and put the below inside of it
    ```
    {
        // Point CMake to the cross-compilers
        "cmake.configureSettings": {
            "CMAKE_C_COMPILER": "C:/msys64/mingw64/bin/arm-none-eabi-gcc.exe",
            "CMAKE_CXX_COMPILER": "C:/msys64/mingw64/bin/arm-none-eabi-g++.exe"
        },

        // Tell VS Code where to find CMake and Ninja (MSYS2 installs)
        "cmake.cmakePath": "C:/msys64/mingw64/bin/cmake.exe",
        "cmake.environment": {
            "PATH": "C:/msys64/mingw64/bin;${env:PATH}"
        },
    }
    ```

6. **Install pre-commit hook in MINGW64 terminal**
    ```sh
    pre-commit install
    ```

7. **Build/Work on a firmware project. The make command works in MINGW64 terminal**
    ```sh
    git checkout -b your-development-branch # Do work on your own dev branch, name it with ur name or feature ur adding
    cd firmware/CAN-DevBoard # cd into the specific project you wanna work on
    make              # Debug build (default)
    make clean        # Clean build folder
    make BUILD_TYPE=Release  # Release build
    ```
    Or if in vscode, install cmake tools and C/C++ extensions, and click build button on the bottom tool bar
    
8. **Import project into CubeIDE so that it can easily be flashed and debugged**
    - Open STM32CubeIDE → File → New → Makefile project with existing code
    - Project Directory: select firmware/YourProject
    - Project Name: should autofill but it would be YourProject
    - Toolchain: choose MCU ARM GCC
    - Languages: Both C and C++ (Should auto fill)
    - CubeIDE will index the project and should build immediately using your Makefile/Ninja/CMake setup (via Project → Build or the hammer icon).

9. **Run and Debug project**
    - Create a Debug Configuration by clicking drop down next to debug button of type STM32 C/C++ Application (Set the ELF path to build/YourProject.elf).
    - You can select the correct one when going to run by adding them to favorites and selecting them with the drop down.
    - Flash and start a debug or flash session.
    - SOMETHING TO NOTE is that its totally possible to flash and debug inside of clion, we just need to figure out how to do so.

10. **SSH key or github desktop**
    - You will likely need to create an ssh key with github or use github desktop in order to push code to this repo
    - Follow the linux instructions here if doing it inside MSYS2 MINGW64 terminal, or windows if doing it outside: https://docs.github.com/en/authentication/connecting-to-github-with-ssh/generating-a-new-ssh-key-and-adding-it-to-the-ssh-agent?platform=linux
## Other Notes

- **For working Intellisense in Vscode**
    Use C/C++ extension for intellisense with CMake Tools as its source.

---

- **From firmware subfolder** (`cd firmware/project1`):

    ```sh
    make                 # Build debug
    make clean           # Remove build
    make BUILD_TYPE=Release   # Build release
    ```
---

- **After modifying ioc file**
    You may need to do `make clean` in the terminal before rebuilding.

---

- **Clang-Format**:  
    Every `git commit`, clang-format is auto-applied to C/C++ files (`.c`, `.cpp`, `.h`, `.hpp`).  
    If changes are made, the commit is **blocked** and you must re-add the formatted files.

    ```
    git add .
    git commit -m "Some message"
    # If "files were modified by this hook", re-add and commit again.
    ```

- **Format all files manually**:
    ```sh
    pre-commit run --all-files
    ```

- **Style config**:  
    See `.clang-format` in the repo root

---

### Individual Project Structure (a project inside of the firmware folder)
```sh
project-folder/
├── build/             # Output folder for build artifacts (e.g., .elf, .hex, .bin files). Does not get pushed to repo
├── Core/              # Main application source code (auto-generated by STM32CubeMX). int main() lives in here, try to write limited code in this folder.
├── CubeMX/            # CubeMX configuration files. Folder can basically be ignored and is just for some build stuff
├── user/              # User-defined source code. Most of our code should live within this folder. 
├── .mxproject         # STM32CubeMX project configuration file
├── CMakeLists.txt     # CMake build script for project compilation
├── Makefile           # Makefile for building project easily (calls cmake).
└── project1.ioc       # STM32CubeMX configuration file describing MCU setup
```
---

### 🧠 VS Code + IntelliSense:
- Open root of repo or open specific subproject
- Point vscode/cmaketools extension to cmakelists.txt in root of whatever you opened

---

### Creating a New Project Steps:
- Open CubeMX, and configure your MCU/board/peripherals as usual.
- Project Manager (before code generation)
- Project Location: set to your repo’s firmware/ folder.
- Toolchain/IDE: select Makefile (We’ll use our own build system; this prevents IDE-specific files.)
- Toolchain Folder Location: add another /CubeMX to the path that is in the box already (This makes CubeMX put startup files/linker script and toolchain bits under firmware/<Project>/CubeMX/.) 
- Generate code
- CubeMX creates Core/, drivers, and the CubeMX/ subfolder inside firmware/<YourProject>/.
- Copy your template Makefile and CMakeLists.txt from another project (use CAN-DevBoard project) into firmware/<YourProject>/.
- Edit the top portion of the CMakeLists.txt with information specific to your project's mcu.
- You should now be able to type `make` in the terminal to build the project.
- You can also add a user/ folder with inc/ and src/ inside of it, to separate our code from the generated code in Core/

---

## 🖥️ Recommended VS Code Settings

There is a way to add the MSYS2 Mingw64 terminal in vscode so you don't have to switch windows to use the terminal.
