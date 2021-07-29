#include "FileResourceManager.h"
namespace AwpSoftGameModule
{
	FileResourceInfo::FileResourceInfo()
	{
		Buffer = nullptr;
		FileID = -1;
		Type = 0x00000000;
		Size = 0;
		Param1 = Param2 = 0.0f;
	}
	FileResourceManager::FileResourceManager(int MaxFileCount)
	{
		if (MaxFileCount <= 0) return;
		FileList = std::move(std::vector<FileResourceInfo>(MaxFileCount));
	}

	FileResourceManager::~FileResourceManager()
	{
		releaseAllFileResource();
	}

	int FileResourceManager::takeOverFileResource(FileResourceInfo fileInfo)
	{
		if (fileInfo.FileID >= (int)FileList.size() || fileInfo.FileID < 0) return -1;
		if (FileList[fileInfo.FileID].Buffer)
		{
			delete[] FileList[fileInfo.FileID].Buffer;
			FileList[fileInfo.FileID].Buffer = nullptr;
		}
		FileList[fileInfo.FileID] = fileInfo;
		return fileInfo.FileID;
	}

	FileResourceInfo FileResourceManager::getFileResourceInfo(int fileID)
	{
		if (fileID >= (int)FileList.size() || fileID < 0)
		{
			return FileResourceInfo();
		}
		else
		{
			return FileList[fileID];
		}
	}

	FileResourceInfo FileResourceManager::takeOutFileResource(int fileID)
	{
		if (fileID >= (int)FileList.size() || fileID < 0)
		{
			return FileResourceInfo();
		}
		else
		{
			FileResourceInfo temp = FileList[fileID];
			FileList[fileID] = std::move(FileResourceInfo());
			return temp;
		}
	}

	unsigned int FileResourceManager::loadFile(const wchar_t * fileName, int fileID, unsigned int fileType, float param1, float param2)
	{
		if (fileID >= (int)FileList.size() || fileID < 0) return 0;
		FileResourceInfo fri;
		FILE* fp = _wfopen(fileName, L"rb");
		if (!fp) return 0;
		fseek(fp, 0, SEEK_END);
		long fsize = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		try
		{
			fri.Buffer = new unsigned char[fsize];
			fread(fri.Buffer, 1, fsize, fp);
			fclose(fp);
		}
		catch (std::bad_alloc&)
		{
			fclose(fp);
			return 0;
		}
		fri.FileID = fileID;
		fri.Size = fsize;
		fri.Type = fileType;
		fri.Param1 = param1;
		fri.Param2 = param2;
		if (takeOverFileResource(fri) != -1) return fsize;
		else return 0;
	}

	unsigned int FileResourceManager::releaseFileResource(int fileID)
	{
		if (fileID >= (int)FileList.size() || fileID < 0) return 0;
		if (FileList[fileID].Buffer) delete[] FileList[fileID].Buffer;
		unsigned int ts = FileList[fileID].Size;
		FileList[fileID] = std::move(FileResourceInfo());
		return ts;
	}

	unsigned long long FileResourceManager::releaseAllFileResource()
	{
		unsigned long long ttSize = 0;
		for (FileResourceInfo& finfo : FileList)
		{
			if (finfo.Buffer)
			{
				ttSize += finfo.Size;
				delete[] finfo.Buffer;
				finfo = std::move(FileResourceInfo());
			}
		}
		return ttSize;
	}

	int FileResourceManager::loadFilesFromPackage(const wchar_t * packageFileName)
	{
		int scnt = 0;
		FileResourceInfo frp;
		FILE* fp;
		fp = _wfopen(packageFileName, L"rb");
		if (!fp) return 0;
		fseek(fp, 0, SEEK_END);
		long fsize = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		fread(&frp.Type, 4, 1, fp);
		if (frp.Type != 0x50464741)
		{
			fclose(fp);
			return 0;
		}
		fsize -= 4;
		while (fsize > 0)
		{
			fread(&frp.FileID, sizeof(frp.FileID), 1, fp);
			fread(&frp.Type, sizeof(frp.Type), 1, fp);
			fread(&frp.Param1, sizeof(frp.Param1), 1, fp);
			fread(&frp.Param2, sizeof(frp.Param2), 1, fp);
			fread(&frp.Size, sizeof(frp.Size), 1, fp);
			fsize -= sizeof(frp.FileID) + sizeof(frp.Type) + sizeof(frp.Param1) + sizeof(frp.Param2) + sizeof(frp.Size);
			if (frp.FileID >= (int)FileList.size() || frp.FileID < 0)
			{
				fsize -= frp.Size;
				fseek(fp, frp.Size, SEEK_CUR);
				continue;
			}
			if (frp.Size > fsize)
			{
				break;
			}
			try
			{
				frp.Buffer = new unsigned char[frp.Size];
				fread(frp.Buffer, 1, frp.Size, fp);
				fsize -= frp.Size;
				if(takeOverFileResource(frp) >= 0) scnt++;
			}
			catch (std::bad_alloc&)
			{
				fsize -= frp.Size;
				fseek(fp, frp.Size, SEEK_CUR);
				continue;
			}

		}
		fclose(fp);
		return scnt;
	}

