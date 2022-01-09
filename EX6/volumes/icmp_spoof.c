#include <stdio.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <netinet/ip.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#define BUFFER 2048


// define ICMP struct
struct icmpheader 
{
	unsigned char type;
	unsigned char code;
	unsigned short int checksum;
	unsigned short int id; 
	unsigned short int seq;
};


//  IP struct
struct ipheader 
{
	
	unsigned char ip_hl: 4; 
	unsigned char ip_v: 4; 
	unsigned short int ip_off: 13; 
	unsigned short int ip_flag: 3; 
	unsigned char ip_tos;
	unsigned short int ip_len;
	unsigned short int ip_id;
	unsigned char ip_ttl;
	unsigned char ip_protocol;
	unsigned short int ip_checksum;
	struct in_addr source_ip;
	struct in_addr destination_ip;
};



// etherent struct
struct ethheader 
{
	u_char ether_dhost [6];
	u_char ether_shost [6];
	u_short ether_type;
};


// CheckSum Function. Taken from BSD website 
unsigned short in_cksum(unsigned short *addr, int len)
{
    int nleft = len;
    int sum = 0;
    unsigned short *w = addr;
    unsigned short answer = 0;

    while (nleft > 1)
    {
        sum += *w++;
        nleft -= 2;
    }

    if (nleft == 1)
    {
        *(unsigned char *)(&answer) = *(unsigned char *)w;
        sum += answer;
    }

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    answer = ~sum;
    return (answer);
}


void send_packet(struct ipheader* ip_packet)
{
	// Construct  the packet
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

	// Send the packet us in the sendto (that beacsue it a raw socket) function
	sendto(sock, ip_packet, ntohs(ip_packet->ip_len), 0, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
	printf("RAW PACKET SENT\n");
	printf("Sent from :%s\n",inet_ntoa(ip_packet->source_ip));
	printf("TO: %s\n", inet_ntoa(ip_packet->destination_ip));
	close(sock);
}


int main()
{
	char buf[BUFFER];
	memset(buf, 0, BUFFER);
	
	// Filling the ICMP header with relevant informatiom
	struct icmpheader *icmp_header = (struct icmpheader *) (buf + sizeof(struct ipheader));
	icmp_header->type = 8; // Set icmp type 8 as echo request
	icmp_header->checksum = 0;
	icmp_header->checksum = in_cksum((unsigned short *)icmp_header, sizeof(struct icmpheader));

	// Filling the IP header with relevant informatiom
	struct ipheader *ip = (struct ipheader *)buf;
	ip->ip_v = 4;
	ip->ip_hl = 5;
	ip->ip_ttl = 64;
	ip->ip_tos =16;
	ip->source_ip.s_addr = inet_addr("1.2.3.4"); // unreal IP
	ip->destination_ip.s_addr = inet_addr("10.0.2.5");
	ip->ip_protocol = IPPROTO_ICMP;
	ip->ip_len=htons(sizeof(struct ipheader) + sizeof(struct icmpheader));

	send_packet(ip);
	return 0;
}
