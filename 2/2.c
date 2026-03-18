#include <stdio.h>
#include <stdlib.h>
#include<pthread.h>

#define ILE_LICZB 100000

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t warunek = PTHREAD_COND_INITIALIZER;
char czy_parzysty=1; //1 parzysty, 0 nie

struct wezel
{
int number ;
struct wezel * next ;
} * top = NULL ;

void push ( int element )
{
   // pthread_mutex_lock(&mutex);
    struct wezel * p ;
    p = top ;
    top = ( struct wezel *) malloc ( sizeof ( struct wezel ) ) ;
   
    top -> number = element ;
    top -> next = p ;
    //pthread_mutex_unlock(&mutex);
}

int pop ( void )
{
   
    int element = 0;
    struct wezel * p ;
    pthread_mutex_lock(&mutex);
    if ( top != NULL )
    {
        p = top ;
        element = top -> number ;
        top = top -> next ;
        free (p) ;
        }
    pthread_mutex_unlock(&mutex);
    return element;
}

//int zwroc_top(void){
  //  if (top!=NULL){
    //    return top->number;
    //}
    //return NULL;
//}
void* parzyste(void* a){
    for (int i=0;ILE_LICZB>i;i+=2){
        pthread_mutex_lock(&mutex);
        while(!czy_parzysty){
            pthread_cond_wait(&warunek,&mutex);
        }
        push(i);
        czy_parzysty=0;
        pthread_cond_signal(&warunek);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void* nie_parzyste(void *a){
    for (int j=1;ILE_LICZB>=j;j+=2){
    pthread_mutex_lock(&mutex);
        while(czy_parzysty){
            pthread_cond_wait(&warunek,&mutex);
        }
        push(j);
        czy_parzysty=1;
        pthread_cond_signal(&warunek);
       pthread_mutex_unlock(&mutex);
    }
    return NULL;
}


int main(){
    pthread_t watek1, watek2;

    pthread_create(&watek2,NULL,nie_parzyste,NULL);
    pthread_create(&watek1,NULL,parzyste,NULL);
    
    pthread_join(watek1,NULL); //czekamy na koniec wątku
    pthread_join(watek2,NULL); //czekamy na koniec wątku

    //wypisz
    int temp_top;
    int c=0, blad=0;
    for (int i=1; top!=NULL ;i++){
        temp_top=pop();
        if (temp_top!=(ILE_LICZB-i))blad++; 
        printf("%d \n" ,temp_top);
        c++;
    }
    printf("Ile na stosie: %d\n" ,c);
    printf("Liczba błędów: %d\n", blad);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&warunek);

    return 0;
}