/* Global variables */

#include "csapp.h"

int writecnt; /* Initially = 0 */
sem_t mutex, r; /* Both initially = 1 */
void write(void)
{
    while (1) {
        P(&mutex);
        writecnt++;
        if (writecnt == 1) /* First in */
            P(&r);
        V(&mutex);
        /* Critical section */
        /* Reading happens */
        P(&mutex);
        writecnt--;
        if (writecnt == 0) /* Last out */
            V(&r);
        V(&mutex);
    }
}
void read(void)
{
    while (1) {
        if (writecnt>=1) continue;
        P(&r);
        /* Critical section */
        /* Writing happens */
        V(&r);
    }
}