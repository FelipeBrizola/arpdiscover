#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <math.h>
#include <netinet/ether.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>

#define IP_SIZE  32
#define BUFFER_SIZE 1600
#define MAC_ADDR_LEN 6


unsigned char mac[6];
struct in_addr *ip;
unsigned char buffer_request[BUFFER_SIZE];

int buildArpBuffer ( char *target_ip, int frame_len) {
    char HTYPE[] = {0x00, 0x01};        // HARDWARE TYPE. ETHERNET = 1
	char PTYPE[] = {0x08, 0x00};        // PROTOCOL TYPE. IPv4 = 0x800
	char HLEN[]  = {0x06};              // HARDWARE SIZE = 6
	char PLEN[]  = {0x04};              // PROTOCOL SIZE = 4
	char REQUEST_OPER[]  = {0x00, 0x01};// OPERATION 1 TO REQUEST. 2 TO REPLY
	char REPLY_OPER[]  = {0x00, 0x02};  // OPERATION 1 TO REQUEST. 2 TO REPLY
	
    unsigned char SENDER_MAC[6];
	unsigned char SENDER_IP[4];
	unsigned char TARGET_MAC[6];
	unsigned char TARGET_IP[4];     

    memset(TARGET_MAC, 0, sizeof(TARGET_MAC));
    memset(SENDER_MAC, 0, sizeof(SENDER_MAC));
    memset(SENDER_IP, 0, sizeof(SENDER_IP));
    memset(TARGET_IP, 0, sizeof(TARGET_IP));

    SENDER_MAC[0] = mac[0];
    SENDER_MAC[1] = mac[1];
    SENDER_MAC[2] = mac[2];
    SENDER_MAC[3] = mac[3];
    SENDER_MAC[4] = mac[4];
    SENDER_MAC[5] = mac[5];

    // my ip
	unsigned char firstOctetHex, secondOctetHex, thirdOctetHex, fourthOctetHex; 
	short int firstOctet, secondOctet, thirdOctet, fourthOctet;

	sscanf(inet_ntoa(*ip), "%d.%d.%d.%d.", &firstOctet, &secondOctet, &thirdOctet, &fourthOctet);
    
	firstOctetHex  = (char) firstOctet;
	secondOctetHex = (char) secondOctet;
	thirdOctetHex  = (char) thirdOctet;
	fourthOctetHex = (char) fourthOctet;

	SENDER_IP[0] = firstOctetHex;
	SENDER_IP[1] = secondOctetHex;
	SENDER_IP[2] = thirdOctetHex;
	SENDER_IP[3] = fourthOctetHex;

    // ip target
	sscanf(target_ip, "%d.%d.%d.%d.", &firstOctet, &secondOctet, &thirdOctet, &fourthOctet);

	firstOctetHex  = (char) firstOctet;
	secondOctetHex = (char) secondOctet;
	thirdOctetHex  = (char) thirdOctet;
	fourthOctetHex = (char) fourthOctet;
	
	TARGET_IP[0] = firstOctetHex;
	TARGET_IP[1] = secondOctetHex;
	TARGET_IP[2] = thirdOctetHex;
	TARGET_IP[3] = fourthOctetHex;

    printf("SENDER_MAC : %02X:%02X:%02X:%02X:%02X:%02X\n" , SENDER_MAC[0], SENDER_MAC[1], SENDER_MAC[2], SENDER_MAC[3], SENDER_MAC[4], SENDER_MAC[5]);
    printf("SENDER_IP  : %d.%d.%d.%d \n", SENDER_IP[0], SENDER_IP[1], SENDER_IP[2], SENDER_IP[3] );
    printf("TARGET_MAC : %02X:%02X:%02X:%02X:%02X:%02X\n" , TARGET_MAC[0], TARGET_MAC[1], TARGET_MAC[2], TARGET_MAC[3], TARGET_MAC[4], TARGET_MAC[5]);
    printf("TARGET_IP  : %d.%d.%d.%d \n", TARGET_IP[0], TARGET_IP[1], TARGET_IP[2], TARGET_IP[3] );
    printf("\n");   

    	/* ARP */
	memcpy(buffer_request + frame_len, HTYPE, sizeof(HTYPE));
    frame_len += sizeof(HTYPE);

    memcpy(buffer_request + frame_len, PTYPE, sizeof(PTYPE));
    frame_len += sizeof(PTYPE);

    memcpy(buffer_request + frame_len, HLEN, sizeof(HLEN));
    frame_len += sizeof(HLEN);

    memcpy(buffer_request + frame_len, PLEN, sizeof(PLEN));
    frame_len += sizeof(PLEN);

    memcpy(buffer_request + frame_len, REQUEST_OPER, sizeof(REQUEST_OPER));
    frame_len += sizeof(REQUEST_OPER);

    memcpy(buffer_request + frame_len, SENDER_MAC, sizeof(SENDER_MAC));
    frame_len += sizeof(SENDER_MAC);

    memcpy(buffer_request + frame_len, SENDER_IP, sizeof(SENDER_IP));
    frame_len += sizeof(SENDER_IP);

    memcpy(buffer_request + frame_len, TARGET_MAC, sizeof(TARGET_MAC));
    frame_len += sizeof(TARGET_MAC);

    memcpy(buffer_request + frame_len, TARGET_IP, sizeof(TARGET_IP));
    frame_len += sizeof(TARGET_IP);

    // coisa mais bizarra do mundo, mas se tirar esse for nao funciona mais
    for(int i = 0; i < sizeof(buffer_request); i++)       

    return frame_len;

}


