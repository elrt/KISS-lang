![screenshot](https://github.com/elrt/KISS-lang/blob/20c13b8220e37171ba0fded7e8745d203699ad5e/screenshots/2025-06-08_19-08.png)

---

# **KISS: The "Keep It Simple Syntax" Esolang**  
*(Because why use 26 variables when one `x` will do?)*  

KISS is a minimalist, Turing-tarpit-adjacent esoteric language where all computation revolves around a **single sacred integer `x`**, worshipped like an ancient deity. Want to compute something? You shall manipulate `x`. Want to print something? You shall channel `x`. Want to include another file? You shall... well, you get it.

---

## **Language Specs (Because Even Esolangs Need Rules)**  

### **1. The One True Variable**  
- There is only **`x`**.  
- `x` is an integer.  
- `x` is your life now.  

### **2. Command Syntax (Do More with Less!)**  
All commands are **single uppercase letters**, followed by *optional* arguments.  

| Command | Effect | Example |  
|---------|--------|---------|  
| `A` | **A**ssign to `x` | `A 42` → `x = 42` <br> `A 'X'` → `x = 88` (ASCII) |  
| `I` | **I**ncrement `x` | `I 5` → `x += 5` |  
| `D` | **D**ecrement `x` | `D 3` → `x -= 3` |  
| `P` | **P**rint `x` (as number) | `P` → Output: `42` |  
| `CP` | **C**haracter **P**rint (ASCII) | `CP` → If `x=65`, prints `'A'` |  
| `#include` | Import another `.kiss` file | `#include "ritual.kiss"` |  

![screenshot](https://github.com/elrt/KISS-lang/blob/7eedafc533457114fe216964f4587275e1ea49ee/screenshots/2025-06-08_19-29.png)
---


### **3. Sample Program (Hello World, Sort Of)**  
```plaintext
# KISS-compliant "Hello World" (if you squint)
A 72    # 'H'
CP
A 101   # 'e'
CP
A 108   # 'l' (x2)
CP
CP
A 111   # 'o'
CP
A 10    # '\n' (newline)
CP
```
**Output:** `Hello` (with a newline)  

*(Yes, this is painful. No, i won’t add strings.)*  

