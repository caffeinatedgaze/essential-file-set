#pragma once

#include <Uefi.h>
#include <Protocol/BlockIo.h>
#include <Protocol/SimpleFileSystem.h>
#include <Library/UefiBootServicesTableLib.h>

EFI_STATUS AppendToLog(IN UINT32 MediaId, IN EFI_LBA Lba, IN UINTN BufferSize, IN VOID * Buffer);

EFI_STATUS EFIAPI FindWritableFs(OUT EFI_FILE_PROTOCOL ** WritableFs);

EFI_STATUS WriteDataToFile(IN VOID * Buffer, IN UINTN BufferSize, IN EFI_FILE_PROTOCOL * File);
