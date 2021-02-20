#include "HookingDriver.h"
#include "Source.h"


EFI_STATUS(*origAddress)(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*, CHAR16*) = 0;
EFI_BLOCK_READ ReadBlocksOrigAddress;
EFI_BLOCK_WRITE WriteBlocksOrigAddress;


EFI_STATUS
EFIAPI
ReadBlocksRandomStaff(
	IN EFI_BLOCK_IO_PROTOCOL* This,
	IN UINT32                 MediaId,
	IN EFI_LBA                Lba,
	IN UINTN                  BufferSize,
	OUT VOID*                 Buffer
)
{

	ReadBlocksOrigAddress(This, MediaId, Lba, BufferSize, Buffer);

	return 0;
}

EFI_STATUS
EFIAPI
WriteBlocksRandomStaff(
	IN EFI_BLOCK_IO_PROTOCOL *This,
	IN UINT32                 MediaId,
	IN EFI_LBA                Lba,
	IN UINTN                  BufferSize,
	IN VOID                   *Buffer
)
{
	//CHAR16* MyString = L"Write override\r\n";

	//if (0 != StrCmp(Buffer, MyString)) 
	//	gST->ConOut->OutputString(gST->ConOut, MyString);

	return WriteBlocksOrigAddress(This, MediaId, Lba, BufferSize, Buffer);
}


EFI_STATUS
EFIAPI
RandomStuff(
	IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL        *This,
	IN CHAR16                                 *String
)
{
	CHAR16 * MyString = L"No way you can print\r\n";
	origAddress(This, MyString);

	return 0;
}


EFI_STATUS
EFIAPI
ReadGpt(
)
{
	EFI_STATUS                  Status;
	EFI_BLOCK_IO_PROTOCOL* BlkIo;
	EFI_HANDLE* BlkIoHandle;
	UINTN                       NoBlkIoHandles;

	//
	// Locate Handles that support BlockIo protocol
	//
	Status = gBS->LocateHandleBuffer(
		ByProtocol,
		&gEfiBlockIoProtocolGuid,
		NULL,
		&NoBlkIoHandles,
		&BlkIoHandle
	);
	if (EFI_ERROR(Status)) {
		return Status;
	}

	for (UINTN Index = 0; Index < NoBlkIoHandles; Index++) {
		Status = gBS->HandleProtocol(
			BlkIoHandle[Index],
			&gEfiBlockIoProtocolGuid,
			(VOID**)&BlkIo
		);

		DEBUG((EFI_D_INFO, "Hooking status, %r \r\n", Status));

		ReadBlocksOrigAddress = BlkIo->ReadBlocks;
		WriteBlocksOrigAddress = BlkIo->WriteBlocks;
		// BlkIo->ReadBlocks = ReadBlocksRandomStaff;
		BlkIo->WriteBlocks = WriteBlocksRandomStaff;
	}

	return Status;
}