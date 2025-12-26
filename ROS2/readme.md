# ROS2 on WSL -- Setup & Notes


This document contains my setup steps and learning notes while working
with **ROS2 (Humble)** using **WSL2** on Windows. It includes
installation instructions, commonly used commands, troubleshooting
fixes, and reference materials.

------------------------------------------------------------------------

## 1. Install WSL

1.  Open **PowerShell** in Administrator mode.
2.  Install WSL:

    ``` bash
    wsl --install
    ```
3.  Restart your system.
4.  List available Linux distros:

    ``` bash
    wsl --list --online
    ```
5.  Install Ubuntu 22.04:

    ``` bash
    wsl --install Ubuntu-22.04
    ```
6.  Create your username and password.
7.  Verify WSL version:

    ``` bash
    wsl -l -v
    ```

    You should see **Version 2**.

------------------------------------------------------------------------

## 2. Running ROS2 Executables

Install turtlesim:

``` bash
sudo apt update
sudo apt install ros-humble-turtlesim
```

Check installed packages:

``` bash
ros2 pkg list
ros2 pkg executables
ros2 pkg executables <package_name>
```

Run nodes:

``` bash
ros2 run <package> <node>
# Example:
ros2 run turtlesim turtlesim_node
```

Get node info:

``` bash
ros2 node info <node_name>
# Example:
ros2 node info /turtlesim
```

> TODO: Write and revise all commands.

------------------------------------------------------------------------

## 3. ROS2 Workspace

*(Reserved for notes on creating workspaces, building packages, CMake,
colcon, etc.)*

------------------------------------------------------------------------

## 4. What I Tried / Additional Notes

-   Running `wsl --install` again installed **Ubuntu 24**, which worked

    properly.
-   In Windows PowerShell, typing:

    ``` bash
    wsl
    ```

    opens the Ubuntu terminal.
-   Apart from **Humble Hawksbill**, you can also use **Jazzy Jalisco**.
-   If installing Ubuntu 22.04 fails, try:

    ``` bash
    wsl --install --web-download -d Ubuntu-22.04
    ```
-   Check Ubuntu version:

    ``` bash
    lsb_release -a
    ```

------------------------------------------------------------------------

## 5. Useful WSL Commands

``` bash
wsl --list
wsl --list --verbose
wsl --setdefault <DISTRO-NAME>
\\wsl$\Ubuntu -: type this in file explorer which can open the folder space and you will be able to paste media.
```

------------------------------------------------------------------------

## 6. Important Info

### Sourcing (Required for ROS2)

Sourcing lets your terminal know where ROS2 is installed.

Temporary sourcing (to be done every new terminal session):

``` bash
source /opt/ros/humble/setup.bash
```

Permanent sourcing (recommended):\
Add the above line to your `~/.bashrc` file.

``` bash
echo "source /opt/ros/humble/setup.bash" >> ~/.bashrc
```

------------------------------------------------------------------------

## 7. Errors & Fixes

### Error: Repository Not Signed

If you encounter:

    E: The repository 'http://packages.ros.org/ros2/ubuntu jammy InRelease' is not signed.

Fix:

``` bash
sudo curl -sSL https://raw.githubusercontent.com/ros/rosdistro/master/ros.key -o /usr/share/keyrings/ros-archive-keyring.gpg
```

------------------------------------------------------------------------

## 8. References

YouTube tutorial:\
https://www.youtube.com/watch?v=HJAE5Pk8Nyw
