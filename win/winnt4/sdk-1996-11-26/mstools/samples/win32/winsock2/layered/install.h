/*++


     Copyright c 1996 Intel Corporation
     All Rights Reserved
     
     Permission is granted to use, copy and distribute this software and 
     its documentation for any purpose and without fee, provided, that 
     the above copyright notice and this statement appear in all copies. 
     Intel makes no representations about the suitability of this 
     software for any purpose.  This software is provided "AS IS."  
     
     Intel specifically disclaims all warranties, express or implied, 
     and all liability, including consequential and other indirect 
     damages, for the use of this software, including liability for 
     infringement of any proprietary rights, and including the 
     warranties of merchantability and fitness for a particular purpose. 
     Intel does not assume any responsibility for any errors which may 
     appear in this software nor any responsibility to update it.


Module Name:

    install.h

Abstract:

    This module contains the defininitions common the the winsock2 layered
    service provider sample and its installation application
    
Author:

    bugs@brandy.jf.intel.com

Notes:

$Revision:   1.0  $

$Modtime:   16 Jul 1996 10:36:34  $

Revision History:

most-recent-revision-date email-name
description

Original version

--*/

#ifndef _INSTALL_H_
#define _INSTALL_H_

#include "windows.h"

#define CONFIGURATION_KEY TEXT("SOFTWARE\\WinSock2\\Layered Provider Sample")

GUID LayeredProviderGuid = { /* 5a21f160-df30-11cf-8927-00aa00539f1c */
    0x5a21f160,
    0xdf30,
    0x11cf,
    {0x89, 0x27, 0x00, 0xaa, 0x00, 0x53, 0x9f, 0x1c}
};

                            
#endif // _INSTALL_H_
