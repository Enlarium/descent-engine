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

int tf(void *arg) {
	(void) arg;
	debug_log(thread_get_name().name, "[%016llX] hello", thread_self());
	return 0;
}

int main (void) {
	debug_log_open("stdout");
	debug_log(thread_get_name().name, "[%016llX] hello", thread_self());

	for (int i = 0; i < 2; ++i) {
		int tcount = 1; //thread_max_concurrent();

		int e;
		int result;
		Thread t[tcount];

		ThreadAttributes attr = {
			.stack_size = 8 * 1024 * 1024
		};

		for (int j = 0; j < tcount; ++j) {
			e = thread_create(&t[j], tf, NULL, &attr);
			if (e) printf("Could not create new thread (%s)\n", thread_error_message(e));
		}
		thread_pause();
		for (int j = 0; j < tcount; ++j) {
			e = thread_join(t[j], &result);
			if (e) printf("Could not detach thread (%s)\n", thread_error_message(e));

			e = thread_join(t[j], &result);
			if (e) printf("Could not detach thread (%s)\n", thread_error_message(e));
		}
	}

	return 0;
}