int intToBinary(int n) {
    int remainder;
    int binary = 0, i = 1;

    while(n != 0) {
        remainder = n%2;
        n = n/2;
        binary= binary + (remainder*i);
        i = i*10;
    }
    return binary;
}

int binaryToInt(char *binaryCharArray) {
    char *start = &binaryCharArray[0];
    int total = 0;
    while (*start) {
        total *= 2;
        if (*start++ == '1')
            total += 1;
    }
    return total;
}


void intToBinOctet(int num, char *str) {
    *(str + 8) = '\0';
    int mask = 0x80 << 1;
    while(mask >>= 1)
        *str++ = !!(mask & num) + '0';
}

void ipToBinary(struct in_addr *ip, char *binaryMask) {

    int firstPart, secondPart, thirdPart, fourthPart;
    char firstOctet[9], secondOctet[9], thirdOctet[9], fourthOctet[9];

    sscanf(inet_ntoa(*ip), "%d.%d.%d.%d.", &firstPart, &secondPart, &thirdPart, &fourthPart);


    intToBinOctet(firstPart, firstOctet);
    intToBinOctet(secondPart, secondOctet);
    intToBinOctet(thirdPart, thirdOctet);
    intToBinOctet(fourthPart, fourthOctet);

    strcpy(binaryMask, firstOctet);

    strcat(binaryMask, secondOctet);
    strcat(binaryMask, thirdOctet);
    strcat(binaryMask, fourthOctet);

}

void binaryIpToDecimalIp(char *binaryIp, char *ret) {
    char firstPartStr[8], secondPartStr[8], thirtPartStr[8], fourthPartStr[8];
    int firstPartInt, secondPartInt, thirtPartInt, fourthPartInt;

    memset(firstPartStr,  '0', 8);
    memset(secondPartStr, '0', 8);
    memset(thirtPartStr,  '0', 8);
    memset(fourthPartStr,  '0', 8);
    memset(ret,        '0', IP_SIZE + 3);

    for (int i = 0; i < IP_SIZE; i++) {
        if (i < 8)
            firstPartStr[i] = binaryIp[i];

        else if (i > 7 && i < 16)
            secondPartStr[i - 8] = binaryIp[i];

        else if (i >= 16 && i < 24)
            thirtPartStr[i - 16] = binaryIp[i];

        else if (i >= 24)
            fourthPartStr[i - 24] = binaryIp[i];

    }

    firstPartStr[8] = '\0';
    secondPartStr[8] = '\0';
    thirtPartStr[8] = '\0';
    fourthPartStr[8] = '\0';

    firstPartInt = binaryToInt(firstPartStr);
    secondPartInt = binaryToInt(secondPartStr);
    thirtPartInt = binaryToInt(thirtPartStr);
    fourthPartInt = binaryToInt(fourthPartStr);

    memset(firstPartStr,  '0', 8);
    memset(secondPartStr, '0', 8);
    memset(thirtPartStr,  '0', 8);
    memset(fourthPartStr,  '0', 8);

    sprintf(firstPartStr, "%d", firstPartInt);
    sprintf(secondPartStr, "%d", secondPartInt);
    sprintf(thirtPartStr, "%d", thirtPartInt);
    sprintf(fourthPartStr, "%d", fourthPartInt);

    strcpy(ret, firstPartStr); strcat(ret, ".");
    strcat(ret, secondPartStr); strcat(ret, ".");
    strcat(ret, thirtPartStr); strcat(ret, ".");
    strcat(ret, fourthPartStr);

}

int getRange(char *binaryMask) {

    // encontra primeiro zero
    int i = strcspn(binaryMask, "0");

    return IP_SIZE - i;
}

