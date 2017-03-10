#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int contador = 0;
pthread_mutex_t cerrojo;

typedef struct thread_params
{
    int threadId;

}thread_params;

void funcParalela(void* params)
{
    thread_params * myparams = (thread_params*)params;

    pthread_mutex_lock(&cerrojo);
    contador++;
    printf("holamundo thread: %d contador: %d\n",myparams->threadId, contador);
    pthread_mutex_unlock(&cerrojo);

    pthread_exit(NULL);

}

int main(int argc, char** argv)
{
    
    pthread_t th[5];
    thread_params* params = (thread_params*)malloc(sizeof(thread_params)*5);
    pthread_mutex_init(&cerrojo,NULL);
    int i;

    for (i=0;i<5;i++)
    {
        params[i].threadId = i;
        pthread_create(&(th[i]), NULL, (void * (*)(void *))&funcParalela, &(params[i]));
    }

    for (i=0;i<5;i++)
    {
        pthread_join(th[i], NULL);
    }

    free(params);
    pthread_mutex_destroy(&cerrojo);

    return 0;
}