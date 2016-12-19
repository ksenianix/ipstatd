#include<netinet/in.h>
#include<errno.h>
#include<netdb.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/ip.h>
#include<net/ethernet.h>  //For ether_header
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/ioctl.h>
#include<sys/time.h>
#include<sys/types.h>
#include<unistd.h>

void ProcessPacket(unsigned char*, int);
void print_ip_header(unsigned char*, int);
void PrintData(unsigned char*, int);

FILE *logfile;
struct sockaddr_in source, dest;
int total = 0, i, j;


int main(int argc, char *argv[]) {
	int saddr_size, data_size;
	struct sockaddr saddr;
	char *iface_sniff = "eth0";

	printf("Source address %s", saddr);
	unsigned char *buffer = (unsigned char *) malloc(65536); //Its Big!

	logfile = fopen("/var/log/ipstatd.log", "a+");
	if (logfile == NULL ) {
		printf("Unable to open log.txt file.");
	}

	int sock_raw = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP));
	setsockopt(sock_raw , SOL_SOCKET , SO_BINDTODEVICE , iface_sniff , strlen(iface_sniff)+ 1 );

	if (sock_raw < 0) {
		//Print the error with proper message
		perror("Socket Error");
		return 1;
	}
	while (1) {
		saddr_size = sizeof saddr;
		//Receive a packet
		data_size = recvfrom(sock_raw, buffer, 65536, 0, &saddr,
				(socklen_t*) &saddr_size);
		if (data_size < 0) {
			printf("Recvfrom error , failed to get packets\n");
			return 1;
		}
		//Now process the packet
		ProcessPacket(buffer, data_size);
	}
	close(sock_raw);
	printf("Finished");
	return 0;
}

void ProcessPacket(unsigned char* buffer, int size) {
	//Get the IP Header part of this packet , excluding the ethernet header

	struct iphdr *iph = (struct iphdr*) buffer;
	++total;
	printf( "Total : %d\r",  total);
}

void print_ip_header(unsigned char* Buffer, int Size) {

	unsigned short iphdrlen;

	struct iphdr *iph = (struct iphdr *) (Buffer + sizeof(struct ethhdr));
	iphdrlen = iph->ihl * 4;

	memset(&source, 0, sizeof(source));
	source.sin_addr.s_addr = iph->saddr;
	fprintf(logfile, inet_ntoa(source.sin_addr));
// check if IP sent anything to this host before

}

void PrintData(unsigned char* data, int Size) {
	int i, j;
	for (i = 0; i < Size; i++) {
		if (i != 0 && i % 16 == 0)  //if one line of hex printing is complete...
				{
			fprintf(logfile, "         ");
			for (j = i - 16; j < i; j++) {
				if (data[j] >= 32 && data[j] <= 128)
					fprintf(logfile, "%c", (unsigned char) data[j]); //if its a number or alphabet

				else
					fprintf(logfile, "."); //otherwise print a dot
			}
			fprintf(logfile, "\n");
		}

		if (i % 16 == 0)
			fprintf(logfile, "   ");
		fprintf(logfile, " %02X", (unsigned int) data[i]);

		if (i == Size - 1)  //print the last spaces
				{
			for (j = 0; j < 15 - i % 16; j++) {
				fprintf(logfile, "   "); //extra spaces
			}

			fprintf(logfile, "         ");

			for (j = i - i % 16; j <= i; j++) {
				if (data[j] >= 32 && data[j] <= 128) {
					fprintf(logfile, "%c", (unsigned char) data[j]);
				} else {
					fprintf(logfile, ".");
				}
			}

			fprintf(logfile, "\n");
		}
	}
}
