#ifndef MYPART_FOR_EDIT_BUFFERPROVIDER_H
#define MYPART_FOR_EDIT_BUFFERPROVIDER_H


#include <stdint.h>

class BufferProvider {
public:

    BufferProvider(int32_t size);

    virtual ~BufferProvider();

    /**
     * 初始化buffer，返回值为0则成功，负数指示参照错误类型
     * @return
     */
    virtual int init();

    virtual void release();
    
    char* getBuffer();

    const uint32_t getSize() const;

protected:

    /* 缓冲区成员 */
    char* m_buff;

    /* 指定大小 */
    const uint32_t m_size;

    bool released = false;

};


#endif //MYPART_FOR_EDIT_BUFFERPROVIDER_H
