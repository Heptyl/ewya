#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ChipPn7160Task"

#include "ChipPn7160Task.h"
#include "XbhLog.h"
#include "XbhMWThread.h"

#include "XbhService.h"


ChipPn7160Task *ChipPn7160Task::mInstance = XBH_NULL;
XbhMutex ChipPn7160Task::mLockObject;
XBH_S32 ChipPn7160Task::mPGpio;
XBH_S32 ChipPn7160Task::mPLevel;
XBH_S32 ChipPn7160Task::mI2cBus;
XBH_S32 ChipPn7160Task::mI2cAddr;
/*********************************************************************************/

namespace Xbhpn7160 
{

#ifndef __PROJ_NFC_API_INCLUDE_H__
#define __PROJ_NFC_API_INCLUDE_H__
enum NFC_PROT_ENUM{
	NFC_PROT_UNDETERMINED    = 0x0,
	NFC_PROT_T1T             = 0x1,
	NFC_PROT_T2T             = 0x2,
	NFC_PROT_T3T             = 0x3,
	NFC_PROT_ISODEP          = 0x4,
	NFC_PROT_NFCDEP          = 0x5,
	NFC_PROT_T5T             = 0x6,
	NFC_PROT_MIFARE          = 0x80,
};


enum NFC_MODE_TECH{
	NFC_TECH_PASSIVE_NFCA    = 0x0,
	NFC_TECH_PASSIVE_NFCB    = 0x1,
	NFC_TECH_PASSIVE_NFCF    = 0x2,
	NFC_TECH_ACTIVE_NFC      = 0x3,
	NFC_TECH_PASSIVE_15693   = 0x6,
};



#define NFC_MAX_ID_LEN  32
struct nfc_card_info_t{
	unsigned char id[NFC_MAX_ID_LEN];
	unsigned char id_len;
    enum NFC_PROT_ENUM protocol;
    enum NFC_MODE_TECH mode_tech;
};


struct nfc_card_ops{
    int (* detect)(struct nfc_card_info_t*);
    void (* remove)(struct nfc_card_info_t*);
    void (* idle)();
};


struct nfc_device_t{
	struct nfc_card_ops* ops;
};


void NFC_Register(struct nfc_card_ops* ops);

int NFC_Init(void);

void NFC_Handle_Process(void);

int NFC_T2T_Read(unsigned char block, unsigned char* buf, int len);

int NFC_T2T_Write(unsigned char block, unsigned char* buf, int len);

int NFC_ISODEP_Select_PPSE(unsigned char* ppse, int len, unsigned char* ret, int ret_size);

int NFC_T5T_Read(unsigned char block, unsigned char* buf, int len);

int NFC_T5T_Write(unsigned char block, unsigned char* buf, int len);

int NFC_Mifare_Auth(unsigned char block, unsigned char key, unsigned char pwd[6]);

int NFC_Mifare_Read(unsigned char block, unsigned char* buf, int len);

int NFC_Mifare_Write(unsigned char block, unsigned char* buf, int len);


#endif //__PROJ_NFC_API_INCLUDE_H__

/*
*         Copyright (c), NXP Semiconductors Caen / France
*
*                     (C)NXP Semiconductors
*       All rights are reserved. Reproduction in whole or in part is
*      prohibited without the written consent of the copyright owner.
*  NXP reserves the right to make changes without notice at any time.
* NXP makes no warranty, expressed, implied or statutory, including but
* not limited to any implied warranty of merchantability or fitness for any
*particular purpose, or that the use will not infringe any third party patent,
* copyright or trademark. NXP must not be liable for any loss or damage
*                          arising from its use.
*/

/***** NFC dedicated setting ****************************************/

/* Following definitions specifies which settings will apply when NxpNci_ConfigureSettings()
 * API is called from the application
 */
#define NXP_CORE_CONF        1
#define NXP_CORE_STANDBY     1
#define NXP_CORE_CONF_EXTN   1
#define NXP_CLK_CONF         1 // 1=Xtal, 2=PLL
#define NXP_TVDD_CONF        1 // 1=3.3V, 2=4.75V
#define NXP_RF_CONF          1

XBH_U8 NxpNci_SettingCurrentTS[32] = {0};

#if NXP_CORE_CONF
/* NCI standard dedicated settings
 * Refer to NFC Forum NCI standard for more details
 */
XBH_U8 NxpNci_CORE_CONF[]={0x20, 0x02, 0x05, 0x01,         /* CORE_SET_CONFIG_CMD */
    0x00, 0x02, 0xFE, 0x01                                  /* TOTAL_DURATION */
};
#endif

#if NXP_CORE_CONF_EXTN
/* NXP-NCI extension dedicated setting
 * Refer to NFC controller User Manual for more details
 */
XBH_U8 NxpNci_CORE_CONF_EXTN[]={0x20, 0x02, 0x05, 0x01,    /* CORE_SET_CONFIG_CMD */
    0xA0, 0x40, 0x01, 0x00                                  /* TAG_DETECTOR_CFG */
};
#endif

#if NXP_CORE_STANDBY
/* NXP-NCI standby enable setting
 * Refer to NFC controller User Manual for more details
 */
XBH_U8 NxpNci_CORE_STANDBY[]={0x2F, 0x00, 0x01, 0x00};    /* last byte indicates enable/disable */
#endif

#if NXP_CLK_CONF
/* NXP-NCI CLOCK configuration
 * Refer to NFC controller Hardware Design Guide document for more details
 */
 #if (NXP_CLK_CONF == 1)
 /* Xtal configuration */
 XBH_U8 NxpNci_CLK_CONF[]={0x20, 0x02, 0x05, 0x01,        /* CORE_SET_CONFIG_CMD */
   0xA0, 0x03, 0x01, 0x08                                  /* CLOCK_SEL_CFG */
 };
 #else
 /* PLL configuration */
 XBH_U8 NxpNci_CLK_CONF[]={0x20, 0x02, 0x09, 0x02,        /* CORE_SET_CONFIG_CMD */
   0xA0, 0x03, 0x01, 0x11,                                 /* CLOCK_SEL_CFG */
   0xA0, 0x04, 0x01, 0x01                                  /* CLOCK_TO_CFG */
 };
 #endif
#endif

#if NXP_TVDD_CONF
/* NXP-NCI TVDD configuration
 * Refer to NFC controller Hardware Design Guide document for more details
 */
 #if (NXP_TVDD_CONF == 1)
 /* TXLDO output voltage set to 3.3V */
 XBH_U8 NxpNci_TVDD_CONF[]={0x20, 0x02, 0x0F, 0x01, 0xA0, 0x0E, 0x0B, 0x11, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x10, 0x00, 0x10, 0x0C};
 #else
 /* TXLDO output voltage set to 4.75V */
 XBH_U8 NxpNci_TVDD_CONF[]={0x20, 0x02, 0x0F, 0x01, 0xA0, 0x0E, 0x0B, 0x11, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x40, 0x00, 0xD0, 0x0C};
 #endif
#endif

#if NXP_RF_CONF
/* NXP-NCI RF configuration
 * Refer to NFC controller Antenna Design and Tuning Guidelines document for more details
 */
/* Following configuration relates to performance optimization of OM27160 NFC Controller demo kit */
XBH_U8 NxpNci_RF_CONF[]={0x20, 0x02, 0x4C, 0x09,
  0xA0, 0x0D, 0x03, 0x78, 0x0D, 0x02,
  0xA0, 0x0D, 0x03, 0x78, 0x14, 0x02,
  0xA0, 0x0D, 0x06, 0x4C, 0x44, 0x65, 0x09, 0x00, 0x00,
  0xA0, 0x0D, 0x06, 0x4C, 0x2D, 0x05, 0x35, 0x1E, 0x01,
  0xA0, 0x0D, 0x06, 0x82, 0x4A, 0x55, 0x07, 0x00, 0x07,
  0xA0, 0x0D, 0x06, 0x44, 0x44, 0x03, 0x04, 0xC4, 0x00,
  0xA0, 0x0D, 0x06, 0x46, 0x30, 0x50, 0x00, 0x18, 0x00,
  0xA0, 0x0D, 0x06, 0x48, 0x30, 0x50, 0x00, 0x18, 0x00,
  0xA0, 0x0D, 0x06, 0x4A, 0x30, 0x50, 0x00, 0x08, 0x00
};
#endif
/*
*         Copyright (c), NXP Semiconductors Caen / France
*
*                     (C)NXP Semiconductors
*       All rights are reserved. Reproduction in whole or in part is
*      prohibited without the written consent of the copyright owner.
*  NXP reserves the right to make changes without notice at any time.
* NXP makes no warranty, expressed, implied or statutory, including but
* not limited to any implied warranty of merchantability or fitness for any
*particular purpose, or that the use will not infringe any third party patent,
* copyright or trademark. NXP must not be liable for any loss or damage
*                          arising from its use.
*/

/***** NFC dedicated interface ****************************************/

/*
 * Status code definition used as API returned values
 */
#define NFC_XBH_TRUE          0
#define NFC_XBH_FALSE            1

/*
 * Flag definition used as Interface values
 */
#define INTF_UNDETERMINED    0x0
#define INTF_FRAME           0x1
#define INTF_ISODEP          0x2
#define INTF_NFCDEP          0x3
#define INTF_TAGCMD          0x80

/*
 * Flag definition used as Protocol values
 */
#define PROT_UNDETERMINED    0x0
#define PROT_T1T             0x1
#define PROT_T2T             0x2
#define PROT_T3T             0x3
#define PROT_ISODEP          0x4
#define PROT_NFCDEP          0x5
#define PROT_T5T             0x6
#define PROT_MIFARE          0x80

/*
 * Flag definition used as Mode values
 */
#define MODE_POLL            0x00
#define MODE_LISTEN          0x80
#define MODE_MASK            0xF0

/*
 * Flag definition used as Technologies values
 */
#define TECH_PASSIVE_NFCA    0
#define TECH_PASSIVE_NFCB    1
#define TECH_PASSIVE_NFCF    2
#define TECH_ACTIVE_NFC      3
#define TECH_PASSIVE_15693   6

/*
 * Flag definition used for NFC library configuration
 */
#define NXPNCI_MODE_CARDEMU    (1<<0)
#define NXPNCI_MODE_P2P        (1<<1)
#define NXPNCI_MODE_RW         (1<<2)

/*
 * Definition of operations handled when processing Reader mode
 */
typedef enum
{
#ifndef REMOVE_NDEF_SUPPORT
    READ_NDEF=0,
    WRITE_NDEF=1,
#endif //#ifndef REMOVE_NDEF_SUPPORT
    PRESENCE_CHECK=2
} NxpNci_RW_Operation_t;

/*
 * Definition of discovered remote device properties information
 */
/* POLL passive type A */
typedef struct
{
    unsigned char SensRes[2];
    unsigned char NfcIdLen;
    unsigned char NfcId[10];
    unsigned char SelResLen;
    unsigned char SelRes[1];
    unsigned char RatsLen;
    unsigned char Rats[20];
} NxpNci_RfIntf_info_APP_t;

/* POLL passive type B */
typedef struct
{
    unsigned char SensResLen;
    unsigned char SensRes[12];
    unsigned char AttribResLen;
    unsigned char AttribRes[17];
} NxpNci_RfIntf_info_BPP_t;

/* POLL passive type F */
typedef struct
{
    unsigned char BitRate;
    unsigned char SensResLen;
    unsigned char SensRes[18];
} NxpNci_RfIntf_info_FPP_t;

/* POLL passive type ISO15693 */
typedef struct
{
    unsigned char AFI;
    unsigned char DSFID;
    unsigned char ID[8];
} NxpNci_RfIntf_info_VPP_t;

typedef union
{
    NxpNci_RfIntf_info_APP_t NFC_APP;
    NxpNci_RfIntf_info_BPP_t NFC_BPP;
    NxpNci_RfIntf_info_FPP_t NFC_FPP;
    NxpNci_RfIntf_info_VPP_t NFC_VPP;
} NxpNci_RfIntf_Info_t;

/*
 * Definition of discovered remote device properties
 */
typedef struct
{
    unsigned char Interface;
    unsigned char Protocol;
    unsigned char ModeTech;
    XBH_BOOL MoreTags;
    NxpNci_RfIntf_Info_t Info;
} NxpNci_RfIntf_t;

/**********************************************************************/

/***** NFC dedicated API **********************************************/

/*
 * Open connection to the NXP-NCI device
 * return NFC_XBH_TRUE or NFC_XBH_FALSE
 */
XBH_BOOL NxpNci_Connect(void);

/*
 * Close connection to the NXP-NCI device
 * return NFC_XBH_TRUE or NFC_XBH_FALSE
 */
XBH_BOOL NxpNci_Disconnect(void);

/*
 * Return NXP-NCI device FW version (3 bytes)
 */
void NxpNci_GetFwVersion(unsigned char fw[3]);

/*
 * Configure NXP-NCI device settings
 * Related settings are defined in Nfc_settings.h header file
 * To be called after NxpNci_Connect() and prior to NxpNci_ConfigureMode() APIs
 * return NFC_XBH_TRUE or NFC_XBH_FALSE
 */
XBH_BOOL NxpNci_ConfigureSettings(void);

/*
 * Configure NXP-NCI device mode
 * - mode: specifies which modes to be configured (see NXPNCI_MODE_xxx flags)
 * return NFC_XBH_TRUE or NFC_XBH_FALSE
 */
XBH_BOOL NxpNci_ConfigureMode(unsigned char mode);

/*
 * Configure NXP-NCI device parameters
 * - pCmd: NCI CORE_SET_CONFIG_CMD frame
 * - CmdSize: NCI CORE_SET_CONFIG_CMD frame size
 * return NFC_XBH_TRUE or NFC_XBH_FALSE
 */
XBH_BOOL NxpNci_ConfigureParams(unsigned char *pCmd, unsigned short CmdSize);

/*
 * Start NFC Discovery loop for remote NFC device detection
 * - pTechTab: list of NFC technologies to look for (see TECH_xxx_xxx flags)
 * - TechTabSize: number of items in the list
 * return NFC_XBH_TRUE or NFC_XBH_FALSE
 */
XBH_BOOL NxpNci_StartDiscovery(unsigned char *pTechTab, unsigned char TechTabSize);

/*
 * Stop NFC Discovery loop
 * return NFC_XBH_TRUE or NFC_XBH_FALSE
 */
XBH_BOOL NxpNci_StopDiscovery(void);

/*
 * Wait until remote NFC device is discovered
 * - pRfIntf: filled with discovered NFC remote device properties
 * return NFC_XBH_TRUE or NFC_XBH_FALSE
 */
XBH_BOOL NxpNci_WaitForDiscoveryNotification(NxpNci_RfIntf_t *pRfIntf);

/**********************************************************************/

/***** Reader/writer dedicated APIs ***********************************/
#ifndef REMOVE_RW_SUPPORT
#ifndef REMOVE_NDEF_SUPPORT
/*
 * Register NDEF message to be written to remote NFC Tag
 * - pMessage: pointer to the NDEF message
 * - Message_size: NDEF message size
 * - pCb: pointer to function to be called back when tag has been written
 * return NFC_XBH_TRUE or NFC_XBH_FALSE
 */
XBH_BOOL RW_NDEF_SetMessage(unsigned char *pMessage, unsigned short Message_size, void *pCb);

/*
 * Register function called when NDEF message is read from remote NFC Tag
 * - pCb: pointer to function to be called back when NDEF message has been read
 */
void RW_NDEF_RegisterPullCallback(void *pCb);
#endif //#ifndef REMOVE_NDEF_SUPPORT

/*
 * Process the operation identified as parameter with discovered remote NFC tag (function is blocking until the end of the operation)
 * - RfIntf: discovered NFC device properties
 * - Operation: select operation to be done with the remote NFC tag
 *  o READ_NDEF: extract NDEF message from the tag, previously registered callback function will be called whenever complete NDEF message is found.
 *  o WRITE_NDEF: write previously registered NDEF message to the tag
 *  o PRESENCE_CHECK: perform presence check until tag has been removed (function is blocking until card is removed)
  */
int NxpNci_ProcessReaderMode(NxpNci_RfIntf_t RfIntf, NxpNci_RW_Operation_t Operation);

/*
 * Perform RAW transceive operation (send then receive) with the remote tag
 * - pCommand: pointer to the command to send
 * - CommandSize: command size
 * - pAnswer: pointer to buffer for getting the response
 * - pAnswerSize: response size
 * return NFC_XBH_TRUE or NFC_XBH_FALSE
 */
XBH_BOOL NxpNci_ReaderTagCmd (unsigned char *pCommand, unsigned char CommandSize, unsigned char *pAnswer, unsigned char *pAnswerSize);

/*
 * Perform activation of the next tag (in case of multiple tag detection or multi-protocol tag)
 * - pRfIntf: filled with discovered NFC remote device properties
 * return NFC_XBH_TRUE or NFC_XBH_FALSE
 */
XBH_BOOL NxpNci_ReaderActivateNext(NxpNci_RfIntf_t *pRfIntf);

/*
 * Perform deactivation then reactivation of the current tag
 * - pRfIntf: filled with discovered NFC remote device properties
 * return NFC_XBH_TRUE or NFC_XBH_FALSE
 */
XBH_BOOL NxpNci_ReaderReActivate(NxpNci_RfIntf_t *pRfIntf);
#endif //#ifndef REMOVE_RW_SUPPORT
/**********************************************************************/

/***** Card Emulation dedicated APIs **********************************/
#ifndef REMOVE_CARDEMU_SUPPORT
#ifndef REMOVE_NDEF_SUPPORT
/*
 * Register NDEF message to be exposed to remote NFC reader Device
 * - pMessage: pointer to the NDEF message
 * - Message_size: NDEF message size
 * - pCb: pointer to function to be called back when tag has been written
 * return NFC_XBH_TRUE or NFC_XBH_FALSE
 */
XBH_BOOL T4T_NDEF_EMU_SetMessage(unsigned char *pMessage, unsigned short Message_size, void *pCb);
#endif //#ifndef REMOVE_NDEF_SUPPORT

/*
 * Expose the previously registered NDEF message to discovered remote NFC reader (function is blocking until the remote reader is lost):
 * - RfIntf: discovered NFC device properties
  */
void NxpNci_ProcessCardMode(NxpNci_RfIntf_t RfIntf);

/*
 * Perform RAW reception of data from the remote reader
 * - pData: pointer to buffer for getting the data
 * - pDataSize: received data size
 * return NFC_XBH_TRUE or NFC_XBH_FALSE
 */
XBH_BOOL NxpNci_CardModeReceive (unsigned char *pData, unsigned char *pDataSize);

/*
 * Perform RAW transmission of data from the remote reader
 * - pData: pointer to data to transmit
 * - DataSize: size of data to transmit
 * return NFC_XBH_TRUE or NFC_XBH_FALSE
 */
XBH_BOOL NxpNci_CardModeSend (unsigned char *pData, unsigned char DataSize);
#endif //#ifndef REMOVE_CARDEMU_SUPPORT

/**********************************************************************/

/***** P2P dedicated APIs *********************************************/
#ifndef REMOVE_P2P_SUPPORT
/*
 * Register NDEF message to be sent to remote NFC Peer to peer Device
 * - pMessage: pointer to the NDEF message
 * - Message_size: NDEF message size
 * - pCb: pointer to function to be called back when tag has been sent
 * return NFC_XBH_TRUE or NFC_XBH_FALSE
 */
XBH_BOOL P2P_NDEF_SetMessage(unsigned char *pMessage, unsigned short Message_size, void *pCb);

/*
 * Register function called when NDEF message is received from remote NFC Peer to peer Device
 * - pCb: pointer to function to be called back when NDEF message has been received
 */
void P2P_NDEF_RegisterPullCallback(void *pCb);

/* Process P2P operation (function is blocking until the remote peer is lost):
 *  ¤ SNEP server to allow receiving NDEF message from remote NFC P2P device
 *  ¤ SNEP client to send previously registered NDEF message
 * - RfIntf: discovered NFC device properties
 */
void NxpNci_ProcessP2pMode(NxpNci_RfIntf_t RfIntf);
#endif //#ifndef REMOVE_P2P_SUPPORT

/**********************************************************************/

/***** Factory Test dedicated APIs *********************************************/
#ifndef REMOVE_FACTORYTEST_SUPPORT
/*
 * Definition of technology types
 */
typedef enum
{
    NFC_A,
    NFC_B,
    NFC_F
} NxpNci_TechType_t;

/*
 * Definition of bitrate
 */
typedef enum
{
    BR_106,
	BR_212,
	BR_424,
	BR_848
} NxpNci_Bitrate_t;

/*
 * Set the NFC Controller in continuous modulated field mode
 * - type: modulation type
 * - bitrate: modulation bitrate
 * return NFC_XBH_TRUE or NFC_XBH_FALSE
 */
XBH_BOOL NxpNci_FactoryTest_Prbs(NxpNci_TechType_t type, NxpNci_Bitrate_t bitrate);

/*
 * Set the NFC Controller in continuous unmodulated field mode
 * return NFC_XBH_TRUE or NFC_XBH_FALSE
 */
XBH_BOOL NxpNci_FactoryTest_RfOn(void);

#endif //#ifndef REMOVE_FACTORYTEST_SUPPORT
/********************************************************************************/
/*
*         Copyright (c), NXP Semiconductors Caen / France
*
*                     (C)NXP Semiconductors
*       All rights are reserved. Reproduction in whole or in part is
*      prohibited without the written consent of the copyright owner.
*  NXP reserves the right to make changes without notice at any time.
* NXP makes no warranty, expressed, implied or statutory, including but
* not limited to any implied warranty of merchantability or fitness for any
*particular purpose, or that the use will not infringe any third party patent,
* copyright or trademark. NXP must not be liable for any loss or damage
*                          arising from its use.
*/


#ifndef REMOVE_CARDEMU_SUPPORT
#endif //#ifndef REMOVE_CARDEMU_SUPPORT

#ifndef REMOVE_P2P_SUPPORT
#endif //#ifndef REMOVE_P2P_SUPPORT

#ifndef REMOVE_RW_SUPPORT
#endif //#ifndef REMOVE_RW_SUPPORT

#define NXPNCI_XBH_TRUE      NFC_XBH_TRUE
#define NXPNCI_XBH_FALSE        NFC_XBH_FALSE



/*
*         Copyright (c), NXP Semiconductors Caen / France
*
*                     (C)NXP Semiconductors
*       All rights are reserved. Reproduction in whole or in part is
*      prohibited without the written consent of the copyright owner.
*  NXP reserves the right to make changes without notice at any time.
* NXP makes no warranty, expressed, implied or statutory, including but
* not limited to any implied warranty of merchantability or fitness for any
*particular purpose, or that the use will not infringe any third party patent,
* copyright or trademark. NXP must not be liable for any loss or damage
*                          arising from its use.
*/
#ifndef REMOVE_P2P_SUPPORT

void P2P_NDEF_Reset(void);
void P2P_NDEF_Next(unsigned char *pCmd, unsigned short Cmd_size, unsigned char *Rsp, unsigned short *pRsp_size);

#endif
/*
*         Copyright (c), NXP Semiconductors Caen / France
*
*                     (C)NXP Semiconductors
*       All rights are reserved. Reproduction in whole or in part is
*      prohibited without the written consent of the copyright owner.
*  NXP reserves the right to make changes without notice at any time.
* NXP makes no warranty, expressed, implied or statutory, including but
* not limited to any implied warranty of merchantability or fitness for any
*particular purpose, or that the use will not infringe any third party patent,
* copyright or trademark. NXP must not be liable for any loss or damage
*                          arising from its use.
*/
#ifndef REMOVE_RW_SUPPORT
#ifndef REMOVE_NDEF_SUPPORT

void RW_NDEF_MIFARE_Reset(void);
void RW_NDEF_MIFARE_Read_Next(unsigned char *pCmd, unsigned short Cmd_size, unsigned char *Rsp, unsigned short *pRsp_size);
void RW_NDEF_MIFARE_Write_Next(unsigned char *pCmd, unsigned short Cmd_size, unsigned char *Rsp, unsigned short *pRsp_size);

#endif
#endif
/*
*         Copyright (c), NXP Semiconductors Caen / France
*
*                     (C)NXP Semiconductors
*       All rights are reserved. Reproduction in whole or in part is
*      prohibited without the written consent of the copyright owner.
*  NXP reserves the right to make changes without notice at any time.
* NXP makes no warranty, expressed, implied or statutory, including but
* not limited to any implied warranty of merchantability or fitness for any
*particular purpose, or that the use will not infringe any third party patent,
* copyright or trademark. NXP must not be liable for any loss or damage
*                          arising from its use.
*/
#ifndef REMOVE_RW_SUPPORT
#ifndef REMOVE_NDEF_SUPPORT

void RW_NDEF_T1T_Reset(void);
void RW_NDEF_T1T_Read_Next(unsigned char *pCmd, unsigned short Cmd_size, unsigned char *Rsp, unsigned short *pRsp_size);

#endif
#endif
/*
*         Copyright (c), NXP Semiconductors Caen / France
*
*                     (C)NXP Semiconductors
*       All rights are reserved. Reproduction in whole or in part is
*      prohibited without the written consent of the copyright owner.
*  NXP reserves the right to make changes without notice at any time.
* NXP makes no warranty, expressed, implied or statutory, including but
* not limited to any implied warranty of merchantability or fitness for any
*particular purpose, or that the use will not infringe any third party patent,
* copyright or trademark. NXP must not be liable for any loss or damage
*                          arising from its use.
*/
#ifndef REMOVE_RW_SUPPORT
#ifndef REMOVE_NDEF_SUPPORT

void RW_NDEF_T2T_Reset(void);
void RW_NDEF_T2T_Read_Next(unsigned char *pCmd, unsigned short Cmd_size, unsigned char *Rsp, unsigned short *pRsp_size);
void RW_NDEF_T2T_Write_Next(unsigned char *pCmd, unsigned short Cmd_size, unsigned char *Rsp, unsigned short *pRsp_size);

#endif
#endif
/*
*         Copyright (c), NXP Semiconductors Caen / France
*
*                     (C)NXP Semiconductors
*       All rights are reserved. Reproduction in whole or in part is
*      prohibited without the written consent of the copyright owner.
*  NXP reserves the right to make changes without notice at any time.
* NXP makes no warranty, expressed, implied or statutory, including but
* not limited to any implied warranty of merchantability or fitness for any
*particular purpose, or that the use will not infringe any third party patent,
* copyright or trademark. NXP must not be liable for any loss or damage
*                          arising from its use.
*/
#ifndef REMOVE_RW_SUPPORT
#ifndef REMOVE_NDEF_SUPPORT

void RW_NDEF_T3T_Reset(void);
void RW_NDEF_T3T_SetIDm(unsigned char *pIDm);
void RW_NDEF_T3T_Read_Next(unsigned char *pCmd, unsigned short Cmd_size, unsigned char *Rsp, unsigned short *pRsp_size);

#endif
#endif
/*
*         Copyright (c), NXP Semiconductors Caen / France
*
*                     (C)NXP Semiconductors
*       All rights are reserved. Reproduction in whole or in part is
*      prohibited without the written consent of the copyright owner.
*  NXP reserves the right to make changes without notice at any time.
* NXP makes no warranty, expressed, implied or statutory, including but
* not limited to any implied warranty of merchantability or fitness for any
*particular purpose, or that the use will not infringe any third party patent,
* copyright or trademark. NXP must not be liable for any loss or damage
*                          arising from its use.
*/
#ifndef REMOVE_RW_SUPPORT
#ifndef REMOVE_NDEF_SUPPORT

void RW_NDEF_T4T_Reset(void);
void RW_NDEF_T4T_Read_Next(unsigned char *pCmd, unsigned short Cmd_size, unsigned char *Rsp, unsigned short *pRsp_size);
void RW_NDEF_T4T_Write_Next(unsigned char *pCmd, unsigned short Cmd_size, unsigned char *Rsp, unsigned short *pRsp_size);

#endif
#endif
/*
*         Copyright (c), NXP Semiconductors Caen / France
*
*                     (C)NXP Semiconductors
*       All rights are reserved. Reproduction in whole or in part is
*      prohibited without the written consent of the copyright owner.
*  NXP reserves the right to make changes without notice at any time.
* NXP makes no warranty, expressed, implied or statutory, including but
* not limited to any implied warranty of merchantability or fitness for any
*particular purpose, or that the use will not infringe any third party patent,
* copyright or trademark. NXP must not be liable for any loss or damage
*                          arising from its use.
*/
#ifndef REMOVE_RW_SUPPORT
#ifndef REMOVE_NDEF_SUPPORT

void RW_NDEF_T5T_Reset(void);
void RW_NDEF_T5T_Read_Next(unsigned char *pCmd, unsigned short Cmd_size, unsigned char *Rsp, unsigned short *pRsp_size);
void RW_NDEF_T5T_Write_Next(unsigned char *pCmd, unsigned short Cmd_size, unsigned char *Rsp, unsigned short *pRsp_size);

#endif
#endif
/*
*         Copyright (c), NXP Semiconductors Caen / France
*
*                     (C)NXP Semiconductors
*       All rights are reserved. Reproduction in whole or in part is
*      prohibited without the written consent of the copyright owner.
*  NXP reserves the right to make changes without notice at any time.
* NXP makes no warranty, expressed, implied or statutory, including but
* not limited to any implied warranty of merchantability or fitness for any
*particular purpose, or that the use will not infringe any third party patent,
* copyright or trademark. NXP must not be liable for any loss or damage
*                          arising from its use.
*/
#ifndef REMOVE_RW_SUPPORT
#ifndef REMOVE_NDEF_SUPPORT

#define RW_MAX_NDEF_FILE_SIZE 500

extern unsigned char NdefBuffer[RW_MAX_NDEF_FILE_SIZE];

typedef void RW_NDEF_PullCallback_t (unsigned char*, unsigned short, unsigned int);
typedef void RW_NDEF_PushCallback_t (unsigned char*, unsigned short);

#define RW_NDEF_TYPE_T1T    0x1
#define RW_NDEF_TYPE_T2T    0x2
#define RW_NDEF_TYPE_T3T    0x3
#define RW_NDEF_TYPE_T4T    0x4
#define RW_NDEF_TYPE_T5T    0x6
#define RW_NDEF_TYPE_MIFARE 0x80

extern unsigned char *pRW_NdefMessage;
extern unsigned short RW_NdefMessage_size;

extern RW_NDEF_PullCallback_t *pRW_NDEF_PullCb;
extern RW_NDEF_PushCallback_t *pRW_NDEF_PushCb;

void RW_NDEF_Reset(unsigned char type);
void RW_NDEF_Read_Next(unsigned char *pCmd, unsigned short Cmd_size, unsigned char *Rsp, unsigned short *pRsp_size);
void RW_NDEF_Write_Next(unsigned char *pCmd, unsigned short Cmd_size, unsigned char *Rsp, unsigned short *pRsp_size);

#endif
#endif
#ifndef _SWI2C_H
#define _SWI2C_H

XBH_BOOL I2C_WriteRegBytes(XBH_U8 u8DeviceAddr, XBH_U8 u8RegAddr, XBH_U16 wrNumber, XBH_U8* wrPointer);
XBH_BOOL I2C_ReadRegBytes(XBH_U8 u8DeviceAddr, XBH_U8 u8RegAddr, XBH_U16 rdNumber, XBH_U8* rdPointer);
XBH_BOOL I2C_ReadBytes(XBH_U8 u8DeviceAddr, XBH_U16 rdNumber, XBH_U8* rdPointer);
#endif 



/*
*         Copyright (c), NXP Semiconductors Caen / France
*
*                     (C)NXP Semiconductors
*       All rights are reserved. Reproduction in whole or in part is
*      prohibited without the written consent of the copyright owner.
*  NXP reserves the right to make changes without notice at any time.
* NXP makes no warranty, expressed, implied or statutory, including but
* not limited to any implied warranty of merchantability or fitness for any
*particular purpose, or that the use will not infringe any third party patent,
* copyright or trademark. NXP must not be liable for any loss or damage
*                          arising from its use.
*/

#ifndef REMOVE_CARDEMU_SUPPORT
#ifndef REMOVE_NDEF_SUPPORT
void T4T_NDEF_EMU_Reset(void);
void T4T_NDEF_EMU_Next(unsigned char *pCmd, unsigned short Cmd_size, unsigned char *Rsp, unsigned short *pRsp_size);
#endif
#endif
/*
*         Copyright (c), NXP Semiconductors Caen / France
*
*                     (C)NXP Semiconductors
*       All rights are reserved. Reproduction in whole or in part is
*      prohibited without the written consent of the copyright owner.
*  NXP reserves the right to make changes without notice at any time.
* NXP makes no warranty, expressed, implied or statutory, including but
* not limited to any implied warranty of merchantability or fitness for any
*particular purpose, or that the use will not infringe any third party patent,
* copyright or trademark. NXP must not be liable for any loss or damage
*                          arising from its use.
*/

#define TIMEOUT_INFINITE	0
#define TIMEOUT_1MS		    1
#define TIMEOUT_10MS		10
#define TIMEOUT_100MS		100
#define TIMEOUT_1S			1000
#define TIMEOUT_2S			2000
#define TIMEOUT_5S			5000



#define NFC_CHECKDEV_WAIT 		    TIMEOUT_100MS
#define NFC_HOST_TRANS_WAIT 	    TIMEOUT_2S

#define NFC_PROC_CARD_MODE_WAIT 	TIMEOUT_2S

#define NFC_RECEPTION_WAIT 	        TIMEOUT_100MS
#define NFC_DISCOVERY_WAIT 	        TIMEOUT_1S

#define NFC_P2P_MODE_WAIT 	        TIMEOUT_2S
#define NFC_READ_NDEF_WAIT 	        TIMEOUT_1S
#define NFC_WRITE_NDEF_WAIT 	    TIMEOUT_2S


void tml_Connect(void);
void tml_Disconnect(void);
void tml_Send(XBH_U8 *pBuffer, XBH_U16 BufferLen, XBH_U16 *pBytesSent);
void tml_Receive(XBH_U8 *pBuffer, XBH_U16 BufferLen, XBH_U16 *pBytes, XBH_U16 timeout);



static struct nfc_device_t nfc_dev;
struct nfc_card_info_t nfc_card_info = {0};
static NxpNci_RfIntf_t RfInterface;
static int nfc_proc_state = 0;

XBH_U32 NFC_STATUS = 0;

/* Discovery loop configuration according to the targeted modes of operation */
static unsigned char DiscoveryTechnologies[] = {
    MODE_POLL | TECH_PASSIVE_NFCA,
    MODE_POLL | TECH_PASSIVE_NFCB,
	MODE_POLL | TECH_PASSIVE_NFCF,
    MODE_POLL | TECH_PASSIVE_15693
};

void displayCardInfo(NxpNci_RfIntf_t RfIntf)
{
    switch(RfIntf.Protocol){
    case PROT_T1T:
    case PROT_T2T:
    case PROT_T3T:
    case PROT_ISODEP:
        XLOGD(" - POLL MODE: Remote T%dT activated\n", RfIntf.Protocol);
        break;
    case PROT_T5T:
        XLOGD(" - POLL MODE: Remote ISO15693 card activated\n");
        break;
    case PROT_MIFARE:
        XLOGD(" - POLL MODE: Remote MIFARE card activated\n");
        break;
    default:
        XLOGD(" - POLL MODE: Undetermined target\n");
        return;
    }

    switch(RfIntf.ModeTech) {
    case (MODE_POLL | TECH_PASSIVE_NFCA):
        XLOGD("\tSENS_RES = 0x%.2x 0x%.2x\n", RfIntf.Info.NFC_APP.SensRes[0], RfIntf.Info.NFC_APP.SensRes[1]);
        //PRINT_BUF("\tNFCID = ", RfIntf.Info.NFC_APP.NfcId, RfIntf.Info.NFC_APP.NfcIdLen);
        if(RfIntf.Info.NFC_APP.SelResLen != 0) XLOGD("\tSEL_RES = 0x%.2x\n", RfIntf.Info.NFC_APP.SelRes[0]);
    break;

    case (MODE_POLL | TECH_PASSIVE_NFCB):
        if(RfIntf.Info.NFC_BPP.SensResLen != 0)// PRINT_BUF("\tSENS_RES = ", RfIntf.Info.NFC_BPP.SensRes, RfIntf.Info.NFC_BPP.SensResLen);
    break;

    case (MODE_POLL | TECH_PASSIVE_NFCF):
        XLOGD("\tBitrate = %s\n", (RfIntf.Info.NFC_FPP.BitRate==1)?"212":"424");
        if(RfIntf.Info.NFC_FPP.SensResLen != 0) //PRINT_BUF("\tSENS_RES = ", RfIntf.Info.NFC_FPP.SensRes, RfIntf.Info.NFC_FPP.SensResLen);
    break;

    case (MODE_POLL | TECH_PASSIVE_15693):
       // PRINT_BUF("\tID = ", RfIntf.Info.NFC_VPP.ID, sizeof(RfIntf.Info.NFC_VPP.ID));
        XLOGD("\tAFI = 0x%.2x\n", RfIntf.Info.NFC_VPP.AFI);
        XLOGD("\tDSFID = 0x%.2x\n", RfIntf.Info.NFC_VPP.DSFID);
    break;

    default:
        break;
    }
}

void NFC_Register(struct nfc_card_ops* ops)
{
    memset(&nfc_dev,0,sizeof(nfc_dev));
    nfc_dev.ops = ops;
}

int NFC_Init()
{
    nfc_proc_state = 0;

    /* Open connection to NXPNCI device */
    if (NxpNci_Connect() == NFC_XBH_FALSE) {
        XLOGD("XBH_FALSE: cannot connect to NXPNCI device\n");
        return NFC_XBH_FALSE;
    }

    if (NxpNci_ConfigureSettings() == NFC_XBH_FALSE) {
        XLOGD("XBH_FALSE: cannot configure NXPNCI settings\n");
        return NFC_XBH_FALSE;
    }

    if (NxpNci_ConfigureMode(NXPNCI_MODE_RW) == NFC_XBH_FALSE)
    {
        XLOGD("XBH_FALSE: cannot configure NXPNCI\n");
        return NFC_XBH_FALSE;
    }

    XLOGD("NXP NFC PN7160 INIT OK\n");
	return NFC_XBH_TRUE;
}


int NFC_Start(void)
{
    if (NxpNci_StartDiscovery(DiscoveryTechnologies,sizeof(DiscoveryTechnologies)) != NFC_XBH_TRUE){

        return 0;
    }
    return 1;
}


int NFC_Wait_Events(void)
{
    if(NxpNci_WaitForDiscoveryNotification(&RfInterface) != NFC_XBH_TRUE){
        XLOGD("***************************112121212121");
		return 0;
	}
	return 1;
}

static int NFC_Detect_Card(NxpNci_RfIntf_t RfIntf)
{
        
    	if( nfc_dev.ops &&  nfc_dev.ops->detect){
	    memset(&nfc_card_info,0,sizeof(nfc_card_info));
        }
		switch(RfInterface.Protocol) {
		case PROT_T1T:
			//PCD_ISO14443_3A_scenario();
		    XLOGD("NFC PROTOCAL PROT_T1T\n");
			break;
		case PROT_T2T:
			//PCD_ISO14443_3A_scenario();
		    XLOGD("NFC PROTOCAL PROT_T2T\n");
			break;
		case PROT_T3T:
			//PCD_ISO14443_3A_scenario();
			//PCD_ISO15693_scenario();
		    XLOGD("NFC PROTOCAL PROT_T3T\n");
			break;
		case PROT_ISODEP:
			//PCD_ISO14443_4_scenario();
		    XLOGD("NFC PROTOCAL PROT_ISODEP\n");
			break;
		case PROT_NFCDEP:
			//PCD_ISO14443_4_scenario();
		    XLOGD("NFC PROTOCAL PROT_NFCDEP\n");
			break;
		case PROT_T5T:
			//PCD_ISO15693_scenario();
		    XLOGD("NFC PROTOCAL PROT_T5T\n");
			break;
		case PROT_MIFARE:
			//PCD_MIFARE_scenario();
		    XLOGD("NFC PROTOCAL PROT_MIFARE\n");
			break;
		default:
		    XLOGD("NFC PROTOCAL PROT_UNDETERMINED\n");
			break;
		}

		switch(RfIntf.ModeTech) {
		case (MODE_POLL | TECH_PASSIVE_NFCA):
		    nfc_card_info.mode_tech = (enum NFC_MODE_TECH) TECH_PASSIVE_NFCA;

	        if(RfIntf.Info.NFC_APP.NfcIdLen > 0 && RfIntf.Info.NFC_APP.NfcIdLen < NFC_MAX_ID_LEN){
		    	memcpy(nfc_card_info.id, RfIntf.Info.NFC_APP.NfcId, RfIntf.Info.NFC_APP.NfcIdLen);
		    	nfc_card_info.id_len = RfIntf.Info.NFC_APP.NfcIdLen;
		    }
			break;
		case (MODE_POLL | TECH_PASSIVE_NFCB):
		    nfc_card_info.mode_tech = (enum NFC_MODE_TECH) TECH_PASSIVE_NFCB;
	        if(RfIntf.Info.NFC_BPP.SensResLen > 0 && RfIntf.Info.NFC_BPP.SensResLen < NFC_MAX_ID_LEN){
		    	memcpy(nfc_card_info.id, RfIntf.Info.NFC_BPP.SensRes, RfIntf.Info.NFC_BPP.SensResLen);
		    	nfc_card_info.id_len = RfIntf.Info.NFC_BPP.SensResLen;
		    }
			break;
		case (MODE_POLL | TECH_PASSIVE_NFCF):
		    nfc_card_info.mode_tech = (enum NFC_MODE_TECH) TECH_PASSIVE_NFCF;
	        if(RfIntf.Info.NFC_FPP.SensResLen > 0 && RfIntf.Info.NFC_FPP.SensResLen < NFC_MAX_ID_LEN){
		    	memcpy(nfc_card_info.id, RfIntf.Info.NFC_FPP.SensRes, RfIntf.Info.NFC_FPP.SensResLen);
		    	nfc_card_info.id_len = RfIntf.Info.NFC_FPP.SensResLen;
		    }
			break;
		case (MODE_POLL | TECH_PASSIVE_15693):
		    {
		        nfc_card_info.mode_tech = (enum NFC_MODE_TECH) TECH_PASSIVE_15693;
		    	memcpy(nfc_card_info.id, RfIntf.Info.NFC_VPP.ID, 8);
		    	nfc_card_info.id_len = 8;
		    }
			break;
		default:
			break;
		}
		//nfc_card_info.protocol = (enum NFC_PROT_ENUM) RfInterface.Protocol;
    XLOGD("nfc_card_detect\n");
    NFC_STATUS=1;
    int i;
    //    PRINTF("===== Card Detect =====\n");
    //    PRINTF("Len: %d, ID: ",(int)info->id_len);

    for (i = 0; i < nfc_card_info.id_len; i++)
    {
        XLOGD("%02X ",nfc_card_info.id[i]);  
		//return nfc_dev.ops->detect(&nfc_card_info);
	}
	return 0;
}

static void NFC_Remove_Card(NxpNci_RfIntf_t RfIntf)
{

}


void NFC_Handle_Events(void)
{
	if ((RfInterface.ModeTech & MODE_MASK) == MODE_POLL)
	{
		/* For each discovered cards */
		while(1){
			/* Display detected card information */
		    //displayCardInfo(RfInterface);
            NFC_Detect_Card(RfInterface);

			/* If more cards (or multi-protocol card) were discovered (only same technology are supported) select next one */
			if(RfInterface.MoreTags) {
				if(NxpNci_ReaderActivateNext(&RfInterface) == NFC_XBH_FALSE) break;
			}
			/* Otherwise leave */
			else{
                break;
            }
		}
	}
	else {
		XLOGD("WRONG DISCOVERY\n");
	}
}

int NFC_Care_Presence_Check(void)
{
	return NxpNci_ProcessReaderMode(RfInterface, PRESENCE_CHECK);
}

int NFC_Care_Remove(void)
{
    NFC_Remove_Card(RfInterface);
	XLOGD("CARD REMOVED\n");
    return 0;
}

int NFC_Care_Idle(void)
{
	if( nfc_dev.ops &&  nfc_dev.ops->idle){
		nfc_dev.ops->idle();
	}
    return 0;
}

void NFC_Handle_Process(void)
{
	switch(nfc_proc_state){
		default:
		    nfc_proc_state = 0;
			break;
            XLOGD("000000000000000000000");
		case 0:
			if(NFC_Start()){
				nfc_proc_state++;
                usleep(50*1000);
                XLOGD("11111212121212121");
			}
            //XLOGD("11111111111111111111");
            NFC_STATUS=0;
			break;
		case 1:
			if(NFC_Wait_Events()){
				nfc_proc_state++;
                usleep(50*1000);
			}else{
				NFC_Care_Idle();
			}
            XLOGD("2222222222222222222");
			break;
		case 2:
			//SEGGER_RTT_XLOGD(0, "NFC_Handle_Events\r\n");
			NFC_Handle_Events();
			nfc_proc_state++;
            usleep(50*1000);
            XLOGD("3333333333333333333");
			break;
		case 3:
			if(!NFC_Care_Presence_Check()){
			    nfc_proc_state++;
			}
            XLOGD("4444444444444444444444");
			break;
		case 4:
		    NFC_Care_Remove();
			nfc_proc_state++;
            XLOGD("55555555555555555555555");
			break;
		case 5:
		    NxpNci_StopDiscovery();
			nfc_proc_state++;
            XLOGD("666666666666666666666666");
			break;
		case 6:
			NFC_Care_Idle();
			nfc_proc_state = 0;
            XLOGD("777777777777777777777777");
			break;
	}
}

int NFC_Mifare_Auth(unsigned char block, unsigned char key, unsigned char pwd[6])
{
    XBH_BOOL status;
    unsigned char Resp[256];
    unsigned char RespSize;
    /* Authenticate sector 1 with generic keys */
    //unsigned char Auth[] = {0x40, BLK_NB_MFC/4, 0x10, KEY_MFC};
    unsigned char Auth[9] = {0};

    Auth[0] = 0x40;
    Auth[1] = block/4;

	if(key==1){
        Auth[2] = 0x10;
	}else if(key==2){
        Auth[2] = 0x90;
	}
	memcpy(&Auth[3], pwd, 6);

    /* Authenticate */
    status = NxpNci_ReaderTagCmd(Auth, sizeof(Auth), Resp, &RespSize);
    if((status == NFC_XBH_FALSE) || (Resp[RespSize-1] != 0))
    {
        XLOGD(" Authenticate sector %d failed with XBH_FALSE 0x%02x\n", Auth[1], Resp[RespSize-1]);
        return -1;
    }

    XLOGD(" Authenticate sector %d key %d succeed\n", key, Auth[1]);
	return 0;
}

static int Copy_Read_Buffer(unsigned char* buf, int len, unsigned char* data, int datalen)
{
	//int i;
	if(datalen > 0 && len > 0){
		if(datalen > len){
			datalen = len;
		}
		if(datalen < 256){
		    memcpy(buf, data, datalen);
            XLOGD("Copy Read Data[%d]:", datalen);
	       // PRINT_BUF(" ", buf, datalen);
	        return (int)datalen;
		}
	}
	return 0;
}

int NFC_Mifare_Read(unsigned char block, unsigned char* buf, int len)
{
    XBH_BOOL status;
    unsigned char Resp[256];
    unsigned char RespSize;
    unsigned char Read[] = {0x10, 0x30, block};

	if(!buf || len<=0){       
		return -1;
	}

    /* Read block */
    status = NxpNci_ReaderTagCmd(Read, sizeof(Read), Resp, &RespSize);
    if((status == NFC_XBH_FALSE) || (Resp[RespSize-1] != 0))
    {
        XLOGD(" Read block %d failed with XBH_FALSE 0x%02x\n", Read[2], Resp[RespSize-1]);
        return -1;
    }
    XLOGD(" Read block %d: return: %d\n", Read[2],RespSize); 
	return Copy_Read_Buffer(buf, len, &Resp[1], (int)(RespSize-2));
}

int NFC_Mifare_Write(unsigned char block, unsigned char* buf, int len)
{
    XBH_BOOL status;
    unsigned char Resp[256];
    unsigned char RespSize;

    unsigned char WritePart1[] = {0x10, 0xA0, block};
    unsigned char WritePart2[18] = {0};
    unsigned char WriteLen;

	if(!buf || len<=0 || len > 16){       
		return -1;
	}

    /* Write block */
    status = NxpNci_ReaderTagCmd(WritePart1, sizeof(WritePart1), Resp, &RespSize);
    if((status == NFC_XBH_FALSE) || (Resp[RespSize-1] != 0x14))
    {
        XLOGD(" Write1 block %d failed with XBH_FALSE 0x%02x\n", WritePart1[2], Resp[RespSize-1]);
        return -1;
    }

    WritePart2[0] = 0x10;
	memcpy(&WritePart2[1],buf,len);
	WriteLen = len + 1;
    status = NxpNci_ReaderTagCmd(WritePart2, WriteLen, Resp, &RespSize);
    if((status == NFC_XBH_FALSE) || (Resp[RespSize-1] != 0x14))
    {
        XLOGD(" Write2 block %d failed with XBH_FALSE 0x%02x\n", WritePart1[2], Resp[RespSize-1]);
        return -1;
    }
    XLOGD(" Block %d written\n", WritePart1[2]);
	return (int)len;
}

int NFC_T5T_Read(unsigned char block, unsigned char* buf, int len)
{
    XBH_BOOL status;
    unsigned char Resp[64];
    unsigned char RespSize;
    unsigned char Read[] =  {0x02, 0x20, block};

    /* Read block */
    status = NxpNci_ReaderTagCmd(Read, sizeof(Read), Resp, &RespSize);
    if((status == NFC_XBH_FALSE) || (Resp[RespSize-1] != 0x00))
    {
        XLOGD(" Read block %d failed with XBH_FALSE 0x%02x\n", Read[2], Resp[RespSize-1]);
        return -1;
    }
    XLOGD(" Read block %d: return: %d\n", Read[2], RespSize); 
	return Copy_Read_Buffer(buf, len, &Resp[1], (int)(RespSize-2));
}

int NFC_T5T_Write(unsigned char block, unsigned char* buf, int len)
{
    XBH_BOOL status;
    unsigned char Resp[64];
    unsigned char RespSize;

    unsigned char WriteLen;
    unsigned char WriteBlock[32] = {0};

	if(!buf || len<=0 || len > 4){       
		return -1;
	}

    /* Write block */
    WriteBlock[0] = 0x02;
    WriteBlock[1] = 0x21;
    WriteBlock[2] = block;
	memcpy(&WriteBlock[3],buf,len);

	WriteLen = len + 3;

    //XLOGD(" Write block %d len %d\n", WriteBlock[2], WriteLen);
	//PRINT_BUF(" ", WriteBlock, WriteLen);
    status = NxpNci_ReaderTagCmd(WriteBlock, WriteLen, Resp, &RespSize);
    if((status == NFC_XBH_FALSE) || (Resp[RespSize-1] != 0))
    {
        XLOGD(" Write block %d failed with XBH_FALSE 0x%02x\n", WriteBlock[2], Resp[RespSize-1]);
        return -1;
    }
    XLOGD(" Block %d written ret: %d\n", (int)WriteBlock[2],(int)RespSize);
	return (int)len;
}

int NFC_T2T_Read(unsigned char block, unsigned char* buf, int len)
{
    XBH_BOOL status;
    unsigned char Resp[64];
    unsigned char RespSize;
    unsigned char Read[] = {0x30, block};

	if(!buf || len<=0 || len > 4){       
		return -1;
	}

    /* Read block */
    status = NxpNci_ReaderTagCmd(Read, sizeof(Read), Resp, &RespSize);
    if((status == NFC_XBH_FALSE) || (Resp[RespSize-1] != 0))
    {
        XLOGD(" Read block %d failed with XBH_FALSE 0x%02x\n", Read[1], Resp[RespSize-1]);
        return -1;
    }
	return Copy_Read_Buffer(buf,len,Resp, 4);
}

int NFC_T2T_Write(unsigned char block, unsigned char* buf, int len)
{
    XBH_BOOL status;
    unsigned char Resp[64];
    unsigned char RespSize;

    unsigned char WriteLen;
    unsigned char WriteBlock[32] = {0};

	if(!buf || len<=0 || len > 4){       
        XLOGD(" Write param fail %d\n", len);
		return -1;
	}

    /* Write block */
    WriteBlock[0] = 0xA2;
    WriteBlock[1] = block;
	memcpy(&WriteBlock[2],buf,len);

	WriteLen = (unsigned char)len + 2;

    //XLOGD(" Write block %d len %d\n", WriteBlock[2], WriteLen);
	//PRINT_BUF(" ", WriteBlock, WriteLen);
    status = NxpNci_ReaderTagCmd(WriteBlock, WriteLen, Resp, &RespSize);
    if((status == NFC_XBH_FALSE) || (Resp[RespSize-1] != 0x14))
    {
        XLOGD(" Write block %d failed with XBH_FALSE 0x%02x\n", WriteBlock[1], Resp[RespSize-1]);
        return -1;
    }
    XLOGD(" Block %d written ret: %d\n", WriteBlock[1],(int)RespSize);
	return (int)len;
}

int NFC_ISODEP_Select_PPSE(unsigned char* ppse, int len, unsigned char* ret, int ret_size)
{
    XBH_BOOL status;
    unsigned char Resp[256];
    unsigned char RespSize;

	if(!ppse || len<=0 || len > 256){       
        XLOGD(" Write param fail %d\n", len);
		return -1;
	}

    status = NxpNci_ReaderTagCmd(ppse, (unsigned char)len, Resp, &RespSize);
    if((status == NFC_XBH_FALSE) || (Resp[RespSize-2] != 0x90) || (Resp[RespSize-1] != 0x00))
    {
        XLOGD(" Select PPSE failed with XBH_FALSE %02x %02x\n", Resp[RespSize-2], Resp[RespSize-1]);
        return -1;
    }
    XLOGD(" Select PPSE Application succeed %d\n",(int)RespSize);

	return Copy_Read_Buffer(ret,ret_size, Resp, RespSize);
}
/*
 *         Copyright (c), NXP Semiconductors Caen / France
 *
 *                     (C)NXP Semiconductors
 *       All rights are reserved. Reproduction in whole or in part is
 *      prohibited without the written consent of the copyright owner.
 *  NXP reserves the right to make changes without notice at any time.
 * NXP makes no warranty, expressed, implied or statutory, including but
 * not limited to any implied warranty of merchantability or fitness for any
 *particular purpose, or that the use will not infringe any third party patent,
 * copyright or trademark. NXP must not be liable for any loss or damagetml_Sen
 *                          arising from its use.
 */



#define MAX_NCI_FRAME_SIZE    258

static XBH_BOOL gRfSettingsRestored_flag = false;
static XBH_U8 gNfcController_fw_version[3] = {0};
static XBH_U8 gNextTag_Protocol = PROT_UNDETERMINED;

static XBH_U8 NCIStartDiscovery[30];
static XBH_U8 NCIStartDiscovery_length = 0;

static XBH_BOOL NxpNci_CheckDevPres(void)
{
    XBH_U8 NCICoreReset[] = {0x20, 0x00, 0x01, 0x01};
    XBH_U8 Answer[100];
    XBH_U16 NbBytes = 0;

    /* Reset RF settings restore flag */
    gRfSettingsRestored_flag = false;

    tml_Send(NCICoreReset, sizeof(NCICoreReset), &NbBytes);
    //NCI_PRINT_BUF("NCI >> ", NCICoreReset, NbBytes);
    if (NbBytes != sizeof(NCICoreReset)){
        XLOGD("tml_Send fail %d\n",NbBytes);
		return NXPNCI_XBH_FALSE;
    }
    usleep(50*1000);
    tml_Receive(Answer, sizeof(Answer), &NbBytes, NFC_CHECKDEV_WAIT);
    //NCI_PRINT_BUF("NCI << ", Answer, NbBytes);
    if ((NbBytes == 0) || (Answer[0] != 0x40) || (Answer[1] != 0x00)){
        XLOGD("##############################tml_Receive %d, %02X, %02X\n",NbBytes, Answer[0], Answer[1]);
		return NXPNCI_XBH_FALSE;
	}
    /* Catch potential notifications */
    //tml_Send(NCICoreReset, sizeof(NCICoreReset), &NbBytes);
    usleep(100*1000);
    XLOGD("**********************************");
    tml_Receive(Answer, sizeof(Answer), &NbBytes, NFC_CHECKDEV_WAIT);
    if (NbBytes != 0)
    {
        //NCI_PRINT_BUF("NCI << ", Answer, NbBytes);
        /* Is CORE_GENERIC_XBH_FALSE_NTF ? */
        if ((Answer[0] == 0x60) && (Answer[1] == 0x07))
        {
            /* Is PN7150B0HN/C11004 Anti-tearing recovery procedure triggered ? */
            if ((Answer[3] == 0xE6)) gRfSettingsRestored_flag = true;
        }
        /* Is NCI 2.0 CORE_RESET_NTF ? */
		else if ((Answer[0] == 0x60) && (Answer[1] == 0x00) && (Answer[5] == 0x20))
        {
			gNfcController_fw_version[0] = Answer[9];
			gNfcController_fw_version[1] = Answer[10];
			gNfcController_fw_version[2] = Answer[11];
        }
        else
        {
            return NXPNCI_XBH_FALSE;
        }
    }

    return NXPNCI_XBH_TRUE;
}

static XBH_BOOL NxpNci_WaitForReception(XBH_U8 *pRBuff, XBH_U16 RBuffSize, XBH_U16 *pBytesread, XBH_U16 timeout)
{
    tml_Receive(pRBuff, RBuffSize, pBytesread, timeout);
    if (*pBytesread == 0){
		return NXPNCI_XBH_FALSE;
	}
    //NCI_PRINT_BUF("NCI << ", pRBuff, *pBytesread);

    return NXPNCI_XBH_TRUE;
}

static XBH_BOOL NxpNci_HostTransceive(XBH_U8 *pTBuff, XBH_U16 TbuffLen, XBH_U8 *pRBuff, XBH_U16 RBuffSize, XBH_U16 *pBytesread)
{
    tml_Send(pTBuff, TbuffLen, pBytesread);
    //NCI_PRINT_BUF("NCI >> ", pTBuff, TbuffLen);
    if (*pBytesread != TbuffLen) return NXPNCI_XBH_FALSE;
    usleep(50*1000);
    tml_Receive(pRBuff, RBuffSize, pBytesread, NFC_HOST_TRANS_WAIT);
    if (*pBytesread == 0) return NXPNCI_XBH_FALSE;
    //NCI_PRINT_BUF("NCI << ", pRBuff, *pBytesread);

    return NXPNCI_XBH_TRUE;
}

static void NxpNci_FillInterfaceInfo(NxpNci_RfIntf_t* pRfIntf, XBH_U8* pBuf)
{
    XBH_U8 i, temp;

    switch(pRfIntf->ModeTech)
    {
    case (MODE_POLL | TECH_PASSIVE_NFCA):
        memcpy(pRfIntf->Info.NFC_APP.SensRes, &pBuf[0], 2);
        temp = 2;
        pRfIntf->Info.NFC_APP.NfcIdLen = pBuf[temp];
        temp++;
        memcpy(pRfIntf->Info.NFC_APP.NfcId, &pBuf[3], pRfIntf->Info.NFC_APP.NfcIdLen);
        temp+=pBuf[2];
        pRfIntf->Info.NFC_APP.SelResLen = pBuf[temp];
        temp++;
        if(pRfIntf->Info.NFC_APP.SelResLen == 1) pRfIntf->Info.NFC_APP.SelRes[0] = pBuf[temp];
        temp+=4;
        if(pBuf[temp] != 0)
        {
            temp++;
            pRfIntf->Info.NFC_APP.RatsLen = pBuf[temp];
            memcpy(pRfIntf->Info.NFC_APP.Rats, &pBuf[temp+1], pBuf[temp]);
        }
        else
        {
            pRfIntf->Info.NFC_APP.RatsLen = 0;
        }
        break;

    case (MODE_POLL | TECH_PASSIVE_NFCB):
        pRfIntf->Info.NFC_BPP.SensResLen = pBuf[0];
        memcpy(pRfIntf->Info.NFC_BPP.SensRes, &pBuf[1], pRfIntf->Info.NFC_BPP.SensResLen);
        temp = pBuf[0] + 4;
        if(pBuf[temp] != 0)
        {
            temp++;
            pRfIntf->Info.NFC_BPP.AttribResLen = pBuf[temp];
            memcpy(pRfIntf->Info.NFC_BPP.AttribRes, &pBuf[temp+1], pBuf[temp]);
        }
        else
        {
            pRfIntf->Info.NFC_BPP.AttribResLen = 0;
        }
        break;

    case (MODE_POLL | TECH_PASSIVE_NFCF):
        pRfIntf->Info.NFC_FPP.BitRate = pBuf[0];
        pRfIntf->Info.NFC_FPP.SensResLen = pBuf[1];
        memcpy(pRfIntf->Info.NFC_FPP.SensRes, &pBuf[2], pRfIntf->Info.NFC_FPP.SensResLen);
        break;

    case (MODE_POLL | TECH_PASSIVE_15693):
        pRfIntf->Info.NFC_VPP.AFI = pBuf[0];
        pRfIntf->Info.NFC_VPP.DSFID = pBuf[1];
        for(i=0; i<8; i++) pRfIntf->Info.NFC_VPP.ID[7-i] = pBuf[2+i];
        break;

    default:
        break;
    }
}

#ifndef REMOVE_CARDEMU_SUPPORT
void NxpNci_ProcessCardMode(NxpNci_RfIntf_t RfIntf)
{
    XBH_U8 Answer[MAX_NCI_FRAME_SIZE];
    XBH_U16 AnswerSize;
    XBH_U8 NCIStopDiscovery[] = {0x21, 0x06, 0x01, 0x00};
    XBH_BOOL FirstCmd = true;

    /* Reset Card emulation state */
    T4T_NDEF_EMU_Reset();

    while(NxpNci_WaitForReception(Answer, sizeof(Answer), &AnswerSize, NFC_PROC_CARD_MODE_WAIT) == NXPNCI_XBH_TRUE)
    {
    	/* is RF_DEACTIVATE_NTF ? */
        if((Answer[0] == 0x61) && (Answer[1] == 0x06))
        {
            if(FirstCmd)
            {
            	/* Restart the discovery loop */
				NxpNci_HostTransceive(NCIStopDiscovery, sizeof(NCIStopDiscovery), Answer, sizeof(Answer), &AnswerSize);
				do
				{
					if ((Answer[0] == 0x41) && (Answer[1] == 0x06)) break;
					NxpNci_WaitForReception(Answer, sizeof(Answer), &AnswerSize, NFC_RECEPTION_WAIT);
				} while (AnswerSize != 0);
				NxpNci_HostTransceive(NCIStartDiscovery, NCIStartDiscovery_length, Answer, sizeof(Answer), &AnswerSize);
            }
            /* Come back to discovery state */
            break;
        }
        /* is DATA_PACKET ? */
        else if((Answer[0] == 0x00) && (Answer[1] == 0x00))
        {
            /* DATA_PACKET */
            XBH_U8 Cmd[MAX_NCI_FRAME_SIZE];
            XBH_U16 CmdSize;

            T4T_NDEF_EMU_Next(&Answer[3], Answer[2], &Cmd[3], (unsigned short *) &CmdSize);

            Cmd[0] = 0x00;
            Cmd[1] = (CmdSize & 0xFF00) >> 8;
            Cmd[2] = CmdSize & 0x00FF;

            NxpNci_HostTransceive(Cmd, CmdSize+3, Answer, sizeof(Answer), &AnswerSize);
        }
        FirstCmd = false;
    }
}

XBH_BOOL NxpNci_CardModeReceive (unsigned char *pData, unsigned char *pDataSize)
{
    XBH_BOOL status = NXPNCI_XBH_FALSE;
    XBH_U8 Ans[MAX_NCI_FRAME_SIZE];
    XBH_U16 AnsSize;

    NxpNci_WaitForReception(Ans, sizeof(Ans), &AnsSize, NFC_PROC_CARD_MODE_WAIT);

    /* Is data packet ? */
    if ((Ans[0] == 0x00) && (Ans[1] == 0x00))
    {
        *pDataSize = Ans[2];
        memcpy(pData, &Ans[3], *pDataSize);
        status = NXPNCI_XBH_TRUE;
    }

    return status;
}

XBH_BOOL NxpNci_CardModeSend (unsigned char *pData, unsigned char DataSize)
{
    XBH_BOOL status;
    XBH_U8 Cmd[MAX_NCI_FRAME_SIZE];
    XBH_U8 Ans[MAX_NCI_FRAME_SIZE];
    XBH_U16 AnsSize;

    /* Compute and send DATA_PACKET */
    Cmd[0] = 0x00;
    Cmd[1] = 0x00;
    Cmd[2] = DataSize;
    memcpy(&Cmd[3], pData, DataSize);
    //status = NxpNci_HostSend(Cmd, DataSize+3);
    status = NxpNci_HostTransceive(Cmd, DataSize+3, Ans, sizeof(Ans), &AnsSize);

    return status;
}
#endif //#ifndef REMOVE_CARDEMU_SUPPORT

#ifndef REMOVE_P2P_SUPPORT
void NxpNci_ProcessP2pMode(NxpNci_RfIntf_t RfIntf)
{
    XBH_U8 Answer[MAX_NCI_FRAME_SIZE];
    XBH_U16 AnswerSize;
    XBH_BOOL status;
    XBH_BOOL restart = false;
    XBH_U8 NCILlcpSymm[] = {0x00, 0x00, 0x02, 0x00, 0x00};
    XBH_U8 NCIRestartDiscovery[] = {0x21, 0x06, 0x01, 0x03};

    /* Reset P2P_NDEF state */
    P2P_NDEF_Reset();

    /* Is Initiator mode ? */
    if((RfIntf.ModeTech & MODE_LISTEN) != MODE_LISTEN)
    {
        /* Initiate communication (SYMM PDU) */
        NxpNci_HostTransceive(NCILlcpSymm, sizeof(NCILlcpSymm), Answer, sizeof(Answer), &AnswerSize);
        /* Save status for discovery restart */
        restart = true;
    }

    status = NxpNci_WaitForReception(Answer, sizeof(Answer), &AnswerSize, NFC_P2P_MODE_WAIT);

    /* Get frame from remote peer */
    while(status == NXPNCI_XBH_TRUE)
    {
    	/* is DATA_PACKET ? */
        if((Answer[0] == 0x00) && (Answer[1] == 0x00))
        {
            XBH_U8 Cmd[MAX_NCI_FRAME_SIZE];
            XBH_U16 CmdSize;
            /* Handle P2P communication */
            P2P_NDEF_Next(&Answer[3], Answer[2], &Cmd[3], (unsigned short *) &CmdSize);
            /* Compute DATA_PACKET to answer */
            Cmd[0] = 0x00;
            Cmd[1] = (CmdSize & 0xFF00) >> 8;
            Cmd[2] = CmdSize & 0x00FF;
            status = NxpNci_HostTransceive(Cmd, CmdSize+3, Answer, sizeof(Answer), &AnswerSize);
        }
        /* is CORE_INTERFACE_XBH_FALSE_NTF ?*/
        else if ((Answer[0] == 0x60) && (Answer[1] == 0x08))
        {
            /* Come back to discovery state */
            break;
        }
        /* is RF_DEACTIVATE_NTF ? */
        else if((Answer[0] == 0x61) && (Answer[1] == 0x06))
        {
            /* Come back to discovery state */
            break;
        }
        /* is RF_DISCOVERY_NTF ? */
        else if((Answer[0] == 0x61) && ((Answer[1] == 0x05) || (Answer[1] == 0x03)))
        {
            do{
                if((Answer[0] == 0x61) && ((Answer[1] == 0x05) || (Answer[1] == 0x03)))
                {
                    if((Answer[6] & MODE_LISTEN) != MODE_LISTEN) restart = true;
                    else restart = false;
                }
                status = NxpNci_WaitForReception(Answer, sizeof(Answer), &AnswerSize, NFC_RECEPTION_WAIT);
            }
            while (AnswerSize != 0);
            /* Come back to discovery state */
            break;
        }

        /* Wait for next frame from remote P2P, or notification event */
        status = NxpNci_WaitForReception(Answer, sizeof(Answer), &AnswerSize, NFC_P2P_MODE_WAIT);
    }

    /* Is Initiator mode ? */
    if(restart)
    {
        /* Communication ended, restart discovery loop */
        NxpNci_HostTransceive(NCIRestartDiscovery, sizeof(NCIRestartDiscovery), Answer, sizeof(Answer), &AnswerSize);
        NxpNci_WaitForReception(Answer, sizeof(Answer), &AnswerSize, NFC_RECEPTION_WAIT);
    }
}
#endif //ifndef REMOVE_P2P_SUPPORT

#ifndef REMOVE_RW_SUPPORT
XBH_BOOL NxpNci_ReaderTagCmd (unsigned char *pCommand, unsigned char CommandSize, unsigned char *pAnswer, unsigned char *pAnswerSize)
{
    XBH_BOOL status = NXPNCI_XBH_FALSE;
    XBH_U8 Cmd[MAX_NCI_FRAME_SIZE];
    XBH_U8 Ans[MAX_NCI_FRAME_SIZE];
    XBH_U16 AnsSize;

    /* Compute and send DATA_PACKET */
    Cmd[0] = 0x00;
    Cmd[1] = 0x00;
    Cmd[2] = CommandSize;
    memcpy(&Cmd[3], pCommand, CommandSize);

    NxpNci_HostTransceive(Cmd, CommandSize+3, Ans, sizeof(Ans), &AnsSize);

    /* Wait for Answer */
    NxpNci_WaitForReception(Ans, sizeof(Ans), &AnsSize, NFC_RECEPTION_WAIT);

    if ((Ans[0] == 0x0) && (Ans[1] == 0x0))
    {
        status = NXPNCI_XBH_TRUE;
    }

    *pAnswerSize = Ans[2];
    memcpy(pAnswer, &Ans[3], *pAnswerSize);

    return status;
}

#ifndef REMOVE_NDEF_SUPPORT
static void NxpNci_ReadNdef(NxpNci_RfIntf_t RfIntf)
{
    XBH_U8 Answer[MAX_NCI_FRAME_SIZE];
    XBH_U16 AnswerSize;
    XBH_U8 Cmd[MAX_NCI_FRAME_SIZE];
    XBH_U16 CmdSize = 0;

    RW_NDEF_Reset(RfIntf.Protocol);

    while(1)
    {
        RW_NDEF_Read_Next(&Answer[3], Answer[2], &Cmd[3], (unsigned short *) &CmdSize);
        if(CmdSize == 0)
        {
            /* End of the Read operation */
            break;
        }
        else
        {
            /* Compute and send DATA_PACKET */
            Cmd[0] = 0x00;
            Cmd[1] = (CmdSize & 0xFF00) >> 8;
            Cmd[2] = CmdSize & 0x00FF;

            NxpNci_HostTransceive(Cmd, CmdSize+3, Answer, sizeof(Answer), &AnswerSize);
            NxpNci_WaitForReception(Answer, sizeof(Answer), &AnswerSize, NFC_READ_NDEF_WAIT);

            /* Manage chaining in case of T4T */
            if((RfIntf.Interface == INTF_ISODEP) && Answer[0] == 0x10)
            {
                XBH_U8 tmp[MAX_NCI_FRAME_SIZE];
                XBH_U8 tmpSize = 0;
                while(Answer[0] == 0x10)
                {
                    memcpy(&tmp[tmpSize], &Answer[3], Answer[2]);
                    tmpSize += Answer[2];

                    NxpNci_WaitForReception(Answer, sizeof(Answer), &AnswerSize, NFC_RECEPTION_WAIT);
                }
                memcpy(&tmp[tmpSize], &Answer[3], Answer[2]);
                tmpSize += Answer[2];
                /* Compute all chained frame into one unique answer */
                memcpy(&Answer[3], tmp, tmpSize);
                Answer[2] = tmpSize;
            }
        }
    }
}

static void NxpNci_WriteNdef(NxpNci_RfIntf_t RfIntf)
{
    XBH_U8 Answer[MAX_NCI_FRAME_SIZE];
    XBH_U16 AnswerSize;
    XBH_U8 Cmd[MAX_NCI_FRAME_SIZE];
    XBH_U16 CmdSize = 0;

    RW_NDEF_Reset(RfIntf.Protocol);

    while(1)
    {
        RW_NDEF_Write_Next(&Answer[3], Answer[2], &Cmd[3], (unsigned short *) &CmdSize);
        if(CmdSize == 0)
        {
            /* End of the Write operation */
            break;
        }
        else
        {
            /* Compute and send DATA_PACKET */
            Cmd[0] = 0x00;
            Cmd[1] = (CmdSize & 0xFF00) >> 8;
            Cmd[2] = CmdSize & 0x00FF;

            NxpNci_HostTransceive(Cmd, CmdSize+3, Answer, sizeof(Answer), &AnswerSize);
            NxpNci_WaitForReception(Answer, sizeof(Answer), &AnswerSize, NFC_WRITE_NDEF_WAIT);
        }
    }
}
#endif //#ifndef REMOVE_NDEF_SUPPORT

static int NxpNci_PresenceCheck(NxpNci_RfIntf_t RfIntf)
{
    XBH_BOOL status;
    XBH_U8 i;
    XBH_U8 Answer[MAX_NCI_FRAME_SIZE];
    XBH_U16 AnswerSize;

    XBH_U8 NCIPresCheckT1T[] = {0x00, 0x00, 0x07, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    XBH_U8 NCIPresCheckT2T[] = {0x00, 0x00, 0x02, 0x30, 0x00};
    XBH_U8 NCIPresCheckT3T[] = {0x21, 0x08, 0x04, 0xFF, 0xFF, 0x00, 0x01};
    XBH_U8 NCIPresCheckIsoDep[] = {0x2F, 0x11, 0x00};
    XBH_U8 NCIPresCheckIso15693[] = {0x00, 0x00, 0x0B, 0x26, 0x01, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    XBH_U8 NCIDeactivate[] = {0x21, 0x06, 0x01, 0x01};
    XBH_U8 NCISelectMIFARE[] = {0x21, 0x04, 0x03, 0x01, 0x80, 0x80};

    switch (RfIntf.Protocol) {
    case PROT_T1T:
        NxpNci_HostTransceive(NCIPresCheckT1T, sizeof(NCIPresCheckT1T), Answer, sizeof(Answer), &AnswerSize);
        NxpNci_WaitForReception(Answer, sizeof(Answer), &AnswerSize, NFC_RECEPTION_WAIT);
        if ((Answer[0] == 0x00) && (Answer[1] == 0x00)){
			return 1;
		}
        break;

    case PROT_T2T:
        NxpNci_HostTransceive(NCIPresCheckT2T, sizeof(NCIPresCheckT2T), Answer, sizeof(Answer), &AnswerSize);
        NxpNci_WaitForReception(Answer, sizeof(Answer), &AnswerSize, NFC_RECEPTION_WAIT);
        if ((Answer[0] == 0x00) && (Answer[1] == 0x00) && (Answer[2] == 0x11)){
			return 1;
		}
        break;

    case PROT_T3T:
        NxpNci_HostTransceive(NCIPresCheckT3T, sizeof(NCIPresCheckT3T), Answer, sizeof(Answer), &AnswerSize);
        NxpNci_WaitForReception(Answer, sizeof(Answer), &AnswerSize, NFC_RECEPTION_WAIT);
        if((Answer[0] == 0x61) && (Answer[1] == 0x08) && ((Answer[3] == 0x00) || (Answer[4] > 0x00))){
			return 1;
		}
        break;

    case PROT_ISODEP:
        NxpNci_HostTransceive(NCIPresCheckIsoDep, sizeof(NCIPresCheckIsoDep), Answer, sizeof(Answer), &AnswerSize);
        NxpNci_WaitForReception(Answer, sizeof(Answer), &AnswerSize, NFC_RECEPTION_WAIT);
        if ((Answer[0] == 0x6F) && (Answer[1] == 0x11) && (Answer[2] == 0x01) && (Answer[3] == 0x01)){
			return 1;
		}
        break;

    case PROT_T5T:
        for(i=0; i<8; i++) NCIPresCheckIso15693[i+6] = RfIntf.Info.NFC_VPP.ID[7-i];
        NxpNci_HostTransceive(NCIPresCheckIso15693, sizeof(NCIPresCheckIso15693), Answer, sizeof(Answer), &AnswerSize);
        status = NxpNci_WaitForReception(Answer, sizeof(Answer), &AnswerSize, NFC_RECEPTION_WAIT);
        if ((status == NXPNCI_XBH_TRUE) && (Answer[0] == 0x00) && (Answer[1] == 0x00) && (Answer[AnswerSize-1] == 0x00)){
			return 1;
		}
        break;

    case PROT_MIFARE:
            /* Deactivate target */
            NxpNci_HostTransceive(NCIDeactivate, sizeof(NCIDeactivate), Answer, sizeof(Answer), &AnswerSize);
            NxpNci_WaitForReception(Answer, sizeof(Answer), &AnswerSize, NFC_RECEPTION_WAIT);
            /* Reactivate target */
            NxpNci_HostTransceive(NCISelectMIFARE, sizeof(NCISelectMIFARE), Answer, sizeof(Answer), &AnswerSize);
            NxpNci_WaitForReception(Answer, sizeof(Answer), &AnswerSize, NFC_RECEPTION_WAIT);
        if ((Answer[0] == 0x61) && (Answer[1] == 0x05)){
			return 1;
		}
        break;

    default:
        /* Nothing to do */
        break;
    }
	return 0;
}

int NxpNci_ProcessReaderMode(NxpNci_RfIntf_t RfIntf, NxpNci_RW_Operation_t Operation)
{
#ifndef REMOVE_NDEF_SUPPORT
	if(RfIntf.Protocol == PROT_T3T)
	{
		RW_NDEF_T3T_SetIDm(RfIntf.Info.NFC_FPP.SensRes);
	}
#endif //#ifndef REMOVE_NDEF_SUPPORT

    switch (Operation)
    {
#ifndef REMOVE_NDEF_SUPPORT
    case READ_NDEF:
        NxpNci_ReadNdef(RfIntf);
        break;
    case WRITE_NDEF:
        NxpNci_WriteNdef(RfIntf);
        break;
#endif //#ifndef REMOVE_NDEF_SUPPORT
    case PRESENCE_CHECK:
        return NxpNci_PresenceCheck(RfIntf);
        break;
    default:
        break;
    }
	return 0;
}

XBH_BOOL NxpNci_ReaderActivateNext(NxpNci_RfIntf_t *pRfIntf)
{
    XBH_U8 NCIStopDiscovery[] = {0x21, 0x06, 0x01, 0x01};
    XBH_U8 NCIRfDiscoverSelect[] = {0x21, 0x04, 0x03, 0x02, PROT_ISODEP, INTF_ISODEP};
    XBH_U8 Answer[MAX_NCI_FRAME_SIZE];
    XBH_U16 AnswerSize;
    XBH_BOOL status = NXPNCI_XBH_FALSE;

    pRfIntf->MoreTags = false;

    if (gNextTag_Protocol == PROT_UNDETERMINED)
    {
        pRfIntf->Interface = INTF_UNDETERMINED;
        pRfIntf->Protocol = PROT_UNDETERMINED;
        return NXPNCI_XBH_FALSE;
    }

    /* First disconnect current tag */
    NxpNci_HostTransceive(NCIStopDiscovery, sizeof(NCIStopDiscovery), Answer, sizeof(Answer), &AnswerSize);
    if((Answer[0] != 0x41) && (Answer[1] != 0x06) && (Answer[3] != 0x00)) return NXPNCI_XBH_FALSE;
    NxpNci_WaitForReception(Answer, sizeof(Answer), &AnswerSize, NFC_RECEPTION_WAIT);
    if((Answer[0] != 0x61) && (Answer[1] != 0x06)) return NXPNCI_XBH_FALSE;

    NCIRfDiscoverSelect[4] = gNextTag_Protocol;
    if (gNextTag_Protocol == PROT_ISODEP) NCIRfDiscoverSelect[5] = INTF_ISODEP;
    else if (gNextTag_Protocol == PROT_ISODEP) NCIRfDiscoverSelect[5] = INTF_NFCDEP;
    else if (gNextTag_Protocol == PROT_MIFARE) NCIRfDiscoverSelect[5] = INTF_TAGCMD;
    else NCIRfDiscoverSelect[5] = INTF_FRAME;
    NxpNci_HostTransceive(NCIRfDiscoverSelect, sizeof(NCIRfDiscoverSelect), Answer, sizeof(Answer), &AnswerSize);
    if ((Answer[0] == 0x41) && (Answer[1] == 0x04) && (Answer[3] == 0x00))
    {
        NxpNci_WaitForReception(Answer, sizeof(Answer), &AnswerSize, NFC_RECEPTION_WAIT);
        if ((Answer[0] == 0x61) || (Answer[1] == 0x05))
        {
            pRfIntf->Interface = Answer[4];
            pRfIntf->Protocol = Answer[5];
            pRfIntf->ModeTech = Answer[6];
            NxpNci_FillInterfaceInfo(pRfIntf, &Answer[10]);
            status = NXPNCI_XBH_TRUE;
        }
    }

    return status;
}

XBH_BOOL NxpNci_ReaderReActivate(NxpNci_RfIntf_t *pRfIntf)
{
    XBH_U8 NCIDeactivate[] = {0x21, 0x06, 0x01, 0x01};
    XBH_U8 NCIActivate[] = {0x21, 0x04, 0x03, 0x01, 0x00, 0x00};
    XBH_U8 Answer[MAX_NCI_FRAME_SIZE];
    XBH_U16 AnswerSize;

    /* First de-activate the target */
    NxpNci_HostTransceive(NCIDeactivate, sizeof(NCIDeactivate), Answer, sizeof(Answer), &AnswerSize);
    NxpNci_WaitForReception(Answer, sizeof(Answer), &AnswerSize, NFC_RECEPTION_WAIT);

    /* Then re-activate the target */
    NCIActivate[4] = pRfIntf->Protocol;
    NCIActivate[5] = pRfIntf->Interface;
    NxpNci_HostTransceive(NCIActivate, sizeof(NCIActivate), Answer, sizeof(Answer), &AnswerSize);
    NxpNci_WaitForReception(Answer, sizeof(Answer), &AnswerSize, NFC_RECEPTION_WAIT);
    if((Answer[0] != 0x61) || (Answer[1] != 0x05)) return NXPNCI_XBH_FALSE;
    return NXPNCI_XBH_TRUE;
}
#endif //#ifndef REMOVE_RW_SUPPORT

XBH_BOOL NxpNci_Connect(void)
{
    XBH_U8 i = 2;
    XBH_U8 NCICoreInit_2_0[] = {0x20, 0x01, 0x02, 0x00, 0x00};
    XBH_U8 Answer[MAX_NCI_FRAME_SIZE];
    XBH_U16 AnswerSize;
    /* Open connection to NXPNCI */

    tml_Connect ();
    /* Loop until NXPNCI answers */
    while(NxpNci_CheckDevPres() != NXPNCI_XBH_TRUE)
    {
        if(i-- == 0) return NXPNCI_XBH_FALSE;
        //Sleep(500);
        usleep(500*1000);
    }
    XLOGD("4***********************************");
    usleep(50*1000);
	NxpNci_HostTransceive(NCICoreInit_2_0, sizeof(NCICoreInit_2_0), Answer, sizeof(Answer), &AnswerSize);
	if ((Answer[0] != 0x40) || (Answer[1] != 0x01) || (Answer[3] != 0x00)){
        //PRINT_BUF("NxpNci_Connect ERR:",Answer,4);
        return NXPNCI_XBH_FALSE;
    }
    XLOGD("5***********************************");
    return NXPNCI_XBH_TRUE;
}

void NxpNci_GetFwVersion(unsigned char fw[3])
{
    fw[0] = gNfcController_fw_version[0];
    fw[1] = gNfcController_fw_version[1];
    fw[2] = gNfcController_fw_version[2];
}

XBH_BOOL NxpNci_Disconnect(void)
{
    /* Close connection to NXPNCI */
    tml_Disconnect ();

    return NXPNCI_XBH_TRUE;
}

XBH_BOOL NxpNci_ConfigureSettings(void)
{
    XBH_U8 Answer[MAX_NCI_FRAME_SIZE];
    XBH_U16 AnswerSize;
    XBH_U8 NCICoreReset[] = {0x20, 0x00, 0x01, 0x00};
    XBH_U8 NCICoreInit_2_0[] = {0x20, 0x01, 0x02, 0x00, 0x00};
#if (NXP_CORE_CONF_EXTN | NXP_CLK_CONF | NXP_TVDD_CONF | NXP_RF_CONF)
    XBH_U8 NCIReadTS[] = {0x20, 0x03, 0x03, 0x01, 0xA0, 0x14};
    XBH_U8 NCIWriteTS[7+32] = {0x20, 0x02, 0x24, 0x01, 0xA0, 0x14, 0x20};
#endif
    XBH_BOOL isResetRequired = false;

    /* Apply settings according definition of Nfc_settings.h header file */
#if NXP_CORE_STANDBY
    if (sizeof(NxpNci_CORE_STANDBY) != 0)
    {
        isResetRequired = true;
        NxpNci_HostTransceive(NxpNci_CORE_STANDBY, sizeof(NxpNci_CORE_STANDBY), Answer, sizeof(Answer), &AnswerSize);
        if ((Answer[0] != 0x4F) || (Answer[1] != 0x00) || (Answer[3] != 0x00)) return NXPNCI_XBH_FALSE;
    }
#endif

#if NXP_CORE_CONF
    if (sizeof(NxpNci_CORE_CONF) != 0)
    {
        NxpNci_HostTransceive(NxpNci_CORE_CONF, sizeof(NxpNci_CORE_CONF), Answer, sizeof(Answer), &AnswerSize);
        if ((Answer[0] != 0x40) || (Answer[1] != 0x02) || (Answer[3] != 0x00) || (Answer[4] != 0x00)) return NXPNCI_XBH_FALSE;
    }
#endif

    /* All further settings are not versatile, so configuration only applied if there are changes (application build timestamp)
       or in case of PN7150B0HN/C11004 Anti-tearing recovery procedure inducing RF setings were restored to their default value */
#if (NXP_CORE_CONF_EXTN | NXP_CLK_CONF | NXP_TVDD_CONF | NXP_RF_CONF)
    /* First read timestamp stored in NFC Controller */
    NxpNci_HostTransceive(NCIReadTS, sizeof(NCIReadTS), Answer, sizeof(Answer), &AnswerSize);
    if ((Answer[0] != 0x40) || (Answer[1] != 0x03) || (Answer[3] != 0x00)) return NXPNCI_XBH_FALSE;

	/* Then compare with current build timestamp, and check RF settings restore flag */
    if(!memcmp(&Answer[8], NxpNci_SettingCurrentTS, sizeof(NxpNci_SettingCurrentTS)) && (gRfSettingsRestored_flag == false))
    {
        /* No change, nothing to do */
    }
    else
	{
        /* Apply settings */
#if NXP_CORE_CONF_EXTN
        if (sizeof(NxpNci_CORE_CONF_EXTN) != 0)
        {
            isResetRequired = true;
            NxpNci_HostTransceive(NxpNci_CORE_CONF_EXTN, sizeof(NxpNci_CORE_CONF_EXTN), Answer, sizeof(Answer), &AnswerSize);
            if ((Answer[0] != 0x40) || (Answer[1] != 0x02) || (Answer[3] != 0x00) || (Answer[4] != 0x00)) return NXPNCI_XBH_FALSE;
        }
#endif

#if NXP_CLK_CONF
        if (sizeof(NxpNci_CLK_CONF) != 0)
        {
            isResetRequired = true;
            NxpNci_HostTransceive(NxpNci_CLK_CONF, sizeof(NxpNci_CLK_CONF), Answer, sizeof(Answer), &AnswerSize);
            if ((Answer[0] != 0x40) || (Answer[1] != 0x02) || (Answer[3] != 0x00) || (Answer[4] != 0x00)) return NXPNCI_XBH_FALSE;
        }
#endif

#if NXP_TVDD_CONF
        if (sizeof(NxpNci_TVDD_CONF) != 0)
        {
            isResetRequired = true;
            NxpNci_HostTransceive(NxpNci_TVDD_CONF, sizeof(NxpNci_TVDD_CONF), Answer, sizeof(Answer), &AnswerSize);
            if ((Answer[0] != 0x40) || (Answer[1] != 0x02) || (Answer[3] != 0x00) || (Answer[4] != 0x00)) return NXPNCI_XBH_FALSE;
        }
#endif

#if NXP_RF_CONF
        if (sizeof(NxpNci_RF_CONF) != 0)
        {
            isResetRequired = true;
            NxpNci_HostTransceive(NxpNci_RF_CONF, sizeof(NxpNci_RF_CONF), Answer, sizeof(Answer), &AnswerSize);
            if ((Answer[0] != 0x40) || (Answer[1] != 0x02) || (Answer[3] != 0x00) || (Answer[4] != 0x00)) return NXPNCI_XBH_FALSE;
        }
#endif
        /* Store curent timestamp to NFC Controller memory for further checks */
        memcpy(&NCIWriteTS[7], NxpNci_SettingCurrentTS, sizeof(NxpNci_SettingCurrentTS));
        NxpNci_HostTransceive(NCIWriteTS, sizeof(NCIWriteTS), Answer, sizeof(Answer), &AnswerSize);
        if ((Answer[0] != 0x40) || (Answer[1] != 0x02) || (Answer[3] != 0x00) || (Answer[4] != 0x00)) return NXPNCI_XBH_FALSE;
    }
#endif

    if(isResetRequired)
    {
        /* Reset the NFC Controller to insure new settings apply */
        NxpNci_HostTransceive(NCICoreReset, sizeof(NCICoreReset), Answer, sizeof(Answer), &AnswerSize);
        if ((Answer[0] != 0x40) || (Answer[1] != 0x00) || (Answer[3] != 0x00)) return NXPNCI_XBH_FALSE;
		/* Catch NTF */
		NxpNci_WaitForReception(Answer, sizeof(Answer), &AnswerSize, NFC_RECEPTION_WAIT);

		NxpNci_HostTransceive(NCICoreInit_2_0, sizeof(NCICoreInit_2_0), Answer, sizeof(Answer), &AnswerSize);
		if ((Answer[0] != 0x40) || (Answer[1] != 0x01) || (Answer[3] != 0x00)) return NXPNCI_XBH_FALSE;
    }

    return NXPNCI_XBH_TRUE;
}

XBH_BOOL NxpNci_ConfigureMode(unsigned char mode)
{
    XBH_U8 Command[MAX_NCI_FRAME_SIZE];
    XBH_U8 Answer[MAX_NCI_FRAME_SIZE];
    XBH_U16 AnswerSize;
    XBH_U8 Item = 0;
    XBH_U8 NCIDiscoverMap[] = {0x21, 0x00};

#ifndef REMOVE_CARDEMU_SUPPORT
    const XBH_U8 DM_CARDEMU[] = {0x4, 0x2, 0x2};
    const XBH_U8 R_CARDEMU[] = {0x1, 0x3, 0x0, 0x1, 0x4};
#endif //#ifndef REMOVE_CARDEMU_SUPPORT

#ifndef REMOVE_P2P_SUPPORT
    const XBH_U8 DM_P2P[] = {0x5, 0x3, 0x3};
    const XBH_U8 R_P2P[] = {0x1, 0x3, 0x0, 0x1, 0x5};
    XBH_U8 NCISetConfig_NFC[] = {0x20, 0x02, 0x1F, 0x02, 0x29, 0x0D, 0x46, 0x66, 0x6D, 0x01, 0x01, 0x11, 0x03, 0x02, 0x00, 0x01, 0x04, 0x01, 0xFA, 0x61, 0x0D, 0x46, 0x66, 0x6D, 0x01, 0x01, 0x11, 0x03, 0x02, 0x00, 0x01, 0x04, 0x01, 0xFA};
#endif //#ifndef REMOVE_P2P_SUPPORT

#ifndef REMOVE_RW_SUPPORT
    const XBH_U8 DM_RW[] = {0x1, 0x1, 0x1, 0x2, 0x1, 0x1, 0x3, 0x1, 0x1, 0x4, 0x1, 0x2, 0x80, 0x01, 0x80};
    XBH_U8 NCIPropAct[] = {0x2F, 0x02, 0x00};
#endif //#ifndef REMOVE_RW_SUPPORT
#if !((defined REMOVE_P2P_SUPPORT) && (defined REMOVED_CARDEMU_SUPPORT))
    XBH_U8 NCIRouting[] = {0x21, 0x01, 0x07, 0x00, 0x01};
    XBH_U8 NCISetConfig_NFCA_SELRSP[] = {0x20, 0x02, 0x04, 0x01, 0x32, 0x01, 0x00};
#endif //#if !((defined REMOVE_P2P_SUPPORT) && (defined REMOVED_CARDEMU_SUPPORT))

    if(mode == 0) return NXPNCI_XBH_TRUE;

#ifndef REMOVE_RW_SUPPORT
    /* Enable Proprietary interface for T4T card presence check procedure */
    if (mode == NXPNCI_MODE_RW)
    {
        NxpNci_HostTransceive(NCIPropAct, sizeof(NCIPropAct), Answer, sizeof(Answer), &AnswerSize);
        if ((Answer[0] != 0x4F) || (Answer[1] != 0x02) || (Answer[3] != 0x00)) return NXPNCI_XBH_FALSE;
    }
#endif //#ifndef REMOVE_RW_SUPPORT

    /* Building Discovery Map command */
    Item = 0;
#ifndef REMOVE_CARDEMU_SUPPORT
    if (mode & NXPNCI_MODE_CARDEMU)
    {
        memcpy(&Command[4+(3*Item)], DM_CARDEMU, sizeof(DM_CARDEMU));
        Item++;
    }
#endif //#ifndef REMOVE_CARDEMU_SUPPORT
#ifndef REMOVE_P2P_SUPPORT
    if (mode & NXPNCI_MODE_P2P)
    {
        memcpy(&Command[4+(3*Item)], DM_P2P, sizeof(DM_P2P));
        Item++;
    }
#endif //#ifndef REMOVE_P2P_SUPPORT
#ifndef REMOVE_RW_SUPPORT
    if (mode & NXPNCI_MODE_RW)
    {
        memcpy(&Command[4+(3*Item)], DM_RW, sizeof(DM_RW));
        Item+=sizeof(DM_RW)/3;
    }
#endif //#ifndef REMOVE_RW_SUPPORT
#if !((defined REMOVE_P2P_SUPPORT) && (defined REMOVED_CARDEMU_SUPPORT) && (defined REMOVE_RW_SUPPORT))
    if (Item != 0)
    {
        memcpy(Command, NCIDiscoverMap, sizeof(NCIDiscoverMap));
        Command[2] = 1 + (Item*3);
        Command[3] = Item;
        NxpNci_HostTransceive(Command, 3 + Command[2], Answer, sizeof(Answer), &AnswerSize);
        if ((Answer[0] != 0x41) || (Answer[1] != 0x00) || (Answer[3] != 0x00)) return NXPNCI_XBH_FALSE;
    }
#endif //#if !((defined REMOVE_P2P_SUPPORT) && (defined REMOVED_CARDEMU_SUPPORT) && (defined REMOVE_RW_SUPPORT))

    /* Configuring routing */
    Item = 0;
#ifndef REMOVE_CARDEMU_SUPPORT
    if (mode & NXPNCI_MODE_CARDEMU)
    {
        memcpy(&Command[5+(5*Item)], R_CARDEMU, sizeof(R_CARDEMU));
        Item++;
    }
#endif //#ifndef REMOVE_CARDEMU_SUPPORT
#ifndef REMOVE_P2P_SUPPORT
    if (mode & NXPNCI_MODE_P2P)
    {
        memcpy(&Command[5+(5*Item)], R_P2P, sizeof(R_P2P));
        Item++;
    }
#endif //#ifndef REMOVE_P2P_SUPPORT
#if !((defined REMOVE_P2P_SUPPORT) && (defined REMOVED_CARDEMU_SUPPORT))
    if (Item != 0)
    {
        memcpy(Command, NCIRouting, sizeof(NCIRouting));
        Command[2] = 2 + (Item*5);
        Command[4] = Item;
        NxpNci_HostTransceive(Command, 3 + Command[2] , Answer, sizeof(Answer), &AnswerSize);
        if ((Answer[0] != 0x41) || (Answer[1] != 0x01) || (Answer[3] != 0x00)) return NXPNCI_XBH_FALSE;
    }
#endif //#if !((defined REMOVE_P2P_SUPPORT) && (defined REMOVED_CARDEMU_SUPPORT))

    /* Setting NFCA SEL_RSP */
#ifndef REMOVE_CARDEMU_SUPPORT
    if (mode & NXPNCI_MODE_CARDEMU)
    {
        NCISetConfig_NFCA_SELRSP[6] += 0x20;
    }
#endif //#ifndef REMOVE_CARDEMU_SUPPORT
#ifndef REMOVE_P2P_SUPPORT
    if (mode & NXPNCI_MODE_P2P)
    {
        NCISetConfig_NFCA_SELRSP[6] += 0x40;
    }
#endif //#ifndef REMOVE_P2P_SUPPORT
#if !((defined REMOVE_P2P_SUPPORT) && (defined REMOVED_CARDEMU_SUPPORT))
    if (NCISetConfig_NFCA_SELRSP[6] != 0x00)
    {
        NxpNci_HostTransceive(NCISetConfig_NFCA_SELRSP, sizeof(NCISetConfig_NFCA_SELRSP), Answer, sizeof(Answer), &AnswerSize);
        if ((Answer[0] != 0x40) || (Answer[1] != 0x02) || (Answer[3] != 0x00)) return NXPNCI_XBH_FALSE;
    }
#endif //#if !((defined REMOVE_P2P_SUPPORT) && (defined REMOVED_CARDEMU_SUPPORT))

#ifndef REMOVE_P2P_SUPPORT
    /* Setting LLCP support */
    if (mode & NXPNCI_MODE_P2P)
    {
        NxpNci_HostTransceive(NCISetConfig_NFC, sizeof(NCISetConfig_NFC), Answer, sizeof(Answer), &AnswerSize);
        if ((Answer[0] != 0x40) || (Answer[1] != 0x02) || (Answer[3] != 0x00)) return NXPNCI_XBH_FALSE;
    }
#endif //#ifndef REMOVE_P2P_SUPPORT

    return NXPNCI_XBH_TRUE;
}

XBH_BOOL NxpNci_ConfigureParams(unsigned char *pCmd, unsigned short CmdSize)
{
    XBH_U8 Answer[MAX_NCI_FRAME_SIZE];
    XBH_U16 AnswerSize;

    NxpNci_HostTransceive(pCmd, CmdSize, Answer, sizeof(Answer), &AnswerSize);
    if ((Answer[0] != 0x40) || (Answer[1] != 0x02) || (Answer[3] != 0x00)) return NXPNCI_XBH_FALSE;

    return NXPNCI_XBH_TRUE;
}

XBH_BOOL NxpNci_StartDiscovery(unsigned char *pTechTab, unsigned char TechTabSize)
{
    XBH_U8 Answer[MAX_NCI_FRAME_SIZE];
    XBH_U16 AnswerSize;
    XBH_U8 i;

    NCIStartDiscovery[0] = 0x21;
    NCIStartDiscovery[1] = 0x03;
    NCIStartDiscovery[2] = (TechTabSize * 2) + 1;
    NCIStartDiscovery[3] = TechTabSize;
    for (i=0; i<TechTabSize; i++)
    {
        NCIStartDiscovery[(i*2)+4] = pTechTab[i];
        NCIStartDiscovery[(i*2)+5] = 0x01;
    }

    NCIStartDiscovery_length = (TechTabSize * 2) + 4;
    NxpNci_HostTransceive(NCIStartDiscovery, NCIStartDiscovery_length, Answer, sizeof(Answer), &AnswerSize);
    if ((Answer[0] != 0x41) || (Answer[1] != 0x03) || (Answer[3] != 0x00))
    {
        //XLOGD("***************************%x %x %x",Answer[0],Answer[1],Answer[3]);
        return NXPNCI_XBH_FALSE;
    }

    return NXPNCI_XBH_TRUE;
}

XBH_BOOL NxpNci_StopDiscovery(void)
{
    XBH_U8 NCIStopDiscovery[] = {0x21, 0x06, 0x01, 0x00};
    XBH_U8 Answer[MAX_NCI_FRAME_SIZE];
    XBH_U16 AnswerSize;

    NxpNci_HostTransceive(NCIStopDiscovery, sizeof(NCIStopDiscovery), Answer, sizeof(Answer), &AnswerSize);
    NxpNci_WaitForReception(Answer, sizeof(Answer), &AnswerSize, NFC_DISCOVERY_WAIT);

    return NXPNCI_XBH_TRUE;
}

XBH_BOOL NxpNci_WaitForDiscoveryNotification(NxpNci_RfIntf_t *pRfIntf)
{
    XBH_U8 NCIRfDiscoverSelect[] = {0x21, 0x04, 0x03, 0x01, PROT_ISODEP, INTF_ISODEP};
    XBH_U8 Answer[MAX_NCI_FRAME_SIZE];
    XBH_U16 AnswerSize;

#ifndef REMOVE_P2P_SUPPORT
    XBH_U8 NCIStopDiscovery[] = {0x21, 0x06, 0x01, 0x00};
    XBH_U8 NCIRestartDiscovery[] = {0x21, 0x06, 0x01, 0x03};
    XBH_U8 saved_NTF[7];
wait:
#endif //#ifndef REMOVE_P2P_SUPPORT
    do
    {
        if(NxpNci_WaitForReception(Answer, sizeof(Answer), &AnswerSize, NFC_RECEPTION_WAIT) == NXPNCI_XBH_FALSE){
			return NXPNCI_XBH_FALSE;
		}
    }while ((Answer[0] != 0x61) || ((Answer[1] != 0x05) && (Answer[1] != 0x03)));

    gNextTag_Protocol = PROT_UNDETERMINED;

    /* Is RF_INTF_ACTIVATED_NTF ? */
    if (Answer[1] == 0x05)
    {
        pRfIntf->Interface = Answer[4];
        pRfIntf->Protocol = Answer[5];
        pRfIntf->ModeTech = Answer[6];
        pRfIntf->MoreTags = false;
        NxpNci_FillInterfaceInfo(pRfIntf, &Answer[10]);

#ifndef REMOVE_P2P_SUPPORT
        /* Verifying if not a P2P device also presenting T4T emulation */
        if ((pRfIntf->Interface == INTF_ISODEP) && (pRfIntf->Protocol == PROT_ISODEP) && ((pRfIntf->ModeTech & MODE_LISTEN) != MODE_LISTEN))
        {
            memcpy(saved_NTF, Answer, sizeof(saved_NTF));
            while(1)
            {
                /* Restart the discovery loop */
                NxpNci_HostTransceive(NCIRestartDiscovery, sizeof(NCIRestartDiscovery), Answer, sizeof(Answer), &AnswerSize);
                NxpNci_WaitForReception(Answer, sizeof(Answer), &AnswerSize, NFC_RECEPTION_WAIT);

                /* Wait for discovery */
                do NxpNci_WaitForReception(Answer, sizeof(Answer), &AnswerSize, NFC_DISCOVERY_WAIT);
                while ((AnswerSize == 4) && (Answer[0] == 0x60) && (Answer[1] == 0x07));

                if ((AnswerSize != 0) && (Answer[0] == 0x61) && (Answer[1] == 0x05))
                {
                    /* Is same device detected ? */
                    if (memcmp(saved_NTF, Answer, sizeof(saved_NTF)) == 0) break;
                    /* Is P2P detected ? */
                    if (Answer[5] == PROT_NFCDEP)
                    {
                        pRfIntf->Interface = Answer[4];
                        pRfIntf->Protocol = Answer[5];
                        pRfIntf->ModeTech = Answer[6];
                        pRfIntf->MoreTags = false;
                        NxpNci_FillInterfaceInfo(pRfIntf, &Answer[10]);
                        break;
                    }
                }
                else
                {
                    if (AnswerSize != 0)
                    {
                        /* Flush any other notification  */
                         NxpNci_WaitForReception(Answer, sizeof(Answer), &AnswerSize, NFC_RECEPTION_WAIT);
                        /* Restart the discovery loop */
                        NxpNci_HostTransceive(NCIRestartDiscovery, sizeof(NCIRestartDiscovery), Answer, sizeof(Answer), &AnswerSize);
                        NxpNci_WaitForReception(Answer, sizeof(Answer), &AnswerSize, NFC_RECEPTION_WAIT);
                    }
                    goto wait;
                }
            }
        }
#endif //#ifndef REMOVE_P2P_SUPPORT
    }
    else /* RF_DISCOVER_NTF */
    {
        pRfIntf->Interface = INTF_UNDETERMINED;
        pRfIntf->Protocol = Answer[4];
        pRfIntf->ModeTech = Answer[5];
        pRfIntf->MoreTags = true;

        /* Get next NTF for further activation */
        do {
            if(NxpNci_WaitForReception(Answer, sizeof(Answer), &AnswerSize, NFC_RECEPTION_WAIT) == NXPNCI_XBH_FALSE)    return NXPNCI_XBH_FALSE;
        } while ((Answer[0] != 0x61) || (Answer[1] != 0x03));
        gNextTag_Protocol = Answer[4];

        /* Remaining NTF ? */
        while(Answer[AnswerSize-1] == 0x02) NxpNci_WaitForReception(Answer, sizeof(Answer), &AnswerSize, NFC_RECEPTION_WAIT);

        /* In case of multiple cards, select the first one */
        NCIRfDiscoverSelect[4] = pRfIntf->Protocol;
        if (pRfIntf->Protocol == PROT_ISODEP) NCIRfDiscoverSelect[5] = INTF_ISODEP;
        else if (pRfIntf->Protocol == PROT_NFCDEP) NCIRfDiscoverSelect[5] = INTF_NFCDEP;
        else if (pRfIntf->Protocol == PROT_MIFARE) NCIRfDiscoverSelect[5] = INTF_TAGCMD;
        else NCIRfDiscoverSelect[5] = INTF_FRAME;
        NxpNci_HostTransceive(NCIRfDiscoverSelect, sizeof(NCIRfDiscoverSelect), Answer, sizeof(Answer), &AnswerSize);
        if ((Answer[0] == 0x41) || (Answer[1] == 0x04) || (Answer[3] == 0x00))
        {
            NxpNci_WaitForReception(Answer, sizeof(Answer), &AnswerSize, NFC_RECEPTION_WAIT);
            if ((Answer[0] == 0x61) || (Answer[1] == 0x05))
            {
                pRfIntf->Interface = Answer[4];
                pRfIntf->Protocol = Answer[5];
                pRfIntf->ModeTech = Answer[6];
                NxpNci_FillInterfaceInfo(pRfIntf, &Answer[10]);
            }
#ifndef REMOVE_P2P_SUPPORT
            /* In case of P2P target detected but lost, inform application to restart discovery */
            else if (pRfIntf->Protocol == PROT_NFCDEP)
            {
                /* Restart the discovery loop */
                NxpNci_HostTransceive(NCIStopDiscovery, sizeof(NCIStopDiscovery), Answer, sizeof(Answer), &AnswerSize);
                NxpNci_WaitForReception(Answer, sizeof(Answer), &AnswerSize, NFC_RECEPTION_WAIT);
                NxpNci_HostTransceive(NCIStartDiscovery, NCIStartDiscovery_length, Answer, sizeof(Answer), &AnswerSize);
                goto wait;
            }
#endif //#ifndef REMOVE_P2P_SUPPORT
        }
    }

    /* In case of unknown target align protocol information */
    if (pRfIntf->Interface == INTF_UNDETERMINED) pRfIntf->Protocol = PROT_UNDETERMINED;

    return NXPNCI_XBH_TRUE;
}

#ifndef REMOVE_FACTORYTEST_SUPPORT
XBH_BOOL NxpNci_FactoryTest_Prbs(NxpNci_TechType_t type, NxpNci_Bitrate_t bitrate)
{
	XBH_U8 NCIPrbs[] = {0x2F, 0x30, 0x06, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01};
    XBH_U8 Answer[MAX_NCI_FRAME_SIZE];
    XBH_U16 AnswerSize;

    NCIPrbs[5] = type;
    NCIPrbs[6] = bitrate;

    NxpNci_HostTransceive(NCIPrbs, sizeof(NCIPrbs), Answer, sizeof(Answer), &AnswerSize);
    if ((Answer[0] != 0x4F) || (Answer[1] != 0x30) || (Answer[3] != 0x00)) return NXPNCI_XBH_FALSE;

    return NXPNCI_XBH_TRUE;
}

XBH_BOOL NxpNci_FactoryTest_RfOn(void)
{
	XBH_U8 NCIRfOn[] = {0x2F, 0x3D, 0x02, 0x20, 0x01};
    XBH_U8 Answer[MAX_NCI_FRAME_SIZE];
    XBH_U16 AnswerSize;

    NxpNci_HostTransceive(NCIRfOn, sizeof(NCIRfOn), Answer, sizeof(Answer), &AnswerSize);
    if ((Answer[0] != 0x4F) || (Answer[1] != 0x3D) || (Answer[3] != 0x00)) return NXPNCI_XBH_FALSE;

    return NXPNCI_XBH_TRUE;
}
#endif //#ifndef REMOVE_FACTORYTEST_SUPPORT
/*
*         Copyright (c), NXP Semiconductors Caen / France
*
*                     (C)NXP Semiconductors
*       All rights are reserved. Reproduction in whole or in part is
*      prohibited without the written consent of the copyright owner.
*  NXP reserves the right to make changes without notice at any time.
* NXP makes no warranty, expressed, implied or statutory, including but
* not limited to any implied warranty of merchantability or fitness for any
*particular purpose, or that the use will not infringe any third party patent,
* copyright or trademark. NXP must not be liable for any loss or damage
*                          arising from its use.
*/

#ifndef REMOVE_P2P_SUPPORT


/* Well-known LLCP SAP Values */
#define SAP_SDP         1
#define SAP_SNEP        4

/* SNEP codes */
#define SNEP_VER10      0x10
#define SNEP_PUT        0x2
#define SNEP_CONTINUE   0x80
#define SNEP_XBH_TRUE    0x81

/* LLCP PDU Types */
#define SYMM            0x0
#define PAX             0x1
#define AGF             0x2
#define UI              0x3
#define CONNECT         0x4
#define DISC            0x5
#define CC              0x6
#define DM              0x7
#define FRMR            0x8
#define SNL             0x9
#define reservedA       0xA
#define reservedB       0xB
#define I               0xC
#define RR              0xD
#define RNR             0xE
#define reservedF       0xF

/* LLCP parameters */
#define VERSION     1
#define MIUX        2
#define WKS         3
#define LTO         4
#define RW          5
#define SN          6

const unsigned char SNEP_PUT_XBH_TRUE[] = {SNEP_VER10, SNEP_XBH_TRUE, 0x00, 0x00, 0x00, 0x00};
const unsigned char SNEP_PUT_CONTINUE[] = {SNEP_VER10, SNEP_CONTINUE, 0x00, 0x00, 0x00, 0x00};
const unsigned char LLCP_CONNECT_SNEP[] = {0x11, 0x20};
const unsigned char LLCP_I_SNEP_PUT_HEADER[] = {SNEP_VER10, SNEP_PUT, 0x00, 0x00, 0x00, 0x00};
const unsigned char LLCP_SYMM[] = {0x00, 0x00};

unsigned char *pNdefMessage;
unsigned int NdefMessage_size = 0;

/* Defines the number of symmetry exchanges is expected before initiating the NDEF push (to allow
 * a remote phone to beam an NDEF message first) */
#define NDEF_PUSH_DELAY_COUNT    2

/* Defines at which frequency the symmetry is exchange (in ms) */
#define SYMM_FREQ    500

#define P2P_NDEF_SNEP_MAX_FRAME_SIZE 122

/* MACRO to increment N(R)/N(S) */
#define INC(x) (++x % 16)

typedef enum
{
    Idle,
    Initial_0,
    DelayingPush,
    SnepClientConnecting,
    SnepClientConnected,
    NdefMsgWaitingForContinue,
    NdefMsgSending,
    NdefMsgSent
} P2P_SnepClient_state_t;

typedef struct
{
    unsigned char Dsap;
    unsigned char Pdu;
    unsigned char Ssap;
    unsigned char Version;
    unsigned short Miux;
    unsigned short Wks;
    unsigned char Lto;
    unsigned char Rw;
    unsigned char Sn[30];
} P2P_NDEF_LlcpHeader_t;

typedef void P2P_NDEF_PushCallback_t (unsigned char*, unsigned short);
typedef void P2P_NDEF_PullCallback_t (unsigned char*, unsigned short, unsigned int);

static P2P_SnepClient_state_t eP2P_SnepClient_State = Initial_0;
static P2P_NDEF_PushCallback_t *pP2P_NDEF_PushCb = XBH_NULL;
static P2P_NDEF_PullCallback_t *pP2P_NDEF_PullCb = XBH_NULL;
static unsigned short P2P_SnepClient_DelayCount = NDEF_PUSH_DELAY_COUNT;
static unsigned int gP2P_NDEF_size;
static unsigned int gP2P_DEF_fragmentation = 0;
static P2P_NDEF_LlcpHeader_t gP2P_NDEF_fragmentation_header;
static unsigned int gP2P_NDEF_ptr;
static unsigned char gP2P_NDEF_curNR;
static unsigned char gP2P_NDEF_curNS;

static void ParseLlcp(unsigned char *pBuf, unsigned short BufSize, P2P_NDEF_LlcpHeader_t *pLlcpHeader)
{
    pLlcpHeader->Dsap = pBuf[0] >> 2;
    pLlcpHeader->Pdu = ((pBuf[0] & 3) << 2) + (pBuf[1] >> 6);
    pLlcpHeader->Ssap = pBuf[1] & 0x3F;

    if ((pLlcpHeader->Pdu == PAX) ||
        (pLlcpHeader->Pdu == CONNECT) ||
        (pLlcpHeader->Pdu == CC) ||
        (pLlcpHeader->Pdu == SNL))
    {
        XBH_U8 i = 2;
        while(i<BufSize)
        {
            switch (pBuf[i]){
            case VERSION:
                pLlcpHeader->Version = pBuf[i+2];
                break;
            case MIUX:
                pLlcpHeader->Miux = (pBuf[i+2] << 8) + pBuf[i+3];
                break;
            case WKS:
                pLlcpHeader->Wks = (pBuf[i+2] << 8) + pBuf[i+3];
                break;
            case LTO:
                pLlcpHeader->Lto = pBuf[i+2];
                break;
            case RW:
                pLlcpHeader->Rw = pBuf[i+2];
                break;
            case SN:
                memcpy(pLlcpHeader->Sn, &pBuf[i+2], pBuf[i+1] < sizeof(pLlcpHeader->Sn) ? pBuf[i+1] : sizeof(pLlcpHeader->Sn));
                break;
            default:
                break;
            }
            i += pBuf[i+1]+2;
        }
    }
    else if (pLlcpHeader->Pdu == I)
    {
        gP2P_NDEF_curNR = pBuf[2] >> 4;
        gP2P_NDEF_curNS = pBuf[2] & 0xF;
    }
    else if (pLlcpHeader->Pdu == RR)
    {
        gP2P_NDEF_curNS = pBuf[2];

    }
}

static void FillLlcp(P2P_NDEF_LlcpHeader_t LlcpHeader, unsigned char *pBuf)
{
    pBuf[0] = (LlcpHeader.Ssap << 2) + ((LlcpHeader.Pdu >> 2) & 3);
    pBuf[1] = (LlcpHeader.Pdu << 6) + LlcpHeader.Dsap;
}

XBH_BOOL P2P_NDEF_SetMessage(unsigned char *pMessage, unsigned int Message_size, void *pCb)
{
    if (pMessage != XBH_NULL)
    {
        pNdefMessage = pMessage;
        NdefMessage_size = Message_size;
        pP2P_NDEF_PushCb = (P2P_NDEF_PushCallback_t*) pCb;
        /* Trigger sending dynamically new message */
        if (eP2P_SnepClient_State == NdefMsgSent)
        {
            eP2P_SnepClient_State = SnepClientConnected;
        }
        return true;
    }
    else
    {
        NdefMessage_size = 0;
        pP2P_NDEF_PushCb = XBH_NULL;
        return false;
    }
}

void P2P_NDEF_RegisterPullCallback(void *pCb)
{
    pP2P_NDEF_PullCb = (P2P_NDEF_PullCallback_t*) pCb;
}

void P2P_NDEF_Reset(void)
{
    gP2P_DEF_fragmentation = 0;

    if (NdefMessage_size != 0)
    {
        eP2P_SnepClient_State = Initial_0;
    }
    else
    {
        eP2P_SnepClient_State = Idle;
    }
}

void P2P_NDEF_Next(unsigned char *pCmd, unsigned short Cmd_size, unsigned char *pRsp, unsigned short *pRsp_size)
{
    P2P_NDEF_LlcpHeader_t LlcpHeader;
    
    /* Initialize with no answer by default */
    *pRsp_size = 0;

    ParseLlcp(pCmd, Cmd_size, &LlcpHeader);

    switch (LlcpHeader.Pdu)
    {
    case CONNECT:
        /* Is connection from SNEP Client ? */
        if ((LlcpHeader.Dsap == SAP_SNEP) || (memcmp(LlcpHeader.Sn, "urn:nfc:sn:snep", 15) == 0))
        {
            /* Only accept the connection is application is registered for NDEF reception */
            if(pP2P_NDEF_PullCb != XBH_NULL)
            {
                /* reset frame numbering */
                gP2P_NDEF_curNR = 0;
                gP2P_NDEF_curNS = 0;
                LlcpHeader.Pdu = CC;
                FillLlcp(LlcpHeader, pRsp);
                *pRsp_size = 2;
            }
        }
        else
        {
            /* Refuse any other connection request */
            LlcpHeader.Pdu = DM;
            FillLlcp(LlcpHeader, pRsp);
            *pRsp_size = 2;
        }
        break;

    case I:
        /* Is no reception already on-going ? */
        if (gP2P_DEF_fragmentation == 0)
        {
            /* Is SNEP PUT ? */
            if ((pCmd[3] == SNEP_VER10) && (pCmd[4] == SNEP_PUT))
            {
                gP2P_NDEF_size = pCmd[8] + (pCmd[7] << 8) + (pCmd[6] << 16) + (pCmd[5] << 24);

                /* Notify application of the NDEF reception */
                if(pP2P_NDEF_PullCb != XBH_NULL) pP2P_NDEF_PullCb(&pCmd[9], Cmd_size-9, gP2P_NDEF_size);

                if (gP2P_NDEF_size <= (unsigned int) (Cmd_size-9))
                {
                    /* Acknowledge the PUT request */
                    gP2P_DEF_fragmentation = 0;
                    LlcpHeader.Pdu = I;
                    FillLlcp(LlcpHeader, pRsp);
                    pRsp[2] = (gP2P_NDEF_curNS << 4) + INC(gP2P_NDEF_curNR);
                    memcpy(&pRsp[3], SNEP_PUT_XBH_TRUE, sizeof(SNEP_PUT_XBH_TRUE));
                    *pRsp_size = 9;
                }
                else
                {
                    gP2P_DEF_fragmentation += Cmd_size-9;
                    /* Acknowledge the PUT request */
                    LlcpHeader.Pdu = I;
                    FillLlcp(LlcpHeader, pRsp);
                    pRsp[2] = (gP2P_NDEF_curNS << 4) + INC(gP2P_NDEF_curNR);
                    memcpy(&pRsp[3], SNEP_PUT_CONTINUE, sizeof(SNEP_PUT_CONTINUE));
                    *pRsp_size = 9;
                }
            }
            else if ((pCmd[3] == SNEP_VER10) && (pCmd[4] == SNEP_CONTINUE))
            {
                eP2P_SnepClient_State = NdefMsgSending;
            }
        }
        else
        {
            /* Notify application of the NDEF reception */
            if(pP2P_NDEF_PullCb != XBH_NULL) pP2P_NDEF_PullCb(&pCmd[3], Cmd_size-3, gP2P_NDEF_size);

            gP2P_DEF_fragmentation += Cmd_size-3;

            /* is fragmentation complete ?*/
            if (gP2P_DEF_fragmentation == gP2P_NDEF_size)
            {
                /* Acknowledge the fragmented PUT request */
                gP2P_NDEF_fragmentation_header.Pdu = I;
                FillLlcp(LlcpHeader, pRsp);
                pRsp[2] = (gP2P_NDEF_curNS << 4) + INC(gP2P_NDEF_curNR);
                memcpy(&pRsp[3], SNEP_PUT_XBH_TRUE, sizeof(SNEP_PUT_XBH_TRUE));
                *pRsp_size = 9;
                gP2P_DEF_fragmentation = 0;
            }
            else
            {
                /* Acknowledge received fragment */
                LlcpHeader.Pdu = RR;
                FillLlcp(LlcpHeader, pRsp);
                pRsp[2] = INC(gP2P_NDEF_curNR);
                *pRsp_size = 3;
            }
        }
        break;

    case CC:
        /* Connection to remote SNEP server completed, send NDEF message inside SNEP PUT request */
        eP2P_SnepClient_State = SnepClientConnected;
        /* reset frame numbering */
        gP2P_NDEF_curNR = 0;
        gP2P_NDEF_curNS = 0;
        break;

    case RR:
        /* Is on-going reception ? */
        if (gP2P_DEF_fragmentation != 0)
        {
            /* Send SYMM without waiting to give up hand */
            memcpy(pRsp, LLCP_SYMM, sizeof(LLCP_SYMM));
            *pRsp_size = sizeof(LLCP_SYMM);
        }
        break;

    default:
        break;

    }

    /* No answer was set */
    if (*pRsp_size == 0)
    {
        switch(eP2P_SnepClient_State)
        {
        case Initial_0:
            if((pP2P_NDEF_PullCb == XBH_NULL) || (NDEF_PUSH_DELAY_COUNT == 0))
            {
                memcpy(pRsp, LLCP_CONNECT_SNEP, sizeof(LLCP_CONNECT_SNEP));
                *pRsp_size = sizeof(LLCP_CONNECT_SNEP);
                eP2P_SnepClient_State = SnepClientConnecting;
            }
            else
            {
                P2P_SnepClient_DelayCount = 1;
                eP2P_SnepClient_State = DelayingPush;
                /* Wait then send a SYMM */
                usleep (SYMM_FREQ*1000);
                memcpy(pRsp, LLCP_SYMM, sizeof(LLCP_SYMM));
                *pRsp_size = sizeof(LLCP_SYMM);
            }
            break;

        case DelayingPush:
            if(P2P_SnepClient_DelayCount == NDEF_PUSH_DELAY_COUNT)
            {
                if(gP2P_DEF_fragmentation == 0)
                {
                    memcpy(pRsp, LLCP_CONNECT_SNEP, sizeof(LLCP_CONNECT_SNEP));
                    *pRsp_size = sizeof(LLCP_CONNECT_SNEP);
                    eP2P_SnepClient_State = SnepClientConnecting;
                }
            }
            else
            {
                P2P_SnepClient_DelayCount++;
                /* Wait then send a SYMM */
                usleep (SYMM_FREQ*1000);
                memcpy(pRsp, LLCP_SYMM, sizeof(LLCP_SYMM));
                *pRsp_size = sizeof(LLCP_SYMM);
            }
            break;

        case SnepClientConnected:
            LlcpHeader.Pdu = I;
            FillLlcp(LlcpHeader, pRsp);
            pRsp[2] = (gP2P_NDEF_curNS << 4) + gP2P_NDEF_curNR;
            pRsp[3] = SNEP_VER10;
            pRsp[4] = SNEP_PUT;
            pRsp[5] = (unsigned char) ((NdefMessage_size >> 24) & 0xFF);
            pRsp[6] = (unsigned char) ((NdefMessage_size >> 16) & 0xFF);
            pRsp[7] = (unsigned char) ((NdefMessage_size >> 8) & 0xFF);
            pRsp[8] = (unsigned char) (NdefMessage_size & 0xFF);

            if (NdefMessage_size <= P2P_NDEF_SNEP_MAX_FRAME_SIZE)
            {
                memcpy(&pRsp[9], pNdefMessage, NdefMessage_size);
                *pRsp_size = 9 + NdefMessage_size;
                eP2P_SnepClient_State = NdefMsgSent;
                /* Notify application of the NDEF push */
                if(pP2P_NDEF_PushCb != XBH_NULL) pP2P_NDEF_PushCb(pNdefMessage, NdefMessage_size);
            }
            else
            {
                memcpy(&pRsp[9], pNdefMessage, P2P_NDEF_SNEP_MAX_FRAME_SIZE);
                gP2P_NDEF_ptr = P2P_NDEF_SNEP_MAX_FRAME_SIZE;
                *pRsp_size = 9 + P2P_NDEF_SNEP_MAX_FRAME_SIZE;
                eP2P_SnepClient_State = NdefMsgWaitingForContinue;
            }
            break;

        case NdefMsgWaitingForContinue:
            memcpy(pRsp, LLCP_SYMM, sizeof(LLCP_SYMM));
            *pRsp_size = sizeof(LLCP_SYMM);
            break;

        case NdefMsgSending:
            LlcpHeader.Pdu = I;
            FillLlcp(LlcpHeader, pRsp);
            pRsp[2] = (gP2P_NDEF_curNS << 4) + gP2P_NDEF_curNR;
            if (gP2P_NDEF_ptr + P2P_NDEF_SNEP_MAX_FRAME_SIZE >= NdefMessage_size)
            {
                memcpy(&pRsp[3], pNdefMessage+gP2P_NDEF_ptr, NdefMessage_size-gP2P_NDEF_ptr);
                *pRsp_size = 3 + NdefMessage_size-gP2P_NDEF_ptr;
                eP2P_SnepClient_State = NdefMsgSent;
                /* Notify application of the NDEF push */
                if(pP2P_NDEF_PushCb != XBH_NULL) pP2P_NDEF_PushCb(pNdefMessage, NdefMessage_size);
            }
            else
            {
                memcpy(&pRsp[3], pNdefMessage+gP2P_NDEF_ptr, P2P_NDEF_SNEP_MAX_FRAME_SIZE);
                gP2P_NDEF_ptr += P2P_NDEF_SNEP_MAX_FRAME_SIZE;
                *pRsp_size = 3 + P2P_NDEF_SNEP_MAX_FRAME_SIZE;
                eP2P_SnepClient_State = NdefMsgSending;
            }
            break;

        default:
            /* Wait then send SYMM */
            usleep (SYMM_FREQ*1000);
            memcpy(pRsp, LLCP_SYMM, sizeof(LLCP_SYMM));
            *pRsp_size = sizeof(LLCP_SYMM);
            break;
        }
    }
}

#endif
/*
*         Copyright (c), NXP Semiconductors Caen / France
*
*                     (C)NXP Semiconductors
*       All rights are reserved. Reproduction in whole or in part is
*      prohibited without the written consent of the copyright owner.
*  NXP reserves the right to make changes without notice at any time.
* NXP makes no warranty, expressed, implied or statutory, including but
* not limited to any implied warranty of merchantability or fitness for any
*particular purpose, or that the use will not infringe any third party patent,
* copyright or trademark. NXP must not be liable for any loss or damage
*                          arising from its use.
*/

#ifndef REMOVE_RW_SUPPORT
#ifndef REMOVE_NDEF_SUPPORT


/*
    TODO: Only simplified scenario is implemented yet:
     NDEF data stored in contiguous sector and started at sector #1
*/

#define MIFARE_FUNCTION_CLUSTER    0xE1
#define MIFARE_NFC_CLUSTER         0x03
#define MIFARE_NDEF_TLV            0x03

typedef enum
{
    Initial_1,
    Authenticated0,
    Reading_GPB,
    Authenticated,
    Reading_FirstBlk,
    Reading_Data,
    Writing_Data1,
    Writing_Data2,
    Writing_Data
} RW_NDEF_MIFARE_state_t;

typedef struct
{
    XBH_BOOL WriteOp;
    unsigned char BlkNb;
    unsigned short MessagePtr;
    unsigned short MessageSize;
    unsigned char *pMessage;
} RW_NDEF_MIFARE_Ndef_t;

static RW_NDEF_MIFARE_state_t eRW_NDEF_MIFARE_State = Initial_1;
static RW_NDEF_MIFARE_Ndef_t RW_NDEF_MIFARE_Ndef;

void RW_NDEF_MIFARE_Reset(void)
{
    eRW_NDEF_MIFARE_State = Initial_1;
    RW_NDEF_MIFARE_Ndef.pMessage = NdefBuffer;
}

void RW_NDEF_MIFARE_Read_Next(unsigned char *pRsp, unsigned short Rsp_size, unsigned char *pCmd, unsigned short *pCmd_size)
{
    /* By default no further command to be sent */
    *pCmd_size = 0;

    switch(eRW_NDEF_MIFARE_State)
    {
    case Initial_1:
        /* Authenticating first sector */
        pCmd[0] = 0x40;
        pCmd[1] = 0x00;
        pCmd[2] = 0x00;
        *pCmd_size = 3;
         eRW_NDEF_MIFARE_State = Authenticated0;
        break;

    case Authenticated0:
        if ((Rsp_size == 2) && (pRsp[Rsp_size-1] == 0x00))
        {
            /* Read GPB */
            pCmd[0] = 0x10;
            pCmd[1] = 0x30;
            pCmd[2] = 0x01;
            *pCmd_size = 3;
            eRW_NDEF_MIFARE_State = Reading_GPB;
        }
        break;

    case Reading_GPB:
        if ((Rsp_size == 18) && (pRsp[Rsp_size-1] == 0x00))
        {
            /* Is NDEF format ?*/
            if ((pRsp[3] == MIFARE_NFC_CLUSTER) && (pRsp[4] == MIFARE_FUNCTION_CLUSTER))
            {
                pCmd[0] = 0x40;
                pCmd[1] = 0x01;
                pCmd[2] = 0x01;
                *pCmd_size = 3;
                eRW_NDEF_MIFARE_State = Authenticated;

                RW_NDEF_MIFARE_Ndef.BlkNb = 4;
            }
        }
        break;

    case Authenticated:
        if ((Rsp_size == 2) && (pRsp[Rsp_size-1] == 0x00))
        {
            /* First NDEF data block to read ?*/
            if(RW_NDEF_MIFARE_Ndef.BlkNb == 4)
            {
                eRW_NDEF_MIFARE_State = Reading_FirstBlk;
            }
            else
            {
                RW_NDEF_MIFARE_Ndef.BlkNb++;
                eRW_NDEF_MIFARE_State = Reading_Data;
            }

            /* Read block */
            pCmd[0] = 0x10;
            pCmd[1] = 0x30;
            pCmd[2] = RW_NDEF_MIFARE_Ndef.BlkNb;
            *pCmd_size = 3;
        }
        break;

    case Reading_FirstBlk:
        if ((Rsp_size == 18) && (pRsp[Rsp_size-1] == 0x00))
        {
            unsigned char Tmp = 1;
            /* If not NDEF Type skip TLV */
            while (pRsp[Tmp] != MIFARE_NDEF_TLV)
            {
                Tmp += 2 + pRsp[Tmp+1];
                if (Tmp > Rsp_size) return;
            }

            if(pRsp[Tmp+1] == 0xFF)
            {
                RW_NDEF_MIFARE_Ndef.MessageSize = (pRsp[Tmp+2] << 8) + pRsp[Tmp+3];
                Tmp += 2;
            }
            else RW_NDEF_MIFARE_Ndef.MessageSize = pRsp[Tmp+1];

            /* If provisioned buffer is not large enough or message is empty, notify the application and stop reading */
            if ((RW_NDEF_MIFARE_Ndef.MessageSize > RW_MAX_NDEF_FILE_SIZE) || (RW_NDEF_MIFARE_Ndef.MessageSize == 0))
            {
                if(pRW_NDEF_PullCb != XBH_NULL) pRW_NDEF_PullCb(XBH_NULL, 0, RW_NDEF_MIFARE_Ndef.MessageSize);
                break;
            }

            /* Is NDEF read already completed ? */
            if (RW_NDEF_MIFARE_Ndef.MessageSize <= ((Rsp_size-1) - Tmp - 2))
            {
                memcpy (RW_NDEF_MIFARE_Ndef.pMessage, &pRsp[Tmp+2], RW_NDEF_MIFARE_Ndef.MessageSize);

                /* Notify application of the NDEF reception */
                if(pRW_NDEF_PullCb != XBH_NULL) pRW_NDEF_PullCb(RW_NDEF_MIFARE_Ndef.pMessage, RW_NDEF_MIFARE_Ndef.MessageSize, RW_NDEF_MIFARE_Ndef.MessageSize);
            }
            else
            {
                RW_NDEF_MIFARE_Ndef.MessagePtr = (Rsp_size-1) - Tmp - 2;
                memcpy (RW_NDEF_MIFARE_Ndef.pMessage, &pRsp[Tmp+2], RW_NDEF_MIFARE_Ndef.MessagePtr);
                RW_NDEF_MIFARE_Ndef.BlkNb++;

                /* Read next block */
                pCmd[0] = 0x10;
                pCmd[1] = 0x30;
                pCmd[2] = RW_NDEF_MIFARE_Ndef.BlkNb;
                *pCmd_size = 3;
                eRW_NDEF_MIFARE_State = Reading_Data;
            }
        }
        break;

    case Reading_Data:
        if ((Rsp_size == 18) && (pRsp[Rsp_size-1] == 0x00))
        {
            /* Is NDEF read already completed ? */
            if ((RW_NDEF_MIFARE_Ndef.MessageSize - RW_NDEF_MIFARE_Ndef.MessagePtr) < 16)
            {
                memcpy (&RW_NDEF_MIFARE_Ndef.pMessage[RW_NDEF_MIFARE_Ndef.MessagePtr], pRsp+1, RW_NDEF_MIFARE_Ndef.MessageSize - RW_NDEF_MIFARE_Ndef.MessagePtr);

                /* Notify application of the NDEF reception */
                if(pRW_NDEF_PullCb != XBH_NULL) pRW_NDEF_PullCb(RW_NDEF_MIFARE_Ndef.pMessage, RW_NDEF_MIFARE_Ndef.MessageSize, RW_NDEF_MIFARE_Ndef.MessageSize);
            }
            else
            {
                memcpy (&RW_NDEF_MIFARE_Ndef.pMessage[RW_NDEF_MIFARE_Ndef.MessagePtr], pRsp+1, 16);
                RW_NDEF_MIFARE_Ndef.MessagePtr += 16;
                RW_NDEF_MIFARE_Ndef.BlkNb++;

                /* Is Blk on next sector ?*/
                if(((RW_NDEF_MIFARE_Ndef.BlkNb+1)%4) == 0)
                {
                    /* Authenticate next block */
                    pCmd[0] = 0x40;
                    pCmd[1] = (RW_NDEF_MIFARE_Ndef.BlkNb+1)/4;
                    pCmd[2] = 0x01;
                    *pCmd_size = 3;
                    eRW_NDEF_MIFARE_State = Authenticated;
                }
                else
                {
                    /* Read next block */
                    pCmd[0] = 0x10;
                    pCmd[1] = 0x30;
                    pCmd[2] = RW_NDEF_MIFARE_Ndef.BlkNb;
                    *pCmd_size = 3;
                }
            }
        }
        break;

    default:
        break;
    }
}

void RW_NDEF_MIFARE_Write_Next(unsigned char *pRsp, unsigned short Rsp_size, unsigned char *pCmd, unsigned short *pCmd_size)
{
    /* By default no further command to be sent */
    *pCmd_size = 0;

    switch(eRW_NDEF_MIFARE_State)
    {
    case Initial_1:
        /* Authenticating first sector */
        pCmd[0] = 0x40;
        pCmd[1] = 0x00;
        pCmd[2] = 0x00;
        *pCmd_size = 3;
         eRW_NDEF_MIFARE_State = Authenticated0;
        break;

    case Authenticated0:
        if ((Rsp_size == 2) && (pRsp[Rsp_size-1] == 0x00))
        {
            /* Read MAD */
            pCmd[0] = 0x10;
            pCmd[1] = 0x30;
            pCmd[2] = 0x01;
            *pCmd_size = 3;
            eRW_NDEF_MIFARE_State = Reading_GPB;
        }
        break;

    case Reading_GPB:
        if ((Rsp_size == 18) && (pRsp[Rsp_size-1] == 0x00))
        {
            /* Is NDEF format ?*/
            if ((pRsp[3] == MIFARE_NFC_CLUSTER) && (pRsp[4] == MIFARE_FUNCTION_CLUSTER))
            {
                pCmd[0] = 0x40;
                pCmd[1] = 0x01;
                pCmd[2] = 0x01;
                *pCmd_size = 3;
                eRW_NDEF_MIFARE_State = Writing_Data1;
                RW_NDEF_MIFARE_Ndef.MessagePtr = 0;
                RW_NDEF_MIFARE_Ndef.BlkNb = 4;
            }
        }
        break;

    case Writing_Data1:
        if ((pRsp[Rsp_size-1] == 0x00) || (pRsp[Rsp_size-1] == 0x14))
        {
            /* Is NDEF write already completed ? */
            if (RW_NdefMessage_size <= RW_NDEF_MIFARE_Ndef.MessagePtr)
            {
                /* Notify application of the NDEF send completion */
                if(pRW_NDEF_PushCb != XBH_NULL) pRW_NDEF_PushCb(pRW_NdefMessage, RW_NdefMessage_size);
            }
            else if(((RW_NDEF_MIFARE_Ndef.BlkNb+1)%4) == 0)
            {
                /* Authenticate next block */
                pCmd[0] = 0x40;
                pCmd[1] = (RW_NDEF_MIFARE_Ndef.BlkNb+1)/4;
                pCmd[2] = 0x01;
                *pCmd_size = 3;
                eRW_NDEF_MIFARE_State = Writing_Data1;
                RW_NDEF_MIFARE_Ndef.BlkNb++;
            }
            else
            {
                pCmd[0] = 0x10;
                pCmd[1] = 0xA0;
                pCmd[2] = RW_NDEF_MIFARE_Ndef.BlkNb;
                *pCmd_size = 3;
                eRW_NDEF_MIFARE_State = Writing_Data2;
            }
        }
        break;

    case Writing_Data2:
        if ((Rsp_size == 3) && (pRsp[Rsp_size-1] == 0x14))
        {
            /* First block to write ? */
            if (RW_NDEF_MIFARE_Ndef.BlkNb == 4)
            {
                pCmd[0] = 0x10;
                pCmd[1] = 0x00;
                pCmd[2] = 0x00;
                pCmd[3] = 0x03;
                if (RW_NdefMessage_size > 0xFF)
                {
                    pCmd[4] = 0xFF;
                    pCmd[5] = (RW_NdefMessage_size & 0xFF00) >> 8;
                    pCmd[6] = RW_NdefMessage_size & 0xFF;
                    memcpy(&pCmd[7], pRW_NdefMessage, 10);
                    RW_NDEF_MIFARE_Ndef.MessagePtr = 10;
                }
                else
                {
                    pCmd[4] = (unsigned char) RW_NdefMessage_size;
                    memcpy(&pCmd[5], pRW_NdefMessage, 12);
                    RW_NDEF_MIFARE_Ndef.MessagePtr = 12;
                }
            }
            else
            {
                pCmd[0] = 0x10;
                memcpy(&pCmd[1], pRW_NdefMessage+RW_NDEF_MIFARE_Ndef.MessagePtr, 16);
                RW_NDEF_MIFARE_Ndef.MessagePtr+=16;
            }
            *pCmd_size = 17;
            RW_NDEF_MIFARE_Ndef.BlkNb++;
            eRW_NDEF_MIFARE_State = Writing_Data1;
        }
        break;

    case Writing_Data:
        if ((Rsp_size == 2) && (pRsp[Rsp_size-1] == 0x00))
        {
            /* Is NDEF write already completed ? */
            if (RW_NdefMessage_size <= RW_NDEF_MIFARE_Ndef.MessagePtr)
            {
                /* Notify application of the NDEF reception */
                if(pRW_NDEF_PushCb != XBH_NULL) pRW_NDEF_PushCb(pRW_NdefMessage, RW_NdefMessage_size);
            }
            else
            {
                /* Write NDEF content */
                pCmd[0] = 0xA2;
                pCmd[1] = RW_NDEF_MIFARE_Ndef.BlkNb;
                memcpy(&pCmd[2], pRW_NdefMessage+RW_NDEF_MIFARE_Ndef.MessagePtr, 4);
                *pCmd_size = 6;

                RW_NDEF_MIFARE_Ndef.MessagePtr+=4;
                RW_NDEF_MIFARE_Ndef.BlkNb++;
            }
        }
        break;

    default:
        break;
    }
}

#endif
#endif
/*
*         Copyright (c), NXP Semiconductors Caen / France
*
*                     (C)NXP Semiconductors
*       All rights are reserved. Reproduction in whole or in part is
*      prohibited without the written consent of the copyright owner.
*  NXP reserves the right to make changes without notice at any time.
* NXP makes no warranty, expressed, implied or statutory, including but
* not limited to any implied warranty of merchantability or fitness for any
*particular purpose, or that the use will not infringe any third party patent,
* copyright or trademark. NXP must not be liable for any loss or damage
*                          arising from its use.
*/

#ifndef REMOVE_RW_SUPPORT
#ifndef REMOVE_NDEF_SUPPORT


#define T1T_MAGIC_NUMBER    0xE1
#define T1T_NDEF_TLV        0x03

const unsigned char T1T_RID[] = {0x78,0x00,0x00,0x00,0x00,0x00,0x00};
const unsigned char T1T_RALL[] = {0x00,0x00,0x00};
const unsigned char T1T_READ8[] = {0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

typedef enum
{
    Initial_2,
    Getting_ID,
    Reading_CardContent,
    Reading_NDEF
} RW_NDEF_T1T_state_t;

typedef struct
{
    unsigned char HR0;
    unsigned char HR1;
    unsigned char UID[4];
    unsigned char BlkNb;
    unsigned short MessagePtr;
    unsigned short MessageSize;
    unsigned char *pMessage;
} RW_NDEF_T1T_Ndef_t;

static RW_NDEF_T1T_state_t eRW_NDEF_T1T_State = Initial_2;
static RW_NDEF_T1T_Ndef_t RW_NDEF_T1T_Ndef;

void RW_NDEF_T1T_Reset(void)
{
    eRW_NDEF_T1T_State = Initial_2;
    RW_NDEF_T1T_Ndef.pMessage = NdefBuffer;
}

void RW_NDEF_T1T_Read_Next(unsigned char *pRsp, unsigned short Rsp_size, unsigned char *pCmd, unsigned short *pCmd_size)
{
    /* By default no further command to be sent */
    *pCmd_size = 0;

    switch(eRW_NDEF_T1T_State)
    {
    case Initial_2:
        /* Send T1T_RID */
        memcpy (pCmd, T1T_RID, sizeof(T1T_RID));
        *pCmd_size = 7;
        eRW_NDEF_T1T_State = Getting_ID;
        break;

    case Getting_ID:
        /* Is CC Read and Is Ndef ?*/
        if ((Rsp_size == 7) && (pRsp[Rsp_size-1] == 0x00))
        {
            /* Fill File structure */
            RW_NDEF_T1T_Ndef.HR0 = pRsp[0];
            RW_NDEF_T1T_Ndef.HR1 = pRsp[1];
            memcpy (RW_NDEF_T1T_Ndef.UID, &pRsp[2], sizeof(RW_NDEF_T1T_Ndef.UID));
            
            /* Read full card content */
            memcpy (pCmd, T1T_RALL, sizeof(T1T_RALL));
            memcpy (&pCmd[3], RW_NDEF_T1T_Ndef.UID, sizeof(RW_NDEF_T1T_Ndef.UID));
            *pCmd_size = sizeof(T1T_RALL) + sizeof(RW_NDEF_T1T_Ndef.UID);
            eRW_NDEF_T1T_State = Reading_CardContent;
        }
        break;

    case Reading_CardContent:
        /* Is Read XBH_TRUE ?*/
        if ((Rsp_size == 123) && (pRsp[Rsp_size-1] == 0x00))
        {
            /* Check CC */ 
            if (pRsp[10] == T1T_MAGIC_NUMBER)
            {
                unsigned char Tmp = 14;
                unsigned char data_size;

                /* If not NDEF Type skip TLV */
                while (pRsp[Tmp] != T1T_NDEF_TLV)
                {
                    Tmp += 2 + pRsp[Tmp+1];
                    if (Tmp > Rsp_size) return;
                }

                RW_NDEF_T1T_Ndef.MessageSize = pRsp[Tmp+1];
                data_size = (Rsp_size - 1) - 16 - Tmp - 2;

                /* If provisioned buffer is not large enough, notify the application and stop reading */
                if (RW_NDEF_T1T_Ndef.MessageSize > RW_MAX_NDEF_FILE_SIZE)
                {
                    if(pRW_NDEF_PullCb != XBH_NULL) pRW_NDEF_PullCb(XBH_NULL, 0, RW_NDEF_T1T_Ndef.MessageSize);
                    break;
                }

                /* Is NDEF read already completed ? */
                if(RW_NDEF_T1T_Ndef.MessageSize <= data_size)
                {
                    memcpy(RW_NDEF_T1T_Ndef.pMessage, &pRsp[Tmp+2], RW_NDEF_T1T_Ndef.MessageSize);

                    /* Notify application of the NDEF reception */
                    if(pRW_NDEF_PullCb != XBH_NULL) pRW_NDEF_PullCb(RW_NDEF_T1T_Ndef.pMessage, RW_NDEF_T1T_Ndef.MessageSize, RW_NDEF_T1T_Ndef.MessageSize);
                }
                else
                {
                    RW_NDEF_T1T_Ndef.MessagePtr = data_size;
                    memcpy (RW_NDEF_T1T_Ndef.pMessage, &pRsp[Tmp+2], RW_NDEF_T1T_Ndef.MessagePtr);
                    RW_NDEF_T1T_Ndef.BlkNb = 0x10;

                    /* Read NDEF content */
                    memcpy (pCmd, T1T_READ8, sizeof(T1T_READ8));
                    pCmd[1] = RW_NDEF_T1T_Ndef.BlkNb;
                    memcpy (&pCmd[10], RW_NDEF_T1T_Ndef.UID, sizeof(RW_NDEF_T1T_Ndef.UID));
                    *pCmd_size = sizeof(T1T_READ8) + sizeof(RW_NDEF_T1T_Ndef.UID);

                    eRW_NDEF_T1T_State = Reading_NDEF;
                }
            }
        }
        break;

        case Reading_NDEF:
            /* Is Read XBH_TRUE ?*/
            if ((Rsp_size == 10) && (pRsp[Rsp_size-1] == 0x00))
            {
                /* Is NDEF read already completed ? */
                if ((RW_NDEF_T1T_Ndef.MessageSize - RW_NDEF_T1T_Ndef.MessagePtr) < 8)
                {
                    memcpy (&RW_NDEF_T1T_Ndef.pMessage[RW_NDEF_T1T_Ndef.MessagePtr], &pRsp[1], RW_NDEF_T1T_Ndef.MessageSize - RW_NDEF_T1T_Ndef.MessagePtr);

                    /* Notify application of the NDEF reception */
                    if(pRW_NDEF_PullCb != XBH_NULL) pRW_NDEF_PullCb(RW_NDEF_T1T_Ndef.pMessage, RW_NDEF_T1T_Ndef.MessageSize, RW_NDEF_T1T_Ndef.MessageSize);
                }
                else
                {
                    memcpy (&RW_NDEF_T1T_Ndef.pMessage[RW_NDEF_T1T_Ndef.MessagePtr], &pRsp[1], 8);
                    RW_NDEF_T1T_Ndef.MessagePtr += 8;
                    RW_NDEF_T1T_Ndef.BlkNb++;

                    /* Read NDEF content */
                    memcpy (pCmd, T1T_READ8, sizeof(T1T_READ8));
                    pCmd[1] = RW_NDEF_T1T_Ndef.BlkNb;
                    memcpy (&pCmd[10], RW_NDEF_T1T_Ndef.UID, sizeof(RW_NDEF_T1T_Ndef.UID));
                    *pCmd_size = sizeof(T1T_READ8) + sizeof(RW_NDEF_T1T_Ndef.UID);
                }
            }
        break;

    default:
        break;
    }
}

#endif
#endif
/*
*         Copyright (c), NXP Semiconductors Caen / France
*
*                     (C)NXP Semiconductors
*       All rights are reserved. Reproduction in whole or in part is
*      prohibited without the written consent of the copyright owner.
*  NXP reserves the right to make changes without notice at any time.
* NXP makes no warranty, expressed, implied or statutory, including but
* not limited to any implied warranty of merchantability or fitness for any
*particular purpose, or that the use will not infringe any third party patent,
* copyright or trademark. NXP must not be liable for any loss or damage
*                          arising from its use.
*/

#ifndef REMOVE_RW_SUPPORT
#ifndef REMOVE_NDEF_SUPPORT



/* TODO: No support for tag larger than 1024 bytes (requiring SECTOR_SELECT command use) */

#define T2T_MAGIC_NUMBER    0xE1
#define T2T_NDEF_TLV        0x03

typedef enum
{
    Initial_3,
    Reading_CC_3,
    Reading_Data_3,
    Reading_NDEF_3,
    Writing_Data_3
} RW_NDEF_T2T_state_t;

typedef struct
{
    unsigned char BlkNb;
    unsigned short MessagePtr;
    unsigned short MessageSize;
    unsigned char *pMessage;
} RW_NDEF_T2T_Ndef_t;

static RW_NDEF_T2T_state_t eRW_NDEF_T2T_State = Initial_3;
static RW_NDEF_T2T_Ndef_t RW_NDEF_T2T_Ndef;

void RW_NDEF_T2T_Reset(void)
{
    eRW_NDEF_T2T_State = Initial_3;
    RW_NDEF_T2T_Ndef.pMessage = NdefBuffer;
}

void RW_NDEF_T2T_Read_Next(unsigned char *pRsp, unsigned short Rsp_size, unsigned char *pCmd, unsigned short *pCmd_size)
{
    /* By default no further command to be sent */
    *pCmd_size = 0;

    switch(eRW_NDEF_T2T_State)
    {
    case Initial_3:
        /* Read CC */
        pCmd[0] = 0x30;
        pCmd[1] = 0x03;
        *pCmd_size = 2;
        eRW_NDEF_T2T_State = Reading_CC_3;
        break;

    case Reading_CC_3:
        /* Is CC Read and Is Ndef ?*/
        if ((Rsp_size == 17) && (pRsp[Rsp_size-1] == 0x00) && (pRsp[0] == T2T_MAGIC_NUMBER))
        {
            /* Read First data */
            pCmd[0] = 0x30;
            pCmd[1] = 0x04;
            *pCmd_size = 2;

            eRW_NDEF_T2T_State = Reading_Data_3;
        }
        break;

    case Reading_Data_3:
        /* Is Read XBH_TRUE ?*/
        if ((Rsp_size == 17) && (pRsp[Rsp_size-1] == 0x00))
        {
            unsigned char Tmp = 0;
            /* If not NDEF Type skip TLV */
            while (pRsp[Tmp] != T2T_NDEF_TLV)
            {
                Tmp += 2 + pRsp[Tmp+1];
                if (Tmp > Rsp_size) return;
            }

            if(pRsp[Tmp+1] == 0xFF)
            {
                RW_NDEF_T2T_Ndef.MessageSize = (pRsp[Tmp+2] << 8) + pRsp[Tmp+3];
                Tmp += 2;
            }
            else RW_NDEF_T2T_Ndef.MessageSize = pRsp[Tmp+1];

            /* If provisioned buffer is not large enough or message is empty, notify the application and stop reading */
            if ((RW_NDEF_T2T_Ndef.MessageSize > RW_MAX_NDEF_FILE_SIZE) || (RW_NDEF_T2T_Ndef.MessageSize == 0))
            {
                if(pRW_NDEF_PullCb != XBH_NULL) pRW_NDEF_PullCb(XBH_NULL, 0, RW_NDEF_T2T_Ndef.MessageSize);
                break;
            }

            /* Is NDEF read already completed ? */
            if (RW_NDEF_T2T_Ndef.MessageSize <= ((Rsp_size-1) - Tmp - 2))
            {
                memcpy (RW_NDEF_T2T_Ndef.pMessage, &pRsp[Tmp+2], RW_NDEF_T2T_Ndef.MessageSize);

                /* Notify application of the NDEF reception */
                if(pRW_NDEF_PullCb != XBH_NULL) pRW_NDEF_PullCb(RW_NDEF_T2T_Ndef.pMessage, RW_NDEF_T2T_Ndef.MessageSize, RW_NDEF_T2T_Ndef.MessageSize);
            }
            else
            {
                RW_NDEF_T2T_Ndef.MessagePtr = (Rsp_size-1) - Tmp - 2;
                memcpy (RW_NDEF_T2T_Ndef.pMessage, &pRsp[Tmp+2], RW_NDEF_T2T_Ndef.MessagePtr);
                RW_NDEF_T2T_Ndef.BlkNb = 8;

                /* Read NDEF content */
                pCmd[0] = 0x30;
                pCmd[1] = RW_NDEF_T2T_Ndef.BlkNb;
                *pCmd_size = 2;
                eRW_NDEF_T2T_State = Reading_NDEF_3;
            }
        }
        break;

    case Reading_NDEF_3:
        /* Is Read XBH_TRUE ?*/
        if ((Rsp_size == 17) && (pRsp[Rsp_size-1] == 0x00))
        {
            /* Is NDEF read already completed ? */
            if ((RW_NDEF_T2T_Ndef.MessageSize - RW_NDEF_T2T_Ndef.MessagePtr) < 16)
            {
                memcpy (&RW_NDEF_T2T_Ndef.pMessage[RW_NDEF_T2T_Ndef.MessagePtr], pRsp, RW_NDEF_T2T_Ndef.MessageSize - RW_NDEF_T2T_Ndef.MessagePtr);

                /* Notify application of the NDEF reception */
                if(pRW_NDEF_PullCb != XBH_NULL) pRW_NDEF_PullCb(RW_NDEF_T2T_Ndef.pMessage, RW_NDEF_T2T_Ndef.MessageSize, RW_NDEF_T2T_Ndef.MessageSize);
            }
            else
            {
                memcpy (&RW_NDEF_T2T_Ndef.pMessage[RW_NDEF_T2T_Ndef.MessagePtr], pRsp, 16);
                RW_NDEF_T2T_Ndef.MessagePtr += 16;
                RW_NDEF_T2T_Ndef.BlkNb += 4;

                /* Read NDEF content */
                pCmd[0] = 0x30;
                pCmd[1] = RW_NDEF_T2T_Ndef.BlkNb;
                *pCmd_size = 2;
            }
        }
        break;

    default:
        break;
    }
}

void RW_NDEF_T2T_Write_Next(unsigned char *pRsp, unsigned short Rsp_size, unsigned char *pCmd, unsigned short *pCmd_size)
{
    /* By default no further command to be sent */
    *pCmd_size = 0;

    switch(eRW_NDEF_T2T_State)
    {
    case Initial_3:
        /* Read CC */
        pCmd[0] = 0x30;
        pCmd[1] = 0x03;
        *pCmd_size = 2;
        eRW_NDEF_T2T_State = Reading_CC_3;
        break;

    case Reading_CC_3:
        /* Is CC Read, Is Ndef and is R/W ?*/
        if ((Rsp_size == 17) && (pRsp[Rsp_size-1] == 0x00) && (pRsp[0] == T2T_MAGIC_NUMBER) && (pRsp[3] == 0x00))
        {
            /* Is size enough ? */
            if (pRsp[2]*8 >= RW_NdefMessage_size)
            {
                /* Write First data */
                pCmd[0] = 0xA2;
                pCmd[1] = 0x04;
                pCmd[2] = 0x03;
                if (RW_NdefMessage_size > 0xFF)
                {
                    pCmd[3] = 0xFF;
                    pCmd[4] = (RW_NdefMessage_size & 0xFF00) >> 8;
                    pCmd[5] = RW_NdefMessage_size & 0xFF;
                    RW_NDEF_T2T_Ndef.MessagePtr = 0;
                }
                else
                {
                    pCmd[3] = (unsigned char) RW_NdefMessage_size;
                    memcpy(&pCmd[4], pRW_NdefMessage, 2);
                    RW_NDEF_T2T_Ndef.MessagePtr = 2;
                }
                RW_NDEF_T2T_Ndef.BlkNb = 5;
                *pCmd_size = 6;
                eRW_NDEF_T2T_State = Writing_Data_3;
            }
        }
        break;

    case Writing_Data_3:
        /* Is Write XBH_TRUE ?*/
        if ((Rsp_size == 2) && (pRsp[Rsp_size-1] == 0x14))
        {
            /* Is NDEF write already completed ? */
            if (RW_NdefMessage_size <= RW_NDEF_T2T_Ndef.MessagePtr)
            {
                /* Notify application of the NDEF send completion */
                if(pRW_NDEF_PushCb != XBH_NULL) pRW_NDEF_PushCb(pRW_NdefMessage, RW_NdefMessage_size);
            }
            else
            {
                /* Write NDEF content */
                pCmd[0] = 0xA2;
                pCmd[1] = RW_NDEF_T2T_Ndef.BlkNb;
                memcpy(&pCmd[2], pRW_NdefMessage+RW_NDEF_T2T_Ndef.MessagePtr, 4);
                *pCmd_size = 6;

                RW_NDEF_T2T_Ndef.MessagePtr+=4;
                RW_NDEF_T2T_Ndef.BlkNb++;
            }
        }
        break;

    default:
        break;
    }
}

#endif
#endif


/*
*         Copyright (c), NXP Semiconductors Caen / France
*
*                     (C)NXP Semiconductors
*       All rights are reserved. Reproduction in whole or in part is
*      prohibited without the written consent of the copyright owner.
*  NXP reserves the right to make changes without notice at any time.
* NXP makes no warranty, expressed, implied or statutory, including but
* not limited to any implied warranty of merchantability or fitness for any
*particular purpose, or that the use will not infringe any third party patent,
* copyright or trademark. NXP must not be liable for any loss or damage
*                          arising from its use.
*/

#ifndef REMOVE_RW_SUPPORT
#ifndef REMOVE_NDEF_SUPPORT


#define T3T_MAGIC_NUMBER    0xE1
#define T3T_NDEF_TLV        0x03

unsigned char T3T_Check[] = {0x10,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x0B,0x00,0x1,0x80,0x00};

typedef enum
{
    Initial_4,
    Getting_AttributeInfo_4,
    Reading_CardContent_4
} RW_NDEF_T3T_state_t;

typedef struct
{
    unsigned char IDm[8];
    unsigned char BlkNb;
    unsigned short Ptr;
    unsigned short Size;
    unsigned char *p;
} RW_NDEF_T3T_Ndef_t;

static RW_NDEF_T3T_state_t eRW_NDEF_T3T_State = Initial_4;
static RW_NDEF_T3T_Ndef_t RW_NDEF_T3T_Ndef;

void RW_NDEF_T3T_Reset(void)
{
    eRW_NDEF_T3T_State = Initial_4;
    RW_NDEF_T3T_Ndef.p = NdefBuffer;
}

void RW_NDEF_T3T_SetIDm(unsigned char *pIDm)
{
    memcpy(RW_NDEF_T3T_Ndef.IDm, pIDm, sizeof(RW_NDEF_T3T_Ndef.IDm));
    memcpy(&T3T_Check[2], pIDm, sizeof(RW_NDEF_T3T_Ndef.IDm));
}

void RW_NDEF_T3T_Read_Next(unsigned char *pRsp, unsigned short Rsp_size, unsigned char *pCmd, unsigned short *pCmd_size)
{
    /* By default no further command to be sent */
    *pCmd_size = 0;

    switch(eRW_NDEF_T3T_State)
    {
    case Initial_4:
        /* Get AttributeInfo */
        memcpy (pCmd, T3T_Check, sizeof(T3T_Check));
        *pCmd_size = sizeof(T3T_Check);
        eRW_NDEF_T3T_State = Getting_AttributeInfo_4;
        break;

    case Getting_AttributeInfo_4:
        /* Is Check XBH_TRUE ?*/
        if ((pRsp[Rsp_size-1] == 0x00) && (pRsp[1] == 0x07) && (pRsp[10] == 0x00) && (pRsp[11] == 0x00))
        {
            /* Fill File structure */
            RW_NDEF_T3T_Ndef.Size = (pRsp[24] << 16) + (pRsp[25] << 8) + pRsp[26];

            /* If provisioned buffer is not large enough or size is XBH_NULL, notify the application and stop reading */
            if ((RW_NDEF_T3T_Ndef.Size > RW_MAX_NDEF_FILE_SIZE) || (RW_NDEF_T3T_Ndef.Size == 0))
            {
                if(pRW_NDEF_PullCb != XBH_NULL) pRW_NDEF_PullCb(XBH_NULL, 0, RW_NDEF_T3T_Ndef.Size);
                break;
            }

            RW_NDEF_T3T_Ndef.Ptr = 0;
            RW_NDEF_T3T_Ndef.BlkNb = 1;

            /* Read first NDEF block */
            memcpy (pCmd, T3T_Check, sizeof(T3T_Check));
            pCmd[15] = 0x01;
            *pCmd_size = sizeof(T3T_Check);
            eRW_NDEF_T3T_State = Reading_CardContent_4;
        }
        break;

    case Reading_CardContent_4:
        /* Is Check XBH_TRUE ?*/
        if ((pRsp[Rsp_size-1] == 0x00) && (pRsp[1] == 0x07) && (pRsp[10] == 0x00) && (pRsp[11] == 0x00))
        {
            /* Is NDEF message read completed ?*/
            if ((RW_NDEF_T3T_Ndef.Size - RW_NDEF_T3T_Ndef.Ptr) <= 16)
            {
                memcpy(&RW_NDEF_T3T_Ndef.p[RW_NDEF_T3T_Ndef.Ptr], &pRsp[13], (RW_NDEF_T3T_Ndef.Size - RW_NDEF_T3T_Ndef.Ptr));
                /* Notify application of the NDEF reception */
                if(pRW_NDEF_PullCb != XBH_NULL) pRW_NDEF_PullCb(RW_NDEF_T3T_Ndef.p, RW_NDEF_T3T_Ndef.Size, RW_NDEF_T3T_Ndef.Size);
            }
            else
            {
                memcpy(&RW_NDEF_T3T_Ndef.p[RW_NDEF_T3T_Ndef.Ptr], &pRsp[13], 16);
                RW_NDEF_T3T_Ndef.Ptr += 16;
                RW_NDEF_T3T_Ndef.BlkNb++;

                /* Read next NDEF block */
                memcpy (pCmd, T3T_Check, sizeof(T3T_Check));
                pCmd[15] = RW_NDEF_T3T_Ndef.BlkNb;
                *pCmd_size = sizeof(T3T_Check);
            }
        }
        break;

    default:
        break;
    }
}

#endif
#endif
/*
*         Copyright (c), NXP Semiconductors Caen / France
*
*                     (C)NXP Semiconductors
*       All rights are reserved. Reproduction in whole or in part is
*      prohibited without the written consent of the copyright owner.
*  NXP reserves the right to make changes without notice at any time.
* NXP makes no warranty, expressed, implied or statutory, including but
* not limited to any implied warranty of merchantability or fitness for any
*particular purpose, or that the use will not infringe any third party patent,
* copyright or trademark. NXP must not be liable for any loss or damage
*                          arising from its use.
*/

#ifndef REMOVE_RW_SUPPORT
#ifndef REMOVE_NDEF_SUPPORT



const unsigned char RW_NDEF_T4T_APP_Select20[] = {0x00,0xA4,0x04,0x00,0x07,0xD2,0x76,0x00,0x00,0x85,0x01,0x01,0x00};
const unsigned char RW_NDEF_T4T_APP_Select10[] = {0x00,0xA4,0x04,0x00,0x07,0xD2,0x76,0x00,0x00,0x85,0x01,0x00};
const unsigned char RW_NDEF_T4T_CC_Select[] = {0x00,0xA4,0x00,0x0C,0x02,0xE1,0x03};
const unsigned char RW_NDEF_T4T_NDEF_Select[] = {0x00,0xA4,0x00,0x0C,0x02,0xE1,0x04};
const unsigned char RW_NDEF_T4T_Read[] = {0x00,0xB0,0x00,0x00,0x0F};
const unsigned char RW_NDEF_T4T_Write[] = {0x00,0xD6,0x00,0x00,0x00};

const unsigned char RW_NDEF_T4T_OK[] = {0x90, 0x00};

#define WRITE_SZ    54

typedef enum
{
    Initial_5,
    Selecting_NDEF_Application20_5,
    Selecting_NDEF_Application10_5,
    Selecting_CC_5,
    Reading_CC_5,
    Selecting_NDEF_5,
    Reading_NDEF_Size_5,
    Reading_NDEF_5,
    Writing_NDEF_5,
    Writing_NDEFsize_5,
    Write_NDEFcomplete_5
} RW_NDEF_T4T_state_t;

typedef struct
{
    unsigned char MappingVersion;
    unsigned short MLe;
    unsigned short MLc;
    unsigned char FileID[2];
    unsigned short MaxNdefFileSize;
    unsigned char RdAccess;
    unsigned char WrAccess;
    unsigned short MessagePtr;
    unsigned short MessageSize;
    unsigned char *pMessage;
} RW_NDEF_T4T_Ndef_t;

static RW_NDEF_T4T_state_t eRW_NDEF_T4T_State = Initial_5;
static RW_NDEF_T4T_Ndef_t RW_NDEF_T4T_Ndef;

void RW_NDEF_T4T_Reset(void)
{
    eRW_NDEF_T4T_State = Initial_5;
    RW_NDEF_T4T_Ndef.pMessage = NdefBuffer;
}

void RW_NDEF_T4T_Read_Next(unsigned char *pRsp, unsigned short Rsp_size, unsigned char *pCmd, unsigned short *pCmd_size)
{
    /* By default no further command to be sent */
    *pCmd_size = 0;

    switch(eRW_NDEF_T4T_State)
    {
    case Initial_5:
        /* Select NDEF Application in version 2.0 */
        memcpy(pCmd, RW_NDEF_T4T_APP_Select20, sizeof(RW_NDEF_T4T_APP_Select20));
        *pCmd_size = sizeof(RW_NDEF_T4T_APP_Select20);
        eRW_NDEF_T4T_State = Selecting_NDEF_Application20_5;
        break;

    case Selecting_NDEF_Application20_5:
        /* Is NDEF Application Selected ?*/
        if (!memcmp(&pRsp[Rsp_size - 2], RW_NDEF_T4T_OK, sizeof(RW_NDEF_T4T_OK)))
        {
            /* Select CC */
            memcpy(pCmd, RW_NDEF_T4T_CC_Select, sizeof(RW_NDEF_T4T_CC_Select));
            *pCmd_size = sizeof(RW_NDEF_T4T_CC_Select);
            eRW_NDEF_T4T_State = Selecting_CC_5;
        }
        else
        {
            /* Select NDEF Application in version 1.0 */
            memcpy(pCmd, RW_NDEF_T4T_APP_Select10, sizeof(RW_NDEF_T4T_APP_Select10));
            *pCmd_size = sizeof(RW_NDEF_T4T_APP_Select10);
            eRW_NDEF_T4T_State = Selecting_NDEF_Application10_5;
        }
        break;

    case Selecting_NDEF_Application10_5:
        /* Is NDEF Application Selected ?*/
        if (!memcmp(&pRsp[Rsp_size - 2], RW_NDEF_T4T_OK, sizeof(RW_NDEF_T4T_OK)))
        {
            /* Select CC */
            memcpy(pCmd, RW_NDEF_T4T_CC_Select, sizeof(RW_NDEF_T4T_CC_Select));
            pCmd[3] = 0x00;
            *pCmd_size = sizeof(RW_NDEF_T4T_CC_Select);
            eRW_NDEF_T4T_State = Selecting_CC_5;
        }
        break;

    case Selecting_CC_5:
        /* Is CC Selected ?*/
        if (!memcmp(&pRsp[Rsp_size - 2], RW_NDEF_T4T_OK, sizeof(RW_NDEF_T4T_OK)))
        {
            /* Read CC */
            memcpy(pCmd, RW_NDEF_T4T_Read, sizeof(RW_NDEF_T4T_Read));
            *pCmd_size = sizeof(RW_NDEF_T4T_Read);
            eRW_NDEF_T4T_State = Reading_CC_5;
        }
        break;

    case Reading_CC_5:
        /* Is CC Read ?*/
        if ((!memcmp(&pRsp[Rsp_size - 2], RW_NDEF_T4T_OK, sizeof(RW_NDEF_T4T_OK))) && (Rsp_size == 15 + 2))
        {
            /* Fill CC structure */
            RW_NDEF_T4T_Ndef.MappingVersion = pRsp[2];
            RW_NDEF_T4T_Ndef.MLe = (pRsp[3] << 8) + pRsp[4];
            RW_NDEF_T4T_Ndef.MLc = (pRsp[5] << 8) + pRsp[6];
            RW_NDEF_T4T_Ndef.FileID[0] = pRsp[9];
            RW_NDEF_T4T_Ndef.FileID[1] = pRsp[10];
            RW_NDEF_T4T_Ndef.MaxNdefFileSize = (pRsp[11] << 8) + pRsp[12];
            RW_NDEF_T4T_Ndef.RdAccess = pRsp[13];
            RW_NDEF_T4T_Ndef.WrAccess = pRsp[14];
            
            /* Select NDEF */
            memcpy(pCmd, RW_NDEF_T4T_NDEF_Select, sizeof(RW_NDEF_T4T_NDEF_Select));
            if (RW_NDEF_T4T_Ndef.MappingVersion == 0x10) pCmd[3] = 0x00;
            pCmd[5] = RW_NDEF_T4T_Ndef.FileID[0];
            pCmd[6] = RW_NDEF_T4T_Ndef.FileID[1];
            *pCmd_size = sizeof(RW_NDEF_T4T_NDEF_Select);
            eRW_NDEF_T4T_State = Selecting_NDEF_5;
        }
        break;

    case Selecting_NDEF_5:
        /* Is NDEF Selected ?*/
        if (!memcmp(&pRsp[Rsp_size - 2], RW_NDEF_T4T_OK, sizeof(RW_NDEF_T4T_OK)))
        {
            /* Get NDEF file size */
            memcpy(pCmd, RW_NDEF_T4T_Read, sizeof(RW_NDEF_T4T_Read));
            *pCmd_size = sizeof(RW_NDEF_T4T_Read);
            pCmd[4] = 2;
            eRW_NDEF_T4T_State = Reading_NDEF_Size_5;
        }
        break;

    case Reading_NDEF_Size_5:
        /* Is Read XBH_TRUE ?*/
        if (!memcmp(&pRsp[Rsp_size - 2], RW_NDEF_T4T_OK, sizeof(RW_NDEF_T4T_OK)))
        {
            RW_NDEF_T4T_Ndef.MessageSize = (pRsp[0] << 8) + pRsp[1];

            /* If provisioned buffer is not large enough, notify the application and stop reading */
            if (RW_NDEF_T4T_Ndef.MessageSize > RW_MAX_NDEF_FILE_SIZE)
            {
                if(pRW_NDEF_PullCb != XBH_NULL) pRW_NDEF_PullCb(XBH_NULL, 0, RW_NDEF_T4T_Ndef.MessageSize);
                break;
            }

            RW_NDEF_T4T_Ndef.MessagePtr = 0;

            /* Read NDEF data */
            memcpy(pCmd, RW_NDEF_T4T_Read, sizeof(RW_NDEF_T4T_Read));
            pCmd[3] =  2;
            pCmd[4] = (RW_NDEF_T4T_Ndef.MessageSize > RW_NDEF_T4T_Ndef.MLe-1) ? RW_NDEF_T4T_Ndef.MLe-1 : (unsigned char) RW_NDEF_T4T_Ndef.MessageSize;
            *pCmd_size = sizeof(RW_NDEF_T4T_Read);
            eRW_NDEF_T4T_State = Reading_NDEF_5;
        }
        break;

    case Reading_NDEF_5:
        /* Is Read XBH_TRUE ?*/
        if (!memcmp(&pRsp[Rsp_size - 2], RW_NDEF_T4T_OK, sizeof(RW_NDEF_T4T_OK)))
        {
            memcpy(&RW_NDEF_T4T_Ndef.pMessage[RW_NDEF_T4T_Ndef.MessagePtr], pRsp, Rsp_size - 2);
            RW_NDEF_T4T_Ndef.MessagePtr += Rsp_size - 2;
            
            /* Is NDEF message read completed ?*/
            if (RW_NDEF_T4T_Ndef.MessagePtr == RW_NDEF_T4T_Ndef.MessageSize)
            {
                /* Notify application of the NDEF reception */
                if(pRW_NDEF_PullCb != XBH_NULL) pRW_NDEF_PullCb(RW_NDEF_T4T_Ndef.pMessage, RW_NDEF_T4T_Ndef.MessageSize, RW_NDEF_T4T_Ndef.MessageSize);
            }
            else
            {
                /* Read NDEF data */
                memcpy(pCmd, RW_NDEF_T4T_Read, sizeof(RW_NDEF_T4T_Read));
                pCmd[3] =  RW_NDEF_T4T_Ndef.MessagePtr + 2;
                pCmd[4] = ((RW_NDEF_T4T_Ndef.MessageSize - RW_NDEF_T4T_Ndef.MessagePtr) > RW_NDEF_T4T_Ndef.MLe-1) ? RW_NDEF_T4T_Ndef.MLe-1 : (unsigned char) (RW_NDEF_T4T_Ndef.MessageSize - RW_NDEF_T4T_Ndef.MessagePtr);
                *pCmd_size = sizeof(RW_NDEF_T4T_Read);
            }
        }
        break;

    default:
        break;
    }
}

void RW_NDEF_T4T_Write_Next(unsigned char *pRsp, unsigned short Rsp_size, unsigned char *pCmd, unsigned short *pCmd_size)
{
    /* By default no further command to be sent */
    *pCmd_size = 0;

    switch(eRW_NDEF_T4T_State)
    {
    case Initial_5:
        /* Select NDEF Application in version 2.0 */
        memcpy(pCmd, RW_NDEF_T4T_APP_Select20, sizeof(RW_NDEF_T4T_APP_Select20));
        *pCmd_size = sizeof(RW_NDEF_T4T_APP_Select20);
        eRW_NDEF_T4T_State = Selecting_NDEF_Application20_5;
        break;

    case Selecting_NDEF_Application20_5:
        /* Is NDEF Application Selected ?*/
        if (!memcmp(&pRsp[Rsp_size - 2], RW_NDEF_T4T_OK, sizeof(RW_NDEF_T4T_OK)))
        {
            /* Select CC */
            memcpy(pCmd, RW_NDEF_T4T_CC_Select, sizeof(RW_NDEF_T4T_CC_Select));
            *pCmd_size = sizeof(RW_NDEF_T4T_CC_Select);
            eRW_NDEF_T4T_State = Selecting_CC_5;
        }
        else
        {
            /* Select NDEF Application in version 1.0 */
            memcpy(pCmd, RW_NDEF_T4T_APP_Select10, sizeof(RW_NDEF_T4T_APP_Select10));
            *pCmd_size = sizeof(RW_NDEF_T4T_APP_Select10);
            eRW_NDEF_T4T_State = Selecting_NDEF_Application10_5;
        }
        break;

    case Selecting_NDEF_Application10_5:
        /* Is NDEF Application Selected ?*/
        if (!memcmp(&pRsp[Rsp_size - 2], RW_NDEF_T4T_OK, sizeof(RW_NDEF_T4T_OK)))
        {
            /* Select CC */
            memcpy(pCmd, RW_NDEF_T4T_CC_Select, sizeof(RW_NDEF_T4T_CC_Select));
            pCmd[3] = 0x00;
            *pCmd_size = sizeof(RW_NDEF_T4T_CC_Select);
            eRW_NDEF_T4T_State = Selecting_CC_5;
        }
        break;

    case Selecting_CC_5:
        /* Is CC Selected ?*/
        if (!memcmp(&pRsp[Rsp_size - 2], RW_NDEF_T4T_OK, sizeof(RW_NDEF_T4T_OK)))
        {
            /* Read CC */
            memcpy(pCmd, RW_NDEF_T4T_Read, sizeof(RW_NDEF_T4T_Read));
            *pCmd_size = sizeof(RW_NDEF_T4T_Read);
            eRW_NDEF_T4T_State = Reading_CC_5;
        }
        break;

    case Reading_CC_5:
        /* Is CC Read ?*/
        if ((!memcmp(&pRsp[Rsp_size - 2], RW_NDEF_T4T_OK, sizeof(RW_NDEF_T4T_OK))) && (Rsp_size == 15 + 2))
        {
            /* Fill CC structure */
            RW_NDEF_T4T_Ndef.MappingVersion = pRsp[2];
            RW_NDEF_T4T_Ndef.MLe = (pRsp[3] << 8) + pRsp[4];
            RW_NDEF_T4T_Ndef.MLc = (pRsp[5] << 8) + pRsp[6];
            RW_NDEF_T4T_Ndef.FileID[0] = pRsp[9];
            RW_NDEF_T4T_Ndef.FileID[1] = pRsp[10];
            RW_NDEF_T4T_Ndef.MaxNdefFileSize = (pRsp[11] << 8) + pRsp[12];
            RW_NDEF_T4T_Ndef.RdAccess = pRsp[13];
            RW_NDEF_T4T_Ndef.WrAccess = pRsp[14];

            /* Select NDEF */
            memcpy(pCmd, RW_NDEF_T4T_NDEF_Select, sizeof(RW_NDEF_T4T_NDEF_Select));
            if (RW_NDEF_T4T_Ndef.MappingVersion == 0x10) pCmd[3] = 0x00;
            pCmd[5] = RW_NDEF_T4T_Ndef.FileID[0];
            pCmd[6] = RW_NDEF_T4T_Ndef.FileID[1];
            *pCmd_size = sizeof(RW_NDEF_T4T_NDEF_Select);
            eRW_NDEF_T4T_State = Selecting_NDEF_5;
        }
        break;

    case Selecting_NDEF_5:
        /* Is NDEF Selected ?*/
        if (!memcmp(&pRsp[Rsp_size - 2], RW_NDEF_T4T_OK, sizeof(RW_NDEF_T4T_OK)))
        {
            /* Clearing NDEF message size*/
            memcpy(pCmd, RW_NDEF_T4T_Write, sizeof(RW_NDEF_T4T_Write));
            pCmd[4] = 2;
            pCmd[5] = 0;
            pCmd[6] = 0;
            *pCmd_size = sizeof(RW_NDEF_T4T_Write) + 2;
            RW_NDEF_T4T_Ndef.MessagePtr = 0;
            eRW_NDEF_T4T_State = Writing_NDEF_5;
        }
        break;

    case Writing_NDEF_5:
        /* Is Write XBH_TRUE ?*/
        if (!memcmp(&pRsp[Rsp_size - 2], RW_NDEF_T4T_OK, sizeof(RW_NDEF_T4T_OK)))
        {
            /* Writing NDEF message */
            memcpy(pCmd, RW_NDEF_T4T_Write, sizeof(RW_NDEF_T4T_Write));
            pCmd[2] = (RW_NDEF_T4T_Ndef.MessagePtr + 2) >> 8;
            pCmd[3] = (RW_NDEF_T4T_Ndef.MessagePtr + 2) & 0xFF;
            if((RW_NdefMessage_size - RW_NDEF_T4T_Ndef.MessagePtr) < WRITE_SZ)
            {
                pCmd[4] = (RW_NdefMessage_size - RW_NDEF_T4T_Ndef.MessagePtr);
                memcpy(&pCmd[5], pRW_NdefMessage + RW_NDEF_T4T_Ndef.MessagePtr, (RW_NdefMessage_size - RW_NDEF_T4T_Ndef.MessagePtr));
                *pCmd_size = sizeof(RW_NDEF_T4T_Write) + (RW_NdefMessage_size - RW_NDEF_T4T_Ndef.MessagePtr);
                eRW_NDEF_T4T_State = Writing_NDEFsize_5;
            }
            else
            {
                pCmd[4] = WRITE_SZ;
                memcpy(&pCmd[5], pRW_NdefMessage + RW_NDEF_T4T_Ndef.MessagePtr, WRITE_SZ);
                *pCmd_size = sizeof(RW_NDEF_T4T_Write) + WRITE_SZ;
                RW_NDEF_T4T_Ndef.MessagePtr += WRITE_SZ;
                eRW_NDEF_T4T_State = Writing_NDEF_5;
            }
        }
        break;

    case Writing_NDEFsize_5:
        /* Is Write XBH_TRUE ?*/
        if (!memcmp(&pRsp[Rsp_size - 2], RW_NDEF_T4T_OK, sizeof(RW_NDEF_T4T_OK)))
        {
            memcpy(pCmd, RW_NDEF_T4T_Write, sizeof(RW_NDEF_T4T_Write));
            pCmd[4] = 2;
            pCmd[5] = RW_NdefMessage_size >> 8;
            pCmd[6] = RW_NdefMessage_size & 0xFF;
            *pCmd_size = sizeof(RW_NDEF_T4T_Write) + 2;
            eRW_NDEF_T4T_State = Write_NDEFcomplete_5;
        }
        break;

    case Write_NDEFcomplete_5:
        /* Is Write XBH_TRUE ?*/
        if (!memcmp(&pRsp[Rsp_size - 2], RW_NDEF_T4T_OK, sizeof(RW_NDEF_T4T_OK)))
        {
            /* Notify application of the NDEF reception */
            if(pRW_NDEF_PushCb != XBH_NULL) pRW_NDEF_PushCb(pRW_NdefMessage, RW_NdefMessage_size);
        }
        break;

    default:
        break;
    }
}

#endif
#endif
/*
*         Copyright (c), NXP Semiconductors Caen / France
*
*                     (C)NXP Semiconductors
*       All rights are reserved. Reproduction in whole or in part is
*      prohibited without the written consent of the copyright owner.
*  NXP reserves the right to make changes without notice at any time.
* NXP makes no warranty, expressed, implied or statutory, including but
* not limited to any implied warranty of merchantability or fitness for any
*particular purpose, or that the use will not infringe any third party patent,
* copyright or trademark. NXP must not be liable for any loss or damage
*                          arising from its use.
*/

#ifndef REMOVE_RW_SUPPORT
#ifndef REMOVE_NDEF_SUPPORT



#define T5T_ADD_MODE_1BYTE    0xE1
#define T5T_ADD_MODE_2BYTE    0xE2
#define T5T_NDEF_TLV		  0x03

typedef enum
{
    Initial_6,
    Reading_CC_6,
    Reading_Data_6,
    Reading_NDEF_6,
    Writing_Data_6
} RW_NDEF_T5T_state_t;

typedef struct
{
    unsigned char BlkNb;
    unsigned short MessagePtr;
    unsigned short MessageSize;
    unsigned char *pMessage;
    unsigned char AddrMode;
} RW_NDEF_T5T_Ndef_t;

static RW_NDEF_T5T_state_t eRW_NDEF_T5T_State = Initial_6;
static RW_NDEF_T5T_Ndef_t RW_NDEF_T5T_Ndef;

static unsigned short T5T_prepare_read_block(unsigned char blk, unsigned char *pCmd)
{
	unsigned short size;

	if(RW_NDEF_T5T_Ndef.AddrMode == T5T_ADD_MODE_1BYTE)
	{
		pCmd[0] = 0x02;
		pCmd[1] = 0x20;
		pCmd[2] = blk;
		size = 3;
	}
	else
	{
		pCmd[0] = 0x02;
		pCmd[1] = 0x30;
		pCmd[2] = 0x00;
		pCmd[3] = blk;
		size = 4;
	}
	return size;
}

static unsigned short T5T_prepare_write_block(unsigned char blk, unsigned char *pCmd, unsigned char *pData)
{
	unsigned short size;

	if(RW_NDEF_T5T_Ndef.AddrMode == T5T_ADD_MODE_1BYTE)
	{
		pCmd[0] = 0x02;
		pCmd[1] = 0x21;
		pCmd[2] = blk;
		memcpy(&pCmd[3], pData, 4);
		size = 7;
	}
	else
	{
		pCmd[0] = 0x02;
		pCmd[1] = 0x31;
		pCmd[2] = 0x00;
		pCmd[3] = blk;
		memcpy(&pCmd[4], pData, 4);
		size = 8;
	}
	return size;
}

void RW_NDEF_T5T_Reset(void)
{
    eRW_NDEF_T5T_State = Initial_6;
    RW_NDEF_T5T_Ndef.pMessage = NdefBuffer;
}

void RW_NDEF_T5T_Read_Next(unsigned char *pRsp, unsigned short Rsp_size, unsigned char *pCmd, unsigned short *pCmd_size)
{
    /* By default no further command to be sent */
    *pCmd_size = 0;

    switch(eRW_NDEF_T5T_State)
    {
    case Initial_6:
        /* Read CC */
		RW_NDEF_T5T_Ndef.AddrMode = T5T_ADD_MODE_1BYTE;
		*pCmd_size = T5T_prepare_read_block(0, pCmd);
        eRW_NDEF_T5T_State = Reading_CC_6;
        break;

    case Reading_CC_6:
        /* Is CC Read and Is Ndef ?*/
        if ((Rsp_size == 6) && (pRsp[Rsp_size-1] == 0x00) && ((pRsp[1] == T5T_ADD_MODE_1BYTE) || (pRsp[1] == T5T_ADD_MODE_2BYTE)))
        {
			RW_NDEF_T5T_Ndef.AddrMode = pRsp[1];
			/* Read First data */
			*pCmd_size = T5T_prepare_read_block(1, pCmd);
            eRW_NDEF_T5T_State = Reading_Data_6;
        }
        break;

    case Reading_Data_6:
        /* Is Read XBH_TRUE ?*/
        if ((Rsp_size == 6) && (pRsp[Rsp_size-1] == 0x00))
        {
            if (pRsp[1] == T5T_NDEF_TLV)
            {
				RW_NDEF_T5T_Ndef.MessageSize = pRsp[2];

				/* If provisioned buffer is not large enough or message is empty, notify the application and stop reading */
				if ((RW_NDEF_T5T_Ndef.MessageSize > RW_MAX_NDEF_FILE_SIZE) || (RW_NDEF_T5T_Ndef.MessageSize == 0))
				{
					if(pRW_NDEF_PullCb != XBH_NULL) pRW_NDEF_PullCb(XBH_NULL, 0, RW_NDEF_T5T_Ndef.MessageSize);
					break;
				}

				/* Is NDEF read already completed ? */
				if (RW_NDEF_T5T_Ndef.MessageSize <= 2)
				{
					memcpy (RW_NDEF_T5T_Ndef.pMessage, &pRsp[3], RW_NDEF_T5T_Ndef.MessageSize);

					/* Notify application of the NDEF reception */
					if(pRW_NDEF_PullCb != XBH_NULL) pRW_NDEF_PullCb(RW_NDEF_T5T_Ndef.pMessage, RW_NDEF_T5T_Ndef.MessageSize, RW_NDEF_T5T_Ndef.MessageSize);
				}
				else
				{
					RW_NDEF_T5T_Ndef.MessagePtr = 2;
					memcpy (RW_NDEF_T5T_Ndef.pMessage, &pRsp[3], RW_NDEF_T5T_Ndef.MessagePtr);
					RW_NDEF_T5T_Ndef.BlkNb = 2;

					/* Read NDEF content */
					*pCmd_size = T5T_prepare_read_block(RW_NDEF_T5T_Ndef.BlkNb, pCmd);
					eRW_NDEF_T5T_State = Reading_NDEF_6;
				}
			}
        }
        break;

    case Reading_NDEF_6:
        /* Is Read XBH_TRUE ?*/
        if ((Rsp_size == 6) && (pRsp[Rsp_size-1] == 0x00))
        {
            /* Is NDEF read already completed ? */
            if ((RW_NDEF_T5T_Ndef.MessageSize - RW_NDEF_T5T_Ndef.MessagePtr) <= 4)
            {
                memcpy (&RW_NDEF_T5T_Ndef.pMessage[RW_NDEF_T5T_Ndef.MessagePtr], &pRsp[1], RW_NDEF_T5T_Ndef.MessageSize - RW_NDEF_T5T_Ndef.MessagePtr);

                /* Notify application of the NDEF reception */
                if(pRW_NDEF_PullCb != XBH_NULL) pRW_NDEF_PullCb(RW_NDEF_T5T_Ndef.pMessage, RW_NDEF_T5T_Ndef.MessageSize, RW_NDEF_T5T_Ndef.MessageSize);
            }
            else
            {
                memcpy (&RW_NDEF_T5T_Ndef.pMessage[RW_NDEF_T5T_Ndef.MessagePtr], &pRsp[1], 4);
                RW_NDEF_T5T_Ndef.MessagePtr += 4;
                RW_NDEF_T5T_Ndef.BlkNb++;

                /* Read NDEF content */
				*pCmd_size = T5T_prepare_read_block(RW_NDEF_T5T_Ndef.BlkNb, pCmd);
            }
        }
        break;

    default:
        break;
    }
}

void RW_NDEF_T5T_Write_Next(unsigned char *pRsp, unsigned short Rsp_size, unsigned char *pCmd, unsigned short *pCmd_size)
{
    /* By default no further command to be sent */
    *pCmd_size = 0;

    switch(eRW_NDEF_T5T_State)
    {
    case Initial_6:
        /* Read CC */
		RW_NDEF_T5T_Ndef.AddrMode = T5T_ADD_MODE_1BYTE;
		*pCmd_size = T5T_prepare_read_block(0, pCmd);
        eRW_NDEF_T5T_State = Reading_CC_6;
        break;

    case Reading_CC_6:
        /* Is CC Read and Is Ndef */
        if ((Rsp_size == 6) && (pRsp[Rsp_size-1] == 0x00) && ((pRsp[1] == T5T_ADD_MODE_1BYTE) || (pRsp[1] == T5T_ADD_MODE_2BYTE)))
        {
			RW_NDEF_T5T_Ndef.AddrMode = pRsp[1];
            /* Is size enough ? */
            if ((pRsp[3]*8 >= RW_NdefMessage_size) && (0xFF >= RW_NdefMessage_size))
            {
				unsigned char data[4] = {0x03};
				data[1] = (XBH_U8) RW_NdefMessage_size;
                memcpy(&data[2], pRW_NdefMessage, 2);
				/* Write First data */
				*pCmd_size = T5T_prepare_write_block(1, pCmd, data);
                RW_NDEF_T5T_Ndef.MessagePtr = 2;
                RW_NDEF_T5T_Ndef.BlkNb = 2;
                eRW_NDEF_T5T_State = Writing_Data_6;
            }
        }
        break;

    case Writing_Data_6:
        /* Is Write XBH_TRUE ?*/
        if ((Rsp_size == 2) && (pRsp[Rsp_size-1] == 0x00))
        {
            /* Is NDEF write already completed ? */
            if (RW_NdefMessage_size <= RW_NDEF_T5T_Ndef.MessagePtr)
            {
                /* Notify application of the NDEF send completion */
                if(pRW_NDEF_PushCb != XBH_NULL) pRW_NDEF_PushCb(pRW_NdefMessage, RW_NdefMessage_size);
            }
            else
            {
                /* Write NDEF content */
				*pCmd_size = T5T_prepare_write_block(RW_NDEF_T5T_Ndef.BlkNb, pCmd, pRW_NdefMessage+RW_NDEF_T5T_Ndef.MessagePtr);
                RW_NDEF_T5T_Ndef.MessagePtr+=4;
                RW_NDEF_T5T_Ndef.BlkNb++;
            }
        }
        break;

    default:
        break;
    }
}

#endif
#endif
/*
*         Copyright (c), NXP Semiconductors Caen / France
*
*                     (C)NXP Semiconductors
*       All rights are reserved. Reproduction in whole or in part is
*      prohibited without the written consent of the copyright owner.
*  NXP reserves the right to make changes without notice at any time.
* NXP makes no warranty, expressed, implied or statutory, including but
* not limited to any implied warranty of merchantability or fitness for any
*particular purpose, or that the use will not infringe any third party patent,
* copyright or trademark. NXP must not be liable for any loss or damage
*                          arising from its use.
*/

#ifndef REMOVE_RW_SUPPORT
#ifndef REMOVE_NDEF_SUPPORT



/* Allocate buffer for NDEF operations */
unsigned char NdefBuffer[RW_MAX_NDEF_FILE_SIZE];

typedef void RW_NDEF_Fct_t (unsigned char *pCmd, unsigned short Cmd_size, unsigned char *Rsp, unsigned short *pRsp_size);

unsigned char *pRW_NdefMessage;
unsigned short RW_NdefMessage_size;

RW_NDEF_PullCallback_t *pRW_NDEF_PullCb;
RW_NDEF_PushCallback_t *pRW_NDEF_PushCb;

static RW_NDEF_Fct_t *pReadFct = XBH_NULL;
static RW_NDEF_Fct_t *pWriteFct = XBH_NULL;

XBH_BOOL RW_NDEF_SetMessage(unsigned char *pMessage, unsigned short Message_size, void *pCb)
{
    if (Message_size <= RW_MAX_NDEF_FILE_SIZE)
    {
        pRW_NdefMessage = pMessage;
        RW_NdefMessage_size = Message_size;
        pRW_NDEF_PushCb = (RW_NDEF_PushCallback_t*) pCb;
        return true;
    }
    else
    {
        RW_NdefMessage_size = 0;
        pRW_NDEF_PushCb = XBH_NULL;
        return false;
    }
}

void RW_NDEF_RegisterPullCallback(void *pCb)
{
    pRW_NDEF_PullCb = (RW_NDEF_PullCallback_t *) pCb;
}

void RW_NDEF_Reset(unsigned char type)
{
    pReadFct = XBH_NULL;
    pWriteFct = XBH_NULL;

    switch (type)
    {
    case RW_NDEF_TYPE_T1T:
        RW_NDEF_T1T_Reset();
        pReadFct = RW_NDEF_T1T_Read_Next;
        break;
    case RW_NDEF_TYPE_T2T:
        RW_NDEF_T2T_Reset();
        pReadFct = RW_NDEF_T2T_Read_Next;
        pWriteFct = RW_NDEF_T2T_Write_Next;
        break;
    case RW_NDEF_TYPE_T3T:
        RW_NDEF_T3T_Reset();
        pReadFct = RW_NDEF_T3T_Read_Next;
        break;
    case RW_NDEF_TYPE_T4T:
        RW_NDEF_T4T_Reset();
        pReadFct = RW_NDEF_T4T_Read_Next;
        pWriteFct = RW_NDEF_T4T_Write_Next;
        break;
    case RW_NDEF_TYPE_T5T:
        RW_NDEF_T5T_Reset();
        pReadFct = RW_NDEF_T5T_Read_Next;
        pWriteFct = RW_NDEF_T5T_Write_Next;
        break;
    case RW_NDEF_TYPE_MIFARE:
        RW_NDEF_MIFARE_Reset();
        pReadFct = RW_NDEF_MIFARE_Read_Next;
        pWriteFct = RW_NDEF_MIFARE_Write_Next;
        break;
    default:
        break;
    }
}

void RW_NDEF_Read_Next(unsigned char *pCmd, unsigned short Cmd_size, unsigned char *Rsp, unsigned short *pRsp_size)
{
    if (pReadFct != XBH_NULL) pReadFct(pCmd, Cmd_size, Rsp, pRsp_size);
}

void RW_NDEF_Write_Next(unsigned char *pCmd, unsigned short Cmd_size, unsigned char *Rsp, unsigned short *pRsp_size)
{
    if (pWriteFct != XBH_NULL) pWriteFct(pCmd, Cmd_size, Rsp, pRsp_size);
}

#endif
#endif

/********************************************************/




/*
*         Copyright (c), NXP Semiconductors Caen / France
*
*                     (C)NXP Semiconductors
*       All rights are reserved. Reproduction in whole or in part is
*      prohibited without the written consent of the copyright owner.
*  NXP reserves the right to make changes without notice at any time.
* NXP makes no warranty, expressed, implied or statutory, including but
* not limited to any implied warranty of merchantability or fitness for any
*particular purpose, or that the use will not infringe any third party patent,
* copyright or trademark. NXP must not be liable for any loss or damage
*                          arising from its use.
*/

#ifndef REMOVE_CARDEMU_SUPPORT
#ifndef REMOVE_NDEF_SUPPORT

const unsigned char T4T_NDEF_EMU_APP_Select[] = {0x00, 0xA4, 0x04, 0x00, 0x07, 0xD2, 0x76, 0x00, 0x00, 0x85, 0x01, 0x01, 0x00};
const unsigned char T4T_NDEF_EMU_CC[] = {0x00, 0x0F, 0x20, 0x00, 0xFF, 0x00, 0xFF, 0x04, 0x06, 0xE1, 0x04, 0x00, 0xFF, 0x00, 0x00};
const unsigned char T4T_NDEF_EMU_CC_Select[] = {0x00, 0xA4, 0x00, 0x0C, 0x02, 0xE1, 0x03};
const unsigned char T4T_NDEF_EMU_NDEF_Select[] = {0x00, 0xA4, 0x00, 0x0C, 0x02, 0xE1, 0x04};
const unsigned char T4T_NDEF_EMU_Read[] = {0x00,0xB0};
const unsigned char T4T_NDEF_EMU_Write[] = {0x00,0xD6};
const unsigned char T4T_NDEF_EMU_OK[] = {0x90, 0x00};
const unsigned char T4T_NDEF_EMU_NOK[] = {0x6A, 0x82};

unsigned char *pT4T_NdefMessage;
unsigned short T4T_NdefMessage_size = 0;

unsigned char T4T_NdefMessageWritten[256];

typedef enum
{
    Ready,
    NDEF_Application_Selected,
    CC_Selected,
    NDEF_Selected,
    DESFire_prod
} T4T_NDEF_EMU_state_t;

typedef void T4T_NDEF_EMU_Callback_t (unsigned char*, unsigned short);

static T4T_NDEF_EMU_state_t eT4T_NDEF_EMU_State = Ready;

static T4T_NDEF_EMU_Callback_t *pT4T_NDEF_EMU_PushCb = XBH_NULL;

static void T4T_NDEF_EMU_FillRsp (unsigned char *pRsp, unsigned short offset, unsigned char length)
{
    if (offset == 0)
    {
        pRsp[0] = (T4T_NdefMessage_size & 0xFF00) >> 8;
        pRsp[1] = (T4T_NdefMessage_size & 0x00FF);
        if(length>2) memcpy(&pRsp[2], &pT4T_NdefMessage[0], length-2);
    }
    else if (offset == 1)
    {
        pRsp[0] = (T4T_NdefMessage_size & 0x00FF);
        if(length>1) memcpy(&pRsp[1], &pT4T_NdefMessage[0], length-1);
    }
    else
    {
        memcpy(pRsp, &pT4T_NdefMessage[offset-2], length);
    }

    /* Did we reached the end of NDEF message ?*/
    if ((offset + length) >= (T4T_NdefMessage_size + 2))
    {
        /* Notify application of the NDEF send */
        if(pT4T_NDEF_EMU_PushCb != XBH_NULL) pT4T_NDEF_EMU_PushCb(pT4T_NdefMessage, T4T_NdefMessage_size);
    }
}

XBH_BOOL T4T_NDEF_EMU_SetMessage(unsigned char *pMessage, unsigned short Message_size, void *pCb)
{
    pT4T_NdefMessage = pMessage;
    T4T_NdefMessage_size = Message_size;
    pT4T_NDEF_EMU_PushCb = (T4T_NDEF_EMU_Callback_t*) pCb;

    return true;
}

void T4T_NDEF_EMU_Reset(void)
{
    eT4T_NDEF_EMU_State = Ready;
}

void T4T_NDEF_EMU_Next(unsigned char *pCmd, unsigned short Cmd_size, unsigned char *pRsp, unsigned short *pRsp_size)
{
    XBH_BOOL eStatus = false;

    if (!memcmp(pCmd, T4T_NDEF_EMU_APP_Select, sizeof(T4T_NDEF_EMU_APP_Select)))
    {
        *pRsp_size = 0;
        eStatus = true;
        eT4T_NDEF_EMU_State = NDEF_Application_Selected;
    }
    else if (!memcmp(pCmd, T4T_NDEF_EMU_CC_Select, sizeof(T4T_NDEF_EMU_CC_Select)))
    {
        if(eT4T_NDEF_EMU_State == NDEF_Application_Selected)
        {
            *pRsp_size = 0;
            eStatus = true;
            eT4T_NDEF_EMU_State = CC_Selected;
        }
    }
    else if (!memcmp(pCmd, T4T_NDEF_EMU_NDEF_Select, sizeof(T4T_NDEF_EMU_NDEF_Select)))
    {
        *pRsp_size = 0;
        eStatus = true;
        eT4T_NDEF_EMU_State = NDEF_Selected;
    }
    else if (!memcmp(pCmd, T4T_NDEF_EMU_Read, sizeof(T4T_NDEF_EMU_Read)))
    {
        if(eT4T_NDEF_EMU_State == CC_Selected)
        {
            unsigned short offset = (pCmd[2] << 8) + pCmd[3];
            unsigned char length = pCmd[4];

            if(length <= (sizeof(T4T_NDEF_EMU_CC) + offset + 2))
            {
                memcpy(pRsp, &T4T_NDEF_EMU_CC[offset], length);
                *pRsp_size = length;
                eStatus = true;
            }

        }
        else if (eT4T_NDEF_EMU_State == NDEF_Selected)
        {
            unsigned short offset = (pCmd[2] << 8) + pCmd[3];
            unsigned char length = pCmd[4];

            if(length <= (T4T_NdefMessage_size + offset + 2))
            {
                T4T_NDEF_EMU_FillRsp(pRsp, offset, length);
                *pRsp_size = length;
                eStatus = true;
            }
        }
    }
    else if (!memcmp(pCmd, T4T_NDEF_EMU_Write, sizeof(T4T_NDEF_EMU_Write)))
    {
        if (eT4T_NDEF_EMU_State == NDEF_Selected)
        {
            
            unsigned short offset = (pCmd[2] << 8) + pCmd[3];
            unsigned char length = pCmd[4];
            if(offset + length <= sizeof(T4T_NdefMessageWritten))
            {
                memcpy(&T4T_NdefMessageWritten[offset-2], &pCmd[5], length);
                pT4T_NdefMessage = T4T_NdefMessageWritten;
                T4T_NdefMessage_size = (pCmd[5] << 8) + pCmd[6];
                *pRsp_size = 0;
                eStatus = true;
            }
        }
    }

    if (eStatus == true)
    {
        memcpy(&pRsp[*pRsp_size], T4T_NDEF_EMU_OK, sizeof(T4T_NDEF_EMU_OK));
        *pRsp_size += sizeof(T4T_NDEF_EMU_OK);
    } else
    {
        memcpy(pRsp, T4T_NDEF_EMU_NOK, sizeof(T4T_NDEF_EMU_NOK));
        *pRsp_size = sizeof(T4T_NDEF_EMU_NOK);
        T4T_NDEF_EMU_Reset();
    }
}

#endif
#endif
/*
*         Copyright (c), NXP Semiconductors Caen / France
*
*                     (C)NXP Semiconductors
*       All rights are reserved. Reproduction in whole or in part is
*      prohibited without the written consent of the copyright owner.
*  NXP reserves the right to make changes without notice at any time.
* NXP makes no warranty, expressed, implied or statutory, including but
* not limited to any implied warranty of merchantability or fitness for any
*particular purpose, or that the use will not infringe any third party patent,
* copyright or trademark. NXP must not be liable for any loss or damage
*                          arising from its use.
*/

static void INTF_INIT(void)
{
   // SW_I2C_Init();
}

static XBH_BOOL INTF_WRITE(XBH_U8 *pBuff, XBH_U16 buffLen)
{
    if(I2C_WriteRegBytes(ChipPn7160Task::mI2cAddr, 0xFF, buffLen, pBuff)){
        return XBH_TRUE;
    }
    return XBH_FALSE;
}

static XBH_BOOL INTF_READ(XBH_U8 *pBuff, XBH_U16 buffLen)
{
    if(I2C_ReadBytes(ChipPn7160Task::mI2cAddr, buffLen, pBuff)){
        return XBH_TRUE;
    }
    return XBH_FALSE;
}

static XBH_BOOL tml_Init(void) 
{
	// init outside
    //gpio_Init(mPGpio,BOARD_NXPNCI_VEN_PIN,BOARD_NXPNCI_IRQ_PORT,BOARD_NXPNCI_IRQ_PIN);
    INTF_INIT();

    return XBH_TRUE;
}

static XBH_BOOL tml_DeInit(void) 
{
    //gpio_SetValue( mPGpio, BOARD_NXPNCI_VEN_PIN, 0);
	XbhService::getModuleInterface()->setGpioOutputValue(ChipPn7160Task::mPGpio, !ChipPn7160Task::mPLevel);
    return XBH_TRUE;
}

static XBH_BOOL tml_Reset(void) 
{
    //gpio_SetValue( mPGpio, BOARD_NXPNCI_VEN_PIN, 0);
	//delay_1ms(100);
    //gpio_SetValue( mPGpio, BOARD_NXPNCI_VEN_PIN, 1);
	//delay_1ms(100);
	//delay_1ms(20);

	XbhService::getModuleInterface()->setGpioOutputValue(ChipPn7160Task::mPGpio, !ChipPn7160Task::mPLevel);
	usleep(100* 1000);
	XbhService::getModuleInterface()->setGpioOutputValue(ChipPn7160Task::mPGpio, ChipPn7160Task::mPLevel);
	usleep(500 * 1000);
	return XBH_TRUE;
}

static XBH_BOOL tml_Tx(XBH_U8 *pBuff, XBH_U16 buffLen) 
{
    if (INTF_WRITE(pBuff, buffLen) != XBH_TRUE)
    {
    	usleep (10*1000);
    	if(INTF_WRITE(pBuff, buffLen) != XBH_TRUE)
    	{
    		return XBH_FALSE;
    	}
    }

	return XBH_TRUE;
}

static XBH_BOOL tml_Rx(XBH_U8 *pBuff, XBH_U16 buffLen, XBH_U16 *pBytesRead) 
{
    if(INTF_READ(pBuff, 3) == XBH_TRUE)
    {
    	if ((pBuff[2] + 3) <= buffLen)
    	{
			if (pBuff[2] > 0)
			{
				if(INTF_READ(&pBuff[3], pBuff[2]) == XBH_TRUE)
				{
					*pBytesRead = pBuff[2] + 3;
				}
				else{
					return XBH_FALSE;
				}
			} else {
				*pBytesRead = 3;
			}
    	}
		else {
			return XBH_FALSE;
		}
   } else {
	   return XBH_FALSE;
   }

	return XBH_TRUE;
}

static XBH_BOOL tml_WaitForRx(uint32_t timeout) 
{
	
	return XBH_TRUE;
}

void tml_Connect(void) 
{
	tml_Init();
    usleep(100*1000);
	tml_Reset();
}

void tml_Disconnect(void) 
{
	tml_DeInit();
}

void tml_Send(XBH_U8 *pBuffer, XBH_U16 BufferLen, XBH_U16 *pBytesSent) 
{   
	if(tml_Tx(pBuffer, BufferLen) == XBH_FALSE) *pBytesSent = 0;
	else *pBytesSent = BufferLen;
}

void tml_Receive(XBH_U8 *pBuffer, XBH_U16 BufferLen, XBH_U16 *pBytes, XBH_U16 timeout) 
{
	//if (tml_WaitForRx(timeout) == XBH_FALSE){
    //    XLOGD("###################IRQ NO 0 ");
	//	*pBytes = 0;
	//}else{
        usleep(100);
		tml_Rx(pBuffer, BufferLen, pBytes);
	//}
}

}//namespace Xbhpn7160


/**********************************************************************************/

//XbhFramework::getIICData(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, XBH_U8* u8Data)
//XBH_S32 XbhFramework::setIICData(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, const XBH_U8* u8Data)

XBH_BOOL Xbhpn7160::I2C_WriteRegBytes(XBH_U8 u8DeviceAddr, XBH_U8 u8RegAddr, XBH_U16 wrNumber, XBH_U8 * wrPointer)
{
    for(int i=0;i<wrNumber;i++)
    {
    // XLOGD("#######################wrPointer=%x",wrPointer[i]);
    }
    XBH_S32 s32Ret =XbhService::getModuleInterface()->setI2cData(ChipPn7160Task::mI2cBus, u8DeviceAddr, wrPointer[0], 1, (XBH_U32)wrNumber-1, wrPointer+1);
    //XLOGD("####################### w s32Ret=%x",s32Ret);
    return XBH_TRUE;
}

XBH_BOOL Xbhpn7160::I2C_ReadRegBytes(XBH_U8 u8DeviceAddr, XBH_U8 u8RegAddr, XBH_U16 rdNumber, XBH_U8 * rdPointer)
{
    
    XBH_S32 s32Ret =XbhService::getModuleInterface()->getI2cData(ChipPn7160Task::mI2cBus, u8DeviceAddr, 0x00, 1,  (XBH_U32)rdNumber, rdPointer);
    for(int i=0;i<rdNumber;i++)
    {
     //XLOGD("111#######################rdPointer=%x",rdPointer[i]);
    }
   // XLOGD("####################### r111 s32Ret=%x",s32Ret);
    return XBH_TRUE;

}

XBH_BOOL Xbhpn7160::I2C_ReadBytes(XBH_U8 u8DeviceAddr, XBH_U16 rdNumber, XBH_U8* rdPointer)
{

    XBH_S32 s32Ret =XbhService::getModuleInterface()->getI2cData(ChipPn7160Task::mI2cBus, u8DeviceAddr, 0x00, 1, rdNumber, rdPointer);
    for(int i=0;i<rdNumber;i++)
    {
    //XLOGD("222#######################rdPointer=%x",rdPointer[i]);
    }
    //XLOGD("####################### r333 s32Ret=%x",s32Ret);  
    return XBH_TRUE;
}

XBH_U8* ChipPn7160Task::GetID(void)
{
    return &(Xbhpn7160::nfc_card_info.id[0]);

}

XBH_U32 ChipPn7160Task::NFCSTATUS(void)
{
    return Xbhpn7160::NFC_STATUS;

}

XBH_S32 ChipPn7160Task::getNfcId(XBH_U8 *data, XBH_U32 *length)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    int num = Xbhpn7160::nfc_card_info.id_len;
    *length = num;
    if(num!=0){
        for(int i=0;i<num;i++){
            data[i]=Xbhpn7160::nfc_card_info.id[i];
             XLOGE("getNfcId[%d]:0x%d \n",i,data[i]);
        }
    }
    return s32Ret;
}

XBH_S32 ChipPn7160Task::Init(void)
{
    // PN7160 VEN
    XBH_S32  ret =  XbhService::getModuleInterface()->setGpioOutputValue(mPGpio, mPLevel);
    //usleep(500*1000);
    //只有当nfc模块被正常初始化后才会启动循环检测
    if(Xbhpn7160::NFC_Init() == NFC_XBH_TRUE)
    {
        XbhMWThread::run(REPEAT);
    }
    return ret;
}

void ChipPn7160Task::run(const void* arg)
{   
    Xbhpn7160::NFC_Handle_Process();
    usleep(500 * 1000);
}

ChipPn7160Task::ChipPn7160Task(XBH_S32 iicBus, XBH_S32 iicAddr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel)
{
    XLOGW("ChipPn7160Task() init");
    XLOGE(" begin ");
    mI2cBus = iicBus;
    if(iicAddr == -1)
    {
        mI2cAddr = 0x28;
    }
    XLOGE(" mI2cBus =%d ",mI2cBus);
    mPGpio = pGpio;
    mPLevel = pLevel;
    mRGpio = rGpio;
    mRLevel = rLevel;

    Init();
    XLOGE(" end ");
}

ChipPn7160Task::~ChipPn7160Task()
{
    XLOGW("ChipPn7160Task() deinit");
}
