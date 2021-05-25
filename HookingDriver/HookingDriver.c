/** @file
  TODO: Brief Description of UEFI Driver HookingDriver

  TODO: Detailed Description of UEFI Driver HookingDriver

  TODO: Copyright for UEFI Driver HookingDriver

  TODO: License for UEFI Driver HookingDriver

**/

#include "HookingDriver.h"
#include "Source.h"
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/DriverBinding.h>
#include "Partition.h"
#include "Logger.h"
#include "..\hashmap.h"


ht_t *gHashmap; // maps protocol handles to context 

///
/// Driver Binding Protocol instance
///
EFI_DRIVER_BINDING_PROTOCOL gHookingDriverDriverBinding = {
  HookingDriverDriverBindingSupported,
  HookingDriverDriverBindingStart,
  HookingDriverDriverBindingStop,
  HOOKING_DRIVER_VERSION,
  NULL,
  NULL
};

/**
  Unloads an image.

  @param  ImageHandle           Handle that identifies the image to be unloaded.

  @retval EFI_SUCCESS           The image has been unloaded.
  @retval EFI_INVALID_PARAMETER ImageHandle is not a valid image handle.

**/
EFI_STATUS
EFIAPI
HookingDriverUnload(
	IN EFI_HANDLE  ImageHandle
)
{
	EFI_STATUS  Status;
	EFI_HANDLE  *HandleBuffer;
	UINTN       HandleCount;
	UINTN       Index;

	Status = EFI_SUCCESS;
	//
	// Retrieve array of all handles in the handle database
	//
	Status = gBS->LocateHandleBuffer(
		AllHandles,
		NULL,
		NULL,
		&HandleCount,
		&HandleBuffer
	);
	if (EFI_ERROR(Status)) {
		return Status;
	}

	//
	// Disconnect the current driver from handles in the handle database 
	//
	for (Index = 0; Index < HandleCount; Index++) {
		Status = gBS->DisconnectController(HandleBuffer[Index], gImageHandle, NULL);
	}

	//
	// Free the array of handles
	//
	FreePool(HandleBuffer);

	//
	// Uninstall protocols installed in the driver entry point
	// 
	Status = gBS->UninstallMultipleProtocolInterfaces(
		ImageHandle,
		&gEfiDriverBindingProtocolGuid, &gHookingDriverDriverBinding,
		&gEfiComponentNameProtocolGuid, &gHookingDriverComponentName,
		&gEfiComponentName2ProtocolGuid, &gHookingDriverComponentName2,
		NULL
	);
	if (EFI_ERROR(Status)) {
		return Status;
	}

	//
	// Do any additional cleanup that is required for this driver
	//

	return EFI_SUCCESS;
}

/**
  This is the declaration of an EFI image entry point. This entry point is
  the same for UEFI Applications, UEFI OS Loaders, and UEFI Drivers including
  both device drivers and bus drivers.

  @param  ImageHandle           The firmware allocated handle for the UEFI image.
  @param  SystemTable           A pointer to the EFI System Table.

  @retval EFI_SUCCESS           The operation completed successfully.
  @retval Others                An unexpected error occurred.
**/
EFI_STATUS
EFIAPI
HookingDriverDriverEntryPoint(
	IN EFI_HANDLE        ImageHandle,
	IN EFI_SYSTEM_TABLE  *SystemTable
)
{
	EFI_STATUS  Status;

	Status = EFI_SUCCESS;

	//
	// Install UEFI Driver Model protocol(s).
	//
	Status = EfiLibInstallDriverBindingComponentName2(
		ImageHandle,
		SystemTable,
		&gHookingDriverDriverBinding,
		ImageHandle,
		&gHookingDriverComponentName,
		&gHookingDriverComponentName2
	);
	ASSERT_EFI_ERROR(Status);

	DEBUG((EFI_D_INFO, "Loading HookingDriver ... \r\n"));

	// Initialize hashmap
	gHashmap = ht_create();

	return Status;
}

