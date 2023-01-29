#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>

#define MaxItems 5 // Maximum items a producer can produce or a consumer can consume
#define BufferSize 5 // Size of the buffer

//krijo dy variabla njonen me tregu qe eshte full
//tjetren qe semafori empty
sem_t empty;
sem_t full;
//variablat per me tregu qe hijn/dalin
int in = 0;
int out = 0;
//variabla per me tregu madhesine aktuale te bufferit
//sa ka nto
int buffer[BufferSize];
//
pthread_mutex_t mutex;

//pjesa e prodhuesit qe mundohet me fut te dhena ne buffer dhe
//si te veproj nese bufferi eshte i mbushur
void *producer(void *pno)
{  
    int item;
    //funksioni per me i prodhu random items
    for(int i = 0; i < MaxItems; i++) {
        item = rand(); //cakton numer te rastsishem te artikullit
        //me prite nese semafori i zbrast
        sem_wait(&empty);
        //e kontrollon a eshte "mshel" threadi per me u mbush me items
        pthread_mutex_lock(&mutex);
        //me tregu qe po hyjn items edhe
        //per me tregu qe artikujt po hyjn
        buffer[in] = item;
        printf("Prodhuesi %d: shton artikullin %d ne %d\n", *((int *)pno),buffer[in],in);
        //e rrite madhesine e bufferit per 1 sa here perseritet loopa
        in = (in+1)%BufferSize;
        //e "qel" threadin per me u mbush me items
        pthread_mutex_unlock(&mutex);
        // poston krejt ato itemsme te cilat eshte i mbushur semafori
        sem_post(&full);
    }
}

//
void *consumer(void *cno)
{  
    for(int i = 0; i < MaxItems; i++) {
        //e shikon a eshte i mbushur plote buffer
        sem_wait(&full);
        //e shikon a eshte "i mbshelur"threadi per me transmetu
        //te dhena per me mbush bufferin
        pthread_mutex_lock(&mutex);
        //items hjeken(remove) edhe printohet itemi qe eshte hjek
        int item = buffer[out];
        printf("Consumatori %d: terhek artikullin %d nga %d\n",*((int *)cno),item, out);
        //shkon duke e hjek/qit cdo item nga nje per sa here perseritet loopa
        out = (out+1)%BufferSize;
        //mbyllet bufferi per mos me u transmetu te dhenat ne thread
        pthread_mutex_unlock(&mutex);
        //tregon qe bufferi eshte empty
        sem_post(&empty);
    }
}

int main()
{  

    //e caktojme madhesine sa items ka me i pas secili buffer
    pthread_t pro[5],con[5];
    //i inicializon threadat
    pthread_mutex_init(&mutex, NULL);
    //einicializon dhe cakton qe bufferi per fillim eshte i zbrazet
    sem_init(&empty,0,BufferSize);
    sem_init(&full,0,0);

    //krijon nje array me 5 anetare qe jane items qe do te perdoren
    int a[5] = {1,2,3,4,5}; //Just used for numbering the producer and consumer

    //nje loop qe bon krijimin dhe inkrementimin e anetareve te prodhuesit
    for(int i = 0; i < 5; i++) {
        pthread_create(&pro[i], NULL, (void *)producer, (void *)&a[i]);
    }
   
    //nje loop qe bon krijimin dhe inkrementimin e anetareve te konsumatorit
    for(int i = 0; i < 5; i++) {
        pthread_create(&con[i], NULL, (void *)consumer, (void *)&a[i]);
    }

    //loopa qe i bon bashke vizualisht paraqitjen e
    //thirrjen e funksionit te prodhuesit
    for(int i = 0; i < 5; i++) {
        pthread_join(pro[i], NULL);
    }
   
    //loopa qe i bon bashke vizualisht paraqitjen e
    //thirrjen e funksionit te konsumatorit
    for(int i = 0; i < 5; i++) {
        pthread_join(con[i], NULL);
    }
   
    //inicilizimi per me i shkaterru threadat, edhe komandat empty e full
    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    return 0;
   
}