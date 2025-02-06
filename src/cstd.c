#include <stdint.h>

#include "main.h"

int32_t _write(int32_t handle, char *data, int32_t len)
{
    int32_t count = 0;
    for (uint32_t i = 0; i < len; i++)
        count += usart_send(data[i]);
    return count;
}

int32_t _read(int32_t handle, char *data, int32_t len)
{
    for (uint32_t i = 0; i < len; i++)
        data[i] = usart_read();
    return len;
}

int32_t _close(int32_t handle)
{
    return -1;
}

int32_t _lseek(int32_t handle, int32_t offset, int32_t whence)
{
    return -1;
}
