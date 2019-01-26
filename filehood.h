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

#define TFTP_TIMEOUT 300
#define FHP_MAX_NODES 20

/**
 * Discovery neighbors
 *    input: timeout - timeout interval in milliseconds
 *           peer_limit - max number of peers
 *           peers[] - pointer on peer list arrey
 *    return: amount of discovered nodes
 */
int fhp_discovery(int timeout, int peer_limit, fhp_td_peer* peers[])

#endif // FILEHOOD_H