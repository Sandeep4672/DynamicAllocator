#include <stdio.h>

#include <windows.h>

typedef char ALIGN[16];

union header {
    struct {
        size_t size;
        unsigned is_free;
        union header *next;
        union header *prev;
    } s;
    ALIGN stub;
};

typedef union header header_t;

header_t *head = NULL, *tail = NULL;

#define ALIGN16(x) (((x) + 15) & ~15)


void split_block(header_t *block, size_t size) {
    size_t total_size = sizeof(header_t) + size;
    if (block->s.size >= total_size + sizeof(header_t) + 16) {
        header_t *new_block = (header_t*)((char*)(block + 1) + size);
        new_block->s.size = block->s.size - total_size;
        new_block->s.is_free = 1;
        new_block->s.next = block->s.next;
        new_block->s.prev = block;
        if (new_block->s.next) {
            new_block->s.next->s.prev = new_block;
        } else {
            tail = new_block;
        }
        block->s.size = size;
        block->s.next = new_block;
    }
}


header_t* get_free_block(size_t size) {
    header_t *curr = head;
    while (curr) {
        if (curr->s.is_free && curr->s.size >= size)
            return curr;
        curr = curr->s.next;
    }
    return NULL;
}

void* my_malloc(size_t size) {
    size = ALIGN16(size); 
    if (!size) return NULL;

    header_t *header = get_free_block(size);
    if (header) {
        header->s.is_free = 0;
        split_block(header, size);
        return (void*)(header + 1);
    }

    size_t total_size = sizeof(header_t) + size;
    void *block = VirtualAlloc(NULL, total_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!block) return NULL;

    header = (header_t*)block;
    header->s.size = size;
    header->s.is_free = 0;
    header->s.next = NULL;
    header->s.prev = tail;

    if (!head)
        head = header;
    if (tail)
        tail->s.next = header;
    tail = header;

    return (void*)(header + 1);
}


void coalesce(header_t *block) {

    if (block->s.next && block->s.next->s.is_free) {
        block->s.size += sizeof(header_t) + block->s.next->s.size;
        block->s.next = block->s.next->s.next;
        if (block->s.next)
            block->s.next->s.prev = block;
        else
            tail = block;
    }


    if (block->s.prev && block->s.prev->s.is_free) {
        block->s.prev->s.size += sizeof(header_t) + block->s.size;
        block->s.prev->s.next = block->s.next;
        if (block->s.next)
            block->s.next->s.prev = block->s.prev;
        else
            tail = block->s.prev;
    }
}


void my_free(void *ptr) {
    if (!ptr) return;

    header_t *header = (header_t*)ptr - 1;
    header->s.is_free = 1;
    coalesce(header);


    if (header == tail && header->s.is_free) {
        if (header->s.prev) {
            tail = header->s.prev;
            tail->s.next = NULL;
        } else {
            head = tail = NULL;
        }
        VirtualFree(header, 0, MEM_RELEASE);
    }
}

int main() {
    int *arr = (int*)my_malloc(5 * sizeof(int));
    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;
    arr[3] = 4;
    arr[4] = 5;

    for (int i = 0; i < 5; i++) {
        printf(" %d", arr[i]);
    }

    my_free(arr);
    return 0;
}
