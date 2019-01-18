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

/**
 * Discovery mode
 *    input: timeout - timeout interval in milliseconds
 */
int fhp_discovery(int timeout);

#endif // FILEHOOD_H