#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <linux/netfilter.h>
#include <libnetfilter_queue/libnetfilter_queue.h>
#include <string.h>
#include<time.h>
#include<sys/time.h>
#include<linux/ip.h>
#include<linux/tcp.h>
#include<linux/udp.h>
#include<linux/icmp.h>

#define MATCH 0
#define NMATCH 1

int enable_flag = 1;
unsigned int controlled_protocol = 0;
unsigned short controlled_srcport = 0;
unsigned short controlled_dstport = 0;
unsigned int controlled_saddr = 0;
unsigned int controlled_daddr = 0; 

struct iphdr *piphdr;
static int fd;//ʹ��netfilter_queue��ʹ�õ��ı���
static struct nfq_handle *h;//ʹ��netfilter_queue��ʹ�õ��ı���
static struct nfq_q_handle *qh;//ʹ��netfilter_queue��ʹ�õ��ı���
static struct nfnl_handle *nh;//ʹ��netfilter_queue��ʹ�õ��ı���

static int callback(struct nfq_q_handle *qh, struct nfgenmsg *nfmsg, struct nfq_data *nfa, void *data)//ʹ��netfilter_queueҪ�õ��ĺ��������ж԰����о�
{
	int id = 0;
	struct nfqnl_msg_packet_hdr *ph;
  	unsigned char *pdata = NULL;
  	int pdata_len;
	int dealmethod = NF_DROP;
	char srcstr[32],deststr[32];
	ph = nfq_get_msg_packet_hdr(nfa);
	if (ph == NULL)
		return 1;
	id = ntohl(ph->packet_id);
	if (enable_flag == 0)
		return nfq_set_verdict(qh, id, NF_ACCEPT, 0, NULL);	
	pdata_len = nfq_get_payload(nfa, (unsigned char**)&pdata);
	if (pdata != NULL)
		piphdr = (struct iphdr *)pdata;
	else 
		return 1;
	inet_ntop(AF_INET,&(piphdr->saddr),srcstr,32);
	inet_ntop(AF_INET,&(piphdr->daddr),deststr,32);
 	printf("get a packet: %s -> %s\n",srcstr,deststr);

	if(piphdr->protocol == controlled_protocol) 
		if (piphdr->protocol  == 1)  //ICMP packet
			dealmethod = icmp_check();
		else if (piphdr->protocol  == 6) //TCP packet
			dealmethod = tcp_check();
		else if (piphdr->protocol  == 17) //UDP packet
			dealmethod = udp_check();
		else	{
			printf("Unkonwn type's packet! \n");
			dealmethod = NF_ACCEPT;
		}
	else 
 			dealmethod = NF_ACCEPT;
	return nfq_set_verdict(qh, id, dealmethod, 0, NULL);
}



int main(int argc, char **argv)
{ 
	char buf[1600];
   int length;
   if (argc == 1)
		enable_flag = 0;
	else { 
		getpara(argc, argv);
		printf("input info: p = %d, x = %d y = %d m = %d n = %d \n", controlled_protocol,controlled_saddr,controlled_daddr,controlled_srcport,controlled_dstport);
}
//	printf("opening library handle\n");
	h = nfq_open();
	if (!h) {
		fprintf(stderr, "error during nfq_open()\n");
		exit(1);
	}

//	printf("unbinding existing nf_queue handler for AF_INET (if any)\n");
	if (nfq_unbind_pf(h, AF_INET) < 0) {
		fprintf(stderr, "already nfq_unbind_pf()\n");
		exit(1);
	}

//	printf("binding nfnetlink_queue as nf_queue handler for AF_INET\n");
	if (nfq_bind_pf(h, AF_INET) < 0) {
		fprintf(stderr, "error during nfq_bind_pf()\n");
		exit(1);
	}

//	printf("binding this socket to queue '0'\n");
	qh = nfq_create_queue(h,0, &callback, NULL);
	if (!qh) {
		fprintf(stderr, "error during nfq_create_queue()\n");
		exit(1);
	}

//	printf("setting copy_packet mode\n");
	if (nfq_set_mode(qh, NFQNL_COPY_PACKET, 0xffff) < 0) {
		fprintf(stderr, "can't set packet_copy mode\n");
		exit(1);
	}

	nh = nfq_nfnlh(h);
	fd = nfnl_fd(nh);
   	while(1)
    	{
		length=recv(fd,buf,1600,0);//�˴�����հ�
		nfq_handle_packet(h, buf,length);//��ɷ�������������
	}
	nfq_destroy_queue(qh);
	nfq_close(h);
	exit(0);
}



void display_usage(char *commandname)
{
	printf("Usage 1: %s \n", commandname);
	printf("Usage 2: %s -x saddr -y daddr -m srcport -n dstport \n", commandname);
}

