from time import sleep

from scapy.layers.inet import IP, ICMP
from scapy.sendrecv import sr

# f = open("ping_results_p.txt", "a")
f = open("ping_results_c.txt", "a")
pack = 0
while True:
    ans, unans = sr(IP(dst="8.8.8.8") / ICMP())
    rx = ans[0][1]
    tx = ans[0][0]
    delta = rx.time - tx.sent_time
    f.write(f"{pack} {delta}\n")
    pack += 1
    sleep(5)
