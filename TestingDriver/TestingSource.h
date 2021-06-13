#pragma once

#include <Uefi.h>
#include <Protocol/SimpleFileSystem.h>


EFI_STATUS
MyReadBlocks();

EFI_STATUS
OpenFile(
	IN  EFI_FILE_PROTOCOL* Volume,
	OUT EFI_FILE_PROTOCOL** File,
	IN  CHAR16* Path
);

EFI_STATUS
CloseFile(
	IN EFI_FILE_PROTOCOL* File
);

EFI_STATUS MyReadFile(VOID);
