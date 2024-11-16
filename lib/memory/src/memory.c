#include <memory.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include <unistd.h>

void debug_message(const char *message) {
    write(STDOUT_FILENO, message, strlen(message));
}

typedef struct s_block *t_block;
void *base = NULL;
int method = 0;


t_block find_block(t_block *last, size_t size){
    t_block b = base;

    if (method == FIRST_FIT){
        while (b && !(b->free && b->size >= size)){
            *last = b;
            b = b->next;
        }

        return (b);
    } else {
        size_t dif = PAGESIZE;
        t_block best = NULL;
        
        while (b){
            if (b->free){
                if (b->size == size){
                    return b;
                }
                if (b->size > size && (b->size - size) < dif){
                    dif = b->size - size;
                    best = b;
                }
            }
            *last = b;
            b = b->next;
        }
        return best;
    }
}

void split_block(t_block b, size_t s){
    if (b->size <= s + BLOCK_SIZE){
        return;
    }
    
    t_block new;
    new = (t_block)(b->data + s);
    new->size = b->size - s - BLOCK_SIZE;
    new->next = b->next;
    new->free = 1;
    b->size = s;
    b->next = new;
}

void copy_block(t_block src, t_block dst){
    int *sdata, *ddata;
    size_t i;
    sdata = src->ptr;
    ddata = dst->ptr;
    
    for (i = 0; (i * 4) < src->size && (i * 4) < dst->size; i++)
        ddata[i] = sdata[i];
}

t_block get_block(void *p){
    char *tmp;
    tmp = p;
    
    if (tmp >= (char *)base + BLOCK_SIZE){
        tmp -= BLOCK_SIZE;
    }
    return (t_block)(tmp);
}

int valid_addr(void *p){
    if (base){
        if (p > base && p < sbrk(0)){
            t_block b = get_block(p);
            return b && (p == b->ptr);
        }
    }
    return (0);
}

t_block fusion(t_block b){
    if (b->next && b->next->free){
        b->size += BLOCK_SIZE + b->next->size;
        b->next = b->next->next;
        
        if (b->next)
            b->next->prev = b;
    }
    return b;
}

t_block extend_heap(t_block last, size_t s){
    t_block b;
    b = mmap(0, s, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    
    if (b == MAP_FAILED){
        return NULL;
    }
    b->size = s;
    b->next = NULL;
    b->prev = last;
    b->ptr = b->data;
    
    if (last)
        last->next = b;
    
    b->free = 0;
    return b;
}

void get_method(int m){
    method = m;
}

void set_method(int m){
    method = m;
}

void malloc_control(int m){
    if (m == 0) {
        set_method(0);
    } else if (m == 1) {
        set_method(1);
    } else {
        printf("Error: invalid method\n");
    }
}

void *malloc(size_t size){
    t_block b, last;
    size_t s;
    s = align(size);

    if (base){
        last = base;
        b = find_block(&last, s);
        if (b) {
            if ((b->size - s) >= (BLOCK_SIZE + 4))
                split_block(b, s);
            b->free = 0;
        } else {
            b = extend_heap(last, s);
            if (!b)
                return (NULL);
        }
    } else {
        b = extend_heap(NULL, s);
        if (!b)
            return (NULL);
        base = b;
    }
    debug_message("malloc: Asignado\n");
    //printf("malloc: Asignado\n");
    return (b->data);
}

void free(void *ptr){
    t_block b;

    if (valid_addr(ptr)){
        b = get_block(ptr);
        b->free = 1;

        if (b->next && b->next->free)
            fusion(b);
        if (b->prev && b->prev->free)
            fusion(b->prev);
        else{
            if (b->next)
                b->next->prev = b;
            if (b->prev)
                b->prev->next = b;
            else
                base = b;
            b->free = 1;
            b->prev = NULL;
        }
    }
}

void *calloc(size_t number, size_t size){
    size_t *new;
    size_t s4, i;

    if (!number || !size){
        return (NULL);
    }
    new = malloc(number * size);
    if (new){
        s4 = align(number * size) << 2;
        for (i = 0; i < s4; i++)
            new[i] = 0;
    }
    return (new);
}

void *realloc(void *ptr, size_t size){
    size_t s;
    t_block b, new;
    void *newp;

    if (!ptr)
        return (malloc(size));

    if (valid_addr(ptr)){
        s = align(size);
        b = get_block(ptr);

        if (b->size >= s){
            if (b->size - s >= (BLOCK_SIZE + 4))
                split_block(b, s);
        } else {
            if (b->next && b->next->free && (b->size + BLOCK_SIZE + b->next->size) >= s){
                fusion(b);
                if (b->size - s >= (BLOCK_SIZE + 4))
                    split_block(b, s);
            } else {
                newp = malloc(s);
                if (!newp)
                    return (NULL);
                new = get_block(newp);
                copy_block(b, new);
                free(ptr);
                return (newp);
            }
        }
        return (ptr);
    }
    return (NULL);
}


void check_heap(void *data) {
    if (data == NULL) {
        printf("Data is NULL\n");
        return;
    }

    t_block block = get_block(data);

    if (block == NULL) {
        printf("Block is NULL\n");
        return;
    }

    printf("\033[1;33mHeap check\033[0m\n");
    printf("Size: %zu\n", block->size);

    if (block->next != NULL) {
        printf("Next block: %p\n", (void *)(block->next));
    } else {
        printf("Next block: NULL\n");
    }

    if (block->prev != NULL) {
        printf("Prev block: %p\n", (void *)(block->prev));
    } else {
        printf("Prev block: NULL\n");
    }

    printf("Free: %d\n", block->free);

    if (block->ptr != NULL) {
        printf("Beginning data address: %p\n", block->ptr);
        printf("Last data address: %p\n", (void *)((char *)(block->ptr) + block->size));
    } else {
        printf("Data address: NULL\n");
    }

    printf("Heap address: %p\n", sbrk(0));

    // Check for inconsistencies
    t_block current = base;
    while (current) {
        // Check for adjacent free blocks
        if (current->free && current->next && current->next->free) {
            printf("Warning: Adjacent free blocks detected at %p and %p\n", (void *)current, (void *)current->next);
        }

        // Check for invalid block sizes
        if (current->size <= 0) {
            printf("Error: Invalid block size detected at %p\n", (void *)current);
        }

        current = current->next;
    }
}

void memory_usage() {
    size_t total_allocated = 0;
    size_t total_free = 0;

    t_block current = base;
    while (current) {
        if (current->free) {
            total_free += current->size;
        } else {
            total_allocated += current->size;
        }
        current = current->next;
    }

    printf("Memory Usage Report:\n");
    printf("Total allocated memory: %zu bytes\n", total_allocated);
    printf("Total free memory: %zu bytes\n", total_free);
}
