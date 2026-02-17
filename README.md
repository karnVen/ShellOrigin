
# 🐚 ShellOrigin
> **To understand the machine, you must build the machine.** 🖥️
> 
> *kvsh_💀:> is a custom, lightweight Unix shell built from scratch in C to demystify operating system internals.*

![C](https://img.shields.io/badge/C-00599C?style=flat-square&logo=c&logoColor=white) ![Linux](https://img.shields.io/badge/Linux-FCC624?style=flat-square&logo=linux&logoColor=black) ![Systems Programming](https://img.shields.io/badge/Systems-Programming-purple?style=flat-square) ![Status](https://img.shields.io/badge/Status-Active-success?style=flat-square)

---

## 🧐 What is ShellOrigin?

**ShellOrigin** is not just a command-line interface; it is a journey into the kernel.

Most developers use shells (Bash, Zsh) every day without knowing how they work. **ShellOrigin** is a custom implementation of a Unix shell that replicates the core lifecycle of a command processor. It handles **process creation**, **memory allocation**, and **signal handling** manually, proving that the magic of the terminal is just C code under the hood.

---

## ⚡ Features

* **🔄 Interactive REPL:** A custom "Read-Eval-Print Loop" that accepts and executes user commands in real-time.
* **🚀 Process Execution:** Launches standard Linux programs (`ls`, `grep`, `vi`, `gcc`) using the `fork()` and `execvp()` system calls.
* **🛠️ Native Built-ins:** Custom implementations of shell-exclusive commands that modify the shell's own state:
    * `cd` (Change Directory)
    * `help` (Documentation)
    * `exit` (Graceful termination)
* **🧠 Dynamic Memory:** Robust handling of user input buffers using `malloc()` and `realloc()`.

---

## 🏗️ Technical Architecture

The shell follows the classic **Initialize → Interpret → Terminate** lifecycle.

### 1. The Core Loop (`lsh_loop`)
The heartbeat of the shell is an infinite loop that performs three critical tasks:
* **Read:** Accepts input from `stdin` using `getline()`.
* **Parse:** Tokenizes the input string into arguments using whitespace delimiters (`strtok`).
* **Execute:** Determines if the command is a *Built-in* or an *External Process*.

### 2. The Fork-Exec Model (`lsh_launch`)
To run external commands (like `ls`), ShellOrigin interacts directly with the OS kernel:
1.  **`fork()`**: Creates a duplicate process (child) of the shell.
2.  **`execvp()`**: Replaces the child process's memory with the new program image.
3.  **`waitpid()`**: The parent process (ShellOrigin) pauses execution until the child process finishes, preventing "zombie" processes.

---

## 🚀 Installation & Usage

You can run **ShellOrigin** on any Linux distribution or Windows Subsystem for Linux (WSL).

### 1. Clone the Repository
```bash
git clone [https://github.com/karnVen/ShellOrigin.git](https://github.com/karnVen/ShellOrigin.git)
cd ShellOrigin

```

### 2. Compile the Source

Use the GCC compiler to build the executable.

```bash
gcc main.c -o shell

```

### 3. Ignite the Shell

Start your custom session.

```bash
./shell

```

**Example Session:**

```bash
kvsh_💀:> ls -a
kvsh_💀:> echo "Hello World"
kvsh_💀:> cd ..
kvsh_💀:> exit

```

---

## 🔮 Future Roadmap

* [ ] **Pipes (`|`)**: Implementing inter-process communication.
* [ ] **Redirection (`>`, `<`)**: Handling standard I/O streams.
* [ ] **Command History**: Pressing `Up` arrow to recall commands.
* [ ] **Config Support**: A `.karnvenrc` file for custom aliases.

---

## 🤝 Credits

Inspired by the classic *Stephen Brennan "Write a Shell in C"* tutorial, extended with custom features and performance optimizations.

---

## 👤 Author

**karnVen**

* 🐙 GitHub: [@karnVen](https://www.google.com/search?q=https://github.com/karnVen)
* 💼 LinkedIn: [karnVen](https://www.linkedin.com/in/karnven)

<p align="center">
<i>Made with 💻🔥 and a lot of pointers (*ptr).</i>
</p>

```

```
