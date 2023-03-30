/*******************************************************************************
 *                    C O P Y R I G H T  (c) 2016-2021
 *             E Y E D R O   G R E E N   S O L U T I O N S   I N C.
 *                             All Rights Reserved
 *******************************************************************************
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF EYEDRO GREEN SOLUTIONS INC.
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO
 * THIS SOFTWARE. THE COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR
 * SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *            The copyright notice above does not evidence any
 *           actual or intended publication of such source code.
 *
 * The software is owned by Eyedro Green Solutions Inc. and/or its supplier,
 * and is protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the user to
 * criminal sanctions under applicable laws, as well as to civil liability for
 * the breach of the terms and conditions of this license.
 ******************************************************************************/
 /**************************************************************************//**
 * @file        EyedroCommon.h
 * @copyright   COPYRIGHT (c) 2021 Eyedro Green Solutions Inc. All rights reserved.
 * @author      Eyedro development team
 * @brief       Constants, types and enumerations common to Eyedro products.
 ******************************************************************************/

#ifndef COMMON_H_
#define COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

// Large buffers used to receive, transmit and prepare data - sizes are application specific and defined in EyedroApp.c
extern char                 g_acRxBuffer[];
extern char                 g_acTxBuffer[];
extern char                 g_acBuffer64[];
extern char                 g_acBuffer256[];
extern char                 g_acLocalRxBuffer[];
extern char                 g_acLocalTxBuffer[];

// Large buffers for command/response FIFO and (raw) reading storage - sizes are application specific and defined in EyedroApp.c
extern char                 g_acCmdFifo[];
extern char                 g_acReadingBuffer1[];
extern char                 g_acReadingBuffer2[];

#define USER_CONFIG_INIT_CODE                0x5A5A

#define CMDCODE_SETEVENT                         (uint8_t)0x50

#define EVENT_STARTUP_V3                         (uint8_t)0x40
#define EVENT_PAYLOAD_LENGTH_STARTUP_V3          1
/** PUBLIC FUNCTION PROTOTYPES ************************************************/



/** DEBUG *********************************************************************/


#ifdef __cplusplus
}
#endif

#endif