/**
  Tests to see if this driver supports a given controller. If a child device is provided,
  it further tests to see if this driver supports creating a handle for the specified child device.

  This function checks to see if the driver specified by This supports the device specified by
  ControllerHandle. Drivers will typically use the device path attached to
  ControllerHandle and/or the services from the bus I/O abstraction attached to
  ControllerHandle to determine if the driver supports ControllerHandle. This function
  may be called many times during platform initialization. In order to reduce boot times, the tests
  performed by this function must be very small, and take as little time as possible to execute. This
  function must not change the state of any hardware devices, and this function must be aware that the
  device specified by ControllerHandle may already be managed by the same driver or a
  different driver. This function must match its calls to AllocatePages() with FreePages(),
  AllocatePool() with FreePool(), and OpenProtocol() with CloseProtocol().
  Because ControllerHandle may have been previously started by the same driver, if a protocol is
  already in the opened state, then it must not be closed with CloseProtocol(). This is required
  to guarantee the state of ControllerHandle is not modified by this function.

  @param[in]  This                 A pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
  @param[in]  ControllerHandle     The handle of the controller to test. This handle
								   must support a protocol interface that supplies
								   an I/O abstraction to the driver.
  @param[in]  RemainingDevicePath  A pointer to the remaining portion of a device path.  This
								   parameter is ignored by device drivers, and is optional for bus
								   drivers. For bus drivers, if this parameter is not NULL, then
								   the bus driver must determine if the bus controller specified
								   by ControllerHandle and the child controller specified
								   by RemainingDevicePath are both supported by this
								   bus driver.

  @retval EFI_SUCCESS              The device specified by ControllerHandle and
								   RemainingDevicePath is supported by the driver specified by This.
  @retval EFI_ALREADY_STARTED      The device specified by ControllerHandle and
								   RemainingDevicePath is already being managed by the driver
								   specified by This.
  @retval EFI_ACCESS_DENIED        The device specified by ControllerHandle and
								   RemainingDevicePath is already being managed by a different
								   driver or an application that requires exclusive access.
								   Currently not implemented.
  @retval EFI_UNSUPPORTED          The device specified by ControllerHandle and
								   RemainingDevicePath is not supported by the driver specified by This.
**/
EFI_STATUS
EFIAPI
HookingDriverDriverBindingSupported(
	IN EFI_DRIVER_BINDING_PROTOCOL  *This,
	IN EFI_HANDLE                   ControllerHandle,
	IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath OPTIONAL
)
{
	// can i open block io ? try to open and close block io 

	EFI_BLOCK_IO_PROTOCOL *BlkIoProtocol = NULL;

	EFI_STATUS Status = gBS->OpenProtocol(
		ControllerHandle,
		&gEfiBlockIoProtocolGuid,
		(VOID **)&BlkIoProtocol,
		gImageHandle,
		ControllerHandle,
		EFI_OPEN_PROTOCOL_GET_PROTOCOL
	);
	if (EFI_ERROR(Status))
		return EFI_UNSUPPORTED;

	return EFI_SUCCESS;
}


EFI_STATUS(*origAddress)(EFI_BLOCK_READ*, CHAR16*) = NULL;
EFI_BLOCK_READ ReadBlocksOrigAddress;
EFI_BLOCK_WRITE WriteBlocksOrigAddress;


EFI_STATUS
EFIAPI
ReadBlocksRandomStaff(
	IN EFI_BLOCK_IO_PROTOCOL* This,
	IN UINT32                 MediaId,
	IN EFI_LBA                Lba,
	IN UINTN                  BufferSize,
	OUT VOID*                 Buffer
)
{
	DEBUG((EFI_D_INFO, "Jesus, I'm reading blocks random staff! <INSIDE>, poarg list: %x %x %x\r\n", MediaId, Lba, BufferSize));

	pHookingContext context = ht_get(gHashmap, This);

	DEBUG((EFI_D_INFO, "%x %x %x\r\n", context, context->originalReadPtr, ReadBlocksOrigAddress));

	RetrieveGUID(context->blkIoHandle, This, context);

	AppendToLog(This, MediaId, Lba, BufferSize, Buffer, TRUE);
	return ReadBlocksOrigAddress(This, MediaId, Lba, BufferSize, Buffer);
}


EFI_STATUS
EFIAPI
WriteBlocksRandomStaff(
	IN EFI_BLOCK_IO_PROTOCOL *This,
	IN UINT32                 MediaId,
	IN EFI_LBA                Lba,
	IN UINTN                  BufferSize,
	IN VOID                   *Buffer
)
{
	//CHAR16* MyString = L"Write override\r\n";

	//if (0 != StrCmp(Buffer, MyString)) 
	//	gST->ConOut->OutputString(gST->ConOut, MyString);

	DEBUG((EFI_D_INFO, "Jesus, I'm writing blocks random staff! <INSIDE>\r\n"));
	// AppendToLog(MediaId, Lba, BufferSize, Buffer);

	return WriteBlocksOrigAddress(This, MediaId, Lba, BufferSize, Buffer);
}

