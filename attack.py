import datetime
from datetime import datetime
import random
import socket
import struct

from scapy.layers.inet import IP, TCP
from scapy.sendrecv import send

f = open("syns_results_p.txt", "a")
time = datetime.now().time()
start_time = time
for it in range(10):
    for syn in range(10):
        src = socket.inet_ntoa(struct.pack('>I', random.randint(1, 0xffffffff)))
        ip = IP(src=src, dst='8.8.8.8')
        SYN = TCP(sport=40508, dport=80, flags="S", seq=it * 10000 + syn)
        send(ip / SYN)
        time = datetime.now().time()
        f.write(f"{it * 10000 + syn} {str(time)}\n")

dateTimeA = datetime.datetime.combine(datetime.date.today(), time)
dateTimeB = datetime.datetime.combine(datetime.date.today(), start_time)
total_time = dateTimeA - dateTimeB
f.write(f"avg time: {total_time.total_seconds() / 1000000}")
