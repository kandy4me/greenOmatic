/*
 * RF.h
 *
 *  Created on: 2016-02-21
 *      Author: jeff
 */

#ifndef RF_H_
#define RF_H_

/// Includes
// greenOmatic System
#include "SystemConfig.h"
#include "ReceivedMessage.h"
// RF Device Drivers
#if defined(INTERFACE_ASK_RX) || defined(INTERFACE_ASK_TX)
    #include "../arduino/RadioHead/RadioHead.h"
    #include "../arduino/RadioHead/RH_ASK.h"
#endif
#ifdef INTERFACE_NRF905
    #include "../arduino/SPI/SPI.h"
    #include "../arduino/RadioHead/RadioHead.h"
    #include "../arduino/RadioHead/RH_NRF905.h"
#endif


/// Functions
// ASK 1-wire
#ifdef INTERFACE_ASK_RX
bool SetupAskRx();
bool RxAskMessage();
#endif
#ifdef INTERFACE_ASK_TX
bool SetupAskTx();
bool TxAskMessage(unsigned char *mem, size_t length);
#endif
// NRF-905
#ifdef INTERFACE_NRF905
bool RxNrfMessage();
bool TxNrfMessage(unsigned char *mem, size_t length);
#endif // #ifdef INTERFACE_NRF905

#ifdef INTERFACE_NRF905
    extern RH_NRF905 NRF_905;
#endif
#ifdef INTERFACE_ASK_RX
    extern RH_ASK ASK_Receiver;
#endif


#endif /* RF_H_ */
