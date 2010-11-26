#include "stdincludes.h"
#include <pthread.h>
#include "pthread_wq_np.h"
#include <dlfcn.h>
int main(void) {
	void * handle = dlopen("libpthread_wq_np.so", RTLD_LAZY );
	int(*init_wqs)(void);
	if(handle == NULL) {
		printf("%s\n", dlerror());
	}
	init_wqs = dlsym(handle, "pthread_workqueue_init_np");
	init_wqs();
	init_wqs();
	pthread_workqueue_init_np();
	return EXIT_SUCCESS;
}
