#pragma once
#include <Windows.h>
#include <new>
#include <vector>
#include <cstdio>
namespace AwpSoftGameModule
{
	constexpr auto __FILETYPE_BMP = 0x504D422E;
	constexpr auto __FILETYPE_PNG = 0x474E502E;
	constexpr auto __FILETYPE_JPG = 0x47504A2E;
	constexpr auto __FILETYPE_GIF = 0x4649472E;
	constexpr auto __FILETYPE_TXT = 0x5458542E;
	constexpr auto __FILETYPE_WAV = 0x5641572E;
	constexpr auto __FILETYPE_PCM = 0x4D43502E;
	constexpr auto __FILETYPE_DAT = 0x5441442E;
	constexpr auto __FILETYPE_INI = 0x494E492E;
	constexpr auto __FILETYPE_BIN = 0x4E49422E;
	constexpr auto __FILETYPE_CSV = 0x5653432E;
	
	struct FileResourceInfo
	{
		unsigned char* Buffer;
		int FileID, Size;
		unsigned int Type;
		float Param1, Param2;
		FileResourceInfo();
	};

	class FileResourceManager
	{
	protected:
		std::vector<FileResourceInfo> FileList;
	public:
		FileResourceManager(int maxFileCount = 0x1000);
		virtual ~FileResourceManager();
		int takeOverFileResource(FileResourceInfo fileInfo);
		FileResourceInfo getFileResourceInfo(int fileID);
		FileResourceInfo takeOutFileResource(int fileID);
		unsigned int loadFile(const wchar_t* fileName, int fileID, unsigned int fileType, float param1 = 0.0f, float param2 = 0.0f); //return fsize,failed:0
		unsigned int releaseFileResource(int fileID);
		unsigned long long releaseAllFileResource();
		virtual int loadFilesFromPackage(const wchar_t* packageFileName);
		virtual int loadRangeFilesFromPackage(const wchar_t* packageFileName, int minFileID = 0, int maxFileID = 0x7FFFFFFF);
		virtual unsigned int loadSingleFileFromPackage(const wchar_t* packageFileName, int fileID);
		virtual int saveAsPackage(const wchar_t* targetPackageName);
		virtual int makePackageFromCSV(const wchar_t* manifestFileName, const wchar_t* targetPackageName);
		static int makeDefaultPackageFromCSV(const wchar_t* manifestFileName, const wchar_t* targetPackageName);
	};

};