int main(int argc, char *argv[]) {
    int fd;
    struct ifreq ifr;

    struct ifreq if_idx;
	struct ifreq if_mac;
    struct sockaddr_ll socket_address;
    char ifname[IFNAMSIZ];

    struct in_addr *mask = malloc(sizeof(struct in_addr));
    char binaryMask[IP_SIZE + 4];
    char binaryIp[IP_SIZE + 4];
    char ipbase[IP_SIZE + 4];
    int range;
    char dest_mac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; //broadcast
    short int ethertype = htons(0x0806);

    int frame_len = 0;

    if (argc != 2) {
        printf("param: interface de rede\n");
        return 1;
    }

    strcpy(ifname, argv[1]);

 
    /* Cria um descritor de socket do tipo RAW */
	if ((fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1) {
		perror("socket");
		exit(1);
	}

	/* Obtem o indice da interface de rede */
	memset(&if_idx, 0, sizeof (struct ifreq));
	strncpy(if_idx.ifr_name, ifname, IFNAMSIZ - 1);
	if (ioctl(fd, SIOCGIFINDEX, &if_idx) < 0) {
		perror("SIOCGIFINDEX");
		exit(1);
	}

	/* Obtem o endereco MAC da interface local */
	memset(&if_mac, 0, sizeof (struct ifreq));
	strncpy(if_mac.ifr_name, ifname, IFNAMSIZ - 1);
	if (ioctl(fd, SIOCGIFHWADDR, &if_mac) < 0) {
        perror("SIOCGIFHWADDR");
		exit(1); 
	}	

    mac[0] = if_mac.ifr_hwaddr.sa_data[0],
    mac[1] = if_mac.ifr_hwaddr.sa_data[1],
    mac[2] = if_mac.ifr_hwaddr.sa_data[2],
    mac[3] = if_mac.ifr_hwaddr.sa_data[3],
    mac[4] = if_mac.ifr_hwaddr.sa_data[4],
    mac[5] = if_mac.ifr_hwaddr.sa_data[5];

     
    
    /* Indice da interface de rede */
	socket_address.sll_ifindex = if_idx.ifr_ifindex;

	/* Tamanho do endereco (ETH_ALEN = 6) */
	socket_address.sll_halen = ETH_ALEN;

	/* Endereco MAC de destino */
	memcpy(socket_address.sll_addr, dest_mac, MAC_ADDR_LEN);

	/* Preenche o buffer com 0s */
	memset(buffer_request, 0, BUFFER_SIZE);

	/* Monta o cabecalho Ethernet */

	/* Preenche o campo de endereco MAC de destino */
	memcpy(buffer_request, dest_mac, MAC_ADDR_LEN);
	frame_len += MAC_ADDR_LEN;

	/* Preenche o campo de endereco MAC de origem */
	memcpy(buffer_request + frame_len, if_mac.ifr_hwaddr.sa_data, MAC_ADDR_LEN);
	frame_len += MAC_ADDR_LEN;

	/* Preenche o campo EtherType */
	memcpy(buffer_request + frame_len, &ethertype, sizeof(ethertype));
	frame_len += sizeof(ethertype);

     // ip
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name , ifname , IFNAMSIZ-1);
    if (0 == ioctl(fd, SIOCGIFADDR, &ifr)) {
        struct sockaddr_in *ipaddr = (struct sockaddr_in*)&ifr.ifr_addr;
        ip =  malloc(sizeof(struct in_addr));
        memcpy(ip, &ipaddr->sin_addr, sizeof(struct in_addr));
       
    }

    // mask
    if (0 == ioctl(fd, SIOCGIFNETMASK, &ifr)) {
        struct sockaddr_in *ipaddr = (struct sockaddr_in*)&ifr.ifr_addr;
        memcpy(mask, &ipaddr->sin_addr, sizeof(struct in_addr));   
    }

    printf("\n");
    printf("----------INFORMACOES DE REDE!----------\n\n");
    printf("MAC ADDRESS : %.2X:%.2X:%.2X:%.2X:%.2X:%.2X\n" , mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    printf("IP ADDRESS: %s\n", inet_ntoa(*ip));
    printf("MASK ADDRESS: %s\n", inet_ntoa(*mask));
    printf("\n");



    ipToBinary(mask, binaryMask);
    range = getRange(binaryMask);

    ipToBinary(ip, binaryIp);
   
    // ----- ips validos -----
    int possibleValues = pow(2, range);
    char availableIp[range];

    // [numero de string][tamanho da string]
    char ipsRange[possibleValues][range + 1];
    char ips[possibleValues][IP_SIZE + 1];

    for(int i = 0; i < possibleValues; i++) {
        // converte inteiro para string e preenche com zeros a esqueda
        sprintf(availableIp, "%0*d", range, intToBinary(i));

        // copia valor para lista de ips
        strcpy(ipsRange[i], availableIp);
    }
    // ----- ips validos -----

    strncpy(ipbase, binaryIp, IP_SIZE - range);

    printf("\n");
    printf("----------AGORA O ARP VAI COMER!----------\n\n");

    int frame_len_backup = frame_len;
    for (int i = 1; i < possibleValues; i++) {
        strcpy(ips[i], ipbase);
        strcat(ips[i], ipsRange[i]);

        char p[IP_SIZE + 3];
        binaryIpToDecimalIp(ips[i], p);
       
        frame_len = buildArpBuffer( p, frame_len);        

        /* Envia arp request para target e router*/
        if (sendto(fd, buffer_request, frame_len, 0, (struct sockaddr *) &socket_address, sizeof (struct sockaddr_ll)) < 0) {
            perror("send");
            close(fd);
            exit(1);
        }      
        frame_len = frame_len_backup;

    }

    close(fd);

    return 0;
}
