#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>

int main() {
    int fd;
    struct ifreq ifr;
    unsigned char *mac = NULL;

    struct in_addr *ip = malloc(sizeof(struct in_addr));
    struct in_addr *mask = malloc(sizeof(struct in_addr));

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


    close(fd);

    return 0;
}