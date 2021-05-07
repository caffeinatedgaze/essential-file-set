#include "Logger.h"
#include <Library/DebugLib.h>
#include <Guid/FileInfo.h>
#include <Library/MemoryAllocationLib.h>


EFI_STATUS
AppendToLog(
	IN UINT32 MediaId,
	IN EFI_LBA Lba,
	IN UINTN BufferSize,
	IN VOID *Buffer,
	IN BOOLEAN isRead
)
{
	DEBUG((EFI_D_INFO, "Appending to log ... \r\n"));

	EFI_STATUS status = 0;
	EFI_FILE_PROTOCOL *Fs;
	EFI_FILE_PROTOCOL *File = NULL;
	CHAR16 FileName[] = L"AAAMyLogFile.txt"; // 0-terminated 8.3 file name


	// Find writable FS
	status = FindWritableFs(&Fs);

	if (EFI_ERROR(status)) {
		DEBUG((EFI_D_ERROR, "AppendToLog: Can't find writable FS\n"));
		return EFI_SUCCESS;
	}

	// Open or create output file
	status = Fs->Open(Fs, &File, FileName, EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, 0);
	if (EFI_ERROR(status)) {
		DEBUG((EFI_D_ERROR, "AppendToLog: Fs->Open of %s returned %r\n", FileName, status));
		return status;
	}

	UINT64 message[4] = { 0 }; // (UINT64) Lba, BufferSize, '\n' };

	message[0] = (UINT64)isRead | (UINT64)MediaId << 32;
	message[1] = (UINT64)Lba;
	message[2] = (UINT64)BufferSize;
	message[3] = (UINT64)0x1111111111111111;


	status = WriteDataToFile(
		message,
		sizeof(message),
		File
	);

	if (EFI_ERROR(status)) {
		DEBUG((EFI_D_ERROR, "AppendToLog: WriteDataToFile of %s returned %r\n", message, status));
		return status;
	}

	//  flush unwritten data
	File->Flush(File);
	//  close file
	File->Close(File);

	return EFI_SUCCESS;
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
WriteDataToFile(
	IN VOID* Buffer,
	IN UINTN BufferSize,
	IN EFI_FILE_PROTOCOL* File
)
{
	UINTN infoBufferSize = 0;
	EFI_FILE_INFO* fileInfo = NULL;

	//  retrieve file info to know its size
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

	//    we need to know file size
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

	//    we move carriage to the end of the file
	status = File->SetPosition(
		File,
		fileInfo->FileSize
	);

	if (EFI_ERROR(status))
	{
		goto FINALLY;
	}

	//    write buffer
	status = File->Write(
		File,
		&BufferSize,
		Buffer
	);

	if (EFI_ERROR(status))
	{
		goto FINALLY;
	}

	//    flush data
	status = File->Flush(File);

FINALLY:

	if (NULL != fileInfo)
	{
		FreePool(fileInfo);
	}

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

