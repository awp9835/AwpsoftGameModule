#include "FileResourceManager.h"

FileResourceParameters::FileResourceParameters()
{
	Buffer = NULL;
	Index = 0xFFFFFFFF;
	Type = 0x00000000;
	P1 = P2 = 0.0f;
	Size = 0;
}
FileResourceManager::FileResourceManager(UINT32 MaxFileCount)
{
	ArrayLength = MaxFileCount;
	try
	{
		FileList = new FileResourceParameters[MaxFileCount];
	}
	catch (std::bad_alloc &)
	{
		MessageBox(NULL, L"Not Enough Memory!", L"Fatal Error!", MB_ICONERROR);
		exit(0);
	}
}

FileResourceManager::~FileResourceManager()
{
	if (FileList)
	{
		ReleaseAllFileResource();
		delete[] FileList;
	}
	FileList = NULL;
}

BOOL FileResourceManager::TakeOverFileResource(FileResourceParameters FRI)
{
	if (FRI.Index >= ArrayLength) return FALSE;
	if (FileList[FRI.Index].Buffer) 
	{
		delete FileList[FRI.Index].Buffer;
		FileList[FRI.Index].Buffer = NULL;
	}
	FileList[FRI.Index] = FRI;
	return TRUE;
}

FileResourceParameters FileResourceManager::GetFileResourceParameters(UINT32 FileID)
{
	if (FileID >= ArrayLength)
	{
		FileResourceParameters temp;
		memset(&temp, 0, sizeof(FileResourceParameters));
		return temp;
	}
	else
	{
		return FileList[FileID];
	}
	
}

FileResourceParameters FileResourceManager::TakeOutFileBuffer(UINT32 FileID)
{
	FileResourceParameters temp;
	if (FileID >= ArrayLength)
	{
		memset(&temp, 0, sizeof(FileResourceParameters));
	}
	else
	{
		temp = FileList[FileID];
		memset(&FileList[FileID], 0, sizeof(FileResourceParameters));
	}
	return temp;
}

UINT32 FileResourceManager::LoadFile(const WCHAR * FileName, UINT32 FileID,DWORD FileType, FLOAT Para1, FLOAT Para2)
{
	if (FileID >= ArrayLength) return 0;
	FILE* fp;
	FileResourceParameters fri;
	fp = _wfopen(FileName, L"rb");
	if (!fp) return 0;
	fseek(fp, 0, SEEK_END);
	LONG fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	try 
	{
		fri.Buffer = new BYTE[fsize];
		fread(fri.Buffer, 1, fsize, fp);
		fclose(fp);
	}
	catch (std::bad_alloc&)
	{
		fclose(fp);
		return 0;
	}
	fri.Index = FileID;
	fri.Size = fsize;
	fri.Type = FileType;
	fri.P1 = Para1;
	fri.P2 = Para2;
	if (TakeOverFileResource(fri))
	{
		return fsize; 
	}
	return 0;
}

UINT32 FileResourceManager::ReleaseFileResource(UINT32 FileID)
{
	if (FileID >= ArrayLength) return 0;
	FileResourceParameters space;
	UINT32 ts=FileList[FileID].Size;
	if (FileList[FileID].Buffer) delete FileList[FileID].Buffer;
	FileList[FileID] = space;
	return ts;
}

UINT64 FileResourceManager::ReleaseAllFileResource()
{
	FileResourceParameters space;
	UINT64 ttSize = 0;
	for (UINT32 i = 0; i < ArrayLength; i++)
	{
		if (FileList[i].Buffer)
		{
			ttSize += FileList[i].Size;
			delete FileList[i].Buffer;
			FileList[i] = space;
		}
	}
	return ttSize;
}

