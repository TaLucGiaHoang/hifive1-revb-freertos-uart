# hifive1-revb-freertos-uart

## SiFive HiFive1 Rev B - FreeRTOS
Example for using uart_printf, uart_gets on FreeRTOS

The project aims to implement printf, vprintf, sprintf, vsprintf functions which are used on FreeRTOS to print log to terminal with safe way

## Materials:
- SiFive HiFive 1 Rev B1 board
- [SiFive Freedom Studio](https://www.sifive.com/boards)
- Source code downloaded from https://github.com/TaLucGiaHoang/hifive1-revb-freertos-uart

## References:
The project was modified base on examples code of FreeRTOS demo for HiFive1 Rev B board.

Refer to FreeRTOS/Demo/RISC-V_RV32_SiFive_HiFive1_FreedomStudio on https://sourceforge.net/p/freertos/code/HEAD/tree/trunk/ (r2752)

## Build Instructions (for Windows 10)
**Step 1:** Clone this repository 
```
$ git clone https://github.com/TaLucGiaHoang/hifive1-revb-freertos-uart.git
```

**Step 2:** Import this project into Freedom Studio workspace
- Open FreedomStudio
- Right click on **Project Explorer**, then choose **Import...** > **Existing Project into Workspace**
- Select root directory: `<hifive1-revb-freertos-uart directory>`
- **DO NOT** select **Copy projects into workspace**
- Click **Finish**

**Step 3:** Select `RTOSDemo` project and build
- **NOTE:** on Windows if you got this error
```
14:40:42 **** Build of configuration Debug for project RTOSDemo ****
make -j4 all 
Cannot run program "make": Launching failed

Error: Program "make" not found in PATH
```

Add these paths into **Environment Variables**
- Open **Environment Variables...** > **User variables** > **Path**
```
<FreedomStudio_directory>\SiFive\riscv64-unknown-elf-gcc-<version>
<FreedomStudio_directory>\SiFive\riscv64-unknown-elf-gcc-<version>\bin
<FreedomStudio_directory>\SiFive\msys64-<version>\usr\bin
```

**Step 4:** Download binary file to board
- Open `Debug Configurations`
- Select **SiFive GDB SEGGER J-Link Debugging**, then click **New launch configuration**
- Debug configuration:
  + Selected cpu: `cpu@0`
  + DTS File:  `bsp\design.dts`
  + Device name: `FE310`
- Click **Debug** to download binary file
 
## Usage
- After downloading binary file onto HiFive1 Rev B board
- Use your terminal monitor (Ex: TeraTerm) to open HiFive1's serial COM (baud rate 115200)
- Press RESET button on-board.
- The program will print to terminal like below to prompt user input characters
```
Bench Clock Reset Complete

ATE0-->ATE0
OK
AT+BLEINIT=0-->OK
AT+CWMODE=0-->OK

prvQueueReceiveTask is running

Input your name:
```

- Type some characters (Ex: `hifive rev B`) and press ENTER
- This will print your input and name of current running tasks
```
prvQueueSendTask is running: hello hifive rev B
prvQueueReceiveTask is running
...
```

![Image of Demo](demo.jpg)