#include <iostream>
#include <common/type_def.h>
#include <net/TcpSocket.h>
#include <filter/NetFilterServiceProvider.h>
#include <util/Singleton.h>
#include <rule/PacketExtractor.h>
#include <linux/icmp.h>
#include <rule/Rule.cpp>
#include <rule/Rule.h>

#include <picojson/picojson.h>
using namespace std;


int main() {
    picojson::value v_json;
    sjtu::Rule a;
    a.dest_ip = 1;
    a.dest_port = 1;
    a.src_ip  = 2;
    a.src_port = 2;
    a.type = 8;
    a.time_pass.data = (uint32_t) 0xffffffff;
    a.protocol = 1;
    a.sub_protocol = 3;
    v_json = a.toJson();
    a.ParseFromJson(v_json);

    printf("%s\n", v_json.serialize(true).c_str());
    printf("%s\n", a.toJson().serialize(true).c_str());






//    int ret = sjtu::Singleton<sjtu::NetFilterServiceProvider>::GetInstance()->start();
//
//    if (ret)
//    {
//        printf("ret = %d", ret);
//    }
}