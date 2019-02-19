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
#include <sys/time.h>

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

int fhp_last_pr = 0;
uint32_t fhp_peer_id = 0;
char id_str[]="xx-xxxx";


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

const char fhp_file_name[sizeof(FHP_INFOFILE)] = FHP_INFOFILE;
const char fhp_magic[4] = {'F', 'H', 'P', 'r'};
const char fhp_tftp_mode[sizeof(TFTP_MODE)] = TFTP_MODE;


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
        char file_name[sizeof(FHP_INFOFILE)];
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
    if (peer_info == NULL)
    {
        fprintf(stderr, "ERR {filehood} Out of memory.\n");
        exit(2);
    }
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
                    (*((uint32_t*)&(peer_info->magic)) == *((uint32_t*) &fhp_magic)))
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
                        if (peer_info == NULL)
                        {
                            fprintf(stderr, "ERR {filehood} Out of memory.\n");
                            exit(2);
                        }
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

void fhp_progress(long sent, long size)
{
    int pr, i;

    if (!sent)
    {
        fhp_last_pr = 0;
        printf("00%% [__________________________________________________]");
        fflush(stdout);
    }
    else if (sent == size)
    {
        printf("\r100%% [##################################################]\n");
    }
    else
    {
        pr = (int) ((sent >> 9) * 100 / (size >> 9));
        if (fhp_last_pr != pr)
        {
            printf("\r%02i%% [", pr);
            fhp_last_pr = pr;
            pr = pr >> 1;
            for (i = 0; i < pr; i++)
                printf("#");
            for (i = pr; i < 50; i++)
                printf("_");
            printf("]");
            fflush(stdout);
        }
    }
}

