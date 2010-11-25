#include "stdincludes.h"
#include <pthread.h>
#include "pthread_wq_np.h"

int main(void) {
	pthread_workqueue_init_np();
	return EXIT_SUCCESS;
}
