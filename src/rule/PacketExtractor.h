#ifndef NETFILTER_SJTU_PACKETEXTRACTOR_H
#define NETFILTER_SJTU_PACKETEXTRACTOR_H

#include "common/type_def.h"
#include <linux/ip.h>
#include <time.h>
#include <string>


namespace sjtu
{
    typedef union {
        uint32_t data;
        struct {
            uint8_t hours[3];
            uint8_t wday;
        };
    } time_pass_type;

    /*
     * ip数据包的包装类
     */
    class PacketExtractor {
    public:

        /**
         * 构造器
         * @param m_pData
         */
        PacketExtractor(const iphdr *m_pData) : m_pData(m_pData) {}

        /*
         * 装饰器
         */
        uint32_t getSrcIp();
        uint16_t getSrcPort();
        uint32_t getDestIp();
        uint16_t getDestPort();
        uint8_t getProtocol();
        uint8_t getICMPType();

        /**
         * 描述数据包
         * @param p 目标数据区
         */
        void describe(char *p);

        /**
         * 获取当前时间戳
         * @return
         */
        inline tm getTime()
        {
            time_t tt = time(NULL);
            return *localtime(&tt);
        }

        /**
         * 获取现在的time pass
         * @return
         */
        time_pass_type getTimePass()
        {
            tm tm_ = getTime();
            time_pass_type _t = {0};

            _t.wday = (uint8_t) 1 << tm_.tm_wday;
            _t.hours[tm_.tm_hour / 8] = (uint8_t) 1 << (tm_.tm_hour & 0x7);

            return _t;
        }



    private:

        template <typename T>
        T *getData()
        {
            return (T*) ((char *)m_pData + m_pData->ihl * 4);
        }

        template <typename T>
        T *getData(char *start, int offset)
        {
            return (T*) (start + offset);
        }

        const iphdr*  m_pData;
    };
}


#endif //NETFILTER_SJTU_PACKETEXTRACTOR_H
