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

//#include "filehood.h"
//#include "common.h"

#define MSG_USAGE "Usage: ./filehood s file_name\n" \
                  "       ./filehood r node_name [directory]\n"
#define MSG_LICENSE "Filehood for Linux v0.01a\n" \
                    "Copyright (c) 2017 Mykhailo Kutsenko\n" \
                    "Licensed under the MIT License.\n" \
                    "\n"

int main(int argc, char* argv[])
{
    printf(MSG_LICENSE);

 /*   if (argc < 3 || argc > 4)
    {
        printf(MSG_USAGE);
        return 1;
    }
    
    sft_time_init();
    sft_log("INF Program started");
    
    if (argv[1][0] == 'r' && argv[1][1] == '\0')
    {
        // Receive a file
        
        char *dst_dir;
        
        sft_log("INF Switch to receive mode");
        
        if (sft_check_nname(argv[2]) == 0)
        {
            
            // set the target directory
            if (argc == 3)
            {
                dst_dir = "./";
            }
            else
            {
                dst_dir = argv[3];
            }
            
            // receive mode
            int sft_tid = sft_server(argv[2], TFTP_TIMEOUT);
            if (sft_tid == 0)
            {
                fprintf(stderr, "Unable receive a file.\n");
                return 1;
            }
                //sft_receive(dst_dir, TFTP_TIMEOUT)
        }
        else
        {
            printf("Incorrect node name.\n");
            return 1;
        }
    }
    else if (argv[1][0] == 's' && argv[1][1] == '\0')
    {
        // Send a file
        
        int nodes;
        
        sft_log("INF Switch to send mode");

        // check parameters
        if (argc != 3)
        {
            printf(MSG_USAGE);
            return 1;
        }
        
        // open input file 
        sft_log("INF Try to open source file.");
        FILE *inptr = fopen(argv[2], "r");
        if (inptr == NULL)
        {
            sft_log("ERR Could not open source file.");
            return 2;
        }
        
        // discover receivers
        if ((nodes = sft_discovery(TFTP_TIMEOUT)) > 0)
        {
            printf("Discovered %i hosts\n", nodes);
        }
        else
        {
            printf("Couldn't discover any hosts\n");
        }
        
        // send file
        //while (fread(buffer, CHUNCK_SIZE, 1, inptr) == 1)
        
        fclose(inptr);
    }
    else
    {
        printf(MSG_USAGE);
        return 1;
    }
    
    sft_log("INF Done.");*/
    return 0;
}