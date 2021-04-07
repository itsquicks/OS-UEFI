#pragma once
#include "Typedefs.h"
#include "Memory/PageFrameAllocator.h"
#include "kstring.h"
#include "Drivers/ATA.h"
#include "TextMode.h"

#define FLAG_READ_ONLY 0x01
#define FLAG_HIDDEN 0x02
#define FLAG_SYSTEM 0x04
#define FLAG_VOLUME_LABEL 0x08
#define FLAG_FOLDER 0x10
#define FLAG_ARCHIVE 0x20
#define FLAG_DEVICE 0x40
#define FLAG_RESERVED 0x80
#define FLAG_LONG_FILE_NAME 0x0f

struct PartitionTableEntry
{
    uint8 status;
    uint8 startHead;
    uint8 startSector;
    uint8 startCylinder;
    uint8 type;
    uint8 endHead;
    uint8 endSector;
    uint8 endCylinder;
    uint32 startLBA;
    uint32 length;

}__attribute__((packed));

struct MBR
{
    uint8 bootloader[440];
    uint32 signature;
    uint16 unused;
    PartitionTableEntry primaryPartition[4];
    uint16 magicNumber;

}__attribute__((packed));

struct BPB
{
    uint8 jump[3];
    uint8 softName[8];
    uint16 bytesPerSector;
    uint8 sectorsPerCluster;
    uint16 reservedSectors;
    uint8 fatCopies;
    uint16 rootDirEntries;
    uint16 totalSectors;
    uint8 mediaType;
    uint16 fatSectorCount;
    uint16 sectorsPerTrack;
    uint16 headCount;
    uint32 hiddenSectors;
    uint32 totalSectorCount;

    uint32 tableSize;
    uint16 extFlags;
    uint16 fatVersion;
    uint32 rootCluster;
    uint16 fatInfo;
    uint16 backupSector;
    uint8 reserved0[12];
    uint8 driveNumber;
    uint8 reserved;
    uint8 bootSignature;
    uint32 volumeID;
    uint8 volumeLabel[11];
    uint8 fatTypeLabel[8];

}__attribute__((packed));

struct DirectoryEntry
{
    uint8 name[8];
    uint8 ext[3];
    uint8 attributes;
    uint8 reserved;
    uint8 cTimeTenth;
    uint16 cTime;
    uint16 cDate;
    uint16 aTime;
    uint16 firstClusterHigh;
    uint16 wTime;
    uint16 wDate;
    uint16 firstClusterLow;
    uint32 size;

}__attribute__((packed));

extern MBR* mbr;
extern BPB* bpb;

extern uint32 fatStart;
extern uint32 dataStart;
extern uint32 rootStart;
extern uint32 dirStart;
extern char* path;

extern DirectoryEntry* entries;
extern int entriesCount;

MBR* ReadMBR();
BPB* ReadBPB(uint32 partitionOffset);

void InitializeFAT32();
void FAT32_ReadEntries();
void FAT32_WriteEntries();

char* FAT32_GetLFN(uint8 index);
char* FAT32_GetName(DirectoryEntry dir);
DirectoryEntry* FAT32_GetEntry(char* name);
bool FAT32_Get_DOS_Name(char* str, uint8* name, uint8* ext, uint8& reserved);

uint32 FAT32_GetFreeCluster();
void SetFAT(uint32 cluster, uint32 value);

int FAT32_ChangeDirectory(char* name);
void FAT32_RemoveDirectory(DirectoryEntry dir);
void FAT32_Remove(DirectoryEntry file, bool withContent = true);
void FAT32_Create(char* name, uint8 attributes);
void FAT32_Rename(DirectoryEntry file, char* newName);
void FAT32_Load(DirectoryEntry file, uint64 address);
void FAT32_Save(DirectoryEntry file, uint64 address, uint32 newSize);

void PrintCreationTime(DirectoryEntry dir, uint8 color);
void PrintCreationDate(DirectoryEntry dir, uint8 color);
void PrintDirectoryContent();
void PrintPath();
void PrintMBR(MBR* mbr);
void PrintBPB(BPB* bpb);