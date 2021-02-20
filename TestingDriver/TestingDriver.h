/** @file
  TODO: Brief Description of UEFI Driver TestingDriver
  
  TODO: Detailed Description of UEFI Driver TestingDriver

  TODO: Copyright for UEFI Driver TestingDriver
  
  TODO: License for UEFI Driver TestingDriver

**/

#ifndef __EFI_TESTING_DRIVER_H__
#define __EFI_TESTING_DRIVER_H__

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
#define TESTING_DRIVER_VERSION  0x00000000

//
// Protocol instances
//
extern EFI_DRIVER_BINDING_PROTOCOL  gTestingDriverDriverBinding;
extern EFI_COMPONENT_NAME2_PROTOCOL  gTestingDriverComponentName2;
extern EFI_COMPONENT_NAME_PROTOCOL  gTestingDriverComponentName;

//
// Include files with function prototypes
//
#include "TestingDriverBinding.h"
#include "TestingComponentName.h"

#endif
