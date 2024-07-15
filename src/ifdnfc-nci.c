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

#include <linux_nfc_api.h>

#include <ifdhandler.h>
#include <debuglog.h>

#include "atr.h"


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
