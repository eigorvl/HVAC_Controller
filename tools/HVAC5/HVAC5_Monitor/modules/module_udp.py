##udp_listener()
##send_udp()
##decode_packet()

import socket, json, time

# отправка пакета
def send_udp(data):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    msg = json.dumps(data).encode("utf-8")
    sock.sendto(msg, ("localhost", 9998))
