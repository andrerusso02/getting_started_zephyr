
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image, CameraInfo
import socket
import numpy as np


def replace_zeros_with_max_range(arr, max_range=4000):
    """Replace zero values with max range to indicate no detection."""
    arr = arr.copy()
    arr[arr == 0] = max_range
    return arr

class VL53L5CXNode(Node):
    def __init__(self):
        super().__init__('vl53l5cx_node')
        # Create publishers for both Image and CameraInfo
        self.img_pub = self.create_publisher(Image, 'vl53l5cx/depth', 10)
        self.info_pub = self.create_publisher(CameraInfo, 'vl53l5cx/camera_info', 10)
        
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind(("0.0.0.0", 5005))
        self.sock.setblocking(False) 
        
        self.timer = self.create_timer(0.01, self.poll_udp) 

    def poll_udp(self):
        try:
            data, _ = self.sock.recvfrom(260)
            if len(data) == 260:
                payload = np.frombuffer(data, dtype=np.float32)
                
                if payload[0] == 64:
                    # Capture timestamp once to ensure perfect synchronization
                    current_time = self.get_clock().now().to_msg()
                    frame_id = "vl53l5cx_link"

                    # 1. Populate Image Message
                    img_msg = Image()
                    img_msg.header.stamp = current_time
                    img_msg.header.frame_id = frame_id
                    img_msg.height, img_msg.width = 8, 8
                    img_msg.encoding = '16UC1' 
                    img_msg.step = 16 
                    
                    distances_mm = replace_zeros_with_max_range(payload[1:]).astype(np.uint16)
                    img_msg.data = distances_mm.tobytes() 
                    
                    # 2. Populate CameraInfo Message
                    info_msg = CameraInfo()
                    info_msg.header.stamp = current_time
                    info_msg.header.frame_id = frame_id
                    info_msg.height, info_msg.width = 8, 8
                    info_msg.distortion_model = "plumb_bob"
                    info_msg.d = [0.0, 0.0, 0.0, 0.0, 0.0] # No distortion
                    
                    # Intrinsic camera matrix (K)
                    # [fx,  0, cx]
                    # [ 0, fy, cy]
                    # [ 0,  0,  1]
                    info_msg.k = [9.656, 0.0, 4.0, 
                                  0.0, 9.656, 4.0, 
                                  0.0, 0.0, 1.0]
                    
                    # Projection matrix (P)
                    info_msg.p = [9.656, 0.0, 4.0, 0.0, 
                                  0.0, 9.656, 4.0, 0.0, 
                                  0.0, 0.0, 1.0, 0.0]

                    # Publish both messages
                    self.img_pub.publish(img_msg)
                    self.info_pub.publish(info_msg)
                    
        except BlockingIOError:
            pass 

def main(args=None):
    rclpy.init(args=args)
    rclpy.spin(VL53L5CXNode())
    rclpy.shutdown()

if __name__ == '__main__':
    main()