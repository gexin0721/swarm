#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import rospy
from std_msgs.msg import String

def test_voice_system():
    rospy.init_node('voice_test', anonymous=True)
    pub = rospy.Publisher('voice/wakeword', String, queue_size=10)
    
    rospy.sleep(1)
    
    print("Sending test wakeword...")
    msg = String()
    msg.data = "test_wake"
    pub.publish(msg)
    
    print("Test message sent!")

if __name__ == '__main__':
    try:
        test_voice_system()
    except rospy.ROSInterruptException:
        pass
