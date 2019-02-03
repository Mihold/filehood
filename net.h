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

#ifndef NET_H
#define NET_H

typedef struct
{
    uint32_t peer_addr;
    uint16_t peer_port;
}
net_tp_peer_addr;

int net_server_init(int port);
int net_server_get_packet(int sfd, void *buf, int buf_size, net_tp_peer_addr *peer);
void net_server_send_packet(int sfd, void *buf, int buf_size, net_tp_peer_addr *peer);
void net_broadcast(int sfd, int dst_port, void *packet, int packet_len);
uint16_t net_encode(uint16_t i);
uint16_t net_decode(uint16_t i);
int net_close(int sfd);

#endif // NET_H