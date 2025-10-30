#  Machine-Oriented Programming – MD407 (STM32F4)

Low-level embedded programming exercises for the **MD407** board, written in **C** and **ARM assembly**.  
Each program demonstrates a specific hardware or register-level concept, including GPIO control, SysTick timing, interrupts, LCD control, and keypad scanning.

##  Hardware
- **Board:** MD407 (based on STM32F407)
- **Processor:** ARM Cortex-M4 (168 MHz)
- **Toolchain:** GNU ARM / `arm-none-eabi-gcc`
- **Interface:** JTAG / ST-Link (via Lab hardware setup)

---

##  Project Structure

###  C Source Files (in `C/`)
| File | Description |
|------|--------------|
| **lcd_display.c** | Controls an HD44780 ASCII LCD via GPIOE. Implements low-level timing, initialization, and character writing using direct register access. |
| **keypad_7seg.c** | Scans a 4×4 keypad connected to Port D and displays the pressed key on a 7-segment LED display. Demonstrates GPIO matrix scanning and bit manipulation. |
| **led_blink_systick.c** | Blinks all LEDs on Port D using SysTick for precise millisecond delays. Demonstrates basic SysTick setup and software delays. |
| **squarewave_timer.c** | Generates a square wave output on a GPIO pin using the SysTick timer in polling mode. Shows periodic toggling without interrupts. |
| **interrupt_keyboard_squarewave.c** | Combines external interrupts (EXTI) and SysTick to generate square waves controlled by keypad input. Demonstrates interrupt handling and multitasking at the register level. |

###  Assembly Files (in `ASM/`)
| File | Description |
|------|--------------|
| **copy_halfword_to_word.asm** | Copies 16-bit values into 32-bit words (LDRH → STR). Demonstrates data widening. |
| **copy_word_to_halfword.asm** | Copies 32-bit words into 16-bit values (LDR → STRH). Demonstrates truncation to lower 16 bits. |
| **copy_selected_elements.asm** | Copies selected elements from a vector to another using indexed addressing and conditional logic. |

---

##  Concepts Demonstrated

- **GPIO configuration**
  - Setting input/output direction (`MODER`)
  - Using bit masking for control signals (`ODR`, `IDR`)
- **SysTick timer**
  - Microsecond and millisecond delay generation
  - Polling and interrupt-based timing
- **External interrupts (EXTI)**
  - Keypad scanning with event-driven input
  - NVIC interrupt enable/disable handling
- **LCD driver logic**
  - RS, RW, E control with direct register manipulation
  - Busy-flag polling and command sequencing
- **Assembly data movement**
  - Word/halfword memory access (`LDR`, `STR`, `LDRH`, `STRH`)
  - Register arithmetic and loop control

---

##  Build & Run

1. **Compile C programs**
   ```bash
   arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -c C/<file>.c
   arm-none-eabi-ld -Ttext 0x0 -o program.elf <file>.o
   arm-none-eabi-objcopy -O binary program.elf program.bin