int main() {
	int things[] = { 0x0001, 0x0002, 0x0003, 0x0004 };
	int other_things[] = { 0x1111, 0x1112, 0x1113, 0x1114 };

	ht_set(gHashmap, &things[0], &other_things[0]);
	ht_set(gHashmap, &things[1], &other_things[1]);
	ht_set(gHashmap, &things[2], &other_things[2]);
	ht_set(gHashmap, &things[3], &other_things[3]);
	ht_set(gHashmap, &things[4], &other_things[4]);

	ht_dump(gHashmap);

	return 0;
}


/**
  Starts a device controller or a bus controller.

  The Start() function is designed to be invoked from the EFI boot service ConnectController().
  As a result, much of the error checking on the parameters to Start() has been moved into this
  common boot service. It is legal to call Start() from other locations, but the following calling restrictions must be followed, or the system behavior will not be deterministic.
  1. ControllerHandle must be a valid EFI_HANDLE.
  2. If RemainingDevicePath is not NULL, then it must be a pointer to a naturally aligned
	 EFI_DEVICE_PATH_PROTOCOL.
  3. Prior to calling Start(), the Supported() function for the driver specified by This must
	 have been called with the same calling parameters, and Supported() must have returned EFI_SUCCESS.

  @param[in]  This                 A pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
  @param[in]  ControllerHandle     The handle of the controller to start. This handle
								   must support a protocol interface that supplies
								   an I/O abstraction to the driver.
  @param[in]  RemainingDevicePath  A pointer to the remaining portion of a device path.  This
								   parameter is ignored by device drivers, and is optional for bus
								   drivers. For a bus driver, if this parameter is NULL, then handles
								   for all the children of Controller are created by this driver.
								   If this parameter is not NULL and the first Device Path Node is
								   not the End of Device Path Node, then only the handle for the
								   child device specified by the first Device Path Node of
								   RemainingDevicePath is created by this driver.
								   If the first Device Path Node of RemainingDevicePath is
								   the End of Device Path Node, no child handle is created by this
								   driver.

  @retval EFI_SUCCESS              The device was started.
  @retval EFI_DEVICE_ERROR         The device could not be started due to a device error.Currently not implemented.
  @retval EFI_OUT_OF_RESOURCES     The request could not be completed due to a lack of resources.
  @retval Others                   The driver failded to start the device.

**/
EFI_STATUS
EFIAPI
HookingDriverDriverBindingStart(
	IN EFI_DRIVER_BINDING_PROTOCOL  *This,
	IN EFI_HANDLE                   ControllerHandle,
	IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath OPTIONAL
)
{
	UINTN	NoBlkIoHandles;
	EFI_HANDLE *BlkIoHandle = NULL;
	EFI_BLOCK_IO_PROTOCOL* BlkIo;

	DEBUG((EFI_D_INFO, "Let's see if I can do that <HERE>\r\n"));
	// main();

	EFI_STATUS Status = gBS->LocateHandleBuffer(
		ByProtocol,
		&gEfiBlockIoProtocolGuid,
		NULL,
		&NoBlkIoHandles,
		&BlkIoHandle
	);

	if (EFI_ERROR(Status))
		return EFI_UNSUPPORTED;

	for (UINTN Index = 0; Index < NoBlkIoHandles; Index++) {
		Status = gBS->HandleProtocol(
			BlkIoHandle[Index],
			&gEfiBlockIoProtocolGuid,
			(VOID**)&BlkIo
		);

		if (EFI_ERROR(Status))
			break;

		if (BlkIo->Media->LogicalPartition)
			continue;

		// PARTITION_PRIVATE_DATA *Private = PARTITION_DEVICE_FROM_BLOCK_IO_THIS (This);
		// DEBUG((EFI_D_INFO, "Incoming signature 1 %x\r\n", BASE_CR(This, PARTITION_PRIVATE_DATA, BlockIo)->Signature));
		// DEBUG((EFI_D_INFO, "Incoming signature 2 %x\r\n", Private->Signature));

		if (ReadBlocksRandomStaff == BlkIo->ReadBlocks)
			continue;

		ReadBlocksOrigAddress = BlkIo->ReadBlocks;
		WriteBlocksOrigAddress = BlkIo->WriteBlocks;

		pHookingContext context = AllocatePool(sizeof(HookingContext));
		context->originalReadPtr = BlkIo->ReadBlocks;
		context->originalWritePtr = BlkIo->WriteBlocks;
		context->blkIoHandle = BlkIoHandle[Index];

		BlkIo->ReadBlocks = ReadBlocksRandomStaff;
		BlkIo->WriteBlocks = WriteBlocksRandomStaff;

		ht_set(gHashmap, BlkIo, context);
		DEBUG((EFI_D_INFO, "Performing right hook <HOOK>\r\n"));
	}

	DEBUG((EFI_D_INFO, "<HASHTABLE DUMP>\r\n"));
    ht_dump(gHashmap);

	if (EFI_ERROR(Status))
		return EFI_UNSUPPORTED;

	return EFI_SUCCESS;
}

