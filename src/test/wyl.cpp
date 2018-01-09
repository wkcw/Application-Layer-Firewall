#include <util/Singleton.h>
#include <common/ConfigAdapter.h>
#include <iostream>
#include <common/type_def.h>
#include <net/TcpSocket.h>

using namespace sjtu;

class CHandler : public BaseHandler
{
public:
    void onNewConnection(sockaddr_in* client) override {
        printf("new conn\n");
    }

    std::string handle(sockaddr_in* client, char *buff, uint32_t size) override {
        *(buff + size) = 0;
        printf("%d %s\n", client->sin_port, buff);
        return "abc";
    }

    void onLostConnection(sockaddr_in* client) override {
        printf("lost conn\n");
    }
};


int main() {

    TcpSocket _tcp("0.0.0.0", 2333);
    CHandler handler;


    _tcp.setHandler(&handler);
    int ret = _tcp.startListening(NULL); // blocked

    LOG_INFO("[ret=%d]\n", ret);

    fflush(stdout);

}