#pragma once

#include <Uefi.h>
#include <Protocol/SimpleFileSystem.h>


EFI_STATUS
OpenVolume(
	OUT EFI_FILE_PROTOCOL** Volume
);

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
