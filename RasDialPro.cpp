/* !CRF! - RasDialPro.cpp */
/* A functional duplicate of Windows NT rasdial.exe that works in Windows 95 (and NT) */
/* Copyright (c) 2002 by Claudio Fahey */

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <windows.h>
#include <ras.h>
//#include <iphlpapi.h>
#include "resource.h"
#include "RasDialProCommon.h"
#include "..\RegCode\RegCodeCommon.h"

#define MAXRASCONN		16
#define SZUNLICENSEDTEXT	"UNLICENSED EVALUATION VERSION -- 30 DAY TRIAL"
#define MAXADDROUTES	1		/* maximum number of add routes in unregistered version */

/* Linked list of route additions */
typedef struct _ADDROUTEINFO
	{
    char szDestIpAddress[64];			// 64 allows users to specify a descriptive network name
	char szDestIpMask[RAS_MaxIpAddress + 1];
	int iMetric;
	struct _ADDROUTEINFO *pNext;
	} ADDROUTEINFO;

static HRASCONN hRasConn = NULL;
static RASCONNSTATUS RasConnStatus = {0};

static BOOL flReturnApiError = FALSE;

/* Reg name and code static variables - overwritten in exe file by register.exe */
static char szRegName[80] =  "XOXOXOXOXO-REGNAME-XOXOXOXOXO";
static char szRegCode1[80] = "XOXOXOXOXO-REGCODE1-XOXOXOXOXO";
static int flRegistered = 0;
static unsigned long dwRegCode1 = 0;

LPTSTR GetLastErrorText(LPTSTR lpszBuf, DWORD dwSize)
	{
    DWORD dwRet;
    LPTSTR lpszTemp = NULL;

    dwRet = FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ARGUMENT_ARRAY,
                           NULL,
                           GetLastError(),
                           LANG_NEUTRAL,
                           (LPTSTR)&lpszTemp,
                           0,
                           NULL );

    // supplied buffer is not long enough
    if ( !dwRet || ( (long)dwSize < (long)dwRet+14 ) )
        lpszBuf[0] = TEXT('\0');
    else
		{
        lpszTemp[lstrlen(lpszTemp)-2] = TEXT('\0');  //remove cr and newline character
        sprintf( lpszBuf, TEXT("%s (0x%x)"), lpszTemp, GetLastError() );
		}

    if ( lpszTemp )
        LocalFree((HLOCAL) lpszTemp );

    return lpszBuf;
	}

void PrintRASError(DWORD dwRc)
	{
	char szErrorString[256] = "";

	switch (dwRc)
		{
		case ERROR_ACCESS_DENIED:
			strcpy(szErrorString, "Access is denied"); 
			break;

		default:
			RasGetErrorString(dwRc, szErrorString, 256);
			if (szErrorString[0] == 0)
				strcpy(szErrorString, "Unknown error");
			break;
		}

	printf(APPNAME ": RAS Error %u: %s\n", dwRc, szErrorString);
	}

void PrintWin32Error(DWORD dwRc)
	{
	char szErrorString[256] = "";

	GetLastErrorText(szErrorString, 256);
	if (szErrorString[0] == 0)
		sprintf(szErrorString, "Unknown error (0x%x)", dwRc);

	printf(APPNAME ": Win32 Error: %s\n", szErrorString);
	}

int GetAppReturnCode(DWORD dwRc)
	{
	return (flReturnApiError ? dwRc : 1);
	}

BOOL CtrlHandler(DWORD fdwCtrlType)
	{ 
	if (hRasConn && RasHangUp(hRasConn) == 0)
		{
		RasConnStatus.dwSize = sizeof(RasConnStatus);				
		while (RasGetConnectStatus(hRasConn, &RasConnStatus) != ERROR_INVALID_HANDLE)
			Sleep(0);
		}
	ExitProcess(2);
	return FALSE;
    } 

void PrintBanner(void)
	{
	printf(
		APPNAME " " VERSION " (" __DATE__ ")\n"
		"Copyright (C) " COPYRIGHTYEAR " Mu-Consulting (www.muconsulting.com)\n"
		"%s%s\n", (flRegistered ? "Licensed to: " : ""), szRegName);
	}

