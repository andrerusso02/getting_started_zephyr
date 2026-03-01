import socket
import struct

UDP_IP = "0.0.0.0" # Listen on all interfaces
UDP_PORT = 5005

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

while True:
    data, addr = sock.recvfrom(24) # 6 floats * 4 bytes = 24 bytes
    # Unpack 6 floats ('f' format character)
    imu_values = struct.unpack('ffffff', data)
    print(f"Received IMU: {imu_values}")