#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>

int count=0;
void packet_recieved(unsigned char* , int );

int main(int argc, char *argv[]) {
    
    // Now we are creating the raw socket and starting to sniff ICMP packets
    // here we use again, like the prev assigment in the htons, to capture all type of packet
    int raw_socket;
    if ((raw_socket = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1) {
        printf("couldnt create raw socket");
        return -1;
    }

    
    // if everything went well we got the ICMP captured packet
    char buff[IP_MAXPACKET];
    while(1) {
        bzero(buff, IP_MAXPACKET);
        int received = recvfrom(raw_socket, buff, ETH_FRAME_LEN, 0, NULL, NULL);
        packet_recieved(buff, received);
    }
}

void packet_recieved(unsigned char* buffer, int size)
{
    // creating IP header 
    struct iphdr *iph = (struct iphdr*)(buffer+ETH_HLEN);
    
    // if the packet is ICMP One
    if (iph->protocol == IPPROTO_ICMP) {

        unsigned short iphdrlen = iph->ihl*4;
        struct icmphdr *icmph = (struct icmphdr *)(buffer+ETH_HLEN+iphdrlen);

        char *icmp_type_names[] = {"Echo (ping) Reply","Unassigned","Unassigned","Destination Unreachable",
                                    "Source Quench","Redirect","Alternate Host Address","Unassigned",
                                    "Echo (ping)","Router Advertisement","Router Selection","Time Exceeded"};

        unsigned int type = (unsigned int)(icmph->type);
        if(type < 11)
        {
            struct sockaddr_in source, dest;
            memset(&source, 0, sizeof(source));
            source.sin_addr.s_addr = iph->saddr;
            memset(&dest, 0, sizeof(dest));
            dest.sin_addr.s_addr = iph->daddr;

            printf("ICMP Packet Number %d\n",++count);

            printf("\nICMP Header:\n");
            printf("\t Type of ICMP: %d - %s\n", (unsigned int)(icmph->type), icmp_type_names[type]);
            printf("\nIP Header:\n");
            printf("\tSource IP: %s\n",inet_ntoa(source.sin_addr));
            printf("\tDestination IP: %s\n",inet_ntoa(dest.sin_addr));
        }
    }
}