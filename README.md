
## Parts List : 

1. Arduino Mega 2560 board 
2. Car kit (four pairs of wheels and DC motors )  
3. two L293D H bridge (to drive motors) 
4. three servos (two for camera, one for distance sensor ) 
5. one FPV camera, video transmitter and receiver 
6. one Pair Xbee module 
7. remote controller and receiver 
8. one 9 volt NIMH battery pack 


This RC car has three control modes : RC mode ; Command mode ; Autonomous mode 

By default, it sets to RC mode.  You can send command from Xbee to switch the mode. 

## command list : 

 'c' : switch to command mode 

 'q' : quit current mode 

 'a' : switch to autonomous mode 
 
 'w' : driving forward 

 's' : driving backward 

 'a' : turn left 

 'd' : turn right 
 
 'i' : switch camera servo, base or angle 

 'k' : reset camera angle 

 'j' : angle minus 

 'l' : angle plus
 
 
## Use with ros 

Ros package is under `rc_car_ros`.  Main code is `/scripts/drive_car.py`. Topic name `/rc_command`

### setup 

Copy `rc_car_ros` folder to your worksapce. i.e. `~/catkin_ws/src`. 

Build workspace: 
```
cd ~/catkin_ws 
catkin_make
```

Start the node:
```
rosrun rc_car_ros drive_car.py
```

### usage 

publish commnad to topic :
```
# forward w 4 times, and backward s 2 time
rostopic pub /rc_command std_msgs/String "wwwwss"
```


## Run in Docker 

Now, I want to run master on my Mac or Fedora, but all other code still on PI.




### install docker image 
We can install docker on Mac or Fedora.  Use docker image:

```
docker pull osrf/ros:kinetic-desktop
```


### run master in docker 

Make sure on PI, you set up env variables : 
```
export ROS_MASTER_URI="http://192.168.1.110:11311"
export ROS_IP="192.168.1.39"  # this is IP for pi
```

1) On linux [Fedora]
```
docker run -it  --rm \
           --net=host \
           --name master \
           -e ROS_HOSTNAME=192.168.1.110 \
           -e ROS_MASTER_URI=http://192.168.1.110:11311\
           osrf/ros:kinetic-desktop
```
**--net=host** make dock image use host network. 

After get into the image:
```
roscore & 
rostopic list # you should see "/rc_command" 
# publish to topic; you should control the car from local !
rostopic pub /rc_command std_msgs/String "wwwwss"
```

2) on Mac
Since on Mac, **--net=host** not working, because on Mac, docker actually inside the VM;
one workaround is : 

```
docker run -it -p 11311:11311 \
          --name master  --rm 
         osrf/ros:kinetic-desktop
```
But this only enable you echo the topic, you can not pushlish to the topic from host,
since docker image can not resolve the ip for PI i.e. "192.168.1.39".

#### Looking for solution here ??

```
# create network use bridge

docker network create --driver=bridge --ip-range=192.168.1.192/29 --subnet=192.168.1.0/24 --gateway=192.168.1.111 --attachable -o "com.docker.network.bridge.name=mybr0" --aux-address="DefaultGatewayIPv4=192.168.1.1" mybr0

# can not ping 192.168.1.192 from PI; can not pushlish topic on PI 
docker run -it --rm  --net=mybr0  --name master -e ROS_MASTER_IP=http://192.168.1.192:11311 -e ROS_IP=192.168.1.192   osrf/ros:kinetic-desktop

# can not ping 192.168.1.192 from PI; still can pushlish topic on PI 
docker run -it --rm  --net=mybr0 -p 11311:11311  --name master -e ROS_MASTER_IP=http://192.168.1.192:11311 -e ROS_IP=192.168.1.192   osrf/ros:kinetic-desktop
```

```
# On PI, still can publish : rostopic pub /rc_command std_msgs/String "wwwwss"
docker run -it --rm -p 11311:11311  --name master osrf/ros:kinetic-desktop
```