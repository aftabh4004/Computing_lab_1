#include "header.h"


void heapify(int i, Heap_dobdiff* heap){
    int smallest = i;
    int lc = 2 * i, rc = 2 * i + 1;


    if(lc <= heap->size && heap->harr[lc]->diff < heap->harr[smallest]->diff)
        smallest = lc;
    
    if(rc <= heap->size && heap->harr[rc]->diff < heap->harr[smallest]->diff)
        smallest = rc;
    

    if(smallest != i){
        Dob_diff* temp = heap->harr[i];
        heap->harr[i] = heap->harr[smallest];
        heap->harr[smallest] = temp;
        heapify(smallest, heap);
    }
    
}

Dob_diff* pop_min(Heap_dobdiff* heap){
    if(heap->size <= 0){
        printf("Pop on empty heap\n");
        return NULL;
    }
    Dob_diff* min = heap->harr[1];
    heap->harr[1] = heap->harr[heap->size];

    heap->harr[heap->size] = min;
    heap->size = heap->size -  1;

    heapify(1, heap); 

    return min;
}