#include "TestingSource.h"
#include "TestingDriver.h"


EFI_STATUS
MyReadBlocks()
{
	EFI_BLOCK_IO_PROTOCOL* blkIo = NULL;
	EFI_STATUS status;

	// get file system protocol
	status = gBS->LocateProtocol(
		&gEfiBlockIoProtocolGuid,
		NULL,
		(VOID**)&blkIo
	);

	if (EFI_ERROR(status))
	{
		return status;
	}

	DEBUG((EFI_D_INFO, "TestSource.c: Reading blocks ... \r\n"));

	status = blkIo->ReadBlocks(
		blkIo,
		0x1337,
		0x1337,
		0x1337,
		NULL
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