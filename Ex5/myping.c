#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/time.h>

#define IP4_HDRLEN 20 // IPV4 header len without changes 
#define ICMP_HDRLEN 8 // ICMP header len for echo requst
#define DESTINATION_IP "127.0.0.1" // google's IP
#define PING "This is the ping.\n"
#define DATA_LEN 19 

unsigned short calculate_checksum(unsigned short * paddress, int len);

int main ()
{
    //creating ICMP Header
    struct icmp icmphdr; 
    // ICMP-header Message Type (8 bits): ICMP_ECHO_REQUEST
    icmphdr.icmp_type = ICMP_ECHO;
    // Message Code (8 bits): echo request
    icmphdr.icmp_code = 0;
    // Identifier (16 bits): some number to trace the response.
    // It will be copied to the response packet and used to map response to the request sent earlier.
    // Thus, it serves as a Transaction-ID when we need to make "ping"
    icmphdr.icmp_id = 18;
    // Sequence Number (16 bits): starts at 0
    icmphdr.icmp_seq = 0;
    // ICMP header checksum (16 bits): set to 0 not to include into checksum calculation
    icmphdr.icmp_cksum = 0;
    // Combine the packet 
    char packet[IP_MAXPACKET];
    // First, IP header.
    //memcpy (packet, &iphdr, IP4_HDRLEN);
    // Next, ICMP header
    memcpy (packet, &icmphdr, ICMP_HDRLEN);
    // After ICMP header, add the ICMP data.
    memcpy ((packet+ICMP_HDRLEN), PING, DATA_LEN);
    // Calculate the ICMP header checksum
    icmphdr.icmp_cksum = calculate_checksum((unsigned short *)packet, (ICMP_HDRLEN+DATA_LEN));
    memcpy (packet, &icmphdr, ICMP_HDRLEN);

    struct sockaddr_in dest_in;
    memset (&dest_in, 0, sizeof (struct sockaddr_in));
    dest_in.sin_family = AF_INET;

   // dest_in.sin_addr.s_addr = iphdr.ip_dst.s_addr;
    dest_in.sin_addr.s_addr = inet_addr(DESTINATION_IP);

    // Create new Raw socket
    int sock = -1;
    if ((sock = socket (AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1) 
    {
        fprintf (stderr, "socket() failed with error: %d", errno);
        fprintf (stderr, "To create a raw socket, the process needs to be run by Admin/root user.\n\n");
        return -1;
    }

    // Now we are sending a request for echo usinf ICMP protocol.

    struct timeval start_Time, end_Time;
    gettimeofday(&start_Time, NULL); // with this 2 variables we will measure our time at the
                                     // of the program and at the end of the program   
    
    // Send the packet using sendto() functions, initialize sent size to the size of the sendto function if -1 there is an error
    int sent_size  = sendto(sock, packet, (ICMP_HDRLEN+DATA_LEN), 0, (struct sockaddr *)&dest_in, sizeof(dest_in));
     // if faild we raise an error message
    if (sent_size == -1) {
        fprintf (stderr, "sendto() failed with error: %d", errno);
        return -1;
    }
    // else, it means that everything went well and the packet has been sent
    printf("One packet has been sent :\n");
    printf("\tTotal Packetsize: %d bytes: IP Header Length is: %d + ICMP Header length is: %d +  Data length :%d\n", sent_size, ICMP_HDRLEN, DATA_LEN);
    printf("\tData: %s \n", packet+ICMP_HDRLEN);
    // After we recive the ICMP echo replay
    bzero(packet,IP_MAXPACKET);

    socklen_t len = sizeof(dest_in);
    int size_buff = -1;
    while (size_buff < 0) {
        size_buff = recvfrom(sock, packet, sizeof(packet), 0, (struct sockaddr *) &dest_in, &len); //filiing the bufer with the data we sent
    }
    printf("One packet has been recived:\n");
    printf("\tTotal Packetsize: %d bytes: IP Header Length is: %d + ICMP Header length is: %d +  Data length :%d\n", size_buff, IP4_HDRLEN, ICMP_HDRLEN, DATA_LEN);
    gettimeofday(&end_Time, NULL); 

    printf("\tThe Data: %s \n", (packet+ICMP_HDRLEN+IP4_HDRLEN));

    // According to the assigemts demands, we need to return thr RTT in milisec and microsec 
    float milliseconds = (end_Time.tv_sec - start_Time.tv_sec) * 1000.0f + (end_Time.tv_usec - start_Time.tv_usec) / 1000.0f;
    unsigned long microseconds = (end_Time.tv_sec - start_Time.tv_sec) * 1000.0f + (end_Time.tv_usec - start_Time.tv_usec);
    printf("The RTT is: %f milliseconds, %ld microseconds\n", milliseconds, microseconds);

    // Closing the raw socket
    close(sock);
    return 0;
}


// Check sum computes using the extara information we got in the assigment
unsigned short calculate_checksum(unsigned short * paddress, int len)
{
	int nleft = len;
	int sum = 0;
	unsigned short * w = paddress;
	unsigned short answer = 0;

	while (nleft > 1)
	{
		sum += *w++;
		nleft -= 2;
	}

	if (nleft == 1)
	{
		*((unsigned char *)&answer) = *((unsigned char *)w);
		sum += answer;
	}

	// add back carry outs from top 16 bits to low 16 bits
	sum = (sum >> 16) + (sum & 0xffff); // add hi 16 to low 16
	sum += (sum >> 16);                 // add carry
	answer = ~sum;                      // truncate to 16 bits

	return answer;
}