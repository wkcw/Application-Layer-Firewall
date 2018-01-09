#ifndef NETFILTER_SJTU_NETFILTERSERVICEPROVIDER_H
#define NETFILTER_SJTU_NETFILTERSERVICEPROVIDER_H

#include <functional>
#include <libnetfilter_queue/libnetfilter_queue.h>
#include <linux/ip.h>
#include <thread>
#include <util/ThreadWrapper.h>
#include "PackerFilterService.h"

namespace sjtu
{

    typedef std::function<int (struct nfq_q_handle*, struct nfgenmsg*, struct nfq_data*, void*)> nfq_callback_wrapper;

    /**
     * netfilter实现类
     */
    class NetFilterServiceProvider : public PackerFilterService {
    public:

        /**
         * 构造器
         */
        NetFilterServiceProvider();

        /**
         * 释放资源
         */
        virtual ~NetFilterServiceProvider();

        /**
         * 开始过滤服务，线程阻塞
         * @return
         */
        int start() override;

        int stop() override;

        /**
         * useless
         * @return
         */
        int join() override;

        /**
         * 回调getter
         * @return
         */
        nfq_callback* getCallback();

        /**
         * 当数据包传递进来时，这个函数会被调用
         * @param qh
         * @param nfmsg
         * @param nfa
         * @param data
         * @return
         */
        int onPacket(struct nfq_q_handle *qh, struct nfgenmsg *nfmsg, struct nfq_data *nfa, void *data);

    private:

        int                     m_enabled;
        int                     m_fd;
        iphdr*                  m_piphdr;
        nfq_handle*             m_handle;
        nfq_q_handle*           m_qHandler;
        nfnl_handle*            m_nlHandler;
        char                    buf[1600];
        int                     length;
        std::thread             m_thread;
        int                     m_runFlag;
    };
}


#endif //NETFILTER_SJTU_NETFILTERSERVICEPROVIDER_H
