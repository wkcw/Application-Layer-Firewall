#include <linux/ip.h>
#include "PacketExtractor.h"
#include <netinet/in.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/icmp.h>
#include <arpa/inet.h>

uint32_t sjtu::PacketExtractor::getSrcIp() {
    return m_pData->saddr;
}

uint16_t sjtu::PacketExtractor::getSrcPort() {
    if (m_pData->protocol == IPPROTO_TCP)
    {
        return getData<tcphdr>()->source;
    }
    if (m_pData->protocol == IPPROTO_UDP)
    {
        return getData<udphdr>()->source;
    }
    return 0;
}

uint32_t sjtu::PacketExtractor::getDestIp() {
    return m_pData->daddr;
}

uint16_t sjtu::PacketExtractor::getDestPort() {
    if (m_pData->protocol == IPPROTO_TCP)
    {
        return getData<tcphdr>()->dest;
    }
    if (m_pData->protocol == IPPROTO_UDP)
    {
        return getData<udphdr>()->dest;
    }
    return 0;
}

uint8_t sjtu::PacketExtractor::getProtocol() {
    return m_pData->protocol;
}

uint8_t sjtu::PacketExtractor::getICMPType() {
    if (m_pData->protocol == IPPROTO_ICMP)
    {
        return getData<icmphdr>()->type;
    }
    return 0;
}

void sjtu::PacketExtractor::describe(char *p) {

    tm t = getTime();

//    sprintf(p, "%s:%d  %s:%d  %d  %d  %d-%d-%d %d:%d:%d",
//    inet_ntoa(getSrcIp()), getSrcPort(), inet(getDestIp()), getDestPort(), getProtocol(), getICMPType(),
//    t.tm_year+1900, t.tm_mon+1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);

}



