#include <ntddk.h>
#include <wdf.h>
#include <string.h>

//Global variables -----------------------------------------------
const wchar_t	*DeviceName = L"\\Device\\MYDRIVER";
const wchar_t	*SymbolicLink = L"\\DosDevices\\MYDRIVER";

UNICODE_STRING	ustrDeviceName;
UNICODE_STRING	ustrSymbolicLink;

PDEVICE_OBJECT	CreatedDeviceObject = NULL;
//----------------------------------------------------------------

//Function prototypes --------------------------------------------
void OnUnload(_In_ PDRIVER_OBJECT DriverObject);

NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT, _In_ PUNICODE_STRING);
void OnUnload(PDRIVER_OBJECT);

int SetupIoDevice(PDRIVER_OBJECT);
NTSTATUS Create_DeviceIo(PDEVICE_OBJECT, PIRP);
NTSTATUS Close_DeviceIo(PDEVICE_OBJECT, PIRP);
NTSTATUS Buffered_Write(PDEVICE_OBJECT, PIRP);
NTSTATUS Io_Unsupported(PDEVICE_OBJECT, PIRP);
//----------------------------------------------------------------

NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath) {
	UNREFERENCED_PARAMETER(RegistryPath);

	DriverObject->DriverUnload = OnUnload;

	unsigned int i;
	for (i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++) {
		DriverObject->MajorFunction[i] = Io_Unsupported;
	}
	DriverObject->MajorFunction[IRP_MJ_CREATE] = Create_DeviceIo;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = Close_DeviceIo;
	DriverObject->MajorFunction[IRP_MJ_WRITE] = Buffered_Write;

	RtlInitUnicodeString(&ustrDeviceName, DeviceName);
	RtlInitUnicodeString(&ustrSymbolicLink, SymbolicLink);
	SetupIoDevice(DriverObject);

	DbgPrint("Driver loaded!");

	return STATUS_SUCCESS;
}

void OnUnload(PDRIVER_OBJECT DriverObject) {
	UNREFERENCED_PARAMETER(DriverObject);

	if (CreatedDeviceObject) {								//Check for NULL pointer
		IoDeleteDevice(CreatedDeviceObject);
		IoDeleteSymbolicLink(&ustrSymbolicLink);
		DbgPrint("I/O device and symbolic link deleted!");
	}

	DbgPrint("Driver unloaded.");
}

/*
Purpose:	Create I/O device
Args:		DriverObject: Driver Object
Return:		0: Failed; 1: Succeed
*/
int SetupIoDevice(PDRIVER_OBJECT DriverObject) {
	DbgPrint("Creating I/O device! name: %ws", ustrDeviceName.Buffer);

	int ret = IoCreateDevice(DriverObject, 0, &ustrDeviceName, FILE_DEVICE_UNKNOWN,
		FILE_DEVICE_SECURE_OPEN, FALSE, &CreatedDeviceObject);
	if (ret != STATUS_SUCCESS) {
		DbgPrint("IoCreateDevice() failed!");
		return 0;
	}
	
	CreatedDeviceObject->Flags |= DO_BUFFERED_IO;
	CreatedDeviceObject->Flags &= (~DO_DEVICE_INITIALIZING);
	DbgPrint("Device created!");

	ret = IoCreateSymbolicLink(&ustrSymbolicLink, &ustrDeviceName);
	if (ret != STATUS_SUCCESS) {
		DbgPrint("IoCreateSymbolicLink() failed!");
		return 0;
	}
	DbgPrint("Symbolic link created!: %ws -> %ws", ustrSymbolicLink.Buffer, ustrDeviceName.Buffer);
	return 1;
}

NTSTATUS Create_DeviceIo(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
	UNREFERENCED_PARAMETER(DeviceObject);
	UNREFERENCED_PARAMETER(Irp);

	DbgPrint("Create_DeviceIo() called!");
	return STATUS_SUCCESS;
}

NTSTATUS Close_DeviceIo(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
	UNREFERENCED_PARAMETER(DeviceObject);
	UNREFERENCED_PARAMETER(Irp);

	DbgPrint("Close_DeviceIo() called!");
	return STATUS_SUCCESS;
}

NTSTATUS Io_Unsupported(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
	UNREFERENCED_PARAMETER(DeviceObject);
	UNREFERENCED_PARAMETER(Irp);

	DbgPrint("Io_Unsupported() called!");
	return STATUS_SUCCESS;
}

NTSTATUS Buffered_Write(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
	UNREFERENCED_PARAMETER(DeviceObject);
	PIO_STACK_LOCATION pIoStack = NULL;
	char *Buffer = NULL;

	DbgPrint("Buffered_Write() called!");

	pIoStack = IoGetCurrentIrpStackLocation(Irp);
	if (pIoStack) {											//Check for NULL pointer
		Buffer = (char*)(Irp->AssociatedIrp.SystemBuffer);

		if (Buffer) {											//Check for NULL pointer
			Buffer[pIoStack->Parameters.Write.Length] = '\0';
			DbgPrint("Message received: size: %u, msg: %s", pIoStack->Parameters.Write.Length, Buffer);
		}
		else {
			DbgPrint("Buffer is a NULL pointer!");
		}
	}
	else {
		DbgPrint("Invalid IRP stack pointer!");
	}

	return STATUS_SUCCESS;
}
