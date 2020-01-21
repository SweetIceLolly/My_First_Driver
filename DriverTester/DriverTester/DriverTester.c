#include <stdio.h>
#include <Windows.h>

#define SERVICE_NAME "MYDRIVER"

//#define USING_GCC

/*
Purpose:	Create the driver service
Args:		ServiceName: The name of the service
.			ExecutablePath: File path of the driver file
Return:		0:Failed; 1: Succeed
*/
int Create_Service(const char *ServiceName, const char *ExecutablePath) {
	printf("Creating service: %s\n", ServiceName);

	SC_HANDLE sh = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);		//Open a handle to Service Manager
	if (sh == INVALID_HANDLE_VALUE) {
		printf("OpenSCManager() failed!\n");
		return 0;
	}

	SC_HANDLE hService = CreateService(sh, ServiceName, ServiceName,		//Create the driver service
		SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
		ExecutablePath, NULL, NULL, NULL, NULL, NULL);
	CloseServiceHandle(sh);													//Use this instead of CloseHandle()

	if (hService == NULL) {
		printf("CreateService() failed!\n");
		return 0;
	}

	CloseServiceHandle(hService);
	return 1;
}

/*
Purpose:	Start the specified service
Args:		ServiceName: Name of the service
Return:		0: Failed; 1: Succeed
*/
int Start_Service(const char *ServiceName) {
	SC_HANDLE sh = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);		//Open a handle to Service Manager
	if (sh == INVALID_HANDLE_VALUE) {
		printf("OpenSCManager() failed!\n");
		return 0;
	}

	SC_HANDLE hService = OpenService(sh, ServiceName, SERVICE_ALL_ACCESS);	//Get a handle to the driver service
	CloseServiceHandle(sh);
	if (hService == NULL) {
		printf("OpenService() failed!\n");
		return 0;
	}

	if (StartService(hService, 0, NULL) == 0) {
		int err_code = (int)GetLastError();
		printf("StartService() failed: %i\n", err_code);
		return 0;
	}

	CloseServiceHandle(hService);
	return 1;
}

/*
Purpose:	Stop the specified service
Args:		ServiceName: Name of the service
Return:		0: Failed; 1: Succeed
*/
int Stop_Service(const char *ServiceName) {
	SC_HANDLE sh = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);		//Open a handle to Service Manager
	if (sh == INVALID_HANDLE_VALUE) {
		printf("OpenSCManager() failed!\n");
		return 0;
	}

	SC_HANDLE hService = OpenService(sh, ServiceName, SERVICE_ALL_ACCESS);	//Get a handle to the driver service
	CloseServiceHandle(sh);
	if (hService == NULL) {
		printf("OpenService() failed!\n");
		return 0;
	}

	SERVICE_STATUS ss;
	if (ControlService(hService, SERVICE_CONTROL_STOP, &ss) == 0) {
		CloseServiceHandle(hService);
		return 0;
	}

	CloseServiceHandle(hService);
	return 1;
}

/*
Purpose:	Delete the specified service
Args:		ServiceName: Name of the service
Return:		0: Failed; 1: Succeed
*/
int Delete_Service(const char *ServiceName) {
	SC_HANDLE sh = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);		//Open a handle to Service Manager
	if (sh == INVALID_HANDLE_VALUE) {
		printf("OpenSCManager() failed!\n");
		return 0;
	}

	SC_HANDLE hService = OpenService(sh, ServiceName, SERVICE_ALL_ACCESS);	//Get a handle to the driver service
	CloseServiceHandle(sh);
	if (hService == NULL) {
		printf("OpenService() failed!\n");
		return 0;
	}

	if (DeleteService(hService) == 0) {
		CloseServiceHandle(hService);
		return 0;
	}

	CloseServiceHandle(hService);
	return 1;
}

