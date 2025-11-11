***Install WSL***
Open Powershell in admin mode 
Run wsl --install
Restart 
Run wsl --list --online to see available linux distros
Install distro wsl --install Ubuntu-22.04
Create username and password
Verify version by running wsl -l -v. Should see version 2 

***What I tried***
ran wsl --install again things worked (this will install latest ubuntu24)

also on windows power shell type wsl to get ubuntu terminal 

apart from humble hawksbill u can use jazzy jalisco

if step 6 (now wsl intall ubuntu 22 ) doesn't work try
wsl --install --web-download -d Ubuntu-22.04

lsb_release -a -> to check ubuntu version

***some WSL commands***
 wsl --list
 wsl --list --verbose 
 wsl --setdefault DISTRO-NAME



***Errors***
if you ger this error
E: The repository 'http://packages.ros.org/ros2/ubuntu jammy InRelease' is not signed.
try running sudo curl -sSL https://raw.githubusercontent.com/ros/rosdistro/master/ros.key -o /usr/share/keyrings/ros-archive-keyring.gpg



***REFERENCES***
YT link: https://www.youtube.com/watch?v=HJAE5Pk8Nyw