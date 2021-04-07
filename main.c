#include <efi.h>
#include <efilib.h>
#include <elf.h>

typedef unsigned long long size_t;

typedef struct
{
	void* BaseAddress;
	size_t BufferSize;
	unsigned int Width;
	unsigned int Height;
	unsigned int PixelsPerScanLine;

} Framebuffer;

#define PSF2_MAGIC0     0x72
#define PSF2_MAGIC1     0xb5
#define PSF2_MAGIC2     0x4a
#define PSF2_MAGIC3     0x86

typedef struct
{
	unsigned char magic[4];
	unsigned int version;
	unsigned int headersize;
	unsigned int flags;
	unsigned int length;
	unsigned int charsize;
	unsigned int height, width;

} PSF2_HEADER;

typedef struct
{
	PSF2_HEADER* header;
	void* glyphBuffer;

} PSF2_FONT;

typedef struct
{
	Framebuffer* framebuffer;
	PSF2_FONT* font;
	EFI_MEMORY_DESCRIPTOR* map;
	UINTN mapSize;
	UINTN descriptorSize;

} BootInfo;

Framebuffer framebuffer;
Framebuffer* InitializeGOP()
{
	EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
	EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
	EFI_STATUS status;

	status = uefi_call_wrapper(BS->LocateProtocol, 3, &gopGuid, NULL, (void**)&gop);

	if (EFI_ERROR(status))
		return NULL;

	framebuffer.BaseAddress = (void*)gop->Mode->FrameBufferBase;
	framebuffer.BufferSize = gop->Mode->FrameBufferSize;
	framebuffer.Width = gop->Mode->Info->HorizontalResolution;
	framebuffer.Height = gop->Mode->Info->VerticalResolution;
	framebuffer.PixelsPerScanLine = gop->Mode->Info->PixelsPerScanLine;

	return &framebuffer;
}

