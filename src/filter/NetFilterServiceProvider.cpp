#include "NetFilterServiceProvider.h"
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <linux/netfilter.h>
#include <libnetfilter_queue/libnetfilter_queue.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/icmp.h>
#include <arpa/inet.h>
#include <util/Singleton.h>


/**
 *
 * @return
 */
int staticCallBackProxy(struct nfq_q_handle *qh, struct nfgenmsg *nfmsg, struct nfq_data *nfa,
                        void *data) {
    return sjtu::Singleton<sjtu::NetFilterServiceProvider>::GetInstance()->onPacket(qh, nfmsg, nfa, data);
}

int sjtu::NetFilterServiceProvider::start() {


	LOG_INFO("opening library handle\n");
    m_handle = nfq_open();
    if (!m_handle) {
        LOG_ERROR("error during nfq_open()\n");
        return -1;
    }

    LOG_INFO("unbinding existing nf_queue handler for AF_INET (if any)\n");
    if (nfq_unbind_pf(m_handle, AF_INET) < 0) {
        LOG_ERROR("already nfq_unbind_pf()\n");
        return -2;
    }

    LOG_INFO("binding nfnetlink_queue as nf_queue handler for AF_INET\n");
    if (nfq_bind_pf(m_handle, AF_INET) < 0) {
        LOG_ERROR("error during nfq_bind_pf()\n");
        return -3;
    }

    LOG_INFO("binding this socket to queue '0'\n");
    m_qHandler = nfq_create_queue(m_handle,0, getCallback(), NULL);
    if (!m_qHandler) {
        LOG_ERROR("error during nfq_create_queue()\n");
        return -4;
    }

    LOG_INFO("setting copy_packet mode\n");
    if (nfq_set_mode(m_qHandler, NFQNL_COPY_PACKET, 0xffff) < 0) {
        LOG_ERROR("can't set packet_copy mode\n");
        return -5;
    }

    m_nlHandler = nfq_nfnlh(m_handle);
    m_fd = nfnl_fd(m_nlHandler);

    m_runFlag = 1;

    while(this->m_runFlag)
    {
        this->length = (int) recv(this->m_fd, this->buf, 1600, 0);
        nfq_handle_packet(this->m_handle, this->buf, this->length);
    }
    return 0;
}

int sjtu::NetFilterServiceProvider::stop() {
    m_runFlag = 0;
    return 0;
}

int sjtu::NetFilterServiceProvider::join() {
    if (m_thread.joinable())
    {
        m_thread.join();
        return 0;
    }
    return 1;
}

int sjtu::NetFilterServiceProvider::onPacket(struct nfq_q_handle *qh, struct nfgenmsg *nfmsg, struct nfq_data *nfa,
                                             void *data) {
    int id = 0;
    struct nfqnl_msg_packet_hdr *ph;
    unsigned char *pdata = NULL;
    int pdata_len;

    ph = nfq_get_msg_packet_hdr(nfa);

    if (ph == NULL)
    {
        LOG_ERROR("null return value in nfq_get_msg_packet_hdr\n");
        return 1;
    }

    id = ntohl(ph->packet_id);

    pdata_len = nfq_get_payload(nfa, (unsigned char**)&pdata);

    if (pdata != NULL)
        m_piphdr = (struct iphdr *)pdata;
    else
        return 1;

    PacketExtractor _pe(m_piphdr);

    return nfq_set_verdict(qh, id, getJudge()->judge(&_pe) ? NF_ACCEPT : NF_DROP, 0, NULL);
}

nfq_callback* sjtu::NetFilterServiceProvider::getCallback() {
    return staticCallBackProxy;
}

sjtu::NetFilterServiceProvider::~NetFilterServiceProvider() {

    nfq_destroy_queue(m_qHandler);
    nfq_close(m_handle);

}

sjtu::NetFilterServiceProvider::NetFilterServiceProvider()
    : m_runFlag(0)
{

}
