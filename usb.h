#pragma once
#include <iostream>
#include <winusb.h>
extern "C"{
	#include <hidsdi.h>
}
#include <setupapi.h>
#include "util.h"

#define SILENT			//Packete werden nicht in den output stream geschrieben
// #define SHOW_ERRORS	//Überschreibt SILENT nur für Fehlernachrichten

//TODO sollte nicht GetLastError() in den cerr schreiben sondern diese sollten auch ErrorCodes sein
ErrCode openDevice(HANDLE& handle, const char* devicePath = "\\\\.\\COM3", DWORD baudrate = 9600){
	HANDLE hDevice = CreateFile(devicePath, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
	if(hDevice == INVALID_HANDLE_VALUE){
//		std::cerr << GetLastError() << std::endl;
		return INVALID_USB_HANDLE;
	}
    DCB dcbSerialParams = {};
    COMMTIMEOUTS timeouts = {};
	//Konfiguriere die Schnittstelle (Baudrate, Datenbits, Parit�t, Stoppbits)
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	if(!GetCommState(hDevice, &dcbSerialParams)){
//		std::cerr << GetLastError() << std::endl;
		CloseHandle(hDevice);
		return COMMSTATE_ERROR;
	}
	dcbSerialParams.BaudRate = baudrate;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;
	if(!SetCommState(hDevice, &dcbSerialParams)){
//		std::cerr << GetLastError() << std::endl;
		CloseHandle(hDevice);
		return COMMSTATE_ERROR;
	}
	//Konfiguriere die Timeouts f�r den Lesevorgang
	timeouts.ReadIntervalTimeout = MAXDWORD;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 0;
	if(!SetCommTimeouts(hDevice, &timeouts)){
//		std::cerr << GetLastError() << std::endl;
		CloseHandle(hDevice);
		return TIMEOUT_SET_ERROR;
	}
	handle = hDevice;
	return SUCCESS;
}

ErrCode closeDevice(HANDLE& handle){
	CloseHandle(handle);
	handle = nullptr;
	return SUCCESS;
}

//ret: Anzahl der gesendeten Bytes, -1 bei Fehler
int sendData(HANDLE hDevice, BYTE* data, DWORD length){
	DWORD bytesSent;
	if(!WriteFile(hDevice, data, length, &bytesSent, 0)){
		std::cout << "Fehler beim Senden der Daten! " << GetLastError() << std::endl;
		return -1;
	}
	return bytesSent;
}

//Liest die Daten im Empfangspuffer, diese können noch unvollständig sein
//ret: Anzahl der gelesenen Bytes
int readData(HANDLE hDevice, BYTE* data, DWORD length){
	DWORD bytesRead;
	if(!ReadFile(hDevice, data, length, &bytesRead, 0)){
		std::cout << "Fehler beim Lesen der Daten! " << GetLastError() << std::endl;
		return -1;
	}
	return bytesRead;
}

//Blockierend
std::string readPacket(HANDLE device){
    std::string ret;
    BYTE readBuffer[32];
    while(1){
        int count = readData(device, readBuffer, 32);
        if(count == -1) exit(-1);
        for(int i=0; i < count; ++i){
            if(readBuffer[i] == '\n') goto exitReadPacket;
            if((readBuffer[i] < 32 || readBuffer[i] > 128)) continue;
            ret += readBuffer[i];
        }
    }
    exitReadPacket:
    return ret;
}