	int FileResourceManager::loadRangeFilesFromPackage(const wchar_t * packageFileName, int minFileID, int maxFileID)
	{
		int scnt = 0;
		FileResourceInfo frp;
		FILE* fp = _wfopen(packageFileName, L"rb");
		if (!fp) return 0;
		fseek(fp, 0, SEEK_END);
		long fsize = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		fread(&frp.Type, 4, 1, fp);
		if (frp.Type != 0x50464741)
		{
			fclose(fp);
			return 0;
		}
		fsize -= 4;
		while (fsize > 0)
		{
			fread(&frp.FileID, sizeof(frp.FileID), 1, fp);
			fread(&frp.Type, sizeof(frp.Type), 1, fp);
			fread(&frp.Param1, sizeof(frp.Param1), 1, fp);
			fread(&frp.Param2, sizeof(frp.Param2), 1, fp);
			fread(&frp.Size, sizeof(frp.Size), 1, fp);
			fsize -= sizeof(frp.FileID) + sizeof(frp.Type) + sizeof(frp.Param1) + sizeof(frp.Param2) + sizeof(frp.Size);
			if (frp.FileID < minFileID || frp.FileID > maxFileID || frp.FileID >= (int)FileList.size() || frp.FileID < 0)
			{
				fsize -= frp.Size;
				fseek(fp, frp.Size, SEEK_CUR);
				continue;
			}
			if (frp.Size > fsize)
			{
				break;
			}
			try
			{
				frp.Buffer = new unsigned char[frp.Size];
				fread(frp.Buffer, 1, frp.Size, fp);
				fsize -= frp.Size;
				if(takeOverFileResource(frp) >= 0) scnt++;
			}
			catch (std::bad_alloc&)
			{
				fsize -= frp.Size;
				fseek(fp, frp.Size, SEEK_CUR);
				continue;
			}
		}
		fclose(fp);
		return scnt;
	}

	unsigned int FileResourceManager::loadSingleFileFromPackage(const wchar_t * packageFileName, int fileID)
	{
		if (fileID >= (int)FileList.size() || fileID < 0) return 0;
		FileResourceInfo frp;
		FILE* fp = _wfopen(packageFileName, L"rb");
		if (!fp) return 0;
		fseek(fp, 0, SEEK_END);
		long fsize = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		fread(&frp.Type, 4, 1, fp);
		if (frp.Type != 0x50464741)
		{
			fclose(fp);
			return 0;
		}
		fsize -= 4;		
		unsigned int ssize = 0;
		while (fsize > 0)
		{
			fread(&frp.FileID, sizeof(frp.FileID), 1, fp);
			fread(&frp.Type, sizeof(frp.Type), 1, fp);
			fread(&frp.Param1, sizeof(frp.Param1), 1, fp);
			fread(&frp.Param2, sizeof(frp.Param2), 1, fp);
			fread(&frp.Size, sizeof(frp.Size), 1, fp);
			fsize -= sizeof(frp.FileID) + sizeof(frp.Type) + sizeof(frp.Param1) + sizeof(frp.Param2) + sizeof(frp.Size);
			if (frp.FileID != fileID)
			{
				fsize -= frp.Size;
				fseek(fp, frp.Size, SEEK_CUR);
				continue;
			}
			if (frp.Size > fsize)
			{
				break;
			}
			try
			{
				frp.Buffer = new unsigned char[frp.Size];
				fread(frp.Buffer, 1, frp.Size, fp);
				fsize -= frp.Size;
				if (takeOverFileResource(frp) >= 0) ssize = frp.Size;
			}
			catch (std::bad_alloc&)
			{
				fsize -= frp.Size;
				fseek(fp, frp.Size, SEEK_CUR);
				continue;
			}
			break;
		}
		fclose(fp);
		return ssize;
	}