UINT64 FileResourceManager::LoadFilesFromPackage(const WCHAR * PackageFileName)
{
	UINT32 scnt = 0, fcnt = 0;
	INT64 fsize = 0, i = 0, tsize=0;
	FileResourceParameters frp;
	FILE* fp;
	fp = _wfopen(PackageFileName,L"rb");
	if (!fp) return 0;
	fseek(fp, 0, SEEK_END);
	fsize = ftell(fp);
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
		fread(&frp.Index, sizeof(frp.Index), 1, fp);
		fread(&frp.Type, sizeof(frp.Type), 1, fp);
		fread(&frp.P1, sizeof(frp.P1), 1, fp);
		fread(&frp.P2, sizeof(frp.P2), 1, fp);
		fread(&frp.Size, sizeof(frp.Size), 1, fp);
		fsize -= sizeof(frp.Index)+ sizeof(frp.Type)+ sizeof(frp.P1)+ sizeof(frp.P2)+ sizeof(frp.Size);
		if (frp.Index >= ArrayLength)
		{
			fcnt++;
			fsize -= frp.Size;
			fseek(fp, frp.Size, SEEK_CUR);
			continue;
		}
		if (frp.Size > fsize)
		{
			fcnt++;
			break;
		}
		try 
		{
			frp.Buffer = new BYTE[frp.Size];
			fread(frp.Buffer, 1, frp.Size, fp);
			fsize -= frp.Size;
			TakeOverFileResource(frp);
			scnt++;
			tsize += frp.Size;
		}
		catch(std::bad_alloc&)
		{
			fcnt++;
			fsize -= frp.Size;
			fseek(fp,frp.Size,SEEK_CUR);
			continue;
		}

	}
	fclose(fp);
	return (((UINT64)fcnt)<<32)|scnt;
}

UINT64 FileResourceManager::LoadRangeFilesFromPackage(const WCHAR * PackageFileName, UINT32 MinFileID, UINT32 MaxFileID)
{
	UINT32 scnt = 0, fcnt = 0;
	INT64 fsize = 0, i = 0, tsize = 0;
	FileResourceParameters frp;
	FILE* fp;
	fp = _wfopen(PackageFileName, L"rb");
	if (!fp) return 0;
	fseek(fp, 0, SEEK_END);
	fsize = ftell(fp);
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
		fread(&frp.Index, sizeof(frp.Index), 1, fp);
		fread(&frp.Type, sizeof(frp.Type), 1, fp);
		fread(&frp.P1, sizeof(frp.P1), 1, fp);
		fread(&frp.P2, sizeof(frp.P2), 1, fp);
		fread(&frp.Size, sizeof(frp.Size), 1, fp);
		fsize -= sizeof(frp.Index) + sizeof(frp.Type) + sizeof(frp.P1) + sizeof(frp.P2) + sizeof(frp.Size);
		if (frp.Index < MinFileID || frp.Index > MaxFileID)
		{
			fsize -= frp.Size;
			fseek(fp, frp.Size, SEEK_CUR);
			continue;
		}
		if (frp.Index >= ArrayLength)
		{
			fcnt++;
			fsize -= frp.Size;
			fseek(fp, frp.Size, SEEK_CUR);
			continue;
		}
		if (frp.Size > fsize)
		{
			fcnt++;
			break;
		}
		try
		{
			frp.Buffer = new BYTE[frp.Size];
			fread(frp.Buffer, 1, frp.Size, fp);
			fsize -= frp.Size;
			TakeOverFileResource(frp);
			scnt++;
			tsize += frp.Size;
		}
		catch (std::bad_alloc&)
		{
			fcnt++;
			fsize -= frp.Size;
			fseek(fp, frp.Size, SEEK_CUR);
			continue;
		}
	}
	fclose(fp);
	return (((UINT64)fcnt) << 32) | scnt;
}

UINT64 FileResourceManager::LoadSingleFileFromPackage(const WCHAR * PackageFileName, UINT32 FileID)
{
	UINT32 scnt = 0, fcnt = 0;
	INT64 fsize = 0, i = 0, tsize = 0;
	FileResourceParameters frp;
	FILE* fp;
	if (FileID >= ArrayLength) return 0;
	fp = _wfopen(PackageFileName, L"rb");
	if (!fp) return 0;
	fseek(fp, 0, SEEK_END);
	fsize = ftell(fp);
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
		fread(&frp.Index, sizeof(frp.Index), 1, fp);
		fread(&frp.Type, sizeof(frp.Type), 1, fp);
		fread(&frp.P1, sizeof(frp.P1), 1, fp);
		fread(&frp.P2, sizeof(frp.P2), 1, fp);
		fread(&frp.Size, sizeof(frp.Size), 1, fp);
		fsize -= sizeof(frp.Index) + sizeof(frp.Type) + sizeof(frp.P1) + sizeof(frp.P2) + sizeof(frp.Size);
		if (frp.Index != FileID)
		{
			fsize -= frp.Size;
			fseek(fp, frp.Size, SEEK_CUR);
			continue;
		}
		if (frp.Index >= ArrayLength)
		{
			fcnt++;
			fsize -= frp.Size;
			fseek(fp, frp.Size, SEEK_CUR);
			continue;
		}
		if (frp.Size > fsize)
		{
			fcnt++;
			break;
		}
		try
		{
			frp.Buffer = new BYTE[frp.Size];
			fread(frp.Buffer, 1, frp.Size, fp);
			fsize -= frp.Size;
			TakeOverFileResource(frp);
			scnt++;
			tsize += frp.Size;
		}
		catch (std::bad_alloc&)
		{
			fcnt++;
			fsize -= frp.Size;
			fseek(fp, frp.Size, SEEK_CUR);
			continue;
		}
		break;
	}
	fclose(fp);
	return (((UINT64)fcnt) << 32) | scnt;
}

