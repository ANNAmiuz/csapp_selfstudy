#include "csapp.h"
#define M 256
#define N 256
#define THREAD 16
#define ROWS_PER_THREAD M / THREAD

//compare the threaded and sequence matrix multiplication (command: time ./prog)
//sequeceMUL:
// real    0m0.062s
// user    0m0.061s
// sys     0m0.000s

//threadedMUL:
// real    0m0.003s
// user    0m0.010s
// sys     0m0.000s

int M1[M][N];
int M2[N][M];
int M1M2[M][M];

void *thread_mul(void *vargp)
{
    int cur = (int)vargp;
    int i, j, k;
    int m = (cur + 1) * ROWS_PER_THREAD;
    for (i = 0; i < m; ++i)
    {
        for (j = 0; j < m; ++j)
        {
            int sum = 0;
            for (k = 0; k < N; ++k)
            {
                sum+=M1[i][k]*M2[k][j];
            }
            M1M2[i][j]=sum;
        }
    }

}

void sequenceMUL()
{
    for (int i = 0; i < M; ++i)
    {
        for (int j = 0; j < M; ++j)
        {
            int sum = 0;
            for (int k = 0; k < N; ++k)
            {
                sum += M1[i][k] * M2[k][j];
            }
            M1M2[i][j] = sum;
        }
    }
}

void threadedMUL()
{
    pid_t pids[THREAD];
    for (int i = 0; i < THREAD; ++i)
    {
        pthread_create(&pids[i], NULL, thread_mul, (void *)i);
    }
    for (int i = 0; i < THREAD; ++i)
    {
        pthread_join(pids[i], NULL);
    }
    exit(0);
}

int main()
{
    threadedMUL();
    exit(0);
}