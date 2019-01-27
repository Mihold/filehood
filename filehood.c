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
#include "net.h"

#define FHP_INFO_FILE_LEN 511
#define FHP_VERSION 1
#define FHP_VERSION_FIX 0
#define FHP_VERSION_BUILD 0
#define FHP_INFOFILE "filehood.info"
#define FHP_SERVER_PORT 1069

const char sft_file_name[14] = FHP_INFOFILE;
const char sft_magic[4] = {'F', 'H', 'P', 'r'};


// Discovery neighbors
int fhp_discovery(int timeout, int peer_limit, fhp_td_peer* peers[])
{
    int tftp_tid;
    
    const struct
    {
        uint16_t opcode;
        char file_name[sizeof FHP_INFOFILE];
        char mode[6];
    } tftp_rrq = {1, FHP_INFOFILE, "octet"};

    // Sending multicast RRQ
    // Prepear a request for gethering peers' info

    // Initiate server on random UDP port
    tftp_tid = net_server_init(0);
    
    // broadcast RRQ request
    net_broadcast(tftp_tid, FHP_SERVER_PORT, (void*) &tftp_rrq, sizeof tftp_rrq);
    
    // Receive all responces till timeout

    return 1;
}
