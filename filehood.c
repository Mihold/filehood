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
#include <string.h>
#include <stdint.h>

#include "filehood.h"
//#include "tftp.h"

#define FHP_INFO_FILE_LEN 511
#define FHP_VERSION 1
#define FHP_VERSION_FIX 0
#define FHP_VERSION_BUILD 0
#define FHP_INFOFILE "filehood.info"

const char sft_file_name[14] = FHP_INFOFILE;
const char sft_magic[4] = {'F', 'H', 'P', 'r'};

typedef struct
{
    uint8_t  main;
    uint8_t  fix;
    uint16_t build;
} __attribute__((__packed__))
fhp_td_version;

typedef struct  // Size - 128 bytes (must be less than 512 bytes)
{
    uint32_t magic;
    fhp_td_version version;
    char reserv1[28];   // fill in by 0x00
    char name[64];
} __attribute__((__packed__))
fhp_td_peer_info;

typedef struct
{
    uint32_t ip4;
    //ip6
    fhp_td_peer_info info;
}
fhp_td_peer;

// Discovery neighbors
int fhp_discovery(int timeout, int peer_limit, fhp_td_peer* peers[])
{
    // Sending multicast RRQ
    // Prepear a request for gethering peers' info

    // Prepare broadcast IP for the network connection
    
    // broadcast RRQ request
    const struct
    {
        uint16_t opcode;
        char file_name[sizeof FHP_INFOFILE];
        char mode[6];
    } tftp_rrq = ( 1, FHP_INFOFILE, "octet");
    //net_send_udp();
    
    // Receive all responces till timeout

    return 1;
}
