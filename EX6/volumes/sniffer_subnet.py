from scapy.all import *

def print_pkt(pkt):
	pkt.show()

interfaces = ['enp0s3','lo']
pkt = sniff(iface=interfaces, filter= 'dst net 128.230.0.0/16', prn=print_pkt)
