#include "Logger.h"
#include <Library/DebugLib.h>
#include <Guid/FileInfo.h>
#include <Library/MemoryAllocationLib.h>


EFI_STATUS
AppendToLog(
	IN UINT32 MediaId,
	IN EFI_LBA Lba,
	IN UINTN BufferSize,
	IN VOID *Buffer
)
{
	DEBUG((EFI_D_INFO, "Appending to log ... \r\n"));

	EFI_STATUS status = 0;

	EFI_GUID fsProtocolGuid = SIMPLE_FILE_SYSTEM_PROTOCOL;
	UINTN  handleCount = 0;
	EFI_HANDLE *handles = NULL;

	status = gBS->LocateHandleBuffer(
		ByProtocol,
		&fsProtocolGuid,
		NULL,
		&handleCount,
		&handles
	);

	for (UINT16 i = 0; i < handleCount; ++i) {
		EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* fs = NULL;

		status = gBS->HandleProtocol(handles[i], &fsProtocolGuid, (VOID**)&fs);

		EFI_FILE_PROTOCOL *root = NULL;
		status = fs->OpenVolume(fs, &root);

		if (EFI_ERROR(status)) {
			return status;
		}

		EFI_FILE_PROTOCOL *token = NULL;
		status = root->Open(
			root,
			&token,
			L"AAAMyLog.txt",
			EFI_FILE_MODE_CREATE,
			0
		);

		if (EFI_ERROR(status)) {
			return status;
		}

		token->Close(token);
	}

	// todo: write to file something

	return EFI_SUCCESS;
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