UINT64 FileResourceManager::MakePackageFromCSV(const WCHAR * ManifestFileName, const WCHAR* TargetPackageName)
{
	/*
	Num,,,,
	Id,0xType,FloatPara1,FloatPara2,FileName(CANNOT contain SPACE!)
	......
	*/
	UINT32 scnt = 0, fcnt = 0;
	UINT32 num = 0, id = 0, type = 0;
	FLOAT p1 = 0.0f, p2 = 0.0f;
	WCHAR name[32];
	FILE *mfp,*ffp,*pfp;
	LONG fsize = 0;
	LPBYTE buffer;
	CHAR c[5] = "AGFP";
	mfp = _wfopen(ManifestFileName,L"r");
	if (!mfp) 
	{
		printf("未找到文件清单");
		return 0; 
	}
	pfp = _wfopen(TargetPackageName, L"wb");
	if (!pfp)
	{
		fclose(mfp);
		printf("创建目标文件失败");
		return 0;
	}
	fwrite(c,1,4,pfp);
	fwscanf(mfp,L"%d,,,,",&num);
	printf("共%d个文件\n", num);
	printf("ID\t类型码   \t附属参数1\t附属参数2\t文件名      \t状态      \t文件大小\n");
	for (UINT32  i = 1; i <= num; i++)
	{
		if (-1==fwscanf_s(mfp, L"%d,%x,%f,%f,%s", &id, &type, &p1, &p2, name, 31))
		{
			printf("警告：文件个数填写错误\n");
			break;
		}
		
		ffp = _wfopen(name, L"rb");
		if(!ffp)
		{
			printf("%-6d\t0x%-08X\t%-8f\t%-8f\t%-12S\t文件打开失败\n", id, type, p1, p2, name);
			fcnt++;
			continue;
		}
		fseek(ffp,0,SEEK_END);
		fsize = ftell(ffp);
		fseek(ffp, 0, SEEK_SET);
		try 
		{
			buffer = new BYTE[fsize];
			fwrite(&id,sizeof(id),1,pfp);
			fwrite(&type, sizeof(type), 1, pfp);
			fwrite(&p1, sizeof(p1), 1, pfp);
			fwrite(&p2, sizeof(p2), 1, pfp);
			fwrite(&fsize, sizeof(fsize), 1, pfp);
			fread(buffer, 1, fsize, ffp);
			fwrite(buffer, 1, fsize, pfp);
			fclose(ffp);
			if (fsize / 1024 >= 1024) 
			{
				printf("%-6d\t0x%-08X\t%-8f\t%-8f\t%-12S\t打包成功\t%-6.2fMB\n", id, type, p1, p2, name, fsize / 1024.0/1024.0);
			}
			else 
			{
				printf("%-6d\t0x%-08X\t%-8f\t%-8f\t%-12S\t打包成功\t%-6.2fKB\n", id, type, p1, p2, name, fsize / 1024.0);
			}
			scnt++;
			delete buffer;
		}
		catch (std::bad_alloc&)
		{
			fclose(ffp);
			printf("系统内存不足，操作终止！\n");
			fcnt = (UINT32)(num - scnt);
			break; 
		}
	}
	fclose(mfp);
	fclose(pfp);	
	printf("文件打包完成，成功%d个，失败%d个\n",scnt,fcnt);
	return (((UINT64)fcnt) << 32) | scnt;
}
