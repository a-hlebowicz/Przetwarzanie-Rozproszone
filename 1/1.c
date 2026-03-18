#include <stdio.h>
#include <stdlib.h>
#include<pthread.h>

#define ILE_LICZB 100000

struct wezel
{
int number ;
struct wezel * next ;
} * top = NULL ;

void push ( int element )
{
    
    struct wezel * p ;
    p = top ;
    top = ( struct wezel *) malloc ( sizeof ( struct wezel ) ) ;
   
    top -> number = element ;
    top -> next = p ;
   
}

int pop ( void )
{
    int element = 0;
    struct wezel * p ;
    if ( top != NULL )
    {
        p = top ;
        element = top -> number ;
        top = top -> next ;
        free (p) ;
        }
    return element;
}

//int zwroc_top(void){
  //  if (top!=NULL){
    //    return top->number;
    //}
    //return NULL;
//}
void* parzyste(void* a){
    for (int i=2;ILE_LICZB>=i;i+=2)
    push(i);
}

void* nie_parzyste(void *a){
    for (int j=1;ILE_LICZB>=j;j+=2)
    push(j);
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

    return 0;
}