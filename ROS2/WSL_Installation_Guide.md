# WSL Installation & Troubleshooting Guide







## 🚀 Install WSL

1. Open **PowerShell in Administrator mode**
2. Run:

   ```bash
   wsl --install
   ```
3. Restart your system
4. Check available Linux distros:

   ```bash
   wsl --list --online
   ```
5. Install a distro (example: Ubuntu 22.04):

   ```bash
   wsl --install Ubuntu-22.04
   ```
6. Create your **username** and **password**
7. Verify WSL version:

   ```bash
   wsl -l -v
   ```

   → Should show **Version 2**

---

## 💡 What I Tried (Fixes / Notes)

- Running `wsl --install` again solved issues in some cases  

  → This installs the **latest Ubuntu (24.x)**
- If you type `wsl` in PowerShell, you directly get the Ubuntu terminal
- ROS supports multiple distros:
  - Humble → Ubuntu 22.04
  - Jazzy → Ubuntu 24.04
- If installing Ubuntu 22.04 fails, try:

  ```bash
  wsl --install --web-download -d Ubuntu-22.04
  ```
- Check Ubuntu version:

  ```bash
  lsb_release -a
  ```

---

## 🧰 Useful WSL Commands

- List distros:

  ```bash
  wsl -l -v
  ```
- Shutdown WSL:

  ```bash
  wsl --shutdown
  ```
- Unregister (delete) a distro:

  ```bash
  wsl --unregister <DistroName>
  ```

### **Goal To Change the directory of WSL root**

1. **Find which Ubuntu you use**
2. **Delete the duplicate distro**
3. **Move the correct one to D:** without losing your files

---

# ✅ Step 0 — Identify which distro you actually use

Run each one manually and check which has your files.

### Start Ubuntu-22.04:

```powershell
wsl -d Ubuntu-22.04
```

Check if it contains your home files:

```bash
ls ~
```

Exit:

```bash
exit
```

### Start Ubuntu:

```powershell
wsl -d Ubuntu
```

Check again:

```bash
ls ~
```

👉 **Whichever contains your files is the one you want to keep.**

* **Ubuntu-22.04**
* **Ubuntu**

---

# ✅ Step 1 — Delete the duplicate distro (the empty one)

After confirming which distro is NOT needed, delete it:

Example if "Ubuntu" is the duplicate:

```powershell
wsl --unregister Ubuntu
```

Or if "Ubuntu-22.04" is duplicate:

```powershell
wsl --unregister Ubuntu-22.04
```

⚠️ **This permanently deletes that distro**, so make sure you checked before.

---

# ✅ Step 2 — Export your good distro to D:

Let’s assume your good distro is **Ubuntu-22.04**
(If it's the other one, replace the name in the commands.)

Create a folder on D drive:

```powershell
mkdir D:\WSL
```

Export the distro:

```powershell
wsl --export Ubuntu-22.04 D:\WSL\Ubuntu.tar
```

This creates a **backup** of your entire Linux system.

---

# ✅ Step 3 — Unregister the old (C: drive) distro

```powershell
wsl --unregister Ubuntu-22.04
```

This removes it from C: completely.

---

# ✅ Step 4 — Import it to D: drive

```powershell
wsl --import Ubuntu-22.04 D:\WSL\ D:\WSL\Ubuntu.tar --version 2
```

This creates a **new ext4.vhdx** **on D:** and restores your Linux installation there.

---

# ✅ Step 5 — Set it as default (optional)

```powershell
wsl --set-default Ubuntu-22.04
```

---

# 🟢 DONE!

Everything you install in WSL now expands **D:\WSL\ext4.vhdx**, not C:.

---
