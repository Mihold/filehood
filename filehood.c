/**
 * Filehood (Simple File Transfer)
 * Transferring a file between two hosts
 *
 * Filehood protocol main routines
 *
 * ------------------------------------------------------------------------------------------
 *  Copyright (c) Mykhailo Kutsenko. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 * ------------------------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "filehood.h"
#include "net.h"

#define FHP_VERSION 1
#define FHP_VERSION_FIX 0
#define FHP_VERSION_BUILD 0
#define FHP_INFOFILE "filehood.info"
#define FHP_SERVER_PORT 1069

#define TFTP_OPCODE_RRQ 0x0100   // net_encode(1) == 0x0100
#define TFTP_OPCODE_WRQ 2
#define TFTP_OPCODE_DAT 3
#define TFTP_OPCODE_ACK 4
#define TFTP_OPCODE_ERR 5

#define TFTP_MODE "octet"

struct tftp_err
{
    uint16_t tftp_opcode;
    uint16_t tftp_errcode;
    char mesage[];
} __attribute__((__packed__));

struct tftp_ack
{
    uint16_t tftp_opcode;
    uint16_t tftp_blockNum;
} __attribute__((__packed__));

const char sft_file_name[14] = FHP_INFOFILE;
const char sft_magic[4] = {'F', 'H', 'P', 'r'};


// Discovery neighbors
int fhp_discovery(int timeout, int peer_limit, fhp_td_peer* peers)
{
    int tftp_tid, tftp_res, i;
    time_t timer;
    int peer_num = 0;
    fhp_td_peer_info *peer_info;
    net_tp_peer_addr peer_ip;
    
    // Prepear a request for gethering peers info
    const struct
    {
        uint16_t opcode;
        char file_name[sizeof FHP_INFOFILE];
        char mode[6];
    } tftp_rrq = {TFTP_OPCODE_RRQ, FHP_INFOFILE, TFTP_MODE};

    const struct tftp_ack tftp_ack1 = {0x0400, 0x0100};


    // Initialize TFTP client on random UDP port
    tftp_tid = net_server_init(0);
    
    // broadcast RRQ request
    net_broadcast(tftp_tid, FHP_SERVER_PORT, (void*) &tftp_rrq, sizeof(tftp_rrq));
    
    // Receive all responces till timeout
    // init a timer
    timer = time(NULL);
    if (timer == -1)
    {
        fprintf(stderr, "ERR {filehood} Cannot initialize the timer.\n");
        return 0;
    }
    timer += (time_t) timeout;
    peer_info = calloc(1, sizeof(fhp_td_peer_info));
    while ((time(NULL) < timer) && (peer_num < peer_limit))
    {
        // Get a DAT1 paccket and it has to be the last packet
        tftp_res = net_server_get_packet(tftp_tid, peer_info, sizeof(fhp_td_peer_info), &peer_ip);
        if (tftp_res)
        {
            if (tftp_res <= sizeof(fhp_td_peer_info))
            {
                peer_info->tftp_opcode = net_decode(peer_info->tftp_opcode);
                peer_info->tftp_block = net_decode(peer_info->tftp_block);
                if ((peer_info->tftp_opcode == TFTP_OPCODE_DAT) &&
                    (peer_info->tftp_block == 1) &&
                    (*((uint32_t*)&(peer_info->magic)) == *((uint32_t*) &sft_magic)))
                {
                    // Send ACK1 to compleat the TFTP session
                    net_server_send_packet(tftp_tid, (void*) &tftp_ack1, sizeof(tftp_ack1), &peer_ip);

                    // Check for duplicates
                    for (i=0; i<peer_num; i++)
                    {
                        if (peer_ip.peer_addr == peers[i].ip4.peer_addr)
                            break;
                    }

                    if (i == peer_num)
                    {
                        // Add a new peer
                        peers[peer_num].ip4 = peer_ip;
                        peer_info->name[63] = 0;
                        peers[peer_num].info = peer_info;
                        peer_info = calloc(1, sizeof(fhp_td_peer_info));
                        peer_num++;
                    }
                }
                else
                {
                    if (peer_info->tftp_opcode == TFTP_OPCODE_ERR)
                    {
                        ((struct tftp_err*) peer_info)->tftp_errcode = net_decode(((struct tftp_err*) peer_info)->tftp_errcode);
                        fprintf(stderr, "ERR {filehood} TFTP: [%i] %s.\n", ((struct tftp_err*) peer_info)->tftp_errcode, ((struct tftp_err*) peer_info)->mesage);
                    }
                    else
                    {
                        fprintf(stderr, "ERR {filehood} Recieved unknown packet.\n");
                    }
                }
            }
            else
            {
                fprintf(stderr, "ERR {filehood} Recieved unknown packet.\n");
            }
        }
        
    }
    free(peer_info);

    net_close(tftp_tid);
    return peer_num;
}
