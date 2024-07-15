/*
 * Copyright (C) 2024 Christoph Honal
 *
 * This file is part of ifdnfc-nci.
 *
 * ifdnfc-nci is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * ifdnfc-nci is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ifdnfc-nci.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <linux_nfc_api.h>

#include <pcsclite.h>
#include <reader.h>
#include <ifdhandler.h>
#include <debuglog.h>

#include "atr.h"


static nfcTagCallback_t tagCb;
static nfc_tag_info_t tag;
static bool tag_present = false;
static unsigned char atr[MAX_ATR_SIZE];
static size_t atr_len = 0;


static void nci_reset_atr()
{
    atr_len = 0;
    memset(atr, 0, MAX_ATR_SIZE);
}

static void nci_onTagArrival(nfc_tag_info_t *pTag)
{
    Log1(PCSC_LOG_DEBUG, "NFC Tag found");
    tag = *pTag;
    tag_present = true;

    atr_len = MAX_ATR_SIZE;
    switch(tag.technology)
    {
        case TARGET_TYPE_ISO14443_3A:
            // TL and CRC1/CRC2 are already stripped
            if (get_atr(ATR_ISO14443A_106, tag.param.pa.ats_res, tag.param.pa.ats_res_len, atr, &atr_len) == 0)
            {
                Log1(PCSC_LOG_DEBUG, "Failed to generate ATR for ISO14443_3A");
                nci_reset_atr();
            }
            break;
        case TARGET_TYPE_ISO14443_3B:
            if (get_atr(ATR_ISO14443B_106, tag.param.pb.atqb, tag.param.pb.atqb_len, atr, &atr_len) == 0)
            {
                Log1(PCSC_LOG_DEBUG, "Failed to generate ATR for ISO1444_3B");
                nci_reset_atr();
            }
            break;
        default:
            // For all other types: Empty ATR
            Log1(PCSC_LOG_INFO, "Returning empty ATR for card without APDU support.");
            nci_reset_atr();
            break;
    }

}

static void nci_onTagDeparture(void)
{
    Log1(PCSC_LOG_DEBUG, "NFC Tag lost");
    tag_present = false;
    nci_reset_atr();
}

static bool nci_start()
{
    Log1(PCSC_LOG_INFO, "Starting NFC-NCI stack");
    if(nfcManager_doInitialize() != 0) return false;

    int fw = nfcManager_getFwVersion();
    if(fw == 0) return false;
    Log4(PCSC_LOG_INFO, "NFC hardware ROM: %d, FW: %d.%d\n", (fw >> 16) & 0xFF, (fw >> 8) & 0xFF, fw & 0xFF);

    tagCb.onTagArrival = nci_onTagArrival;
    tagCb.onTagDeparture = nci_onTagDeparture;
    nfcManager_registerTagCallback(&tagCb);
    nfcManager_enableDiscovery(DEFAULT_NFA_TECH_MASK, 1, 0, 0);

    return true;
}

static bool nci_stop()
{
    Log1(PCSC_LOG_INFO, "Stopping NFC-NCI stack");
    bool res = true;
     if(nfcManager_disableDiscovery() != 0) res = false;
    nfcManager_deregisterTagCallback();
    if(nfcManager_doDeinitialize() != 0) res = false;
    return res;
}


RESPONSECODE IFDHCreateChannel(DWORD Lun, DWORD Channel)
{
    if((Lun & 0xffff) != 0) return IFD_COMMUNICATION_ERROR; 

    return nci_start()? IFD_SUCCESS:IFD_COMMUNICATION_ERROR;
}

RESPONSECODE IFDHCreateChannelByName(DWORD Lun, LPSTR DeviceName)
{
    return IFDHCreateChannel(Lun, 0);
}

RESPONSECODE IFDHCloseChannel(DWORD Lun)
{
    if((Lun & 0xffff) != 0) return IFD_COMMUNICATION_ERROR; 

    return nci_stop()? IFD_SUCCESS:IFD_COMMUNICATION_ERROR;
}

RESPONSECODE IFDHICCPresence(DWORD Lun)
{
    if((Lun & 0xffff) != 0) return IFD_COMMUNICATION_ERROR; 

    if(nfcManager_isNfcActive())
    {
        return tag_present? IFD_ICC_PRESENT:IFD_ICC_NOT_PRESENT;
    }

    return IFD_COMMUNICATION_ERROR;
}

RESPONSECODE IFDHGetCapabilities(DWORD Lun, DWORD Tag, PDWORD Length, PUCHAR Value)
{
    if((Lun & 0xffff) != 0) return IFD_COMMUNICATION_ERROR; 

    if (!Length || !Value) return IFD_COMMUNICATION_ERROR;

    switch (Tag) 
    {
        case TAG_IFD_ATR:
        case SCARD_ATTR_ATR_STRING:
            if(IFDHICCPresence(Lun) != IFD_ICC_PRESENT) return IFD_COMMUNICATION_ERROR;
            if(atr_len == 0) return IFD_COMMUNICATION_ERROR;
            if(*Length < atr_len) return IFD_ERROR_INSUFFICIENT_BUFFER;
            memcpy(Value, atr, atr_len);
            *Length = atr_len;
            break;
        case TAG_IFD_SIMULTANEOUS_ACCESS:
        case TAG_IFD_THREAD_SAFE:
            if (*Length < 1) return IFD_ERROR_INSUFFICIENT_BUFFER;
            *Value = 0;
            *Length = 1;
            break;
        case TAG_IFD_SLOTS_NUMBER:
            if (*Length < 1) return IFD_ERROR_INSUFFICIENT_BUFFER;
            *Value  = 1;
            *Length = 1;
            break;
        case TAG_IFD_STOP_POLLING_THREAD:
        case TAG_IFD_POLLING_THREAD_WITH_TIMEOUT:
        case TAG_IFD_POLLING_THREAD_KILLABLE:
            return IFD_ERROR_NOT_SUPPORTED;
        default:
            Log3(PCSC_LOG_ERROR, "Tag %08lx (%lu) not supported", Tag, (unsigned long) Tag);
            return IFD_ERROR_TAG;
    }

    return IFD_SUCCESS;
}

RESPONSECODE IFDHSetCapabilities(DWORD Lun, DWORD Tag, DWORD Length, PUCHAR Value)
{
    return IFD_ERROR_VALUE_READ_ONLY;
}

RESPONSECODE IFDHSetProtocolParameters(DWORD Lun, DWORD Protocol, 
    UCHAR Flags, UCHAR PTS1, UCHAR PTS2, UCHAR PTS3)
{
    return (Protocol == SCARD_PROTOCOL_T1)? IFD_SUCCESS:IFD_PROTOCOL_NOT_SUPPORTED;
}

RESPONSECODE IFDHPowerICC(DWORD Lun, DWORD Action, PUCHAR Atr, PDWORD AtrLength)
{
    if((Lun & 0xffff) != 0) return IFD_COMMUNICATION_ERROR; 

    if(IFDHICCPresence(Lun) != IFD_ICC_PRESENT) return IFD_COMMUNICATION_ERROR;

    if(!Atr || !AtrLength) return IFD_COMMUNICATION_ERROR;

    switch (Action) {
        case IFD_POWER_DOWN:
            // IFD_POWER_DOWN: Power down the card (Atr and AtrLength should be zeroed)
            // Warning, this means putting card to HALT, not reader to IDLE or RFoff
            *AtrLength = 0;
            return IFD_SUCCESS;
        case IFD_RESET:
            // IFD_RESET: Perform a warm reset of the card (no power down). 
            // If the card is not powered then power up the card
            // In contactless, ATR on warm reset is always same as on cold reset
            if(!nfcTag_reconnect())
            {
                nci_reset_atr();
                *AtrLength = 0;
                return IFD_ERROR_POWER_ACTION;
            }
            // fall through
        case IFD_POWER_UP:
            if (*AtrLength < atr_len) return IFD_COMMUNICATION_ERROR;
            memcpy(Atr, atr, atr_len);
            *AtrLength = atr_len;
            return IFD_SUCCESS;
        default:
            Log2(PCSC_LOG_ERROR, "Action %lu not supported", (unsigned long) Action);
            return IFD_NOT_SUPPORTED;
    }

    return IFD_SUCCESS;
}

RESPONSECODE IFDHTransmitToICC(DWORD Lun, SCARD_IO_HEADER SendPci, PUCHAR TxBuffer, 
    DWORD TxLength, PUCHAR RxBuffer, PDWORD RxLength, PSCARD_IO_HEADER RecvPci)
{
    if((Lun & 0xffff) != 0) return IFD_COMMUNICATION_ERROR; 

    if (!RxLength || !RecvPci) return IFD_COMMUNICATION_ERROR;

    if(IFDHICCPresence(Lun) != IFD_ICC_PRESENT) return IFD_ICC_NOT_PRESENT;

    // GET DATA command
    if ((TxBuffer[0] == 0xFF) && (TxBuffer[1] == 0xCA))
    {
        LogXxd(PCSC_LOG_INFO, "Intercepting GetData\n", TxBuffer, TxLength);
        size_t Le = TxBuffer[4];
        int RxOff = 0;
        uint8_t* Data;
        size_t DataLength;
        RecvPci->Protocol = 1; // Needed ?
        if (TxLength != 5)
        {
            // Wrong length
            RxBuffer[RxOff++] = 0x67;
            RxBuffer[RxOff++] = 0x00;
            *RxLength = RxOff;
            return IFD_SUCCESS;
        }
        switch (TxBuffer[2]) 
        {
            case 0x00: // Get UID
                Data = tag.uid;
                DataLength = tag.uid_length;
                break;
            case 0x01: // Get ATS hist bytes
                if (tag.technology == TARGET_TYPE_ISO14443_3A)
                {
                    Data = tag.param.pa.his_byte;
                    DataLength = tag.param.pa.his_byte_len;
                    if (DataLength)
                    {
                        size_t idx = 1;
                        // Bits 5 to 7 tell if TA1/TB1/TC1 are available
                        if (Data[0] & 0x10) idx++; // TA
                        if (Data[0] & 0x20) idx++; // TB
                        if (Data[0] & 0x40) idx++; // TC
                        if (DataLength > idx)
                        {
                            DataLength -= idx;
                            Data += idx;
                        }
                        else
                        {
                            DataLength = 0;
                        }
                    }
                    break;
                } // else fall though
            default:
                // Function not supported
                RxBuffer[RxOff++] = 0x6A;
                RxBuffer[RxOff++] = 0x81;
                *RxLength = RxOff;
                return IFD_SUCCESS;
        }
        if (Le == 0) Le = DataLength;
        if (Le < DataLength)
        {
            // Wrong length
            RxBuffer[RxOff++] = 0x6C;
            RxBuffer[RxOff++] = DataLength;
            *RxLength = RxOff;
            return IFD_SUCCESS;
        }
        RxOff = DataLength;
        memcpy(RxBuffer, Data, RxOff);
        if (Le > RxOff)
        {
            // End of data reached before Le bytes
            for (;RxOff < Le;) RxBuffer[RxOff++] = 0;
            RxBuffer[RxOff++] = 0x62;
            RxBuffer[RxOff++] = 0x82;
        }
        else
        {
            RxBuffer[RxOff++] = 0x90;
            RxBuffer[RxOff++] = 0x00;
        }
        *RxLength = RxOff;
        return IFD_SUCCESS;
    }

    LogXxd(PCSC_LOG_DEBUG, "Sending to NFC target\n", TxBuffer, TxLength);

    // Timeout pushed to 5000ms, cf FWTmax in ISO14443-4
    int res = nfcTag_transceive(tag.handle, TxBuffer, TxLength, RxBuffer, *RxLength, 5000);
    if (res == 0)
    {
        Log1(PCSC_LOG_ERROR, "Could not transceive data");
        *RxLength = 0;
        return IFD_COMMUNICATION_ERROR;
    }
    *RxLength = res;
    RecvPci->Protocol = 1;
    LogXxd(PCSC_LOG_DEBUG, "Received from NFC target\n", RxBuffer, *RxLength);

    return IFD_SUCCESS;
}

RESPONSECODE IFDHControl(DWORD Lun, DWORD dwControlCode, PUCHAR TxBuffer, DWORD TxLength,
    PUCHAR RxBuffer, DWORD RxLength, LPDWORD pdwBytesReturned)
{
    if((Lun & 0xffff) != 0) return IFD_COMMUNICATION_ERROR; 

    if(pdwBytesReturned) *pdwBytesReturned = 0;

    return IFD_ERROR_NOT_SUPPORTED;
}
