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
#define FHP_MAX_NODES 20


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


/**
 * Discovery neighbors
 *    input: timeout - timeout interval in milliseconds
 *           peer_limit - max number of peers
 *           peers[] - pointer on peer list arrey
 *    return: amount of discovered nodes
 */
int fhp_discovery(int timeout, int peer_limit, fhp_td_peer* peers[]);

#endif // FILEHOOD_H