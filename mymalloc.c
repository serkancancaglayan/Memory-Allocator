/*
    Serkan Can Caglayan 
    26.03.2020
*/ 

#include "mymalloc.h"
void *mymalloc(size_t size){
    if(size > 1024){
        fprintf(stderr, "no available space\n");
        return NULL;
    }
    static int firstcall = 1;
    //yuvarlama
    size = size - (size % 16) + 16;
    if(firstcall){
        firstcall = 0;
        heap_start = sbrk((sizeof(Block) * 2) + 1024);
        if(heap_start == NULL){
            fprintf(stderr, "sbrk error\n");
            exit(EXIT_FAILURE);
        }
        heap_start->info.size = 1024;
        heap_start->info.isfree = 0;
        free_list = heap_start;
    }
    if(free_list == NULL){
        fprintf(stderr, "no available space\n");
        exit(EXIT_FAILURE);
    }
    if(strategy == bestFit){
        best_fit(size);
    }
    else if(strategy == worstFit){
        worst_fit(size);
    }
    else{
        first_fit(size);
    }

}

void *best_fit(size_t size){
    Block *temp = free_list;
    Block *best = free_list;
    while(temp != NULL){
        if(temp->info.size >= size &&
         temp->info.size <= best->info.size){
            best = temp;
        }
        temp = temp->next;
    }
    //aradigimiz boyutta bos block yok
    // free listte tek blok oldugunda da best == free_list olur  
    if(best == free_list && free_list->next != NULL){  
        fprintf(stderr, "no available space\n");
        return NULL;
    }else{
        remove_from_freelist(best);
        return (void*)best->data;
    }
}

void *worst_fit(size_t size){
    Block *temp = free_list;
    Block *largest = free_list;
    while(temp != NULL){
        //en buyuk blogu buluyoruz
        if(temp->info.size > largest->info.size){
            largest = temp;
        }
        temp = temp->next;
    }
    if(largest->info.size < size){
        fprintf(stderr, "no available space\n");
        return NULL;
    }
    //split fonksiyonunda kalan parcayi free list'e geri ekliyoruz
    remove_from_freelist(largest);
    //en buyuk blogu size'imiza gore split ediyoruz
    Block *b = split(largest, size);
    return (void *)b->data;
}

void *first_fit(size_t size){
    //eger free listte tek blok varsa
    if(free_list->next == NULL){
        remove_from_freelist(free_list);
        Block *b = split(free_list, size);
        return (void*)b->data;
    }
    
    Block *temp = free_list;
    while(temp != NULL){
        if(temp->info.size > size){
            //split fonksiyonunda kalan parcayi free list'e geri ekliyoruz
            remove_from_freelist(temp);
            Block *b = split(temp, size);
            return (void*)b->data;
        }
        temp = temp->next;
    }
    if(temp == NULL){
        fprintf(stderr, "no available space\n");
        return NULL;
    }
    return NULL;
}

//gonderilen blogu free listten cikarir
void remove_from_freelist(Block *p){
    if(free_list == NULL){
        return;
    }
    Block * temp = free_list;
    while(temp != NULL){
        if(temp->next == p){
            temp->next = p->next;
            p->info.isfree = 0;
            return;
        }
        temp = temp->next;
    }
    //gonderilen block zaten free listte yok
    if(temp == NULL){
        fprintf(stderr, "");
        return;
    }
}

//gonderilen blogu free liste ekler
void add_to_freelist(Block *p){
    p->info.isfree = 1;
    if(p == NULL){
        fprintf(stderr, "null pointer sent to freelist");
        return;
    }
    if(free_list == NULL){
        free_list = p;
        p->next = NULL;
    }
    else if(free_list->next == NULL){
        free_list->next = p;
        p->next = NULL;
    }
    else{
        p->next = free_list;
        free_list = p;
    }
}

//gonderilen blogu, gonderilen boyuta gore ikiye ayirir
Block *split(Block *b, size_t size){
    int oldsize = b->info.size;
    b->info.size = size;

    Block *q = ((void*)b + b->info.size + sizeof(Block));
    q->info.size = oldsize - size - sizeof(Block);
    add_to_freelist(q);

    return b;
}

//gonderilen blogu free eder, onceki ve sonraki bloklar bos ise onlarla birlestirir ve
//free liste ekler
void *myfree(void *p){
    Block *b = (Block *)(p - sizeof(Block));
    b->info.isfree = 1;

    //sonraki blok bossa birlestirdik
    Block *next = b->next;
    if(next != NULL && next->info.isfree){
        b->info.size += next->info.size + sizeof(Block);
        next->info.isfree = 0; 
    }

    /*segmentasyon error
    Block *prev = (void *)b - sizeof(Block);
    if(prev !=  NULL && prev->info.isfree){
        prev->info.size += b->info.size + sizeof(Block);
        b->info.isfree = 0;
        b = prev;
        prev->info.isfree = 1;
    }*/
    add_to_freelist(b);
    return (void*)b;
}

int test(int fitStrategy){
    char s[3][10] = {"Best Fit", "Worst Fit", "First Fit"};
    strategy = fitStrategy;
    for(int i = 0; i < 32; i++){
        size_t random = rand() % 128;
        int *x = mymalloc(random);
        if(x == NULL){
            printf("%s stratejisi hatali.\n", s[strategy]);
            return -1;
        }
        //yazma ve okuma
        *x = i;
        printf("%d\n", *x);

        //free testi
        myfree(x);
    }
    printf("%s stratejisi duzgun calisiyor.\n", s[strategy]);
    return 1;
}

int main(){

    int best_f = test(bestFit);
    int first_f = test(firstFit);
    int worst_f = test(worstFit);

    return 0;
}
