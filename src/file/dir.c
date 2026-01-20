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

#include <descent/file.h>

#include <descent/rcode.h>
#include "path.h"
#include "dir.h"

int dir_create(const char *dpath) {
	if (!dpath) return DESCENT_ERROR_NULL;
	
	Path path = {0};

	int result = path_resolve_dpath(&path, dpath);
	if (result) return result;

	return dir_create_path(&path);
}

int dir_ensure(const char *dpath) {
	if (!dpath) return DESCENT_ERROR_NULL;
	
	Path path = {0};

	int result = path_resolve_dpath(&path, dpath);
	if (result) return result;

	return dir_ensure_path(&path);
}

int dir_create_recursive(const char *dpath) {
	if (!dpath) return DESCENT_ERROR_NULL;
	
	Path path = {0};

	int result = path_resolve_dpath(&path, dpath);
	if (result) return result;

	return dir_create_recursive_path(&path);
}

int dir_remove(const char *dpath) {
	if (!dpath) return DESCENT_ERROR_NULL;
	
	Path path = {0};

	int result = path_resolve_dpath(&path, dpath);
	if (result) return result;

	return dir_remove_path(&path);
}

int dir_delete(const char *dpath) {
	if (!dpath) return DESCENT_ERROR_NULL;
	
	Path path = {0};

	int result = path_resolve_dpath(&path, dpath);
	if (result) return result;

	return dir_delete_path(&path);
}

int dir_remove_recursive(const char *dpath) {
	if (!dpath) return DESCENT_ERROR_NULL;
	
	Path path = {0};

	int result = path_resolve_dpath(&path, dpath);
	if (result) return result;

	return dir_remove_recursive_path(&path);
}

int dir_exists(const char *dpath) {
	FobjInfo info = {0};

	if(!fobj_stat(&info, dpath) && info.type == FOBJ_DIR) return 1;

	return 0;
}
