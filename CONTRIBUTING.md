# 🛠️ Contributing to RouteX

Thank you for your interest in contributing to **RouteX**! This project is a systems-level network interceptor built in C that redirects raw TCP traffic through local Tor circuits.

Whether you are a general open-source developer looking to optimize our socket layers or a student participating in a specific open-source program, this guide will help you get your contributions formatted, verified, and merged successfully.

---

## 🌐 Part 1: General Contribution Workflow

If you want to suggest improvements, fix memory safety issues, or add clean diagnostic clients, please follow this standard workflow:

### ⚙️ 1. Codebase Architecture & Technical Constraints

RouteX runs as an injected dynamic wrapper library (`tord.so`). Any extensions to the source files (`tord.c`, `tord.h`, `client_ssl.c`) must remain fully compliant with these core rules:

- 📐 **Formatting & Indentation:** Use `4 spaces` for indentation. Strictly avoid mixing raw tabs with spaces in C blocks.
- 🏷️ **Naming Schemes:**
  - Functional methods, internal configurations, and pointers must use `snake_case` *(e.g., `real_connect`, `socks5_handshake`)*
  - System macros, boundaries, and protocol headers must be `UPPERCASE` *(e.g., `ROUTEX_HOSTNAME`, `AF_INET`)*
- 🔒 **Thread Safety:** Following recent thread-safe updates to the socket connection routine, any changes touching dynamic loading resolutions (`dlfcn.h`) must adapt seamlessly to the `pthread_once` instantiation loop.
- 🧹 **Resource Preservation:** All dynamically allocated memory blocks, pointers, or duplicate descriptors must be cleanly freed before functional boundaries to prevent runtime memory leaks within background system processes.

---

### 🌿 2. Isolated Git Branching Strategy

Never push your experimental features or patches directly onto your fork's `main` branch:

**🔄 Synchronize Main Branch:**
```bash
git checkout main
git pull upstream main
```

**🌱 Spawn a Segmented Feature Branch:** Name your working branch contextual to your workload focus:
```bash
# ✨ For new features
git checkout -b feat/your-feature-name

# 🐛 For bug fixes
git checkout -b fix/your-patch-name
```

---

### 📝 3. Conventional Commit Standards

We enforce structured Git log messages. Format your commit titles exactly as:

| Prefix | When to Use |
|--------|-------------|
| ✨ `feat:` | Appending new network logic, diagnostics, or proxy implementations |
| 🐛 `fix:` | Patching segmentation faults, multi-thread racing issues, or compilation errors |
| 📚 `docs:` | Editing guides, inline comments, or markdown structures |
| ♻️ `refactor:` | Optimizing logic paths without shifting performance properties |

---

## 🏆 Part 2: Special Guidelines for NSoC '26 Participants

If you are contributing to RouteX as part of the **Nexus Spring of Code (NSoC'26)** program, you must strictly follow these additional operational rules to ensure your points are tracked correctly on the public leaderboard:

> ⚠️ **Important:** Non-compliance with these rules may result in your contribution not being counted toward the leaderboard.

- 🎯 **Issue Assignment First:** Do not submit a Pull Request (PR) for an issue that has not been explicitly assigned to you. Express your interest in a specific open issue by commenting, and wait for the project maintainer (`@Ujjwal5705`) to assign it.

- 👤 **Single Active Issue Rule:** To maintain a fair environment across the NSoC ecosystem, contributors should only occupy **one active issue claim** at any given time.

- 🔗 **Linking Issues in PRs:** When opening your Pull Request, you must link it to your tracked issue. Write `Closes #IssueNumber` or `Fixes #IssueNumber` clearly in the PR description text so the issue closes automatically upon merge.

- ⏳ **3-Day Stale Allocation Policy:** If an assigned issue shows zero communication or progress updates for more than **3 days**, it will be automatically unassigned and put back into the public pool for other contributors to claim.

---

## 📋 Part 3: Pre-Commit Validation Checklist

Before staging your final files to your remote branch and opening a Pull Request, execute these local safety checks on your machine:

-  ✅ **Clean Compilation:** Execute `make` inside your root directory to ensure the codebase compiles cleanly into `tord.so` and `client_ssl` without generating active warnings.

-  🔍 **Interception Verification:** Run our standard execution wrapper tool `./routex www.google.com` to confirm that standard POSIX socket hijacking routes traffic safely without dropping any network packets.

-  🚫 **No Environmental Hardcoding:** Ensure target parameters remain entirely dynamic and bounded inside the loaded `ROUTEX_HOSTNAME` variable space.
