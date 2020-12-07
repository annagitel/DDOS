#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //for exit(0);
#include<sys/socket.h>
#include<errno.h> //For errno - the error number
#include<pthread.h>
#include<netdb.h>	//hostend
#include<arpa/inet.h>
#include<netinet/tcp.h>	//Provides declarations for tcp header
#include<netinet/ip.h>	//Provides declarations for ip header
#include <time.h>
#include <stdlib.h>
#include <sys/time.h>

void * receive_ack( void *ptr );
void process_packet(unsigned char* , int);

char * hostname_to_ip(char * );
int get_local_ip (char *);

struct pseudo_header{
    unsigned int source_address;
    unsigned int dest_address;
    unsigned char placeholder;
    unsigned char protocol;
    unsigned short tcp_length;
    struct tcphdr tcp;
};

struct in_addr dest_ip;

int main(int argc, char *argv[]) {
    FILE *fptr;
    fptr = fopen("syns_results_c.txt","w");
    time_t t;
    srand((unsigned) time(&t));
    //Create a raw socket
    int s = socket (AF_INET, SOCK_RAW , IPPROTO_TCP);
    if(s < 0){
        printf ("Error creating socket. Error number : %d . Error message : %s \n" , errno , strerror(errno));
        exit(0);
    }
    else{
        printf("Socket created.\n");
    }

    //Datagram to represent the packet
    char datagram[4096];

    //IP header
    struct iphdr *iph = (struct iphdr *) datagram;

    //TCP header
    struct tcphdr *tcph = (struct tcphdr *) (datagram + sizeof (struct ip));
    struct sockaddr_in  dest;
    struct pseudo_header psh;
    dest_ip.s_addr = inet_addr( "10.0.2.14" );
    int source_port = 43591;
    char source_ip[20];
    int a,b,c,d;
    // Initialization, should only be called once.
    a = rand()%256;
    b = rand()%256;
    c = rand()%256;
    d = rand()%256;

    sprintf(source_ip,"%d.%d.%d.%d",a,b,c,d);

    memset (datagram, 0, 4096);	/* zero out the buffer */

    //printf("Starting sniffer thread...\n")
    int port;
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = dest_ip.s_addr;
    a = rand()%256;
    b = rand()%256;
    c = rand()%256;
    d = rand()%256;
    struct timeval stop, start;
    int count = 0;
  for(int i = 0;i<100;i++) {
      for (port = 1; port < 10000; port++) {
          count++;
          gettimeofday(&start, NULL);
          if (port % 2 == 0)
              a = (a + 1) % 256;
          if (port % 3 == 0)
              b = (b + 1) % 265;
          if (port % 5 == 0)
              c = (c + 2) % 256;
          d = (d + port + a) % 256;
          sprintf(source_ip, "%d.%d.%d.%d", a, b, c, d);
          //Fill in the IP Header
          iph->ihl = 5;
          iph->version = 4;
          iph->tos = 0;
          iph->tot_len = sizeof(struct ip) + sizeof(struct tcphdr);
          iph->id = htons (54321);    //Id of this packet
          iph->frag_off = htons(16384);
          iph->ttl = 64;
          iph->protocol = IPPROTO_TCP;
          iph->check = 0;        //Set to 0 before calculating checksum
          iph->saddr = inet_addr(source_ip);    //Spoof the source ip address
          iph->daddr = dest_ip.s_addr;

          //TCP Header
          tcph->source = htons (source_port);
          tcph->dest = htons (80);
          tcph->seq = htonl(1105024978);
          tcph->ack_seq = 0;
          tcph->doff = sizeof(struct tcphdr) / 4;        //Size of tcp header
          tcph->fin = 0;
          tcph->syn = 1;
          tcph->rst = 0;
          tcph->psh = 0;
          tcph->ack = 0;
          tcph->urg = 0;
          tcph->window = htons (14600);    // maximum allowed window size
          tcph->check = 0; //if you set a checksum to zero, your kernel's IP stack should fill in the correct checksum during transmission
          tcph->urg_ptr = 0;

          //IP_HDRINCL to tell the kernel that headers are included in the packet
          int one = 1;
          const int *val = &one;

          if (setsockopt(s, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0) {
              printf("Error setting IP_HDRINCL. Error number : %d . Error message : %s \n", errno, strerror(errno));
              exit(0);
          }

          tcph->dest = htons (80);
          tcph->check = 0;    // if you set a checksum to zero, your kernel's IP stack should fill in the correct checksum during transmission

          psh.source_address = inet_addr(source_ip);
          psh.dest_address = dest.sin_addr.s_addr;
          psh.placeholder = 0;
          psh.protocol = IPPROTO_TCP;
          psh.tcp_length = htons(sizeof(struct tcphdr));

          memcpy(&psh.tcp, tcph, sizeof(struct tcphdr));


          if (sendto(s, datagram, sizeof(struct iphdr) + sizeof(struct tcphdr), 0, (struct sockaddr *) &dest,
                     sizeof(dest)) < 0) {
              printf("Error sending syn packet. Error number : %d . Error message : %s \n", errno, strerror(errno));
              exit(0);
          }
          else
          	printf("packet sent\n");
          gettimeofday(&stop, NULL);

          fprintf(fptr, "%d %lu\n",count,
                  (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
      }
  }
    fclose(fptr);
    return 0;
}
