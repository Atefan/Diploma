
#include <memory.h>
#include "queue.h"

struct queue
{
    union
    {
        struct
        {
            uint8_t * head, *tail;
        };
        uint64_t atomic;
    };
    
};
enum {buffer_size = 1 << 17};

static uint8_t buf[ buffer_size ];
static struct queue queue = {{{buf, buf}}};

bool enqueue(uint8_t data)
{
    struct queue old = queue, new;
    do
    {
        // If there is enough free space
        if(old.head < old.tail && old.tail - old.head <= 1 || old.tail < old.head && old.head - old.tail <= 1)
            return false;

        new.head = old.head + 1;
        if(new.head - buf >= buffer_size )
            new.head = buf;
        *old.head = data;

    } while (__atomic_compare_exchange_n(&queue.atomic, &old.atomic, new.atomic, true, __ATOMIC_RELEASE, __ATOMIC_RELAXED));

    return true;    
}

bool dequeue(void * ptr, int size)
{
    struct queue old = queue, new;
    do
    {
        // If there is enough free space
        if(old.head < old.tail && buffer_size - (old.tail - old.head) < size || old.tail < old.head && old.head - old.tail < size)
            return false;

        new.tail = old.tail + size;

        if(new.tail - buf >= buffer_size )
        {
            int sz2 = buf + buffer_size - old.tail;
            int sz1 = size - sz2;
            new.tail = buf + sz1;
            memcpy( ptr , old.tail, sz1);
            memcpy( (uint8_t *)ptr+ sz1, buf, sz2);
        }
        else
            memcpy( (uint8_t *)ptr, old.tail, size);

    } while (__atomic_compare_exchange_n(&queue.atomic, &old.atomic, new.atomic, true, __ATOMIC_RELEASE, __ATOMIC_RELAXED));
    
    return true;
}