/*
Purpose:	Get an IO handle
Args:		DeviceName: Device name
Return:		The IO handle. -1 if failed
Note:		Remember to close the handle with Close_IO_Handle()
*/
HANDLE Get_IO_Handle(char *DeviceName) {
	printf("Opening: %s\n", DeviceName);
	return (HANDLE)CreateFile(DeviceName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, 0);
}

/*
Purpose:	Close an IO handle
Args:		hIO: The IO handle
*/
void Close_IO_Handle(HANDLE hIO) {
	if (hIO != INVALID_HANDLE_VALUE) {
		CloseHandle(hIO);
	}
}

/*
Purpose:	Write data to an IO handle
Args:		hDevice: The IO handle
.			Buffer: Data to write
.			WriteSize: The length of data to write
Return:		0: failed; 1: succeed
*/
int Write_IO_Handle(HANDLE hDevice, const char *Buffer, const int WriteSize) {
	if (WriteFile(hDevice, Buffer, WriteSize, NULL, NULL) != 0) {
		return 1;
	}
	else {
		return 0;
	}
}

int main(void) {
	char	DriverFile[255] = { 0 };
	char	DeviceName[255] = { 0 };
	char    WriteBuffer[255] = { 0 };
	static HANDLE  hIoHandle = INVALID_HANDLE_VALUE;
	int		Selection;

	printf("File name: ");
#ifndef USING_GCC
	gets_s(DriverFile, 255);
#else
	gets(DriverFile);
#endif

	printf(
		"\n"
		"1: Create_Service\n"
		"2: Start_Service\n"
		"3: Get_IO_Handle\n"
		"4: Write_IO_Handle\n"
		"5: Close_IO_Handle\n"
		"6: Stop_Service\n"
		"7: Delete_Service\n"
		"8: Bye!\n"
		"\n"
		);

	for (;;) {
#ifndef USING_GCC
		scanf_s("%i", &Selection);
#else
		scanf("%i", &Selection);
#endif
		switch (Selection) {
		case 1:
			if (Create_Service(SERVICE_NAME, DriverFile) == 1) {
				printf("Create_Service() succeed!\n");
			}
			else {
				printf("Create_Service() failed!\n");
			}
			break;

		case 2:
			if (Start_Service(SERVICE_NAME) == 1) {
				printf("Start_Service() succeed!\n");
			}
			else {
				printf("Start_Service() failed!\n");
			}
			break;

		case 3:
			printf("Device name: ");
			fflush(stdin);
#ifndef USING_GCC
			gets_s(DeviceName, 255);
#else
			gets(DeviceName);
#endif
			hIoHandle = Get_IO_Handle(DeviceName);			// \\.\MYDRIVER
			if (hIoHandle != INVALID_HANDLE_VALUE) {
				printf("Get_IO_Handle() succeed!\n");
			}
			else {
				printf("Get_IO_Handle() failed: %i\n", GetLastError());
			}
			break;

		case 4:
			printf("Message: ");
			fflush(stdin);
#ifndef USING_GCC
			gets_s(WriteBuffer, 255);
#else
			gets(WriteBuffer);
#endif
			if (Write_IO_Handle(hIoHandle, WriteBuffer, (int)strlen(WriteBuffer)) == 1) {
				printf("Write_IO_Handle() succeed!\n");
			}
			else {
				printf("Write_IO_Handle() failed!\n");
			}
			break;

		case 5:
			Close_IO_Handle(hIoHandle);
			printf("Close_IO_Handle() called!\n");
			break;

		case 6:
			if (Stop_Service(SERVICE_NAME) == 1) {
				printf("Stop_Service() succeed!\n");
			}
			else {
				printf("Stop_Service() failed!\n");
			}
			break;

		case 7:
			if (Delete_Service(SERVICE_NAME) == 1) {
				printf("Delete_Service() succeed!\n");
			}
			else {
				printf("Delete_Service() failed!\n");
			}
			break;

		case 8:
			return 0;

		default:
			printf("No gd ar u!\n");
		}
	}

	return 0;
}