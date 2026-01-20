/* Copyright 2025 XavierHarkonnen9 and Enlarium
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef DESCENT_SOURCE_FILE_FOBJ_H
#define DESCENT_SOURCE_FILE_FOBJ_H

#include <errno.h>

#include <descent/utilities/platform.h>
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#include <sys/stat.h>
#include <time.h>
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <descent/file.h>
#include "path.h"

// TODO: Get permission information

//#if defined(DESCENT_PLATFORM_TYPE_WINDOWS)
//static inline uint64_t filetime_to_unix(const FILETIME ft) {
//	ULARGE_INTEGER ull;
//	ull.LowPart = ft.dwLowDateTime;
//	ull.HighPart = ft.dwHighDateTime;
//
//	return (ull.QuadPart - 116444736000000000ULL) / 10000000ULL;
//}
//#endif

static inline int fobj_stat_path(FobjInfo *info, const Path *path) {
	info->size = 0;
	info->atime = 0;
	info->mtime = 0;
	info->ctime = 0;
	info->type = FOBJ_NONE;

#if defined(DESCENT_PLATFORM_TYPE_POSIX)

	struct stat st;
	if (stat(path->string, &st) != 0) {
		if (errno == ENOENT || errno == ENOTDIR) {
			info->type = FOBJ_NONE;
		}
		else info->type = FOBJ_OTHER;
		return 0;
	}
	
	if (S_ISREG(st.st_mode)) info->type = FOBJ_FILE;
	else if (S_ISDIR(st.st_mode)) info->type = FOBJ_DIR;
	else info->type = FOBJ_OTHER;

	info->atime = st.st_atim.tv_sec;
	info->mtime = st.st_mtim.tv_sec;
	info->ctime = st.st_ctim.tv_sec;

#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)

	struct _stat st;
	if (_wstat(path->string, &st) != 0) {
		if (errno == ENOENT || errno == ENOTDIR) {
			info->type = FOBJ_NONE;
		}
		else info->type = FOBJ_OTHER;
		return 0;
	}

	if (st.st_mode & _S_IFREG) info->type = FOBJ_FILE;
	else if (st.st_mode & _S_IFDIR) info->type = FOBJ_DIR;
	else info->type = FOBJ_OTHER;

	info->atime = st.st_atime;
	info->mtime = st.st_mtime;
	info->ctime = st.st_ctime;

#endif

	info->size  = st.st_size;

	return 0;
}

#endif