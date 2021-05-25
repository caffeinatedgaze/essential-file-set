/** @file
  TODO: Brief Description of UEFI Driver HookingDriver

  TODO: Detailed Description of UEFI Driver HookingDriver

  TODO: Copyright for UEFI Driver HookingDriver

  TODO: License for UEFI Driver HookingDriver

**/

#ifndef __EFI_HOOKING_DRIVER_H__
#define __EFI_HOOKING_DRIVER_H__

#include <Uefi.h>

//
// Libraries
//
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DevicePathLib.h>
#include <Library/DebugLib.h>

//
// UEFI Driver Model Protocols
//
#include <Protocol/DriverBinding.h>
#include <Protocol/ComponentName2.h>
#include <Protocol/ComponentName.h>

//
// Consumed Protocols
//
#include <Protocol/PciIo.h>

//
// Produced Protocols
//

//
// Guids
//

//
// Driver Version
//
#define HOOKING_DRIVER_VERSION  0x00000000

//
// Protocol instances
//
extern EFI_DRIVER_BINDING_PROTOCOL  gHookingDriverDriverBinding;
extern EFI_COMPONENT_NAME2_PROTOCOL  gHookingDriverComponentName2;
extern EFI_COMPONENT_NAME_PROTOCOL  gHookingDriverComponentName;

//
// Include files with function prototypes
//
#include "DriverBinding.h"
#include "ComponentName.h"

#include "Protocol/BlockIo.h"

typedef struct HookingContext {
	// todo: store guids here
	EFI_HANDLE blkIoHandle;
	EFI_BLOCK_READ originalReadPtr;
	EFI_BLOCK_WRITE originalWritePtr;
} HookingContext, *pHookingContext;

EFI_STATUS EFIAPI RetrieveGUID(
	IN EFI_HANDLE				    BlkIoHandle,
	IN EFI_BLOCK_IO_PROTOCOL		*BlkIo,
	IN pHookingContext				context
);

#endif
