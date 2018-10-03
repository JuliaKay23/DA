#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "da.h"


struct da {
    void **store;
    int size;
    int capacity;
    void (*display)(void *, FILE *);
    void (*free)(void *);
    int debug;
};

DA *newDA(void) {
    DA *items = malloc(sizeof(DA));
    items->debug = 0;
    items->capacity = 1;
    items->size = 0;
    items->store = malloc(sizeof(void *));
    items->display = NULL;
    items->free = NULL;
    assert(items != 0);
    return items;
}

void  setDAdisplay(DA *items,void (*display)(void *,FILE *)){
    items->display = display;
}

void  setDAfree(DA *items,void (*free)(void *)){
    items->free = free;
}

void growDA(DA *items){
    if (items->size >= items->capacity){
        items->capacity *= 2;
        items->store = realloc(items->store, sizeof(void *) * items->capacity);
    }
}

void  insertDA(DA *items,int index,void *value) {
    assert(index >= 0 && items->size >= index);

    if (items->size >= items->capacity) {
        growDA(items);
    }

    if (index == items->size && items->size < items->capacity) {
        items->size += 1;
        setDA(items, index, value);
    }

    else if (index < items->size && items->size < items->capacity) {
        for (int i = (items->size - 1); i >= index; i--) {
            setDA(items,i+1,getDA(items,i));
        }
        setDA(items,index,value);
    }

    assert(items->store != 0);
}

void *removeDA(DA *items,int index) {
    assert(index >= 0 && index < items->size && items->size > 0);

    void *removedItem = getDA(items,index);

    if (index == (items->size - 1)) {
        items->size -= 1;
    }

    else if (index < items->size && index >= 0) {
        for (int i = index; i <= (items->size - 2); i++) {
            setDA(items,i,getDA(items,i+1));
        }
        items->size -= 1;
    }

    if (items->size < (items->capacity / 4) && (items->capacity / 2) >= 1) {
        items->capacity = items->capacity / 2;
    }

    if (items->size == 0){
        items->capacity = 1;
    }

    return removedItem;
}

void  unionDA(DA *recipient,DA *donor){
    for (int i = 0; i < donor->size; i++){
        insertDA(recipient, recipient->size, getDA(donor,i));
    }
    donor->size = 0;
    donor->capacity = 1;
    donor->store = realloc(donor->store, sizeof(*donor->store) * donor->capacity);
}

void *getDA(DA *items,int index){
    assert(index >= 0 && index <= items->size);
    return items->store[index];
}

void *setDA(DA *items,int index,void *value){
    assert(index >= 0 && index <= items->size);
    if (index == items->size){
        insertDA(items,index,value);
        return 0;
    }
    else {
        items->store[index] = value;
        return items->store[index];
    }
}

int   sizeDA(DA *items){
    return items->size;
}

void  displayDA(DA *items,FILE *fp) {
    if (items->display == NULL){
        if (items->size == 0 && items->debug == 0){
            fprintf(fp,"[]");
        }
        if (items->size == 0 && items->debug > 0){
            int space = items->capacity - items->size;
            fprintf(fp,"[[%d]]",space);
        }
        else if (items->size > 0) {
            fprintf(fp, "[");
            if (items->debug == 0) {
                for (int i = 0; i < (items->size - 1); i++) {
                    fprintf(fp, "@%p,", getDA(items,i));
                }
                fprintf(fp, "@%p", getDA(items,items->size - 1));
            } else if (items->debug > 0) {
                int space = items->capacity - items->size;
                for (int i = 0; i < items->size; i++) {
                    fprintf(fp, "@%p,", getDA(items,i));
                }
                fprintf(fp, "[%d]", space);
            }
            fprintf(fp, "]");
        }
    }

    else {
        if (items->size == 0 && items->debug == 0) {
            fprintf(fp, "[]");
        }
        if (items->size == 0 && items->debug > 0) {
            int space = items->capacity - items->size;
            fprintf(fp, "[[%d]]", space);
        } else if (items->size > 0) {
            fprintf(fp, "[");
            if (items->debug == 0) {
                for (int i = 0; i < (items->size - 1); i++) {
                    items->display(getDA(items,i), fp);
                    fprintf(fp, ",");
                }
                items->display(getDA(items,items->size - 1), fp);
            } else if (items->debug > 0) {
                int space = items->capacity - items->size;
                for (int i = 0; i < items->size; i++) {
                    items->display(getDA(items,i), fp);
                    fprintf(fp, ",");
                }
                fprintf(fp, "[%d]", space);

            }
            fprintf(fp, "]");
        }
    }
}

int   debugDA(DA *items,int level) {
    int oldDebug = items->debug;
    items->debug = level;
    return oldDebug;
}

void  freeDA(DA *items){
    if (items->free == NULL) {
        free(items->store);
    }

    else {
        for (int i = 0; i < items->size; i++) {
            items->free(getDA(items,i));
        }
        free(items->store);


    }
    free(items);
}
