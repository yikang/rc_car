#!/usr/bin/python 

import serial 
import rospy 
from std_msgs.msg import String

class drive_car:
 
    ## api send command to Ardunio 

    def __init__(self,device="/dev/ttyUSB0",rate=19200):
        self.device = device 
        self.rate = rate
        # set up connection 
        self.conn = serial.Serial(device,rate)
        # getinto command mode 
        self.conn.write("c") 

    def send_command(self,comm,n=1):
        # send comm n times
	for _ in range(n):
            self.conn.write(comm)

def callback(data,dc):
    commd  = data.data
    rospy.loginfo(commd)
    dc.send_command(commd)

def drive_listener():
    # init node 
    rospy.init_node("drive_listener",anonymous=True)
    dc = drive_car()
    rospy.Subscriber("rc_command",String, callback, dc)
    rospy.spin()

if __name__ == "__main__":
    print("running car drive scripts")
    drive_listener()
