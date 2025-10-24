// Copyright 2025 XavierHarkonnen9 and Enlarium
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>

#include <descent/thread.h>
#include <descent/utilities/debug.h>

#include <unistd.h>

// Here's an example using threads

int tf(void*) {
	debug_log(thread_name(), "[%016llX] hello", thread_self());
	for (int i = 0; i < 10; ++i) thread_yield();
	return 0;
}

int main () {
	debug_log_open("stdout");
	debug_log(thread_name(), "[%016llX] hello", thread_self());

	for (int i = 0; i < 2; ++i) {
		int tcount = 1; //thread_max_concurrent();

		int e;
		int result;
		Thread t[tcount];

		ThreadAttributes attr = {
			.name = "newbie",
			.affinity = 1,
			.priority = THREAD_PRIORITY_DEFAULT,
			.stack_size = 8 * 1024 * 1024
		};

		for (int j = 0; j < tcount; ++j) {
			if (e = thread_create_attr(&t[j], tf, NULL, &attr)) printf("Could not create new thread (%s)\n", thread_error(e));
		}
		thread_yield();
		for (int j = 0; j < tcount; ++j) {
			if (e = thread_join(t[j], &result)) printf("Could not detach thread (%s)\n", thread_error(e));
			if (e = thread_join(t[j], &result)) printf("Could not detach thread (%s)\n", thread_error(e));
		}
	}

	return 0;
}