void PrintUsage(void)
	{
	if (flRegistered)
		PrintBanner();	// show banner here because it doesn't print at startup

	printf(
		"USAGE:\n"
		"\tTo dial:\n"
		"\t" EXENAME " entryname [username [password]] [/DOMAIN:domain]\n"
		"\t          [/PHONE:phonenumber] [/CALLBACK:callbacknumber]\n"
		"\t          [/RETRYCOUNT:n] [/RETRYDELAY:s] [/VERBOSE]\n"
		"\t          [/ADDROUTE destination [MASK netmask] [METRIC metric]]...\n"
		"\t          [/APIERROR]\n"
		"\n"
		"\tTo hangup:\n"
		"\t" EXENAME " [entryname] /DISCONNECT\n\n"
		"\tTo display active connections:\n"
		"\t" EXENAME "\n\n"
		"\tTo query the status of a connection:\n"
		"\t" EXENAME " [entryname] /QUERY\n");
	}

BOOL RegQueryInt(HKEY hkey, LPCTSTR szValueName, DWORD &dw)
	{
	LONG lRc = 0;
	DWORD dwType = 0, cb = 0, dwBuf = 0;
	
	if (!hkey)
		return FALSE;

	cb = sizeof(DWORD);
	lRc = RegQueryValueEx(hkey, szValueName, NULL, &dwType, (LPBYTE) &dwBuf, &cb);
	if (lRc == ERROR_SUCCESS && dwType == REG_DWORD)
		{
		dw = dwBuf;
		return TRUE;
		}
	else
		return FALSE;
	}

