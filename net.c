/**
 * Filehood (Simple File Transfer)
 * Transferring a file between two hosts
 *
 * Filehood protocol network routines
 *
 * ------------------------------------------------------------------------------------------
 *  Copyright (c) Mykhailo Kutsenko. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 * ------------------------------------------------------------------------------------------
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <linux/if.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <sys/select.h>

// teporaryly
#include <stdio.h>
#include <errno.h>
#include <arpa/inet.h>

//#include "common.h"
#include "net.h"

#define LISTEN_BACKLOG 3


/**
 * open listening port
 *    input: port - server port number for UDP
 *    return: socket idetifer or -1 on error
 */
int net_server_init(int port)
{
    struct sockaddr_in server_addr;
    socklen_t server_addr_size = sizeof server_addr;
    int sfd;
    
    // init server
    memset(&server_addr, 0, server_addr_size);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;  // IP: 0.0.0.0
    
    if ((sfd = socket(server_addr.sin_family, SOCK_DGRAM, 0)) != -1)
    {
        if (bind(sfd, (struct sockaddr *) &server_addr, server_addr_size) == -1)
        {
            fprintf(stderr, "ERR {net} bind error %s\n", strerror(errno));
            
            close(sfd);
            sfd = -1;
        }
    }
    
    return sfd;
}


/**
 * Get incomeing message
 *    input: socket idetifer
 */
int net_server_get_packet(int sfd, void *buf, int buf_size, net_tp_peer_addr *peer)
{
    struct sockaddr_in peer_addr;
    socklen_t peer_addr_len = sizeof(peer_addr);
    int res;
    struct timeval tv = {.tv_sec = 0, .tv_usec = 0};
    fd_set rfds;
    
    FD_ZERO(&rfds);
    FD_SET(sfd, &rfds);
    
    res = select(sfd + 1, &rfds, NULL, NULL, &tv);
    if (res == -1)
    {
        fprintf(stderr, "ERR {net} select error %s\n", strerror(errno));
        res = 0;
    }
    else if (res != 0)
    {
        res = recvfrom(sfd, buf, buf_size, 0, (struct sockaddr *) &peer_addr, &peer_addr_len);
        if (res == -1)
        {
            fprintf(stderr, "ERR {net} recvfrom error %s\n", strerror(errno));
            res = 0;
        }
        else if (res < 4 || res > 516)
        {
            fprintf(stderr, "ERR {net} unknown datagram\n");
            res = 0;
        }
        else
        {
            peer->peer_addr = peer_addr.sin_addr.s_addr;
            peer->peer_port = ntohs(peer_addr.sin_port);
        }
    }

    return res;
}


/**
 * Send a message
 *    input: socket idetifer
 */
void net_server_send_packet(int sfd, void *buf, int buf_size, net_tp_peer_addr *peer)
{
    struct sockaddr_in peer_addr;
    socklen_t peer_addr_len = sizeof(struct sockaddr_in);
    int res;
    
    memset(&peer_addr, 0, peer_addr_len);
    peer_addr.sin_family = AF_INET;
    peer_addr.sin_port = htons(peer->peer_port);
    peer_addr.sin_addr.s_addr = peer->peer_addr;
    
    // send the packet
    res = sendto(sfd, buf, buf_size, 0, (struct sockaddr *) &peer_addr, peer_addr_len);
    if (res == -1)
    {
        fprintf(stderr, "ERR {net} sendto error %s\n", strerror(errno));
    }
}



/**
 * 
 * broadcast a packet on all interfaces
 *    input: socket idetifer
 */
void net_broadcast(int sfd, int dst_port, void *packet, int packet_len)
{
    struct sockaddr_in peer_addr;
    struct ifaddrs *ifaddr, *ifa;
    socklen_t peer_addr_len = sizeof(struct sockaddr_in);
    int res;
    
    memset(&peer_addr, 0, peer_addr_len);
    peer_addr.sin_family = AF_INET;
    peer_addr.sin_port = htons(dst_port);
    
    // find interface for broadcast
    if (getifaddrs(&ifaddr) == 0)
    {
        // enable broadcast on the socket
        int broadcastEnable=1;
        if (setsockopt(sfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) == -1)
        {
            fprintf(stderr, "ERR {net} Cannot enable broadcast on the socket. error - %s\n", strerror(errno));
        }
        for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
        {
            if ((ifa->ifa_flags & (IFF_UP + IFF_BROADCAST + IFF_POINTOPOINT + IFF_RUNNING)) == (IFF_UP + IFF_BROADCAST + IFF_RUNNING)
                 && (ifa->ifa_addr->sa_family == AF_INET))
            {
                peer_addr.sin_addr.s_addr = ((struct sockaddr_in *) ifa->ifa_addr)->sin_addr.s_addr;
                peer_addr.sin_addr.s_addr |= ~((struct sockaddr_in *) ifa->ifa_netmask)->sin_addr.s_addr;
                //printf(stderr, "INF {net} Can broadcast on %s [%s]\n", ifa->ifa_name, inet_ntoa(peer_addr.sin_addr));
                
                // send the packet
                //printf(stderr, "INF {net} broadcast the packet.\n");
                res = sendto(sfd, packet, packet_len, 0, (struct sockaddr *) &peer_addr, peer_addr_len);
                if (res == -1)
                {
                    fprintf(stderr, "ERR {net} sendto error %s\n", strerror(errno));
                }
            }
        }
    }
    freeifaddrs(ifaddr);
}


/**
 * convert bite order from net to host
 *    input: short int in net format
 */
uint16_t net_decode(uint16_t i)
{
    return ntohs(i);
}

/**
 * convert bite order from host to net
 *    input: short int in host format
 */
uint16_t net_encode(uint16_t i)
{
    return htons(i);
}

/**
 * close connection
 *    input: socket idetifer
 */
int net_close(int sfd)
{
    close(sfd);
    
    return 1;
}
