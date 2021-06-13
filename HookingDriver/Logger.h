#pragma once

#include <Uefi.h>
#include <Protocol/BlockIo.h>
#include <Protocol/SimpleFileSystem.h>
#include <Library/UefiBootServicesTableLib.h>

#include "../queue.h"
#include "Hook.h"


Queue *gLog;

EFI_STATUS DoesFileExist(CHAR16 * FileName);

EFI_STATUS DumpLogToFile(VOID);


EFI_STATUS AppendToLog(IN EFI_BLOCK_IO_PROTOCOL * BlockIo, IN UINT32 MediaId, IN EFI_LBA Lba, IN UINTN BufferSize, IN VOID * Buffer, IN BOOLEAN isRead, IN EFI_HANDLE BlkIoHandle, IN EFI_BLOCK_IO_PROTOCOL * BlkIo, IN pHookingContext context);

EFI_STATUS EFIAPI FindWritableFs(OUT EFI_FILE_PROTOCOL ** WritableFs);

EFI_STATUS WriteDataToFile(IN VOID * Buffer, IN UINTN BufferSize, IN EFI_FILE_PROTOCOL * File);

EFI_STATUS AppPrintBuffer(CHAR16 * Buffer);
