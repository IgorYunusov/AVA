#include <stdbool.h>
#include "shared.h"
#include "thread.h"
#include "tls.h"

THREAD_LOCAL bool (*jobdone)(void *job) = 0;
THREAD_LOCAL void*(*jobwork)(void *job, int *len) = 0;
THREAD_LOCAL bool (*jobvibe)(void *job) = 0;
THREAD_LOCAL void (*jobsend)(void *job, void *ptr, int len) = 0;
THREAD_LOCAL void (*jobwait)(void *job) = 0;
THREAD_LOCAL void (*jobdrop)(void *job) = 0;

void jobworker( void *job ) {
    // todo
    // - [ ] triple buffering: uint8_t *buffers[3..NCORES+3] = {0,0,0};
    //       int consuming = 0, processing = 1, reading = 2;

    while( jobvibe(job) && !jobdone(job) ) {
        int len;
        void *ptr = jobwork(job, &len);
        jobsend(job, ptr, len);
        jobwait(job);
    }
    jobsend(job, 0, 0);
    jobdrop(job);
}

