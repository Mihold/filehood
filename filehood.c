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
#define FHP_MAX_NODES 20
#define FHP_VERSION 1
#define FHP_VERSION_FIX 0
#define FHP_VERSION_BUILD 0

const char sft_file_name[12] = "filehood.info";
const char sft_magic[4] = {'F', 'H', 'P', 'r'};

typedef struct
{
    uint32_t magic;
    uint32_t version;
    char reservd[8];   // fill in by 0x00
    char name[64];
} __attribute__((__packed__))
fhp_td_peer_info;

typedef struct
{
    uint8_t  main;
    uint8_t  fix;
    uint16_t build;
} __attribute__((__packed__))
fhp_td_version;


/**
 * Discovery neighbors
 *    input: timeout - timeout interval in milliseconds
 *    return: amount of discovered nodes
 */
int fhp_discovery(int timeout)
{
   // tftp_td_files_list *file_list;
    
   // if ((file_list = tftp_get_broadcast_mem((char *) &sft_file_name[0], SFT_MAX_NODES, SFT_INFO_FILE_LEN, timeout)) == NULL)
   //     return 0;
    return 1;
}
