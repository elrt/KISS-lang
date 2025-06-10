p.s https://esolangs.org/wiki/Keep-It-Simple-Syntax

# **KISS: The "Keep It Simple Syntax" Esolang** 
![screenshot](https://github.com/elrt/KISS-lang/blob/20c13b8220e37171ba0fded7e8745d203699ad5e/screenshots/2025-06-08_19-08.png)

*(Because why use 26 variables when one `x` will do?)*  
KISS is a minimalist, Turing-tarpit-adjacent esoteric language where all computation revolves around a **single sacred integer `x`**, worshipped like an ancient deity. Want to compute something? You shall manipulate `x`. Want to print something? You shall channel `x`. Want to include another file? You shall... well, you get it.

---
## Changelog

**Recent Changes:**

*   **10-06-2025:**
    *   Added new commands and some bug fixes:p (commit: `c1bf16d`)
      
 

---
## **Language Specs (Because Even Esolangs Need Rules)**  

### **1. The One True Variable**  
- There is only **`x`**.  
- `x` is an integer.  
- `x` is your life now.  

### **2. Command Syntax (Do More with Less!)**  
All commands are **single uppercase letters**, followed by *optional* arguments.  

![screenshot](https://github.com/elrt/KISS-lang/blob/b94a79762733243ec9be9a9e00c1bb8128e3115a/screenshots/2025-06-08_19-29.png)

| Command | Effect | Example |
|---------|--------|---------|
| `A` | **A**ssign to `x` | `A 42` → `x = 42` <br> `A 'X'` → `x = 88` (ASCII) |
| `I` | **I**ncrement `x` | `I 5` → `x += 5` |
| `D` | **D**ecrement `x` | `D 3` → `x -= 3` |
| `P` | **P**rint `x` (as number) | `P` → Output: `42` |
| `CP` | **C**haracter **P**rint (ASCII) | `CP` → If `x=65`, prints `'A'` |
| `CI` | **C**haracter **I**nput (ASCII) | `CI` → `x = getchar()` |
| `C` | **C**lear `x` | `C` → `x = 0` |
| `X` | **X**OR `x` with value | `X 5` → `x ^= 5` |
| `M` | **M**ultiply `x` | `M 2` → `x *= 2` |
| `Q` | **Q**uotient (Divide `x`) | `Q 3` → `x /= 3` |
| `R` | **R**emainder (Modulo `x`) | `R 4` → `x %= 4` |
| `S` | **S**wap `x` and value | `S 5` → swaps `x` with `y`, then `x = 5` |
| `N` | **N**egate `x` | `N` → `x = -x` |
| `B` | **B**itwise NOT `x` | `B` → `x = ~x` |
| `Y` | Store `x` in `y` | `Y` → `y = x` |
| `V` | Retrieve `y` to `x` | `V` → `x = y` |
| `E` | **E**quals jump (skip next if `x != val`) | `E 5` → skip next line if `x != 5` |
| `L` | **L**ess-than jump (skip next if `x < val`) | `L 10` → skip next line if `x < 10` |
| `G` | **G**oto label | `G loop` → jumps to `:loop` |
| `^` | Square `x` | `^` → `x = x * x` |
| `t` | Integer Square Root of `x` | `t` → `x = (int)sqrt(x)` |
| `?` | Random number (0 to value - 1) | `? 10` → `x = random(0, 9)` |
| `>` | Store `x` in memory location | `> 0` → `memory[0] = x` |
| `<` | Load `x` from memory location | `< 1` → `x = memory[1]` |
| `:` | Label definition | `:loop` → defines a label named `loop` |
| `#include` | Import another `.kiss` file | `#include "lib.kiss"` |


### **3. Labels and Control Flow**  
Labels are defined with `:` prefix:  
```plaintext
:loop
A 1
P
D 1
E 0
G loop
```
This prints numbers from 1 down to 0.

### **4. Sample Program (Hello World)**  
```plaintext
# KISS-compliant "Hello World"
A 72    # 'H'
CP
A 101   # 'e'
CP
A 108   # 'l' (x2)
CP
CP
A 111   # 'o'
CP
A 32    # ' '
CP
A 87    # 'W'
CP
A 111   # 'o'
CP
A 114   # 'r'
CP
A 108   # 'l'
CP
A 100   # 'd'
CP
A 10    # '\n'
CP
```
**Output:** `Hello World`  

*(Yes, this is painful. No, I won't add strings.)*  


.
