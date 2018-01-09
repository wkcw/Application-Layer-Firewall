#ifndef NETFILTER_SJTU_BASEHANDLER_H
#define NETFILTER_SJTU_BASEHANDLER_H


#include <cstdint>
#include <string>
#include <netinet/in.h>

/**
 * 服务器的监听回调接口
 */
class BaseHandler {
public:

    /**
     * 连接建立
     * @param client
     */
    virtual void onNewConnection(sockaddr_in* client) = 0;

    /**
     * 数据回调
     * @param client
     * @param buff
     * @param size
     * @return
     */
    virtual std::string handle(sockaddr_in* client, char* buff, uint32_t size) = 0;

    /**
     * 连接关闭或丢失
     * @param client
     */
    virtual void onLostConnection(sockaddr_in* client) = 0;
};


#endif //NETFILTER_SJTU_BASEHANDLER_H
