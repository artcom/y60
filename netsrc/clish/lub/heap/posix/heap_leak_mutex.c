#include <pthread.h>

#include "../private.h"
#include "../context.h"

static pthread_mutex_t leak_mutex = PTHREAD_MUTEX_INITIALIZER;

/*--------------------------------------------------------- */
void
lub_heap_leak_mutex_lock(void)
{
    pthread_mutex_lock(&leak_mutex);
}
/*--------------------------------------------------------- */
void
lub_heap_leak_mutex_unlock(void)
{
    pthread_mutex_unlock(&leak_mutex);
}
/*--------------------------------------------------------- */
