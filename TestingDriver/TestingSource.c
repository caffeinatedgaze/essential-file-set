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


#include <Guid/FileInfo.h>

EFI_STATUS
OpenVolume(
	OUT EFI_FILE_PROTOCOL** Volume
)
{
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* fsProto = NULL;
	EFI_STATUS status;
	*Volume = NULL;

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
EFIAPI
FindWritableFs(
	OUT EFI_FILE_PROTOCOL **WritableFs
)
{
	EFI_HANDLE *HandleBuffer = NULL;
	UINTN      HandleCount;
	UINTN      i;

	// Locate all the simple file system devices in the system
	EFI_STATUS Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiSimpleFileSystemProtocolGuid, NULL, &HandleCount, &HandleBuffer);
	if (!EFI_ERROR(Status)) {
		EFI_FILE_PROTOCOL *Fs = NULL;
		// For each located volume
		for (i = 0; i < HandleCount; i++) {
			EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFs = NULL;
			EFI_FILE_PROTOCOL *File = NULL;

			// Get protocol pointer for current volume
			Status = gBS->HandleProtocol(HandleBuffer[i], &gEfiSimpleFileSystemProtocolGuid, (VOID **)&SimpleFs);
			if (EFI_ERROR(Status)) {
				DEBUG((EFI_D_ERROR, "FindWritableFs: gBS->HandleProtocol[%d] returned %r\n", i, Status));
				continue;
			}

			// Open the volume
			Status = SimpleFs->OpenVolume(SimpleFs, &Fs);
			if (EFI_ERROR(Status)) {
				DEBUG((EFI_D_ERROR, "FindWritableFs: SimpleFs->OpenVolume[%d] returned %r\n", i, Status));
				continue;
			}

			// Try opening a file for writing
			Status = Fs->Open(Fs, &File, L"crsdtest.fil", EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, 0);
			if (EFI_ERROR(Status)) {
				DEBUG((EFI_D_ERROR, "FindWritableFs: Fs->Open[%d] returned %r\n", i, Status));
				continue;
			}

			// Writable FS found
			Fs->Delete(File);
			*WritableFs = Fs;
			Status = EFI_SUCCESS;
			break;
		}
	}

	// Free memory
	if (HandleBuffer) {
		gBS->FreePool(HandleBuffer);
	}

	return Status;
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
		EFI_FILE_MODE_READ,
		0
	);

	Print(L"OpenFile: return status %r\r\n", status);

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

EFI_STATUS
ReadFromFile(
	IN EFI_FILE_PROTOCOL* File
)
{
	UINTN infoBufferSize = 0;
	EFI_FILE_INFO* fileInfo = NULL;

	//  retrieve file info to know it size
	EFI_STATUS status = File->GetInfo(
		File,
		&gEfiFileInfoGuid,
		&infoBufferSize,
		(VOID*)fileInfo
	);

	if (EFI_BUFFER_TOO_SMALL != status)
	{
		return status;
	}

	fileInfo = AllocatePool(infoBufferSize);

	if (NULL == fileInfo)
	{
		status = EFI_OUT_OF_RESOURCES;
		return status;
	}

	//	we need to know file size
	status = File->GetInfo(
		File,
		&gEfiFileInfoGuid,
		&infoBufferSize,
		(VOID*)fileInfo
	);

	if (EFI_ERROR(status))
	{
		goto FINALLY;
	}

	//	we move carriage to the end of the file
	// status = File->SetPosition(
	// 	File,
	// 	fileInfo->FileSize
	// );

	if (EFI_ERROR(status))
	{
		goto FINALLY;
	}

	CHAR16 *buffer = AllocateZeroPool(32 * sizeof(CHAR16));
	UINTN size = 32 * sizeof(CHAR16);

	//	write buffer
	status = File->Read(
		File,
		&size,
		buffer
	);
	Print(L"Data has been read: %s\r\n", buffer);
	//status = File->Write(
	//  File,
	//  &BufferSize,
	//  Buffer
	//);

	if (EFI_ERROR(status))
	{
		goto FINALLY;
	}

	//	flush data
	// status = File->Flush(File);

FINALLY:

	if (NULL != fileInfo)
	{
		FreePool(fileInfo);
	}

	return status;
}

EFI_STATUS
DoesFileExist(
	CHAR16 *FileName
)
/*
	Return EFI_SUCCESS if file exists
*/
{
	EFI_STATUS status = 0;
	EFI_FILE_PROTOCOL *Fs;
	EFI_FILE_PROTOCOL *File = NULL;

	status = FindWritableFs(&Fs);

	if (EFI_ERROR(status)) {
		DEBUG((EFI_D_ERROR, "DoesFileExist: Can't find writable FS\n"));
		return EFI_SUCCESS;
	}

	status = Fs->Open(Fs, &File, FileName, EFI_FILE_MODE_READ, 0);
	if (EFI_ERROR(status)) {
		return status;
	}
	return EFI_SUCCESS;
}


EFI_STATUS
MyReadFile(
	VOID
)
{
	EFI_STATUS status = 0;
	EFI_FILE_PROTOCOL *Fs;
	EFI_FILE_PROTOCOL *File = NULL;
	CHAR16 FileName[] = L"file.txt";

	// Find a writable FS
	status = FindWritableFs(&Fs);

	if (EFI_ERROR(status)) {
		DEBUG((EFI_D_ERROR, "WriteToFile: Can't find writable FS\n"));
		return EFI_SUCCESS;
	}

	status = DoesFileExist(FileName);

	if (EFI_SUCCESS != status) {
		// failure
		Print(L"Failed to find the file: %s, %r\r\b", FileName, status);
		return EFI_SUCCESS;
	}

	// Open an output file
	status = Fs->Open(Fs, &File, FileName, EFI_FILE_MODE_READ, 0);
	if (EFI_ERROR(status)) {
		DEBUG((EFI_D_ERROR, "WriteToFile: Fs->Open of %s returned %r\n", FileName, status));
		return status;
	}

	status = ReadFromFile(File);

	CloseFile(File);

	return status;
}