	int FileResourceManager::saveAsPackage(const wchar_t* targetPackageName)
	{
		int scnt = 0;
		CHAR c[5] = "AGFP";
		FILE* pfp = _wfopen(targetPackageName, L"wb");
		if (!pfp)
		{
			printf("创建目标文件失败");
			return 0;
		}
		fwrite(c, 1, 4, pfp);
		printf("ID\t类型码   \t附属参数1\t附属参数2\t文件名      \t状态      \t文件大小\n");
		for (FileResourceInfo& finfo: FileList)
		{
			if (finfo.FileID < 0) continue;
			if ((int)fwrite(finfo.Buffer, 1, finfo.Size, pfp) != finfo.Size)
			{
				printf("IO异常，写入到Package文件失败。\n");
				break;
			}
			scnt++;
		}
		fclose(pfp);
		printf("文件打包结束，成功%d个。", scnt);
		return scnt;
	}

	int FileResourceManager::makePackageFromCSV(const wchar_t* manifestFileName, const wchar_t* targetPackageName)
	{
		return makeDefaultPackageFromCSV(manifestFileName, targetPackageName);
	}

	int FileResourceManager::makeDefaultPackageFromCSV(const wchar_t * manifestFileName, const wchar_t* targetPackageName)
	{
		/*
		Num,,,,
		Id,0xType,FloatPara1,FloatPara2,FileName(CANNOT contain SPACE and CHINESE...!)
		......
		*/
		int scnt = 0, id = 0, num = 0; 
		unsigned int type = 0;
		float p1 = 0.0f, p2 = 0.0f;
		char name[256];
		FILE *mfp, *ffp, *pfp;
		LONG fsize = 0;
		unsigned char* buffer;
		CHAR c[5] = "AGFP";
		mfp = _wfopen(manifestFileName, L"r");
		if (!mfp)
		{
			printf("未找到文件清单");
			return 0;
		}
		pfp = _wfopen(targetPackageName, L"wb");
		if (!pfp)
		{
			fclose(mfp);
			printf("创建目标文件失败");
			return 0;
		}
		fwrite(c, 1, 4, pfp);
		fwscanf(mfp, L"%d,,,,", &num);
		printf("共%d个文件\n", num);
		printf("ID\t类型码   \t附属参数1\t附属参数2\t文件名      \t状态      \t文件大小\n");
		for (int i = 1; i <= num; i++)
		{
			if (-1 == fscanf_s(mfp, "%d,%x,%f,%f,%s", &id, &type, &p1, &p2, name, 31))
			{
				printf("警告：文件个数填写错误\n");
				break;
			}

			ffp = fopen(name, "rb");
			if (!ffp)
			{
				printf("%-6d\t0x%-08X\t%-8f\t%-8f\t%-12s\t文件打开失败\n", id, type, p1, p2, name);
				continue;
			}
			fseek(ffp, 0, SEEK_END);
			fsize = ftell(ffp);
			fseek(ffp, 0, SEEK_SET);
			try
			{
				buffer = new unsigned char[fsize];
				fwrite(&id, sizeof(id), 1, pfp);
				fwrite(&type, sizeof(type), 1, pfp);
				fwrite(&p1, sizeof(p1), 1, pfp);
				fwrite(&p2, sizeof(p2), 1, pfp);
				fwrite(&fsize, sizeof(fsize), 1, pfp);
				if ((int)fread(buffer, 1, fsize, ffp) != fsize)
				{
					fclose(ffp);
					delete[] buffer;
					printf("错误：文件%s读取异常！\n", name);
					continue;
				}
				fclose(ffp);
				if ((int)fwrite(buffer, 1, fsize, pfp) != fsize)
				{
					delete[] buffer;
					printf("IO异常，写入到Package文件失败！\n");
					break;
				}
				if (fsize / 1024 >= 1024)
				{
					printf("%-6d\t0x%-08X\t%-8f\t%-8f\t%-12s\t打包成功\t%-6.2fMB\n", id, type, p1, p2, name, fsize / 1024.0 / 1024.0);
				}
				else
				{
					printf("%-6d\t0x%-08X\t%-8f\t%-8f\t%-12s\t打包成功\t%-6.2fKB\n", id, type, p1, p2, name, fsize / 1024.0);
				}
				scnt++;
				delete[] buffer;
			}
			catch (std::bad_alloc&)
			{
				fclose(ffp);
				printf("系统内存不足，操作终止！\n");
				break;
			}
		}
		fclose(mfp);
		fclose(pfp);
		printf("文件打包结束，成功%d个。", scnt);
		return scnt;
	}
};