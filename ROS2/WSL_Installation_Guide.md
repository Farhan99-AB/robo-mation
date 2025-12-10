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