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

ROS WS is basically a folder
package builder used colcon.

Steps to create a WS and setup colcon:
* install colcon using 
    sudo apt install python3-colcon-common-extensions
* create a folder 
* create a folder named src
    can use mkdir -p ~/ros2_ws/src and then cd ~/ros2_ws
* git clone a repo for testing 
    eg: git clone https://github.com/ros/ros_tutorials.git -b humble
* setup colon tab completion (auto complete):
    echo "source /usr/share/colcon_argcomplete/hook/colcon-argcomplete.bash" >> ~/.bashrc
* do colon simlink install 
    colcon build --symlink-install
* do ls n check for build install and log files
* Source overlay from ros2 ws
    source install/local_setup.bash
* next to make sure our changes reflect we will do changes in turtle_frame.cpp (ctrl+p to search) 
at line 52 change the window title to "MyTurtleSim"
* then build n source again
    colcon build --symlink-install
    source install/local_setup.bash
    ros2 run turtlesim turtlesim_node 

Steps to create a package:
* to create a package
    ros2 pkg create --build-type ament_cmake --node-name my_node my_package
* to build 
    colcon build -- packages-select my_package
* Source overlay from ros2 ws
    source install/local_setup.bash
* run node 
    ros2 run my_package my_node

Creating Pub Sub package
* Install CMake extension to view CMakeLists.txt files with syntax coloring. 
* Create package from package folder with the following CMakeLists.txt , and  ~/ros2_ws/src . 
  We will make a  cpp_pubsub generated include , package. xml
  
  ros2 pkg create --build-type ament_cmake cpp_pubsub

Chhecking missing dependencies
* rosdep install -i --from-path src --rosdistro humble -y

Running launch files
* ros2 launch <package_name> <launch_file>
    eg: ros2 launch gazebo_tutorial gazebo.launch.py

Running URDF(s)
* can use similar syntax ffor link, joints etc.
* ros2 launch urdf_tutorial display.launch.py model:= urdf/01-simple-link.urdf

Running XACRO
* xacro model..xacro > model.urdf
* other way is to use the robot state publisher
* another way is to use the URDF launch
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

### Publisher and Subscriber 

After creating a publisher and subscriber you would also need to edit the CMakelist.txt and packagexml
files too. 

### Launch Files 

These are kinda like the automation files which helps run the commands which we need to build and run ros pacakges.
Basically they help with automating running the commands frorm terminal to launch a package.

### URDF (Unified Robotics Description Format)

Just an xml file which describes the visual, physics and connections of a robot with
joints and links.

main contents in the file (general structure):
#### Links
* visual
* geometry
* collision
* inertial

#### Joints
* parent 
* child


### XACRO

Xacro is like a macros defining file where you can define functions that you will be 
reusing.
you can use it to write a xacro property which is used across multiple urdff.
And also can you the XACRO macro

### RVIZ

visualisation tool
  
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