// Send a file to a peer
void fhp_send(FILE* inptr, fhp_td_peer* peer, char* filename)
{
    int tftp_tid, j, err, block_size, tftp_wrq_size;
    int filename_len, fhp_tftp_ack_block;
    uint16_t i;
    char* tftp_buffer;
    long sz, fhp_sent;
    net_tp_peer_addr fhp_peer_ip, fhp_tftp_asc_ip;
    time_t timer;
    struct tftp_ack* fhp_tftp_ack;


    // Get the file size
    fseek(inptr, 0L, SEEK_END);
    sz = ftell(inptr);
    if (sz == 0)
    {
        fprintf(stderr, "ERR {filehood} The file is empty.\n");
        return;
    }
    rewind(inptr);

    err = 0;
    
    // Initialize TFTP client on random UDP port
    tftp_tid = net_server_init(0);
    fhp_peer_ip = peer->ip4;

    // Send WRQ
    tftp_buffer = malloc(516);      // 2(opcode)+2(block)+512(data)
    fhp_tftp_ack = calloc(1, 516);  // 2(opcode)+2(errcode)+512(errmes)
    if ((fhp_tftp_ack == NULL) || (tftp_buffer == NULL))
    {
        fprintf(stderr, "ERR {filehood} Out of memory.\n");
        exit(2);
    }
    filename_len = strlen(filename) + 1;
    tftp_wrq_size = 2 + sizeof(TFTP_MODE) + filename_len;
    if (tftp_wrq_size > 514)
    {
        fprintf(stderr, "ERR {filehood} The filename is too long.\n");
        exit(2);
    }
    *((uint16_t*) tftp_buffer) = net_encode(TFTP_OPCODE_WRQ);
    strcpy(tftp_buffer + 2, filename);
    strcpy(tftp_buffer + 2 + filename_len, &fhp_tftp_mode[0]);
    fhp_peer_ip.peer_port = FHP_SERVER_PORT;
    net_server_send_packet(tftp_tid, (void*) tftp_buffer, tftp_wrq_size, &fhp_peer_ip);
    timer = time(NULL);     // init the timer
    if (timer == -1)
    {
        fprintf(stderr, "ERR {filehood} Cannot initialize the timer.\n");
        err = 1;
    }
    else
        timer += (time_t) FHP_TIMEOUT;
    while ((time(NULL) < timer) && (fhp_peer_ip.peer_port == FHP_SERVER_PORT) && !err)
    {
        if (net_server_get_packet(tftp_tid, fhp_tftp_ack, 516, &fhp_tftp_asc_ip) == 4)
        {
            // The packet has to be ACK0 from the peer
            if ((fhp_tftp_asc_ip.peer_addr == fhp_peer_ip.peer_addr) &&
                (net_decode(fhp_tftp_ack->tftp_opcode) == TFTP_OPCODE_ACK) &&
                (net_decode(fhp_tftp_ack->tftp_blockNum) == 0))
            {
                fhp_peer_ip.peer_port = fhp_tftp_asc_ip.peer_port;
            }
        }
    }

    fhp_tftp_ack_block = 0;
    if ((fhp_peer_ip.peer_port != FHP_SERVER_PORT) && !err)
    {
        // Send the file to the peer
        fhp_sent = 0;
        fhp_progress(fhp_sent, sz);
        *((uint16_t*) tftp_buffer) = net_encode(TFTP_OPCODE_DAT);
        i = 1;
        while (((block_size = fread((void*) (tftp_buffer + 4), 1, 512, inptr)) == 512) && !err)
        {
            *((uint16_t*) (tftp_buffer + 2)) = net_encode((uint16_t) i);
            j = 4;      // 4 attempt to send the block
            while ((j != 0) && (fhp_tftp_ack_block != i))
            {
                net_server_send_packet(tftp_tid, (void*) tftp_buffer, 516, &fhp_peer_ip);
                // Wait for ACK
                timer = time(NULL);     // init the timer
                if (timer == -1)
                {
                    fprintf(stderr, "ERR {filehood} Cannot initialize the timer.\n");
                    err = 1;
                    break;
                }
                timer += (time_t) 3;        // Timeout
                
                // Waiting for ACK
                while ((time(NULL) < timer) && (fhp_tftp_ack_block != i))
                {
                    if (net_server_get_packet(tftp_tid, fhp_tftp_ack, 516, &fhp_tftp_asc_ip) == 4)
                    {
                        // The packet has to be ACK from the peer
                        if ((fhp_tftp_asc_ip.peer_addr == fhp_peer_ip.peer_addr) &&
                            (fhp_tftp_asc_ip.peer_port == fhp_peer_ip.peer_port) &&
                            (net_decode(fhp_tftp_ack->tftp_opcode) == TFTP_OPCODE_ACK) &&
                            (net_decode(fhp_tftp_ack->tftp_blockNum) == i))
                        {
                            fhp_tftp_ack_block = i;
                            fhp_sent += block_size;
                            fhp_progress(fhp_sent, sz);
                        }
                    }
                }
                if ((fhp_tftp_ack_block != i) && (j != 1))      // Do not show the message after the last attempt
                    fprintf(stderr, "INF {filehood} TFTP: Resend DAT%i.\n", i);
                j--;
            }
            if (fhp_tftp_ack_block != i)
            {
                fprintf(stderr, "ERR {filehood} TFTP timeout.\n");
                err = 1;
            }
            else
                i++;
        }

        // Send the last block
        if ((fhp_tftp_ack_block != i) && !err)
        {
            *((uint16_t*) (tftp_buffer + 2)) = net_encode((uint16_t) i);
            j = 4;      // 4 attempt to send the block
            while ((j != 0) && (fhp_tftp_ack_block != i))
            {
                net_server_send_packet(tftp_tid, (void*) tftp_buffer, block_size + 4, &fhp_peer_ip);
                // Wait for ACK
                timer = time(NULL);     // init the timer
                if (timer == -1)
                {
                    fprintf(stderr, "ERR {filehood} Cannot initialize the timer.\n");
                    break;
                }
                timer += (time_t) 3;        // Timeout
                
                // Waiting for ACK
                while ((time(NULL) < timer) && (fhp_tftp_ack_block != i))
                {
                    if (net_server_get_packet(tftp_tid, fhp_tftp_ack, 516, &fhp_tftp_asc_ip) == 4)
                    {
                        // The packet has to be ACK from the peer
                        if ((fhp_tftp_asc_ip.peer_addr == fhp_peer_ip.peer_addr) &&
                            (fhp_tftp_asc_ip.peer_port == fhp_peer_ip.peer_port) &&
                            (net_decode(fhp_tftp_ack->tftp_opcode) == TFTP_OPCODE_ACK) &&
                            (net_decode(fhp_tftp_ack->tftp_blockNum) == i))
                        {
                            fhp_tftp_ack_block = i;
                            fhp_sent += block_size;
                            fhp_progress(fhp_sent, sz);
                        }
                    }
                }
                if ((fhp_tftp_ack_block != i) && (j != 1))      // Do not show the message after the last attempt
                    fprintf(stderr, "INF {filehood} TFTP: Resend DAT%i.\n", i);
                j--;
            }
            if (fhp_tftp_ack_block != i)
                fprintf(stderr, "ERR {filehood} TFTP timeout.\n");
        }
    }
    
    free(tftp_buffer);
    free(fhp_tftp_ack);
    net_close(tftp_tid);
}


// Peer ID generator
uint32_t fhp_id_get(void)
{
    struct timeval cur_time;
    
    if (!fhp_peer_id)
    {
        // Generate a new ID
        
        if (gettimeofday(&cur_time, NULL) == -1)
        {
            fprintf(stderr, "ERR {filehood} Cannot get time.\n");
            exit(2);
        }
        fhp_peer_id = (cur_time.tv_sec << 20) & 0x3ff00000;
        fhp_peer_id += cur_time.tv_usec & 0x000fffff ;
        printf("%li -> %li\n", cur_time.tv_sec, cur_time.tv_sec & 0x04ff);
    }
    return fhp_peer_id;
}

// Decode peer ID
char *fhp_id_decode(uint32_t id)
{
    uint32_t tmp;
    char decode[]="0123456789ACDEFGHJKLMNPQRTUVWXYZ";

    tmp=id;
    for (int i=6; i >= 0; i--)
    {
        id_str[i] = decode[tmp & 0x0000001f];
        tmp = tmp >> 5;
        if (i == 3)
            i--;
    }
    return &(id_str[0]);
}