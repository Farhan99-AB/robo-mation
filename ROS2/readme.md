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

List nodes:
ros2 node list

List topics:
ros2 topic list
ros2 topic list -t -> will list the types of topics

ros2 topic echo /topic_name -> will show the data pubd on the topic (when you echo and refresh rqt graph you will see changes in the graph)
eg: ros2 topic echo /tutle1/cmd_vel 

ros2 topic info /topic_name -> will give type pub and sub count
eg: ros2 topic info /tutle1/cmd_vel 

ros2 interface show /topic_type -> give the interface definition
eg: ros2 interface show geometry_msgs/msg/Twist (turtlesim cm_vel interface def)

ros2 topic pub /topic_name /msg_type -> pub data to a topic directly --once means only once
eg: ros2 topic pub --once /tutle1/cmd_vel geometry_msgs/msg/Twist "{linear: {x: 2.0, y: 0.0, z: 0.0}, angular: {x: 0.0, y: 0.0, z: 1.8}}"

ros2 topic pub --rate 1 /topic_name /msg_type -> pub data to a topic directly with a rate of 1
eg: ros2 topic pub --rate 1 /tutle1/cmd_vel geometry_msgs/msg/Twist "{linear: {x: 2.0, y: 0.0, z: 0.0}, angular: {x: 0.0, y: 0.0, z: 1.8}}"

ros2 topic hz /topic_name -> gives topic freq
eg: ros2 topic hz /turtle1/pose

Rqt Graph:
rqt_graph


Services:
ros2 service list 
ros2 service list -t -> list services along with their type

ros2 service find /service_type
eg: ros2 service find std_srvs/srv/Empty

ros2 interface show /service_type
eg: ros2 interface show turtlesim/srv/Spawn

ros2 service call /service_name /service_type -> calling a service
eg: clear a drawing 
ros2 service call /clear std_srvs/srv/Empty

Node Params:

ros2 param list 

ros2 param get /node_name /param_name
eg: ros2 param get /turtle_sim background_g

ros2 param set /turtle_sim background_g 255

ros2 param dump /node_name
eg: ros2 param dump /turtle_sim -> view all params of a node    

Store params to a yaml file 
ros2 param dump /turtle_sim > turtle_sim.yaml

to load from taml 
ros2 param load /turtle_sim turtle_sim.yaml

Load params on startup
ros2 run turtlesim turtlesim_node -- ros-args -- params-file turtlesim.yaml

Actions:

ros2 node info /teleop_turtle

ros2 action list 
ros2 action list -t 
ros2 action info /turtle1/rotate_absolute
ros2 interface show turtlesim/action/RotateAbsolute

ros2 action send_goal /action_name /action_type /goal
ros2 action send_goal /turtle1/rotate absolute turtlesim/action/RotateAbsolute "{theta: 1.57}"  I 

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

### Topics

Topics are like named communication channels or buses which carry data through pub-sub model.
They are strongly typed by message type.

### Services

Work as client server models where when client request a data and the server needs to respond before the client can 
recieve the data

### Params to Nodes

This is like calling a function with parameters.

### Actions

Actions are similar to client service model but along with a goal, feedback and result. This can be seen with the 
rotate command of turtlesim which is an action.
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
