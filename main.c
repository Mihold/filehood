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
    fhp_td_peer* fh_peer_list, fh_peer_selested;
    int fh_peer_num, i;
    int fh_peer = 0;
    
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
        fh_peer_list = calloc(FHP_MAX_NODES, sizeof(fhp_td_peer));
        if (fh_peer_list == NULL)
        {
            fprintf(stderr, "ERR Out of memory.\n");
            return 2;
        }
        while (!fh_peer)
        {
            fh_peer_num = fhp_discovery(FHP_DISCOVERY_INT, FHP_MAX_NODES, fh_peer_list);
            if (fh_peer_num > 0)
            {
                // Sort the list
                
                // Output formated list
                printf("Peers:\n");
                for (i=1; i<=fh_peer_num; i++)
                {
                    printf("%2i %.30s\n", i, fh_peer_list[i-1].info->name);
                }
                printf("Enter a number from the list to send the file or 0 to rescan the network: ");
            }
            else
            {
                printf("Didn't find any peers on the network.\n");
            }
            // Choose a peer from the list
            while (!scanf("%d", &fh_peer) || (fh_peer < 0) || (fh_peer > (fh_peer_num + 1)))
            {
                while (fgetc(stdin) != '\n');
            }
        }
        fh_peer--;
        fh_peer_selested = fh_peer_list[fh_peer];

        // Free up memory
        for (i=0; i<fh_peer_num; i++)
        {
            if (i != fh_peer)
            {
                free(fh_peer_list[i].info);
                fh_peer_list[i].info = NULL;
            }
        }
        
        // ToDo - send the file to the peer
        printf("Sending the file to %s\n", fh_peer_list[fh_peer].info->name);
        
        fclose(inptr);
        free(fh_peer_selested.info);
        free(fh_peer_list);
    }
    else
    {
        printf(MSG_USAGE);
        return 1;
    }
    
    return 0;
}