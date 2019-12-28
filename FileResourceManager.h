#pragma once
#include <Windows.h>
#include <new>
#include <cstdio>

#define __FILETYPE_BMP 0x504D422E
#define __FILETYPE_PNG 0x474E502E
#define __FILETYPE_JPG 0x47504A2E
#define __FILETYPE_GIF 0x4649472E
#define __FILETYPE_TXT 0x5458542E
#define __FILETYPE_WAV 0x5641572E
#define __FILETYPE_PCM 0x4D43502E
#define __FILETYPE_DAT 0x5441442E
#define __FILETYPE_INI 0x494E492E
#define __FILETYPE_BIN 0x4E49422E
#define __FILETYPE_CSV 0x5653432E
struct FileResourceParameters
{
	LPBYTE Buffer;
	UINT32 Index,Type;
	FLOAT P1, P2;
	UINT32 Size;
	FileResourceParameters();
};
class FileResourceManager
{
private:
	FileResourceParameters* FileList;
	UINT32 ArrayLength;
public:
	FileResourceManager(UINT32 MaxFileCount = 0x1000);
	virtual ~FileResourceManager();
	BOOL TakeOverFileResource(FileResourceParameters FRI);
	FileResourceParameters GetFileResourceParameters(UINT32 FileID);
	UINT32 LoadFile(const WCHAR* FileName, UINT32 FileID, DWORD FileType,FLOAT Para1=0.0f,FLOAT Para2=0.0f);//return fsize,failed:0
	UINT32 ReleaseFileResource(UINT32 FileID);
	UINT64 ReleaseAllFileResource();
	UINT64 LoadFilesFromPackage(const WCHAR* PackageFileName);// 成功/替换数:Low 32 Bits，失败数:High 32 Bits
	static UINT64 MakePackageFromCSV(const WCHAR* ManifestFileName,const WCHAR* TargetPackageName);//成功数:Low 32 Bits，失败数:High 32 Bits
};