EFI_FILE* LoadFile(EFI_FILE* Directory, CHAR16* Path, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{
	EFI_FILE* LoadedFile;

	EFI_LOADED_IMAGE_PROTOCOL* LoadedImage;
	SystemTable->BootServices->HandleProtocol(ImageHandle, &gEfiLoadedImageProtocolGuid, (void**)&LoadedImage);

	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* FileSystem;
	SystemTable->BootServices->HandleProtocol(LoadedImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (void**)&FileSystem);

	if (Directory == NULL)
	{
		FileSystem->OpenVolume(FileSystem, &Directory);
	}

	EFI_STATUS s = Directory->Open(Directory, &LoadedFile, Path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
	if (s != EFI_SUCCESS)
	{
		return NULL;
	}
	return LoadedFile;
}

PSF2_FONT* LoadPSF2(EFI_FILE* Directory, CHAR16* Path, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{
	EFI_FILE* font = LoadFile(Directory, Path, ImageHandle, SystemTable);

	if (font == NULL)
		return NULL;

	PSF2_HEADER* fontHeader;
	SystemTable->BootServices->AllocatePool(EfiLoaderData, sizeof(PSF2_HEADER), (void**)&fontHeader);
	UINTN size = sizeof(PSF2_HEADER);
	font->Read(font, &size, fontHeader);

	if (fontHeader->magic[0] != PSF2_MAGIC0 || fontHeader->magic[1] != PSF2_MAGIC1 || fontHeader->magic[2] != PSF2_MAGIC2 || fontHeader->magic[3] != PSF2_MAGIC3)
		return NULL;

	UINTN glyphBufferSize = fontHeader->charsize * 256;

	void* glyphBuffer;
	{
		font->SetPosition(font, sizeof(PSF2_HEADER));
		SystemTable->BootServices->AllocatePool(EfiLoaderData, glyphBufferSize, (void**)&glyphBuffer);
		font->Read(font, &glyphBufferSize, glyphBuffer);
	}

	PSF2_FONT* finishedFont;
	SystemTable->BootServices->AllocatePool(EfiLoaderData, sizeof(PSF2_FONT), (void**)&finishedFont);
	finishedFont->header = fontHeader;
	finishedFont->glyphBuffer = glyphBuffer;

	return finishedFont;
}

int memcmp(const void* aptr, const void* bptr, size_t n)
{
	const unsigned char* a = aptr, * b = bptr;
	for (size_t i = 0; i < n; i++)
	{
		if (a[i] < b[i])
			return -1;
		else if (a[i] > b[i])
			return 1;
	}
	return 0;
}

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{
	InitializeLib(ImageHandle, SystemTable);

	EFI_FILE* Kernel = LoadFile(NULL, L"kernel.elf", ImageHandle, SystemTable);

	if (Kernel == NULL)
	{
		Print(L"Could not load kernel\n\r");
		return EFI_SUCCESS;
	}

	Elf64_Ehdr header;
	{
		UINTN FileInfoSize;
		EFI_FILE_INFO* FileInfo;
		Kernel->GetInfo(Kernel, &gEfiFileInfoGuid, &FileInfoSize, NULL);
		SystemTable->BootServices->AllocatePool(EfiLoaderData, FileInfoSize, (void**)&FileInfo);
		Kernel->GetInfo(Kernel, &gEfiFileInfoGuid, &FileInfoSize, (void**)&FileInfo);

		UINTN size = sizeof(header);
		Kernel->Read(Kernel, &size, &header);
	}

	if (
		memcmp(&header.e_ident[EI_MAG0], ELFMAG, SELFMAG) != 0 ||
		header.e_ident[EI_CLASS] != ELFCLASS64 ||
		header.e_ident[EI_DATA] != ELFDATA2LSB ||
		header.e_type != ET_EXEC ||
		header.e_machine != EM_X86_64 ||
		header.e_version != EV_CURRENT)
	{
		Print(L"Kernel format is bad\n\r");
		return EFI_SUCCESS;
	}

	Elf64_Phdr* phdrs;
	{
		Kernel->SetPosition(Kernel, header.e_phoff);
		UINTN size = header.e_phnum * header.e_phentsize;
		SystemTable->BootServices->AllocatePool(EfiLoaderData, size, (void**)&phdrs);
		Kernel->Read(Kernel, &size, phdrs);
	}

	for (
		Elf64_Phdr* phdr = phdrs;
		(char*)phdr < (char*)phdrs + header.e_phnum * header.e_phentsize;
		phdr = (Elf64_Phdr*)((char*)phdr + header.e_phentsize))
	{
		switch (phdr->p_type)
		{
		case PT_LOAD:
		{
			int pages = (phdr->p_memsz + 0x1000 - 1) / 0x1000;
			Elf64_Addr segment = phdr->p_paddr;
			SystemTable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, pages, &segment);

			Kernel->SetPosition(Kernel, phdr->p_offset);
			UINTN size = phdr->p_filesz;
			Kernel->Read(Kernel, &size, (void*)segment);
			break;
		}
		}
	}

	Print(L"Kernel Loaded\n\r");

	PSF2_FONT* font = LoadPSF2(NULL, L"zap-light20.psf", ImageHandle, SystemTable);
	if (font == NULL)
		Print(L"Could not load font\n\r");
	else
		Print(L"Font width: %d\r\nFont height: %d\r\nFont length: %d\r\nFont version: %d\r\n", font->header->width, font->header->height, font->header->length, font->header->version);

	Framebuffer* fb = InitializeGOP();
	if (fb == NULL)
		Print(L"Unable to locate GOP\r\n");
	else
		Print(L"Base: 0x%x\r\nSize: 0x%x\r\nWidth: %d\r\nHeight: %d\r\nPPS: %d\r\n", fb->BaseAddress, fb->BufferSize, fb->Width, fb->Height, fb->PixelsPerScanLine);

	EFI_MEMORY_DESCRIPTOR* Map = NULL;
	UINTN MapSize;
	UINTN MapKey;
	UINTN DescriptorSize;
	UINT32 DescriptorVersion;

	SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
	SystemTable->BootServices->AllocatePool(EfiLoaderData, MapSize, (void**)&Map);
	SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);

	void (*KernelStart)(BootInfo*) = ((__attribute__((sysv_abi)) void (*)(BootInfo*))header.e_entry);

	BootInfo bootInfo;
	bootInfo.framebuffer = fb;
	bootInfo.font = font;
	bootInfo.map = Map;
	bootInfo.mapSize = MapSize;
	bootInfo.descriptorSize = DescriptorSize;

	SystemTable->BootServices->ExitBootServices(ImageHandle, MapKey);

	KernelStart(&bootInfo);

	return EFI_SUCCESS;
}