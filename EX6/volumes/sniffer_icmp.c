#include <pcap.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>

// etharnet struct.
struct ethheader 
{
	u_char ether_dhost [6];
	u_char ether_shost [6];
	u_short ether_type;
};


// IPV4 struct.
struct ipheader 
{
	unsigned char ip_hl: 4; 
	unsigned char ip_v: 4;
	unsigned short int ip_flag: 3; 
	unsigned short int ip_off: 13; 
	struct in_addr source_ip;
	struct in_addr destination_ip;
	// unsigned char ip_tos;      // this is more fields that ip have
	// unsigned short int ip_len; // but in this function we are not using them
	// unsigned short int ip_id;
	// unsigned char ip_ttl;
	// unsigned char ip_protocol;
	// unsigned short int ip_checksum;
};



void got_packet(u_char *args, const struct pcap_pkthdr *header,
const u_char *packet)
{
	printf("A Packet has been Captured\n");
	struct ethheader *e = (struct ethheader *)packet;
	if( ntohs(e->ether_type)== 0x0800)
	{
		struct ipheader *ip = (struct ipheader*) (packet + sizeof(struct ethheader));
		printf("  Source: %s\n", inet_ntoa(ip->source_ip));
		printf("  Destination: %s\n",inet_ntoa(ip->destination_ip));

}
}
int main()
{
pcap_t *handle;
char errbuf[PCAP_ERRBUF_SIZE];
struct bpf_program fp;
char filter_exp[] = "host 10.9.0.5 or host 10.9.0.1";
bpf_u_int32 net;
handle = pcap_open_live("br-d922e3bc743e", BUFSIZ, 1, 1000, errbuf);
// Step 2: Compile filter_exp into BPF psuedo-code
pcap_compile(handle, &fp, filter_exp, 0, net);
if (pcap_setfilter(handle, &fp) !=0) {
pcap_perror(handle, "Error:");
exit(EXIT_FAILURE);
}
// Step 3: Capture packets
pcap_loop(handle, -1, got_packet, NULL);
pcap_close(handle); //Close the handle
return 0;
}

