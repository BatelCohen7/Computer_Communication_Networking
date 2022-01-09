from scapy.all import *

def print_pkt(pkt):
	if pkt[TCP] is not None:
		print("--------TCP PACKET-----")
		print(f"\tSOURCE: {pkt[IP].src}")
		print(f"\tDEST: {pkt[IP].dst}")
		print(f"\t TCP SRC PORT:{pkt[TCP].sport}")
		print(f"\t TCP DEST PORT:{pkt[TCP].dport}")

interfaces=['enp0s3','lo']
pkt= sniff(iface=interfaces, filter='tcp port 23 and src host 10.0.2.5', prn=print_pkt)
