
/*
 * BSD 3-Clause License
 * 
 * Copyright (c) 2019, zhanglw (zhanglw366@163.com)
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "qc_file.h"
#include "qc_log.h"


struct __QcFile{
	HANDLE hdl;
};



wchar_t* convertW(const char *input)
{
	int num = MultiByteToWideChar(0, 0, input, -1, NULL, 0);
	wchar_t *wide = malloc(num * sizeof(wchar_t));
	MultiByteToWideChar(0, 0, input, -1, wide, num);
	return wide;
}


QcFile* qc_file_open(const char *pathname, int oflag)
{
	QcFile *File;
	DWORD dwAccess = 0; DWORD dwShard = 0;
	DWORD dwCreatDis = 0; DWORD dwFlagsAndAttrs = 0;
	LPSECURITY_ATTRIBUTES qcSecurity;
	LARGE_INTEGER off;
	HANDLE hTempFile;

	qc_assert(pathname);

	File = malloc(sizeof(struct __QcFile));
	if(NULL == File)
	{
		printf("file hdl malloc failed\n");
		return NULL;
	}

	//dwDesiredAccess 
	if(O_RDONLY & oflag) dwAccess = GENERIC_READ;
	if(O_WRONLY & oflag) dwAccess = GENERIC_WRITE;
	if(O_RDWR   & oflag) dwAccess = (GENERIC_READ|GENERIC_WRITE);

	//dwShareMode
	dwShard = (FILE_SHARE_READ|FILE_SHARE_WRITE);

	//qcSecurityAttributes
	qcSecurity = NULL;

	//dwCreationDisposition
	if(O_CREAT & oflag) dwCreatDis = OPEN_ALWAYS;
	if(O_EXCL  & oflag) dwCreatDis = CREATE_NEW;
	if(O_TRUNC & oflag) dwCreatDis = TRUNCATE_EXISTING;
	if (0 == oflag) dwCreatDis = OPEN_EXISTING;

	//dwFlagsAndAttributes
	dwFlagsAndAttrs = FILE_ATTRIBUTE_NORMAL;

	//hTemplateFile
	hTempFile = NULL;

	LPCWSTR lpfname = convertW(pathname);
	File->hdl = CreateFileW((LPCWSTR)lpfname, dwAccess, dwShard, \
		               qcSecurity, dwCreatDis, dwFlagsAndAttrs, hTempFile );
	free((void*)lpfname);

	if(INVALID_HANDLE_VALUE == File->hdl)
	{
		printf("file(%s) open failed\n", pathname);
		free(File);
		return NULL;
	}

	if(O_APPEND & oflag)
	{
		GetFileSizeEx(File->hdl, &off);   
		SetFilePointerEx(File->hdl,off,0,FILE_BEGIN); 
	}

	return File;
}


int qc_file_close(QcFile *file)
{
	qc_assert(file);

	if(!CloseHandle(file->hdl))
	{
		printf("file close failed\n");
		return -1;
	}

	free(file);
	return 0;
}


size_t qc_file_read(QcFile *file, void *buf, size_t nbytes)
{
	size_t ssz=0;

	qc_assert(file);

	if(!ReadFile(file->hdl, (LPVOID)buf, (DWORD)nbytes, (LPDWORD)&ssz, NULL))
	{
		printf("file read failed\n");
		return -1;
	}

	return ssz;
}


size_t qc_file_write(QcFile *file, const void *buf, size_t nbytes)
{
	size_t ssz=0;

	qc_assert(file);

	if(!WriteFile(file->hdl, (CONST VOID*)buf, (DWORD)nbytes, (LPDWORD)&ssz, NULL))
	{
		printf("file write failed\n");
		return -1;
	}

	return ssz;
}


int qc_file_sync(QcFile *file)
{
	qc_assert(file);

	if(!FlushFileBuffers(file->hdl))
	{
		printf("file sync failed\n");
		return -1;
	}

	return 0;
}


off_t qc_file_seek(QcFile *file, off_t offset, int whence)
{
	LARGE_INTEGER off_2move;
	LARGE_INTEGER off_new;

	qc_assert(file);

	off_2move.QuadPart = offset;

	if(!SetFilePointerEx(file->hdl, off_2move, &off_new, (DWORD)whence))
	{
		printf("file seek(offset=%d, whence=%d) failed\n", offset, whence);
		return -1;
	}

	return off_new.LowPart;
}


off_t qc_file_tell(QcFile *file)
{
	LARGE_INTEGER off2move;
	LARGE_INTEGER off;

	qc_assert(file);

	off2move.QuadPart = 0;

	if(!SetFilePointerEx(file->hdl, off2move, &off, FILE_CURRENT))
	{
		printf("file tell failed\n");
		return -1;
	}

	return (off_t)off.QuadPart;
}


int qc_file_exist(const char *pathname)
{
	qc_assert(pathname);

	if(0 != _access_s(pathname, 0))
	{
		return -1;
	}

	return 0;
}


size_t qc_file_size(const char *pathname)
{
	size_t filesize = -1;
	struct stat statbuff;
	if (stat(pathname, &statbuff) < 0) {
		return filesize;
	}
	else {
		filesize = statbuff.st_size;
	}
	return filesize;
}


int qc_file_remove(const char *pathname)
{
	qc_assert(pathname);

     if (!DeleteFileW(convertW(pathname)))
	 {
		 printf("file(%s) remove failed\n", pathname);
         return -1;
	 }

	 return 0;
}


int qc_file_rename(const char *oldname, const char *newname)
{
	qc_assert(oldname);
	qc_assert(newname);

    if (0 != rename(oldname, newname))
	{
		printf("file rename(%s to %s) failed\n", oldname, newname);
        return -1;
	}

	return 0;
}


int qc_file_truncate(const char *pathname, off_t length)
{
	HANDLE hdl;
	LPCWSTR lpfname = convertW(pathname);

	hdl = CreateFileW((LPCWSTR)lpfname, GENERIC_WRITE, FILE_SHARE_WRITE, \
		NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	free((void*)lpfname);

	if (INVALID_HANDLE_VALUE == hdl)
		return -1;

	DWORD ret = SetFilePointer(hdl, length, 0, FILE_BEGIN);
	if (INVALID_SET_FILE_POINTER == ret) {
		DWORD r = GetLastError();
		CloseHandle(hdl);
		return -1;
	}

	if (!SetEndOfFile(hdl)) {
		DWORD r = GetLastError();
		CloseHandle(hdl);
		return -1;
	}

	CloseHandle(hdl);

	return 0;
}

/*--------------------------------------------------------------------------------------------------*/


int qc_dir_exist(const char *dirname)
{
	qc_assert(dirname);

	if(0 != _access_s(dirname, 0))
	{
		printf("dir(%s) asscess failed\n", dirname);
		return -1;
	}

	return 0;
}


int qc_dir_remove(const char *dirname)
{
	qc_assert(dirname);

	if(!RemoveDirectoryW(convertW(dirname)))
	{
		printf("dir(%s) remove failed\n", dirname);
		return -1;
	}

	return 0;
}


int qc_dir_rename(const char *dirname, const char *newname)
{
	qc_assert(dirname);
	qc_assert(newname);

    if (0 != rename(dirname, newname))
	{
		printf("dir rename(%s to %s) failed\n", dirname, newname);
        return -1;
	}

	return 0;
}


int qc_dir_make(const char *dirname)
{
	qc_assert(dirname);

    if (!CreateDirectoryW(convertW(dirname), NULL))
	{
		printf("dir(%s) make failed\n", dirname);
        return -1;
	}

	return 0;
}


char* qc_dir_getcwd(char *buff, int maxlen)
{
	return _getcwd(buff, maxlen);
}

