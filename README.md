
# 🐚 ShellOrigin (kaiVen)
> **To understand the machine, you must build the machine.** 🖥️
> *A robust, modular Unix shell bridging low-level kernel interactions with cloud-based AI.*

![C](https://img.shields.io/badge/C-00599C?style=flat-square&logo=c&logoColor=white) ![Linux](https://img.shields.io/badge/Linux-FCC624?style=flat-square&logo=linux&logoColor=black) ![libcurl](https://img.shields.io/badge/libcurl-073551?style=flat-square&logo=curl&logoColor=white) ![cJSON](https://img.shields.io/badge/cJSON-000000?style=flat-square&logo=json&logoColor=white) ![Google Gemini](https://img.shields.io/badge/Google%20Gemini-8E75B2?style=flat-square&logo=googlegemini&logoColor=white)

---

## 🧐 What is ShellOrigin?

Most developers use shells (Bash, Zsh) every day without knowing how they work. **ShellOrigin (kaiVen)** is a custom implementation of a command-line processor that handles process creation, memory allocation, file descriptor hijacking, and signal handling manually.

Recently upgraded for modern workflows, this shell features a native AI integration module, proving that the magic of the terminal—and cloud APIs—is just C code and HTTP protocols under the hood.

---

## ⚡ Features

* **🔄 Interactive REPL:** A custom "Read-Eval-Print Loop" featuring a dynamic, ANSI-colored prompt that actively tracks your Current Working Directory (`getcwd`).
* **🚀 Process Execution:** Launches standard Linux programs (`ls`, `grep`, `vi`, `gcc`) utilizing the classic `fork()` and `execvp()` system calls.
* **🛡️ Signal Trapping:** Intercepts `SIGINT` (Ctrl+C) via `<signal.h>` to prevent accidental shell termination while safely killing runaway child processes.
* **📂 Output Redirection:** Implements file descriptor hijacking (`dup2`) to route standard output streams into files (e.g., `echo "test" > file.txt`).
* **🤖 Cloud AI Integration:** A deeply integrated `suggest` command that makes secure, memory-safe HTTP POST requests to the Google Gemini API for real-time CLI assistance.
* **🛠️ Native Built-ins:** Custom implementations of internal state-modifying commands:
    * `cd` (Change Directory)
    * `help` (Documentation)
    * `suggest` (AI CLI Assistant)
    * `exit` (Graceful termination)

---

## 🏗️ Technical Architecture

The codebase has evolved from a monolithic script into a professional, multi-file modular architecture.

### 1. Process Management (`main.c`)
The heartbeat of the shell is an infinite loop handling the **Initialize → Parse → Execute** lifecycle.
* **Process Cloning:** Uses `fork()` & `execvp()` to create child processes for external binaries.
* **File Descriptors:** Parses the `>` token, opens target files using `open()`, and reroutes `STDOUT_FILENO` using `dup2()` prior to execution.
* **Zombie Prevention:** The parent process explicitly suspends using `waitpid()` until the child terminates cleanly.

### 2. Network & AI Module (`ai_module.c`)
Unlike high-level languages, C lacks native internet capabilities. This module relies heavily on `libcurl` and custom memory management.
* **Defensive Memory Allocation:** Incoming network packets arrive in unpredictable chunk sizes. The `write_memory_callback` function uses dynamic `realloc()` arithmetic to stretch a memory buffer in real-time without overflowing.
* **JSON Syntax Tree:** Parses the raw HTTP response using `cJSON`, utilizing strict defensive type-checking (`cJSON_IsArray`, `cJSON_IsString`) to prevent segmentation faults during payload extraction.
* **Environment Security:** API keys are strictly fetched via `getenv()`, preventing hardcoded credentials in the source code.

---

## 🚀 Installation & Usage

You can run **ShellOrigin** on any standard Linux distribution or Windows Subsystem for Linux (WSL).

### 1. Install Dependencies
You must install the `libcurl` development headers to compile the networking module.
```bash
sudo apt-get update
sudo apt-get install libcurl4-openssl-dev

```

### 2. Clone the Repository

```bash
git clone [https://github.com/karnVen/ShellOrigin.git](https://github.com/karnVen/ShellOrigin.git)
cd ShellOrigin

```

### 3. Set the Environment Variable

For the `suggest` AI command to function, export your Gemini API key to your local environment. *(Tip: Add this to your `~/.bashrc` or `~/.zshrc` to make it permanent).*

```bash
export GEMINI_API_KEY="your_actual_api_key_here"

```

### 4. Compile the Source

Use the GCC compiler to link the core shell, the AI module, the JSON parser, and the curl library.

```bash
gcc main.c ai_module.c cJSON.c -o kaiVen -lcurl

```

### 5. Ignite the Shell

```bash
./kaiVen

```

**Example Session:**

```bash
kaiVen:/home/karnVen/ShellOrigin$ ls -la
kaiVen:/home/karnVen/ShellOrigin$ echo "Systems engineering" > notes.txt
kaiVen:/home/karnVen/ShellOrigin$ suggest how to debug a segmentation fault in C
kaiVen:/home/karnVen/ShellOrigin$ exit

```

---

## 🔮 Future Roadmap

* [ ] **Pipes (`|`)**: Implementing inter-process communication by wiring the `stdout` of one child to the `stdin` of another.
* [ ] **Input Redirection (`<`)**: Reading standard input from files via descriptor hijacking.
* [ ] **Command History**: Implementing buffer tracking for Up-arrow command recall.

---

## 🤝 Credits

* Shell architecture inspired by the classic Stephen Brennan "Write a Shell in C" tutorial.
* JSON parsing handled by the lightweight, single-file `cJSON` library by Dave Gamble.
* Built by **Team Karnage**.

---

## 👤 Author

**Karan Singh**

* 🐙 GitHub: [@karnVen](https://www.google.com/search?q=https://github.com/karnVen)
* 💼 LinkedIn: [Karan Singh](https://www.google.com/search?q=https://www.linkedin.com/in/karnven)

<p align="center">

```

Would you like me to draft an updated one-liner for your main profile README to reflect this new "kaiVen" AI integration?

```
