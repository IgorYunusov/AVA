 int cores();
 int pid();
 int tid();
void sleep(float ss, float ms, float us, float ns);
void yield();
bool detach( void (*func)(void *), void *arg );
bool join(int thread_id);
bool thread(int thread_id, void (*func)(void *), void *arg);

