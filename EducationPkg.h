/** @file
  TODO: Brief Description of UEFI Driver EducationPkg
  
  TODO: Detailed Description of UEFI Driver EducationPkg

  TODO: Copyright for UEFI Driver EducationPkg
  
  TODO: License for UEFI Driver EducationPkg

**/

#ifndef __EFI_EDUCATION_PKG_H__
#define __EFI_EDUCATION_PKG_H__

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
#define EDUCATION_PKG_VERSION  0x00000000

//
// Protocol instances
//
extern EFI_DRIVER_BINDING_PROTOCOL  gEducationPkgDriverBinding;
extern EFI_COMPONENT_NAME2_PROTOCOL  gEducationPkgComponentName2;
extern EFI_COMPONENT_NAME_PROTOCOL  gEducationPkgComponentName;

//
// Include files with function prototypes
//
#include "DriverBinding.h"
#include "ComponentName.h"

#endif
