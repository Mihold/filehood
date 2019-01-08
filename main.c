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
            printf("ERR Could not open '%s'.\n", argv[2]);
            return 2;
        }

        // To do - request a list of peers

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