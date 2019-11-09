import socket
import struct

server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
server_socket.bind(('', 12000))

while True:
	data, address = server_socket.recvfrom(1024)
	if data:
		values = data.decode().split(" ")
		for x in range(len(values)):
			values[x] = float(values[x])
		print(values, values[0] + values[1])
		server_socket.sendto(data, address)