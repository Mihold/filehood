/**
 * Filehood (Simple File Transfer)
 * Transferring a file between two hosts
 *
 * UI for CLI Linux
 * Usage: ./filehood -s|-r file_name|directory
 *
 * ------------------------------------------------------------------------------------------
 *  Copyright (c) Mykhailo Kutsenko. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 * ------------------------------------------------------------------------------------------
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "filehood.h"

#define MSG_USAGE "Usage: ./filehood s file_name\n" \
                  "       ./filehood r node_name [directory]\n"

#define MSG_LICENSE "Filehood for Linux v0.01a\n" \
                    "Copyright (c) 2017 Mykhailo Kutsenko\n" \
                    "Licensed under the MIT License.\n" \
                    "\n"


int main(int argc, char* argv[])
{
    fhp_td_peer* fh_peer_list;
    int fh_peer_num;
    
    printf(MSG_LICENSE);

    if (argc < 3 || argc > 4)
    {
        printf(MSG_USAGE);
        return 1;
    }
    
    if (argv[1][0] == 's' && argv[1][1] == '\0')
    {
        // Send the file

        // check parameters
        if (argc != 3)
        {
            printf(MSG_USAGE);
            return 1;
        }
        printf("Sending %s\n", argv[2]);

        // open the source file 
        FILE *inptr = fopen(argv[2], "r");
        if (inptr == NULL)
        {
            fprintf(stderr, "ERR Could not open '%s'.\n", argv[2]);
            return 2;
        }
        
        // Requesting a list of peers
        printf("Looking for peers...\n");
        
        // Peer list - limit 20 hosts
        // <number> <name> <peer ID>
        
        // Get the list
        fh_peer_list = malloc(FHP_MAX_NODES * sizeof(fhp_td_peer));
        if (fh_peer_list == NULL)
        {
            fprintf(stderr, "ERR Out of memory.\n");
            return 2;
        }
        fh_peer_num = fhp_discovery(FHP_DISCOVERY_INT, FHP_MAX_NODES, fh_peer_list);
        if (fh_peer_num > 0)
        {
            // Sort the list
            
            // Output formated list
        }
        else
        {
            printf("Didn't find any peers on the network.\n");
        }
        // To do - choose a peer from the list
        
        // To do - send the file to the peer
        
        fclose(inptr);
    }
    else
    {
        printf(MSG_USAGE);
        return 1;
    }
    
    return 0;
}