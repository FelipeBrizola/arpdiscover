#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>

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


char *getAllAvailableIps(int range) {
    int possibleValues = pow(2, range);
    char availableIp[range];

    // [numero de string][tamanho da string]
    char ips[possibleValues][range + 1];

    for(int i = 0; i < possibleValues; i++) {
        // converte inteiro para decimal e preenche com zeros a esqueda
        sprintf(availableIp, "%0*d", range, intToBinary(i));

        // copia valor para lista de ips
        strcpy(ips[i], availableIp);
    }
}

void decimalToBin(int num, char *str) {
    *(str + 8) = '\0';
    int mask = 0x80 << 1;
    while(mask >>= 1)
        *str++ = !!(mask & num) + '0';
}

void ipToBinary(struct in_addr *ip, char *binaryMask) {

    int firstPart, secondPart, thirdPart, fourtPart;
    char firstOctet[9], secondOctet[9], thirdOctet[9], fourtOctet[9];

    sscanf(inet_ntoa(*ip), "%d.%d.%d.%d.", &firstPart, &secondPart, &thirdPart, &fourtPart);


    decimalToBin(firstPart, firstOctet);
    decimalToBin(secondPart, secondOctet);
    decimalToBin(thirdPart, thirdOctet);
    decimalToBin(fourtPart, fourtOctet);

    strcpy(binaryMask, firstOctet);

    strcat(binaryMask, secondOctet);
    strcat(binaryMask, thirdOctet);
    strcat(binaryMask, fourtOctet);

}

int getRange(char *binaryMask) {
    int i = strcspn(binaryMask, "0");
    int binaryIpSize = 32;

    return binaryIpSize - i;
}

int main() {
    int fd;
    struct ifreq ifr;
    unsigned char *mac = NULL;

    struct in_addr *ip = malloc(sizeof(struct in_addr));
    struct in_addr *mask = malloc(sizeof(struct in_addr));

    char binaryMask[36];
    char binaryIp[36];

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




    getAllAvailableIps(range);

    close(fd);

    return 0;
}
