from scapy.all import *

bool = True
i = 1
while bool:
	a = IP(dst = '8.8.8.8', ttl = i)#creating variable which changing ttl
	b= ICMP()
	get = sr1(a/b , timeout = 6, verbose=0)

	if get is None:
		print(f"{i} TIME OUT!")
	elif get.type == 0:
		print(f"{i} {get.src}")
		bool= False
	else:
		print (f"{i} {get.src}")
	
	i= i+1
