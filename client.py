import socket
import sys
import struct

client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
address = ('127.0.0.1', 12000)
for message in sys.stdin:
	message = message.rstrip() #remove spaces to the right of the string
	client_socket.sendto(message.encode(), address)
	data, server = client_socket.recvfrom(1024)