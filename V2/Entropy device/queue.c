
#include <memory.h>
#include "queue.h"

#define RED 10
#define GREEN 11
#define BLUE 12

#include "hardware/gpio.h"


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
    struct queue old = queue, new = old;
    do
    {
        // Check if the queue is full
        if ((old.head >= old.tail && (old.head - old.tail == buffer_size - 1)) ||
            (old.head < old.tail && (old.tail - old.head == 1)))
        {
            return false; // Queue is full
        }

        *new.head = data;

        // Move head to the next position, wrapping around if necessary
        if (old.head + 1 >= buf + buffer_size)
            new.head = buf;
        else
            new.head = old.head + 1;

    } while (!__atomic_compare_exchange_n(&queue.atomic, &old.atomic, new.atomic, true, __ATOMIC_RELEASE, __ATOMIC_RELAXED));

    gpio_put(RED, 1);

    return true;
}

bool dequeue(void *ptr, int size)
{
    struct queue old = queue, new = old;
    do
    {
        // Check if the queue is empty or there is not enough data to dequeue
        int available_data = (old.head >= old.tail) ? (old.head - old.tail) : (buffer_size - (old.tail - old.head));
        if (available_data < size)
        {
            return false; // Not enough data to dequeue
        }

        // Calculate the new tail position
        if (old.tail + size >= buf + buffer_size)
        {
            // Handle wrap-around
            int sz1 = buf + buffer_size - old.tail; // Space available till end
            int sz2 = size - sz1;                  // Remaining size after wrapping

            memcpy(ptr, old.tail, sz1);             // Copy first part
            memcpy((uint8_t *)ptr + sz1, buf, sz2); // Copy second part
            new.tail = buf + sz2;                   // Update tail after wrap
        }
        else
        {
            memcpy(ptr, old.tail, size);
            new.tail = old.tail + size; // Update tail normally
        }

    } while (!__atomic_compare_exchange_n(&queue.atomic, &old.atomic, new.atomic, true, __ATOMIC_RELEASE, __ATOMIC_RELAXED));

    gpio_put(BLUE, 1);
    gpio_put(GREEN, 1);

    return true;
}