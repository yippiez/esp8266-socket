import socket
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(("192.168.1.26", 1342))
m = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
m.connect(("192.168.1.26", 1342))
s.close()
m.send(b"test")
m.recv(4)
m.close()