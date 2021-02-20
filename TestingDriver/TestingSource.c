#include "TestingSource.h"
#include "TestingDriver.h"


EFI_STATUS
OpenVolume(
	OUT EFI_FILE_PROTOCOL** Volume
)
{
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* fsProto = NULL;
	EFI_STATUS status;
	*Volume = NULL;

	// get file system protocol
	status = gBS->LocateProtocol(
		&gEfiSimpleFileSystemProtocolGuid,
		NULL,
		(VOID**)&fsProto
	);

	if (EFI_ERROR(status))
	{
		return status;
	}

	status = fsProto->OpenVolume(
		fsProto,
		Volume
	);

	return status;
}

EFI_STATUS
OpenFile(
	IN  EFI_FILE_PROTOCOL* Volume,
	OUT EFI_FILE_PROTOCOL** File,
	IN  CHAR16* Path
)
{
	EFI_STATUS status;
	*File = NULL;

	//  from root file we open file specified by path
	status = Volume->Open(
		Volume,
		File,
		Path,
		EFI_FILE_MODE_CREATE |
		EFI_FILE_MODE_WRITE |
		EFI_FILE_MODE_READ,
		0
	);

	return status;
}

EFI_STATUS
CloseFile(
	IN EFI_FILE_PROTOCOL* File
)
{
	//  flush unwritten data
	File->Flush(File);
	//  close file
	File->Close(File);

	return EFI_SUCCESS;
}