# 2D Car Parking Game — Academic Project Guide

**Institution:** MIT Academy of Engineering, Alandi Pune | Dept. of Computer Engineering  
**Course:** SY B.Tech Semester III | Integrated Project Review  
**Course Outcomes (COs) Covered Across 4 Subjects:**
- **COA (2310221L):** CO3 (Repeated addition square loops & "PARK" string validation), CO4 (Instruction set branching control flow & ALU condition flags)
- **OOP (2310261L):** CO3 (Custom exception handling hierarchy & input validation), CO4 (Decision rules: accessibility slot selection & tiered fees)
- **Programming Lab / DSA (2304214):** CO3 (FIFO waiting queue & LIFO maneuver stack with undo), CO4 (Salted password security & account management), CO5 (Chained hash table with insert, search, and delete player)
- **Computer Graphics (2310218L):** CO3 (2D primitives), CO4 (Viewport alignment & transformations), CO5 (Clipping & orthographic projection), CO6 (Shading, headlights, road texturing, navigation path & pulsing beacons)

---

## 🏛️ Subject Architecture & Implementation Details

### 1. COA: Pure x86-64 NASM Assembly Engine
The Computer Organization & Architecture component is implemented directly in native **x86-64 NASM assembly**:
- **Source File Path:** [`COA_Processor.asm`](file:///c:/Users/Sumit/2D-Car-Parking/COA_Processor.asm)
- **C++ Header Interface:** [`COA_Processor.h`](file:///c:/Users/Sumit/2D-Car-Parking/COA_Processor.h)
- **Assembled via:** `nasm -f win64 COA_Processor.asm -o COA_Processor_asm.o`
- **Key Routines:**
  1. `coa_asm_calculate_score`: Repeated addition multiplication loop for score calculation.
  2. `coa_asm_parking_check`: Euclidean distance squared ($X^2 + Y^2$) computed via successive addition loops, followed by character-by-character string validation of the `"PARK"` command.
  3. Control flow branching using `sub`, `cmp`, `jg`, `je` to determine parking success and set ALU flags (`ZF`, `SF`, `TOL_OK`, `ROT_OK`).

### 2. OOP: Exception Hierarchy & Decision Logic
Implemented in [`PL_DataStructures.h`](file:///c:/Users/Sumit/2D-Car-Parking/PL_DataStructures.h) and [`PL_DataStructures.cpp`](file:///c:/Users/Sumit/2D-Car-Parking/PL_DataStructures.cpp):
- **Custom Exception Hierarchy:** `ParkingException` $\to$ `InvalidInputException`, `SlotConflictException`, `LotFullException`.
- **License Plate Validation:** `validPlate()` checking standard state registration syntax.
- **Decision Algorithms:**
  - `chooseBestSlot()`: Selects accessible slots for large vs. compact vehicles.
  - `calculateParkingFee()`: Calculates tiered hourly fees based on vehicle type and duration.

### 3. DSA / PL: Queue, Stack & Chained Hash Table
- **FIFO Waiting Gate Queue (`CarQueue`):** Manages vehicles queued at the entrance gate.
- **LIFO Maneuver Stack (`ManeuverStack`):** Records player car movements every $0.3\text{s}$. Pressing **`U`** pops and undos the previous maneuver!
- **Dynamic Secure Player Database:**
  - **Chained Hash Table (`PlayerHashTable`):** 16-bucket collision-resolved hash table with $O(1)$ search, insert, and delete.
  - **Salted Password Hashing (`hashPassword`):** Ensures player accounts cannot be accessed or overwritten by other users.
  - **Delete Player:** Allows removing players directly from the database file and UI.

### 4. Computer Graphics: Visual Rendering Pipeline
Implemented in [`CG_Renderer.cpp`](file:///c:/Users/Sumit/2D-Car-Parking/CG_Renderer.cpp) and [`OOPS_ParkingLot.cpp`](file:///c:/Users/Sumit/2D-Car-Parking/OOPS_ParkingLot.cpp):
- **Visual Navigation Guide Path:** Chevrons and dashed waypoints leading directly to the target slot.
- **Target Beacon:** Pulsing green highlight indicating the designated parking space.
- **Headlight Cones:** Smooth blended `GL_TRIANGLES` headlight illumination from the front bumper.
- **Asphalt Surfaces & Lane Dividers:** Realistic road textures with yellow center dashed lines.

### 5. Gameplay, Solid Parked Cars & Audio
- **Solid Parked Car Collisions:** Occupied cars act as solid physical barriers with center-to-center distance checks and pushback.
- **Alert Banner:** Displays `"Car slot is occupied!"` prominently on collision.
- **Fahh Sound Effect:** Windows MCI playback of `fahh.mp3` on collisions and level failures (`-lwinmm`).

---

## 🌐 Instant Web Edition (Shareable Everywhere — 0 Downloads, 0 Blocks!)

The game now features an **Official Cross-Platform Web Edition** located at [`index.html`](file:///c:/Users/Sumit/2D-Car-Parking/index.html) and [`play.html`](file:///c:/Users/Sumit/2D-Car-Parking/play.html):
- **Universal Device Support:** Plays instantly on **iOS (iPhone/iPad), Android, Mac, Windows, Linux, and Chromebook** in any browser (Chrome, Safari, Edge, Firefox).
- **Zero Downloads & Zero Antivirus/Smart App Control Blocks:** No need to download compilers, MinGW, or freeglut DLLs. Share a link or file and friends/professors can play instantly!
- **Zero-Delay Web Audio API:** In-memory pre-decoded `fahh.mp3` sound effect with sub-millisecond audio response.
- **Full Touch & Keyboard Controls:** Includes responsive on-screen steering and pedal controls for phones/tablets, and standard W/A/S/D / arrow keys for PCs.
- **Academic CO Faithfulness:** 1:1 implementation of the architectural blueprints, COA register emulation, DSA maneuver stack undo ('U' key), gate queue, and encrypted password player database.

### How to Play or Share the Web Edition:
1. **Play Locally:** Double-click [`index.html`](file:///c:/Users/Sumit/2D-Car-Parking/index.html) in your browser.
2. **Share Anywhere:** Send `index.html` via WhatsApp, Discord, or Google Drive — recipient opens it with 1 tap.
3. **Host Free Online:** Push this folder to a GitHub repository and turn on **GitHub Pages** (Settings $\to$ Pages $\to$ Deploy from branch) to get a public link like `https://<your-username>.github.io/<repo>/`!

---

## 🛠️ Windows Desktop Edition (C++ & Pure NASM Assembly)

### How to Bypass Smart App Control on Windows 11:
Windows 11 Smart App Control blocks locally compiled open-source executables that do not have a paid digital certificate. To run `2D_Car_Parking.exe`:
1. **Option A (PowerShell):**
   ```powershell
   Unblock-File .\2D_Car_Parking.exe
   ```
2. **Option B (File Explorer):**
   - Right-click `2D_Car_Parking.exe` $\to$ **Properties**.
   - Check the **Unblock** checkbox at the bottom of the *General* tab $\to$ Click **Apply** and **OK**.

### How to Build:
```cmd
cd c:\Users\Sumit\2D-Car-Parking
build.bat
.\2D_Car_Parking.exe
```

### Controls
- **`W / A / S / D`** or **Arrow Keys**: Drive and steer
- **`Spacebar`**: Handbrake
- **`U`**: Undo maneuver (demonstrates DSA LIFO Stack)
- **`T`**: Switch target slot
- **`P`**: Display slot array and obstacle list
- **`R`**: Restart level
- **`Esc`**: Return to menu
- **Mouse**: Buttons, account selection, password modal, and win/fail dialogs
