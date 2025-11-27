# Mitsubishi RV-2SDB Robot Simulation

A 3D simulation of the Mitsubishi MELFA RV-2SDB industrial robot using **Raylib**, **ImGui**, and **C++**.

## Features

* **Kinematics:** Procedural geometric visualization of the 6-axis robot arm.
* **Controls:** Manual joint control via slider GUI.
* **Demo Mode:** Automated "Pick and Place" demonstration sequence.
* **Networking:** TCP Server (Port 10001) for external control (Python/MATLAB).
* **Visuals:** Work envelope visualization and adjustable scale.

## Prerequisites

* CMake (3.16+)
* C++ Compiler (GCC/Clang/MSVC)
* Internet connection (CMake fetches dependencies automatically: Raylib, Jolt, ImGui)

## Build Instructions

1.  **Clone or create the project folder.**
2.  **Create a build directory:**
    ```bash
    mkdir build
    cd build
    ```
3.  **Configure with CMake:**
    ```bash
    cmake ..
    ```
4.  **Compile:**
    ```bash
    make
    ```
5.  **Run:**
    ```bash
    ./Yuki
    ```

## Controls

* **Camera Movement:**
    * `W`, `A`, `S`, `D`: Move Camera
    * `Mouse`: Look around
    * `Shift`: Move faster
* **UI Interaction:**
    * `TAB`: Toggle Mouse Cursor (Unlock cursor to interact with UI buttons)
* **External Control (Optional):**
    * Connect via TCP to port `10001`
    * Commands: `GETPOS`, `MOVE <J1> <J2> ...`