# Security Policy

## 🛡 Supported Versions

Only the latest version of EZdlc is actively supported.

| Version | Supported |
|--------|-----------|
| Latest | ✅ Yes |
| Older versions | ❌ No |

---

## 🚨 Reporting a Vulnerability

If you discover a security issue, bug, or unexpected behavior, please report it responsibly.

### 📬 How to report

Do NOT open a public issue for security problems.

Instead, report it privately via:

- GitHub Security Advisory (recommended)
- Or contact the repository maintainer directly (ffuy525@gmail.com)

---

## 🔍 What counts as a security issue?

We consider the following valid security concerns:

- Path traversal vulnerabilities
- Unauthorized file access outside selected directories
- Symlink abuse or unsafe link creation
- Privilege escalation issues (e.g., requiring admin unexpectedly)
- Crashes caused by malformed input that could be exploited
- Unsafe file overwrite behavior

---

## ⚠️ What is NOT a security issue?

The following are NOT considered vulnerabilities:

- User misconfiguration (wrong folder selection)
- Missing game files or invalid installs
- Performance issues
- Expected Windows permission prompts (e.g., symlink admin requirement)

---

## 🧠 Design Philosophy

EZdlc is designed with safety in mind:

- No external network access
- No downloading of files
- No modification of game executables
- Only operates within user-selected directories
- Requires explicit user confirmation before file changes

---

## 🔒 File Safety Rules

EZdlc follows these safety rules:

- Never writes outside selected BO3/BOIII folders
- Always shows a dry-run preview before changes
- Requires confirmation before execution
- Uses std::filesystem for safe path handling
- Optional symlink mode requires administrator privileges

---

## 📌 Disclaimer

This tool is intended for legitimate file organization and personal game management only.

Users are responsible for ensuring they comply with applicable laws and game licensing agreements.

No game content is included, distributed, or modified by this project.