int getpara(int argc, char *argv[]){
	int optret;
	unsigned short tmpport;
	optret = getopt(argc,argv,"pxymnh");
	while( optret != -1 ) {
			//printf(" first in getpara: %s\n",argv[optind]);
        	switch( optret ) {
        	case 'p':
        		if (strncmp(argv[optind], "ping",4) == 0 )
					controlled_protocol = 1;
				else if ( strncmp(argv[optind], "tcp",3) == 0  )
					controlled_protocol = 6;
				else if ( strncmp(argv[optind], "udp",3) == 0 )
					controlled_protocol = 17;
				else {
					printf("Unkonwn protocol! please check and try again! \n");
					exit(1);
				}
        		break;
         case 'x':   //get source ipaddr 
				if ( inet_aton(argv[optind], (struct in_addr* )&controlled_saddr) == 0){
					printf("Invalid source ip address! please check and try again! \n ");
					exit(1);
				}
         	break;
         case 'y':   //get destination ipaddr
				if ( inet_aton(argv[optind], (struct in_addr* )&controlled_daddr) == 0){
					printf("Invalid destination ip address! please check and try again! \n ");
					exit(1);
				}
         	break;
         case 'm':   //get destination ipaddr
				tmpport = atoi(argv[optind]);
				if (tmpport == 0){
					printf("Invalid source port! please check and try again! \n ");
					exit(1);
				}
				controlled_srcport = htons(tmpport);
         	break;
        case 'n':   //get destination ipaddr
				tmpport = atoi(argv[optind]);
				if (tmpport == 0){
					printf("Invalid source port! please check and try again! \n ");
					exit(1);
				}
				controlled_dstport = htons(tmpport);
         	break;
         case 'h':   /* fall-through is intentional */
         case '?':
         	display_usage(argv[0]);
         	exit(1);;
                
         default:
				printf("Invalid parameters! \n ");
         	display_usage(argv[0]);
         	exit(1);;
        	}
		optret = getopt(argc,argv,"pxymnh");
	}
}



int port_check(unsigned short srcport, unsigned short dstport){
	if ((controlled_srcport== 0 ) && ( controlled_dstport == 0 ))
		return MATCH;
	if ((controlled_srcport != 0 ) && ( controlled_dstport == 0 ))
	{
		if (controlled_srcport == srcport) 
			return MATCH;
		else
			return NMATCH;
	}
	if ((controlled_srcport== 0 ) && ( controlled_dstport != 0 ))
	{
		if (controlled_dstport == dstport) 
			return MATCH;
		else
			return NMATCH;
	}
	if ((controlled_srcport != 0 ) && ( controlled_dstport != 0 ))
	{
		if ((controlled_srcport == srcport) && (controlled_dstport == dstport)) 
			return MATCH;
		else
			return NMATCH;
	}
	return NMATCH;
}


int ipaddr_check(unsigned int saddr, unsigned int daddr){
	if ((controlled_saddr == 0 ) && ( controlled_daddr == 0 ))
		return MATCH;
	if ((controlled_saddr != 0 ) && ( controlled_daddr == 0 ))
	{
		if (controlled_saddr == saddr) 
			return MATCH;
		else
			return NMATCH;
	}
	if ((controlled_saddr == 0 ) && ( controlled_daddr != 0 ))
	{
//		printf("controlled_daddr,%d,daddr,%d.\n",controlled_daddr,daddr);
		if (controlled_daddr == daddr) 
			return MATCH;
		else
			return NMATCH;
	}
	if ((controlled_saddr != 0 ) && ( controlled_daddr != 0 ))
	{
		if ((controlled_saddr == saddr) && (controlled_daddr == daddr)) 
			return MATCH;
		else
			return NMATCH;
	}
	return NMATCH;
}

int icmp_check(void){
	struct icmphdr *picmphdr;

	picmphdr = (struct icmphdr *)((char *)piphdr +(piphdr->ihl*4));
	if((picmphdr->type != 0)&&(picmphdr->type != 8))  	  
  		return NF_ACCEPT;
	else   //only controlling the echo type
	{
 		
  		if ((picmphdr->type == 0)&&(ipaddr_check(piphdr->daddr,piphdr->saddr) == MATCH)){
			printf("an ICMP packet is denied.\n");
 			return NF_DROP;	
		}
		else if ((picmphdr->type == 8)&&(ipaddr_check(piphdr->saddr,piphdr->daddr) == MATCH)){
			printf("an ICMP packet is denied.\n");			
			return NF_DROP;
		}
		else
	     		return NF_ACCEPT;
	}
}

int tcp_check(void){
	struct tcphdr *ptcphdr;
   	//printf("<0>This is an tcp packet.\n");
   	ptcphdr = (struct tcphdr *)((char *)piphdr +(piphdr->ihl*4));
	if ((ipaddr_check(piphdr->saddr,piphdr->daddr) == MATCH) && (port_check(ptcphdr->source,ptcphdr->dest) == MATCH)){
		 printf("block an tcp packet.\n");
		return NF_DROP;
	}
	else
      return NF_ACCEPT;
}

int udp_check(void){
	struct udphdr *pudphdr;	
   	//printf("<0>This is an udp packet.\n");
   	pudphdr = (struct udphdr *)((char *)piphdr +(piphdr->ihl*4));
	if ((ipaddr_check(piphdr->saddr,piphdr->daddr) == MATCH) && (port_check(pudphdr->source,pudphdr->dest) == MATCH) ){
		 printf("block an udp packet.\n");
		return NF_DROP;
	}
	else
      return NF_ACCEPT;
}
