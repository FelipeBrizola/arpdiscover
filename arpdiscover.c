#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>

#define IP_SIZE  32


static char format[] = "ping -c 1 -w 1 %s";
static char command[128];

int ping (char *ip) {
    char str[1024];
	FILE *fpipe;

	sprintf(command, format, ip);

	/* executa comando redirecionando a saida para o pipe. */
	if ( (fpipe = popen(command, "r"))<0){
		perror("popen");
	}

	/* Ignora primeira linha */
	/* PING 10.32.175.200 (10.32.175.200) 56(84) bytes of data. */
	fgets(str, 1023, fpipe);

	/* Verifica se teve sucesso no ping com a segunda linha */
	/* 64 bytes from 10.32.175.200: icmp_req=1 ttl=64 time=0.023 ms */
	fgets(str, 1023, fpipe);
	if(strcmp(str, "\n"))
		return 1;
	else
		return 0;

	fclose(fpipe);
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

    int firstPart, secondPart, thirdPart, fourtPart;
    char firstOctet[9], secondOctet[9], thirdOctet[9], fourtOctet[9];

    sscanf(inet_ntoa(*ip), "%d.%d.%d.%d.", &firstPart, &secondPart, &thirdPart, &fourtPart);


    intToBinOctet(firstPart, firstOctet);
    intToBinOctet(secondPart, secondOctet);
    intToBinOctet(thirdPart, thirdOctet);
    intToBinOctet(fourtPart, fourtOctet);

    strcpy(binaryMask, firstOctet);

    strcat(binaryMask, secondOctet);
    strcat(binaryMask, thirdOctet);
    strcat(binaryMask, fourtOctet);

}

void binaryIpToDecimalIp(char *binaryIp, char *ret) {
    char firstPartStr[8], secondPartStr[8], thirtPartStr[8], fourtPartStr[8];
    int firstPartInt, secondPartInt, thirtPartInt, fourtPartInt;

    memset(firstPartStr,  '0', 8);
    memset(secondPartStr, '0', 8);
    memset(thirtPartStr,  '0', 8);
    memset(fourtPartStr,  '0', 8);
    memset(ret,        '0', IP_SIZE + 3);

    for (int i = 0; i < IP_SIZE; i++) {
        if (i < 8)
            firstPartStr[i] = binaryIp[i];

        else if (i > 7 && i < 16)
            secondPartStr[i - 8] = binaryIp[i];

        else if (i >= 16 && i < 24)
            thirtPartStr[i - 16] = binaryIp[i];

        else if (i >= 24)
            fourtPartStr[i - 24] = binaryIp[i];

    }

    firstPartStr[8] = '\0';
    secondPartStr[8] = '\0';
    thirtPartStr[8] = '\0';
    fourtPartStr[8] = '\0';

    firstPartInt = binaryToInt(firstPartStr);
    secondPartInt = binaryToInt(secondPartStr);
    thirtPartInt = binaryToInt(thirtPartStr);
    fourtPartInt = binaryToInt(fourtPartStr);

    memset(firstPartStr,  '0', 8);
    memset(secondPartStr, '0', 8);
    memset(thirtPartStr,  '0', 8);
    memset(fourtPartStr,  '0', 8);

    sprintf(firstPartStr, "%d", firstPartInt);
    sprintf(secondPartStr, "%d", secondPartInt);
    sprintf(thirtPartStr, "%d", thirtPartInt);
    sprintf(fourtPartStr, "%d", fourtPartInt);

    strcpy(ret, firstPartStr); strcat(ret, ".");
    strcat(ret, secondPartStr); strcat(ret, ".");
    strcat(ret, thirtPartStr); strcat(ret, ".");
    strcat(ret, fourtPartStr);

}

int getRange(char *binaryMask) {

    // encontra primeiro zero
    int i = strcspn(binaryMask, "0");

    return IP_SIZE - i;
}

int main() {
    int fd;
    struct ifreq ifr;
    unsigned char *mac = NULL;

    struct in_addr *ip = malloc(sizeof(struct in_addr));
    struct in_addr *mask = malloc(sizeof(struct in_addr));

    char binaryMask[IP_SIZE + 4];
    char binaryIp[IP_SIZE + 4];

    char ipbase[IP_SIZE + 4];

    int range;

    char iface[] = "enp0s3";

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    //Type of address to retrieve - IPv4 IP address
    ifr.ifr_addr.sa_family = AF_INET;

    //Copy the interface name in the ifreq structure
    strncpy(ifr.ifr_name , iface , IFNAMSIZ-1);

    // mac
    if (0 == ioctl(fd, SIOCGIFHWADDR, &ifr)) {
        mac = malloc(sizeof (struct sockaddr));
        memcpy(mac, ifr.ifr_hwaddr.sa_data, sizeof (struct sockaddr));
        printf("MAC ADDRESS : %.2X:%.2X:%.2X:%.2X:%.2X:%.2X\n" , mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }

    // ip
    if (0 == ioctl(fd, SIOCGIFADDR, &ifr)) {
        struct sockaddr_in *ipaddr = (struct sockaddr_in*)&ifr.ifr_addr;
        memcpy(ip, &ipaddr->sin_addr, sizeof(struct in_addr));
        printf("IP ADDRESS: %s\n", inet_ntoa(*ip));
    }

     // mask
    if (0 == ioctl(fd, SIOCGIFNETMASK, &ifr)) {
        struct sockaddr_in *ipaddr = (struct sockaddr_in*)&ifr.ifr_addr;
        memcpy(mask, &ipaddr->sin_addr, sizeof(struct in_addr));
        printf("MASK ADDRESS: %s\n", inet_ntoa(*mask));
    }

    ipToBinary(mask, binaryMask);
    range = getRange(binaryMask);
    printf("BINARY MASK:  %s\n", binaryMask);

    ipToBinary(ip, binaryIp);
    printf("BINARY IP:  %s\n", binaryIp);

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

    printf("%s\n", ipbase);


    for (int i = 0; i < possibleValues; i++) {
        strcpy(ips[i], ipbase);
        strcat(ips[i], ipsRange[i]);

        char p[IP_SIZE + 3];
        binaryIpToDecimalIp(ips[i], p);

        int success = 0;

        if (i != possibleValues)
            success = ping(p);

        if (success == 1)
            printf("PING SUCCESS: %s\n", p);
        else
            printf("PING FAILED: %s\n", p);

    }

    close(fd);

    return 0;
}
