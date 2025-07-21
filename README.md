# Euclidean Distance Kernel Optimization Project (C and x86-64 Assembly)

## Project Overview

This project implements a high-performance kernel for calculating the Euclidean distance between corresponding points in two 2D vectors. The kernel is developed in two versions: a standard C language implementation and an optimized x86-64 assembly language implementation that leverages scalar Single Instruction, Multiple Data (SIMD) floating-point instructions. The goal is to compare the performance and verify the correctness of the assembly version against the C baseline.

The kernel calculates `Z[i] = sqrt((X2[i] - X1[i])^2 + (Y2[i] - Y1[i])^2)` for each element `i` of the input vectors `X1`, `X2`, `Y1`, `Y2`, storing results in `Z`.

## Group Members

*   Joshua Nathaniel C. Tating
*   Vincent Tabuzo

## How to Build and Run the Project

This project is configured to be built and run using **Visual Studio 2022** on Windows, integrating the NASM assembler via a Custom Build Tool.

### Prerequisites:

1.  **Visual Studio 2022 (Community Edition is free):** Ensure "Desktop development with C++" workload is installed.
2.  **NASM Assembler:**
    *   Download `nasm.exe` from the official NASM website ([nasm.us](https://www.nasm.us/)).
    *   Create a dedicated folder for NASM (e.g., `C:\NASM\`) and place `nasm.exe` inside it.

### Setup Instructions:

1.  **Clone the Repository Directly in Visual Studio 2022:**
    *   Launch Visual Studio 2022.
    *   On the start screen, click **"Clone a repository"**.
    *   In the "Repository location" field, enter: `https://github.com/Aspectzxcc/LBYARCH_MP2.git`
    *   Choose your desired local path where you want to clone the repository.
    *   Click **"Clone"**.
    *   Visual Studio will automatically clone the repository and open the solution.

2.  **Ensure x64 Platform is Selected:**
    *   In the Visual Studio toolbar, make sure "Solution Platforms" is set to **`x64`**.

3.  **Verify/Add Source Files:**
    *   In the "Solution Explorer" pane, expand your project (`LBYARCH_MP2`).
    *   Ensure `main.c`, `kernel.c`, `kernel.h`, and `kernel.asm` are present under "Source Files" or "Header Files". If any are missing, right-click on "LBYARCH_MP2" > `Add` > `Existing Item...` and add them.

4.  **Configure NASM Custom Build Tool for `kernel.asm` (Crucial):**
    *   In "Solution Explorer", click once on `kernel.asm`.
    *   Open the "Properties Window" (`F4`).
    *   Go to `Configuration Properties` > `General`.
    *   Set **"Item Type"** to **`Custom Build Tool`**.
    *   Expand `Custom Build Tool` > `General`. Set the following properties **EXACTLY**:
        *   **Command Line:** `"C:\PATH_TO_YOUR_NASM_FOLDER\nasm.exe -f win64 kernel.asm"`
            *   *(**IMPORTANT:** Replace `C:\PATH_TO_YOUR_NASM_FOLDER\nasm.exe` with the actual path where you placed your `nasm.exe` if it's different. Ensure the path is in quotes if it contains spaces.)*
        *   **Outputs:** `kernel.obj`

5.  **Build the Project:**
    *   Go to `Build` > `Clean Solution`.
    *   Go to `Build` > `Rebuild Solution` (`F7`).
    *   Check the "Output" window for any errors. It should show "Build: 1 succeeded".

6.  **Run the Program:**
    *   Go to `Debug` > `Start Without Debugging` (`Ctrl + F5`).
    *   A console window will appear displaying the performance test results.

## Deliverables

This GitHub repository contains the complete Visual Studio 2022 project folder, including:

*   Source code files (`.c`, `.h`, `.asm`)
*   Visual Studio solution (`.sln`) and project (`.vcxproj`) files

Additionally, per project requirements, the following are also provided in the outputs folder:

*   Comparative execution time and short analysis of the performance of the kernels (`analysis.md`)
*   Screenshot displaying program output (`output_image.jpg`)
*   Video demonstrating C source code, compilation, and execution. (`output_video.mp4 / output_video_yt_link.txt`)
*   Video demonstrating x86-64 assembly source code, compilation, and execution.(`output_video.mp4 / output_video_yt_link.txt`)