/**
  Stops a device controller or a bus controller.

  The Stop() function is designed to be invoked from the EFI boot service DisconnectController().
  As a result, much of the error checking on the parameters to Stop() has been moved
  into this common boot service. It is legal to call Stop() from other locations,
  but the following calling restrictions must be followed, or the system behavior will not be deterministic.
  1. ControllerHandle must be a valid EFI_HANDLE that was used on a previous call to this
	 same driver's Start() function.
  2. The first NumberOfChildren handles of ChildHandleBuffer must all be a valid
	 EFI_HANDLE. In addition, all of these handles must have been created in this driver's
	 Start() function, and the Start() function must have called OpenProtocol() on
	 ControllerHandle with an Attribute of EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER.

  @param[in]  This              A pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
  @param[in]  ControllerHandle  A handle to the device being stopped. The handle must
								support a bus specific I/O protocol for the driver
								to use to stop the device.
  @param[in]  NumberOfChildren  The number of child device handles in ChildHandleBuffer.
  @param[in]  ChildHandleBuffer An array of child handles to be freed. May be NULL
								if NumberOfChildren is 0.

  @retval EFI_SUCCESS           The device was stopped.
  @retval EFI_DEVICE_ERROR      The device could not be stopped due to a device error.

**/
EFI_STATUS
EFIAPI
HookingDriverDriverBindingStop(
	IN EFI_DRIVER_BINDING_PROTOCOL  *This,
	IN EFI_HANDLE                   ControllerHandle,
	IN UINTN                        NumberOfChildren,
	IN EFI_HANDLE                   *ChildHandleBuffer OPTIONAL
)
{
	return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
RetrieveGUID(
	IN EFI_HANDLE				    BlkIoHandle,
	IN EFI_BLOCK_IO_PROTOCOL		*BlkIo,
	IN pHookingContext				context
)
{
	EFI_STATUS                  Status;
	UINT32                      BlockSize;
	EFI_DEVICE_PATH_PROTOCOL    *DevPath;
	CHAR16                      *DevPathString;
	EFI_PARTITION_TABLE_HEADER  *PartHdr;
	MASTER_BOOT_RECORD          *PMBR;

	//
	// Locate Handles that support BlockIo protocol
	//

	if (BlkIo->Media->LogicalPartition) {  // skip if partition
		return 0;
	}
	DevPath = DevicePathFromHandle(BlkIoHandle);
	if (DevPath == NULL) {
		return 0;
	}

	DevPathString = ConvertDevicePathToText(DevPath, TRUE, FALSE);

	BlockSize = BlkIo->Media->BlockSize;
	PartHdr = AllocateZeroPool(BlockSize);
	PMBR = AllocateZeroPool(BlockSize);

	//read LBA0
	Status = context->originalReadPtr(
		BlkIo,
		BlkIo->Media->MediaId,
		(EFI_LBA)0,							//LBA 0, MBR/Protetive MBR
		BlockSize,
		PMBR
	);
	//read LBA1
	Status = context->originalReadPtr(
		BlkIo,
		BlkIo->Media->MediaId,
		(EFI_LBA)1,							//LBA 1, GPT
		BlockSize,
		PartHdr
	);

	// check if GPT
	if (PartHdr->Header.Signature == EFI_PTAB_HEADER_ID) {

		if (PMBR->Signature == MBR_SIGNATURE) {
			DEBUG((EFI_D_INFO, "####^^^&&& Found MBR.\n"));
		}
		DEBUG((EFI_D_INFO, "PartHdr: %x\r\n", PartHdr));
	}
	else if (PMBR->Signature == MBR_SIGNATURE) {
		DEBUG((EFI_D_INFO, "PartHdr: %x\r\n", PartHdr));
	}

	FreePool(PartHdr);
	FreePool(PMBR);
	return Status;
}

