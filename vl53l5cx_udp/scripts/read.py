import socket
import struct

UDP_IP = "0.0.0.0" # Listen on all interfaces (captures broadcasts)
UDP_PORT = 5005
PAYLOAD_SIZE = 260 # 65 floats * 4 bytes

# Setup UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

print(f"Listening for VL53L5CX data on port {UDP_PORT}...")

while True:
    data, addr = sock.recvfrom(PAYLOAD_SIZE)
    
    # Ensure we received the exact expected payload size
    if len(data) == PAYLOAD_SIZE:
        # Unpack 65 floats ('65f')
        unpacked_data = struct.unpack('65f', data)
        
        # First float is the number of zones
        num_zones = int(unpacked_data[0])
        
        # The remaining 64 floats are the distance measurements
        distances = unpacked_data[1:]
        
        print(f"\nReceived from {addr} | Zones: {num_zones}")
        
        # Format and print the data as an 8x8 or 4x4 grid
        
        if num_zones == 64:
            for row in range(8):
                row_data = distances[row*8 : (row+1)*8]
                print(" ".join([f"{d:6.2f}" for d in row_data]))
        elif num_zones == 16:
            for row in range(4):
                row_data = distances[row*4 : (row+1)*4]
                print(" ".join([f"{d:6.2f}" for d in row_data]))
        else:
            print(f"Unexpected number of zones: {num_zones}")
            
    else:
        print(f"Ignored packet of incorrect size: {len(data)} bytes")