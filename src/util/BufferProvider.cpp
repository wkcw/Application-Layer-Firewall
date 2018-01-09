#include <malloc.h>
#include "BufferProvider.h"

int BufferProvider::init() {

    m_buff = (char *) malloc(m_size);

    if (m_buff == NULL)
        return -1;

    return 0;
}

void BufferProvider::release() {
    released = true;

    free(m_buff);
    m_buff = 0;
}

char *BufferProvider::getBuffer() {
    return m_buff;
}

BufferProvider::BufferProvider(int32_t size)
    :m_size(size)
{

}

BufferProvider::~BufferProvider() {
    if (!released)
        release();
}

const uint32_t BufferProvider::getSize() const {
    return m_size;
}