int main(int argc, char *argv[])
	{
	RASDIALPARAMS rasdialparams = {0};
	char *szEntryName = "", *szUserName = "", *szPassword = "", *szDomain = "",
		*szPhoneNumber = "", *szCallbackNumber = "";
	DWORD dwRc = 0;
	BOOL flDisconnect = FALSE, flDial = FALSE, flRc, flAddRoute = TRUE, flVerbose = FALSE,
		 flQuery = FALSE;
	RASCONN RasConn[MAXRASCONN] = {0};
	DWORD cb = sizeof(RasConn), cConnections = 0;
	DWORD iConnection = 0;
	int cRetryCount = 0, iRetry = 0, iRetryDelay = 0;
	ADDROUTEINFO arFirst = {0}, *parNext = &arFirst;
	int car = 0;	// count of add routes
	BOOL fPassword = 0, flShowPass = 0;

	/* Get reg name and code from static variables */
	dwRegCode1 = strtoul(szRegCode1, 0, 16);
	SetRegCodeSeed(REGCODESEED);
	if (ValidateRegCode(szRegName, dwRegCode1))
		{
		//printf("Licensed to: %s\n", szRegName);
		//printf("Registration Code: %010u\n", dwRegCode1);
		flRegistered = 1;
		}
	else
		{
		dwRegCode1 = 0;
		strcpy(szRegName, SZUNLICENSEDTEXT);
		//printf("%s\n", SZUNLICENSEDTEXT);
		}

	if (!flRegistered)
		PrintBanner();

	//
	// If unregistered, verify that 30 day evaluation is not over
	//

	if (!flRegistered)
		{
		BOOL flExpired = TRUE;
		FILETIME ftNow = {0}, ftInstall = {0};
		ULARGE_INTEGER uliNow = {0}, uliInstall = {0};
		ULONGLONG ullDiff = 0;
		DWORD dwRegCodeA = 0, dwRegCodeB = 0;
		double dEvalDaysLeft = 0;
	
		// get current time
		GetSystemTimeAsFileTime(&ftNow);
		uliNow.LowPart = ftNow.dwLowDateTime;
		uliNow.HighPart = ftNow.dwHighDateTime;

		char *szKey = SZREGKEYCOMMON;

		//
		// Open registry key and read or write install time
		//
		try
			{
			LONG lRc = 0;
			BOOL flRc = 0;
			HKEY hkey = 0;

			// get install time from registry

			lRc = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szKey, 0, 
				KEY_QUERY_VALUE, &hkey);
			if (lRc != ERROR_SUCCESS)
				hkey = 0;

			if (hkey)
				{
				flRc = RegQueryInt(hkey, "RegCodeA", dwRegCodeA);
				flRc = RegQueryInt(hkey, "RegCodeB", dwRegCodeB);
				RegCloseKey(hkey);
				hkey = 0;
				}

			ftInstall.dwLowDateTime = dwRegCodeA;
			ftInstall.dwHighDateTime = dwRegCodeB;
			uliInstall.LowPart = ftInstall.dwLowDateTime;
			uliInstall.HighPart = ftInstall.dwHighDateTime;

			// if install time doesn't exists...
			if (uliInstall.QuadPart == 0)
				{
				// set first run time to now
				uliInstall.QuadPart = uliNow.QuadPart;

				// Save first run time (install) in registry
				//printf("creating key\n");
				DWORD dwDisposition = 0;
				lRc = RegCreateKeyEx(HKEY_LOCAL_MACHINE, szKey, 0, "", 0, KEY_WRITE | KEY_READ, 0, 
					&hkey, &dwDisposition);
				if (lRc != ERROR_SUCCESS)
					throw (DWORD) lRc;

				lRc = RegSetValueEx(hkey, "RegCodeA", 0, REG_DWORD,
					(CONST BYTE*) &uliInstall.LowPart, sizeof(DWORD));
				if (lRc != ERROR_SUCCESS)
					throw (DWORD) lRc;

				lRc = RegSetValueEx(hkey, "RegCodeB", 0, REG_DWORD,
					(CONST BYTE*) &uliInstall.HighPart, sizeof(DWORD));			
				if (lRc != ERROR_SUCCESS)
					throw (DWORD) lRc;

				if (hkey)
					RegCloseKey(hkey);
				}

			if (uliNow.QuadPart >= uliInstall.QuadPart)
				{
				ullDiff = uliNow.QuadPart - uliInstall.QuadPart;
				if (EVAL100NS >= ullDiff)
					{
					// installed for less than 30 days
					dEvalDaysLeft = ((double) ((LONGLONG) (EVAL100NS - ullDiff) / 10 / 1000 / 1000))
							/ 60.0 / 60.0 / 24.0;
					flExpired = FALSE;
					}
				}
			else
				{
				// Install time is in future!
				}
			}
		catch (DWORD dwErr)
			{
			SetLastError(dwErr);
			PrintWin32Error(dwErr);
			printf(APPNAME ": Unable to access registry key %s\n", szKey);
			}

		if (flExpired)
			{
			printf(APPNAME ": This unlicensed version has exceeded its %u day evaluation and has expired.\n"
				"To purchase this product, visit http://" SZWEBSITE ".\n", EVALDAYS);
			return 1;
			}
		else
			{
			printf("You have %0.2lf days left to evaluate this product.\n"
				"To purchase this product, visit "
				"http://" SZWEBSITE ".\n", dEvalDaysLeft);
			}
		}

	printf("\n");
		
	/*
	 * Process arguments
	 */
	while (*(++argv))
		{
		if ((*argv)[0] == '/')
			{
			/*
			 * A parameter is given that starts with "/"
			 */
			if (strnicmp("/DOMAIN:", *argv, 8) == 0)
				szDomain = *argv+8;
			else if (stricmp("/DISCONNECT", *argv) == 0)
				flDisconnect = TRUE;
			else if (stricmp("/QUERY", *argv) == 0)
				flQuery = TRUE;
			else if (strnicmp("/PHONE:", *argv, 7) == 0)
				szPhoneNumber = *argv+7;
			else if (strnicmp("/CALLBACK:", *argv, 10) == 0)
				szCallbackNumber = *argv+10;
			else if (strnicmp("/RETRYCOUNT:", *argv, 12) == 0)				cRetryCount = atol(*argv+12);
			else if (strnicmp("/RETRYDELAY:", *argv, 12) == 0)
				iRetryDelay = atol(*argv+12);
			else if (stricmp("/VERBOSE", *argv) == 0)
				flVerbose = TRUE;
			else if (flRegistered && stricmp("/SHOWPASS", *argv) == 0)
				flShowPass = TRUE;
			else if (stricmp("/APIERROR", *argv) == 0)
				flReturnApiError = TRUE;
			else if (stricmp("/ADDROUTE", *argv) == 0)
				{
				argv++;
				if (*argv == 0)
					{
					printf(APPNAME ": Destination network not specified after /ADDROUTE parameter.\n");
					PrintUsage();
					return 1;
					}

				if (!flRegistered && car >= MAXADDROUTES)
					{
					printf(APPNAME ": The unlicensed version allows a maximum of %d route addition%s.\n",
						MAXADDROUTES, MAXADDROUTES == 1 ? "" : "s");
					return 1;
					}

				// create ADDROUTEINFO record and add to linked list
				parNext->pNext = (ADDROUTEINFO*) malloc(sizeof(ADDROUTEINFO));
				if (!parNext->pNext)
					{
					printf(APPNAME ": Unable to allocate memory.\n");
					return 1;
					}
				parNext = parNext->pNext;
				memset(parNext, 0, sizeof(ADDROUTEINFO));
				car++;

				// Set Dest IP Address
				strncpy(parNext->szDestIpAddress, *argv, sizeof(parNext->szDestIpAddress)-1);
				parNext->szDestIpAddress[sizeof(parNext->szDestIpAddress)-1] = 0;

				// Add mask if specified
				if (*(argv+1) && strnicmp("MASK", *(argv+1), 4) == 0)
					{
					argv+=2;
					if (*argv == 0)
						{
						printf(APPNAME ": Mask not specified after MASK specifier.\n");
						PrintUsage();
						return 1;
						}
					strncpy(parNext->szDestIpMask, *argv, sizeof(parNext->szDestIpMask)-1);
					parNext->szDestIpMask[sizeof(parNext->szDestIpMask)-1] = 0;

					// For unregistered version, check that mask starts with "255.255.255."
					if (!flRegistered && strncmp("255.255.255.", parNext->szDestIpMask, 12) != 0)
						{
						printf(APPNAME ": In the unlicensed version, the mask must begin with\n"
							"\"255.255.255.\".  You must purchase " APPNAME " in order to route to a network\nlarger than a class C network.\n");
						return 1;
						}
					}

				// Add metric if specified
				if (*(argv+1) && strnicmp("METRIC", *(argv+1), 6) == 0)
					{
					argv+=2;
					if (*argv == 0)
						{
						printf(APPNAME ": Metric not specified after METRIC specifier.\n");
						PrintUsage();
						return 1;
						}
					parNext->iMetric = atoi(*argv);
					}
				}
			else if (strcmp("/?", *argv) == 0 || stricmp("/H", *argv) == 0)
				{
				PrintUsage();
				printf("\nCommand completed successfully.\n");
				return 0;
				}
			else
				{
				printf(APPNAME ": Unknown option %s.\n", *argv);
				PrintUsage();
				return 1;
				}
			}
		else
			{
			/*
			 * A parameter is given that doesn't start with "/"
			 */
			flDial = TRUE;
			if (szEntryName[0] == 0)
				szEntryName = *argv;
			else if (szUserName[0] == 0)
				szUserName = *argv;
			else if (szPassword[0] == 0)
				szPassword = *argv;
			else
				{
				printf(APPNAME ": Unknown argument %s.\n", *argv);
				PrintUsage();
				return 1;
				}
			}
		}

	/*
	 * Enumerate connections
	 */
	RasConn[0].dwSize = sizeof(RasConn[0]);
	//printf("sizeof(RasConn[0]) = %d\n", RasConn[0].dwSize);
	dwRc = RasEnumConnections(RasConn, &cb, &cConnections);
	if (dwRc)
		{
		PrintRASError(dwRc);
		return GetAppReturnCode(dwRc);
		}

	if (flDisconnect)
		{
		/*
		 * Disconnect RAS connection
		 */
		for (iConnection = 0 ; iConnection < cConnections ; iConnection++)
			{
			/*
			 * If no entry is specified, disconnect first connection
			 */
			if (szEntryName[0] == 0 || stricmp(RasConn[iConnection].szEntryName, szEntryName) == 0)
				{
				printf("Disconnecting from %s...", RasConn[iConnection].szEntryName);
				
				dwRc = RasHangUp(RasConn[iConnection].hrasconn);
				if (dwRc)
					{
					return GetAppReturnCode(dwRc);
					}
				RasConnStatus.dwSize = sizeof(RasConnStatus);				
				while ((dwRc = RasGetConnectStatus(RasConn[iConnection].hrasconn, &RasConnStatus)) != ERROR_INVALID_HANDLE)
					Sleep(0);

				printf("\nSuccessfully disconnected from %s.\n", RasConn[iConnection].szEntryName);
				printf("Command completed successfully.\n");
				return 0;
				}
			}
		if (szEntryName[0] == 0)
			printf("No connections\n");
		else
			printf("You are not connected to %s.\n", szEntryName);
		printf("Command completed successfully.\n");
		return 0;
		}
	else if (flQuery)
		{
		/*
		 * Query RAS connection
		 */
		for (iConnection = 0 ; iConnection < cConnections ; iConnection++)
			{
			/*
			 * If no entry is specified, query first connection
			 */
			if (szEntryName[0] == 0 || stricmp(RasConn[iConnection].szEntryName, szEntryName) == 0)
				{
				printf("You are connected to %s.\n", RasConn[iConnection].szEntryName);
				printf("Command completed successfully.\n");
				return 0;
				}
			}
		if (szEntryName[0] == 0)
			printf("No connections\n");
		else
			printf("You are not connected to %s.\n", szEntryName);
		printf("Command completed successfully.\n");
		return 1;
		}
	else if (flDial)
		{
		/*
		 * Dial RAS connection
		 */

		for (iConnection = 0 ; iConnection < cConnections ; iConnection++)
			if (stricmp(RasConn[iConnection].szEntryName, szEntryName) == 0)
				{
				printf("You are already connected to %s.\n", RasConn[iConnection].szEntryName);
				printf("Command completed successfully.\n");
				return 0;
				}

		flRc = SetConsoleCtrlHandler((PHANDLER_ROUTINE) CtrlHandler, TRUE);
		if (!flRc)
			{
			printf(APPNAME ": Warning: Unable to set control handler.\n");
			}

		rasdialparams.dwSize = sizeof(rasdialparams);
		strncpy(rasdialparams.szEntryName, szEntryName, RAS_MaxEntryName);
		dwRc = RasGetEntryDialParams(NULL, &rasdialparams, &fPassword);
		if (dwRc != 0)
			{
			PrintRASError(dwRc);
			return GetAppReturnCode(dwRc);
			}

		#if 0
		RASCREDENTIALS cred = {0};
		cred.dwSize = sizeof(RASCREDENTIALS);
		cred.dwMask = RASCM_UserName | RASCM_Password | RASCM_Domain;
		dwRc = RasGetCredentials(NULL, szEntryName, &cred);
		if (dwRc == 0)
			{
			printf("Username: %s\nPassword(%d): %s\nDomain: %s\n", cred.szUserName, 
				cred.dwMask & RASCM_Password, cred.szPassword, cred.szDomain);
			memcpy(rasdialparams.szUserName, cred.szUserName, sizeof(rasdialparams.szUserName));
			memcpy(rasdialparams.szPassword, cred.szPassword, sizeof(rasdialparams.szPassword));
			memcpy(rasdialparams.szDomain, cred.szDomain, sizeof(rasdialparams.szDomain));
			}
		else
			PrintRASError(dwRc);
		#endif
		
		if (szPhoneNumber[0])
			strncpy(rasdialparams.szPhoneNumber, szPhoneNumber, RAS_MaxPhoneNumber);
		if (szCallbackNumber[0])
			strncpy(rasdialparams.szCallbackNumber, szCallbackNumber, RAS_MaxCallbackNumber);
		if (szUserName[0] || szPassword[0] || szDomain[0])
			{
			strncpy(rasdialparams.szUserName, szUserName, UNLEN);
			strncpy(rasdialparams.szPassword, szPassword, PWLEN);
			strncpy(rasdialparams.szDomain, szDomain, DNLEN);
			}

		if (flVerbose)
			{
			printf("Using the following credentials:\n\tUsername: %s\n\tDomain: %s\n", 
				rasdialparams.szUserName, rasdialparams.szDomain);
			if (flShowPass)
				printf("\tPassword: %s\n", fPassword ? rasdialparams.szPassword : "(not available)");
			}

		for (iRetry = 0 ;;)
			{		
			printf("Connecting to %s...", rasdialparams.szEntryName);

			dwRc = RasDial(NULL, NULL, &rasdialparams, 0, NULL, &hRasConn);
			printf("\n");
			if (dwRc == 0)
				{
				printf("Successfully connected to %s\n", rasdialparams.szEntryName);

				if (flAddRoute)
					{
					RASPPPIP RasPPPIP = {0};
					DWORD cbRasPPPIP = sizeof(RasPPPIP);

					RasPPPIP.dwSize = sizeof(RASPPPIP);
					dwRc = RasGetProjectionInfo(hRasConn, RASP_PppIp, &RasPPPIP, &cbRasPPPIP);
					if (dwRc == 0)
						{
						PROCESS_INFORMATION pi = {0};
						STARTUPINFO si = {0};
						char lpCommandLine[1024] = {0};

						if (flVerbose)
							printf("Obtained the following IP addresses:\n\tClient IP: %s\n\tServer IP: %s\n",
								RasPPPIP.szIpAddress, RasPPPIP.szServerIpAddress);

						//CreateIpForwardEntry();

						// run 'route add' command

						if (flVerbose)
							printf("Adding routes...\n");

						for (parNext = arFirst.pNext ; parNext ; parNext = parNext->pNext)
							{
							sprintf(lpCommandLine, "ROUTE ADD %s", parNext->szDestIpAddress);
							if (parNext->szDestIpMask[0])
								sprintf(lpCommandLine + strlen(lpCommandLine), " MASK %s", parNext->szDestIpMask);
							if (parNext->iMetric > 0)
								sprintf(lpCommandLine + strlen(lpCommandLine), " METRIC %d", parNext->iMetric);
							sprintf(lpCommandLine + strlen(lpCommandLine), " %s", RasPPPIP.szIpAddress);
							
							if (flVerbose)
								printf("\t%s\n", lpCommandLine);
								
							flRc = CreateProcess(NULL, lpCommandLine, NULL, NULL, FALSE, 0, NULL, NULL,
									&si, &pi);
							dwRc = WaitForSingleObject(pi.hProcess, 60000);		// wait 1 minute for route to complete
							if (dwRc)
								PrintWin32Error(dwRc);
							}
						}
					else
						PrintRASError(dwRc);
					}

				printf("Command completed successfully.\n");
				return 0;
				}

			PrintRASError(dwRc);
			if (hRasConn && RasHangUp(hRasConn) == 0)
				{
				RasConnStatus.dwSize = sizeof(RasConnStatus);				
				while (RasGetConnectStatus(hRasConn, &RasConnStatus) != ERROR_INVALID_HANDLE)
					Sleep(0);
				hRasConn = 0;
				}

			iRetry++;
			if (iRetry <= cRetryCount)
				{
				printf("Waiting for %d seconds for retry %d...", iRetryDelay, iRetry);
				Sleep(iRetryDelay * 1000);
				printf("\n");
				}
			else
				{
				/* Dialing failed after specified number of retries */
				return GetAppReturnCode(dwRc);
				}
			}
		}
	else
		{
		/*
		 * Show current connections
		 */
		if (cConnections == 0)
			printf("No connections\n");
		else
			{
			printf("Connected to\n");
			for (iConnection = 0 ; iConnection < cConnections ; iConnection++)
				printf("%s\n", RasConn[iConnection].szEntryName);
			}
		printf("Command completed successfully.\n");
		return 0;
		}

	return 0;
	}
