#include <stdio.h>

#include <linux_nfc_api.h>
#include <ifdhandler.h>


RESPONSECODE IFDHCreateChannelByName(DWORD Lun, LPSTR DeviceName)
{
	return IFD_SUCCESS;
}

RESPONSECODE IFDHCreateChannel(DWORD Lun, DWORD Channel)
{
	return IFD_SUCCESS;
}

RESPONSECODE IFDHCloseChannel(DWORD Lun)
{
	return IFD_SUCCESS;
}

RESPONSECODE IFDHGetCapabilities(DWORD Lun, DWORD Tag, PDWORD Length, PUCHAR Value)
{
	return IFD_SUCCESS;
}

RESPONSECODE IFDHSetCapabilities(DWORD Lun, DWORD Tag, DWORD Length, PUCHAR Value)
{
	return IFD_ERROR_VALUE_READ_ONLY;
}

RESPONSECODE IFDHSetProtocolParameters(DWORD Lun, DWORD Protocol, 
	UCHAR Flags, UCHAR PTS1, UCHAR PTS2, UCHAR PTS3)
{
	if (Protocol != SCARD_PROTOCOL_T1)
	{
    	return IFD_PROTOCOL_NOT_SUPPORTED;
	}

  	return IFD_SUCCESS;
}

RESPONSECODE IFDHPowerICC(DWORD Lun, DWORD Action, PUCHAR Atr, PDWORD AtrLength)
{
	return IFD_SUCCESS;
}

RESPONSECODE IFDHTransmitToICC(DWORD Lun, SCARD_IO_HEADER SendPci, PUCHAR TxBuffer, 
	DWORD TxLength, PUCHAR RxBuffer, PDWORD RxLength, PSCARD_IO_HEADER RecvPci)
{
	return IFD_SUCCESS;
}

RESPONSECODE IFDHICCPresence(DWORD Lun)
{
	return IFD_SUCCESS;
}

RESPONSECODE IFDHControl(DWORD Lun, DWORD dwControlCode, PUCHAR TxBuffer, DWORD TxLength,
	PUCHAR RxBuffer, DWORD RxLength, LPDWORD pdwBytesReturned)
{
	return IFD_SUCCESS;
}
