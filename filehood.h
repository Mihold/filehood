/**
 * Filehood (Simple File Transfer)
 * Transferring a file between two hosts
 *
 * Filehood protocol main routines header
 *
 * ------------------------------------------------------------------------------------------
 *  Copyright (c) Mykhailo Kutsenko. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 * ------------------------------------------------------------------------------------------
 */

#ifndef FILEHOOD_H
#define FILEHOOD_H

#define FHP_TIMEOUT 300
#define FHP_DISCOVERY_INT 3		// 3 seconds
#define FHP_MAX_NODES 20
#define FHP_INFO_FILE_LEN 511 	// Max size of an info file

#include <stdint.h>
#include "net.h"

typedef struct
{
    uint8_t  main;
    uint8_t  fix;
    uint16_t build;
} __attribute__((__packed__))
fhp_td_version;

typedef struct  // Size - 116 bytes (must be less than 516 bytes)
{
    uint16_t tftp_opcode;
    uint16_t tftp_block;
    char magic[4];
    fhp_td_version version;
    char reserv1[8];       // fill in by 0x00
    unsigned char uuid[16];
    char reserv2[16];
    char name[64];
} __attribute__((__packed__))
fhp_td_peer_info;

typedef struct
{
    net_tp_peer_addr ip4;		// IPv4
    //uint16_t ip6[8];	// IPv6
    fhp_td_peer_info *info;
}
fhp_td_peer;


/**
 * Discovery neighbors
 *    input: timeout - timeout interval in milliseconds
 *           peer_limit - max number of peers
 *           peers[] - pointer on peer list arrey
 *    return: amount of discovered nodes
 */
int fhp_discovery(int timeout, int peer_limit, fhp_td_peer* peers);

#endif // FILEHOOD_H