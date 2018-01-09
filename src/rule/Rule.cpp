#include <netinet/in.h>
#include <arpa/inet.h>
#include "Rule.h"
#include <linux/icmp.h>
#include <string.h>
#include "PacketExtractor.h"

#define valid_and_check_(stand, value) ((!(stand)) || (value)==(stand))
#define ret_if_false(exp)   if(!(exp)) return false
#define ret_if_nvalid_and_check(stand, value) ret_if_false(valid_and_check_(stand, value))

#define __CASE_RET__(name) case #name : return (name)
#define __CASE_RET2__(name) case name : return #name
#define __IF_CMP_RET__(p, name) if(!strcmp(p, #name)) return name;
#define __IF_CMP_RET__(p, name) if(!strcmp(p, #name)) return name;

bool sjtu::Rule::leastCheck(sjtu::PacketExtractor &pe) {

    ret_if_nvalid_and_check(protocol, pe.getProtocol());
    ret_if_nvalid_and_check(src_ip, pe.getSrcIp());
    ret_if_nvalid_and_check(dest_ip, pe.getDestIp());

    ret_if_false(time_pass.data & pe.getTimePass().data);

    return true;
}


bool sjtu::Rule::check(sjtu::PacketExtractor &pe) {

    ret_if_false(leastCheck(pe));

    switch (pe.getProtocol())
    {
        case IPPROTO_TCP:
        case IPPROTO_UDP:
        {
            ret_if_nvalid_and_check(src_port, pe.getSrcPort());
            ret_if_nvalid_and_check(dest_port, pe.getDestPort());
            break;
        }
        case IPPROTO_ICMP:
        {
            ret_if_nvalid_and_check(sub_protocol, pe.getICMPType());
            break;
        }
        default:
        {
            break;
        }
    }

    return true;
}

int sjtu::Rule::ParseFromJson(char *s) {
    picojson::value v;
    std::string err = picojson::parse(v, std::string(s));
    if (!err.empty())
    {
        LOG_ERROR("error parsing rule collection: %s\n", err.c_str());
        return -1;
    }
    return ParseFromJson(v);
}


int sjtu::Rule::ParseFromJson(picojson::value &v) {
    //TODO
    memset(this, 0, sizeof(*this));

    this->src_ip = inet_addr(getString(v, "src_ip", "0.0.0.0").c_str());
    this->src_port = ntohs((uint16_t) getInteger(v, "src_port", 0));
    this->dest_ip = inet_addr(getString(v, "target_ip", "0.0.0.0").c_str());
    this->dest_port = ntohs((uint16_t) getInteger(v, "target_port", 0));
    this->type = (uint8_t) getInteger(v, "type", 0);

    std::string protocol = getString(v, "protocol", "");

    char *pProto = (char*) protocol.c_str();
    if (!protocol.empty())
    {

        while (*(++pProto) && *pProto != '.');
        if (*pProto == '.')
        {
            this->sub_protocol = getIcmpSubType(pProto + 1);
        }
        *pProto = 0;
        this->protocol = getProtocolType((char *) protocol.c_str());
    }
    this->time_pass.data = (uint32_t) getInteger(v, "time_pass", 0xffffffff);

    return 0;
}

picojson::value sjtu::Rule::toJson() {
    picojson::value v;
    picojson::object obj;

    in_addr _addr;
    _addr.s_addr = this->src_ip;
    obj["src_ip"] = picojson::value(std::string(inet_ntoa(_addr)));

    _addr.s_addr = this->dest_ip;
    obj["target_ip"] = picojson::value(std::string(inet_ntoa(_addr)));

    obj["src_port"] = picojson::value((double) ntohs(this->src_port));
    obj["target_port"] = picojson::value((double) ntohs(this->dest_port));
    obj["type"] = picojson::value((double) (this->type));
    obj["timepass"] = picojson::value((double) (this->time_pass.data));

    std::string protocol_tmp = getProtocolString(this->protocol);
    if(this->sub_protocol == IPPROTO_ICMP){
        protocol_tmp += "." + getIcmpString(this->sub_protocol);
    }
    obj["protocol"] = picojson::value(protocol_tmp);

    v.set<picojson::object>(obj);

    return v;
}



std::string sjtu::Rule::getIcmpString(uint8_t type) {
    switch (type)
    {
        __CASE_RET2__(ICMP_ECHOREPLY);
        __CASE_RET2__(ICMP_DEST_UNREACH);
        __CASE_RET2__(ICMP_SOURCE_QUENCH);
        __CASE_RET2__(ICMP_REDIRECT);
        __CASE_RET2__(ICMP_TIME_EXCEEDED);
        __CASE_RET2__(ICMP_PARAMETERPROB);
        __CASE_RET2__(ICMP_TIMESTAMP);
        __CASE_RET2__(ICMP_TIMESTAMPREPLY);
        __CASE_RET2__(ICMP_INFO_REQUEST);
        __CASE_RET2__(ICMP_INFO_REPLY);
        __CASE_RET2__(ICMP_ADDRESS);
        __CASE_RET2__(ICMP_ADDRESSREPLY);
    }
    return "";
}

uint8_t sjtu::Rule::getIcmpSubType(char *p) {
    __IF_CMP_RET__(p, ICMP_ECHOREPLY);
    __IF_CMP_RET__(p, ICMP_DEST_UNREACH);
    __IF_CMP_RET__(p, ICMP_SOURCE_QUENCH);
    __IF_CMP_RET__(p, ICMP_REDIRECT);
    __IF_CMP_RET__(p, ICMP_TIME_EXCEEDED);
    __IF_CMP_RET__(p, ICMP_PARAMETERPROB);
    __IF_CMP_RET__(p, ICMP_TIMESTAMP);
    __IF_CMP_RET__(p, ICMP_TIMESTAMPREPLY);
    __IF_CMP_RET__(p, ICMP_INFO_REQUEST);
    __IF_CMP_RET__(p, ICMP_INFO_REPLY);
    __IF_CMP_RET__(p, ICMP_ADDRESS);
    __IF_CMP_RET__(p, ICMP_ADDRESSREPLY);
    return 0;
}

std::string sjtu::Rule::getProtocolString(uint8_t type) {
    switch (type)
    {
        case IPPROTO_ICMP:
            return "icmp";
        case IPPROTO_UDP:
            return "udp";
        case IPPROTO_TCP:
            return "tcp";
    }
    return "unknown";
}

uint8_t sjtu::Rule::getProtocolType(char *p) {
    if (!strcmp(p, "tcp")) return IPPROTO_TCP;
    if (!strcmp(p, "udp")) return IPPROTO_UDP;
    if (!strcmp(p, "icmp")) return IPPROTO_ICMP;
    return 0;
}


#undef __CASE_RET__
#undef __CASE_RET__
#undef __IF_CMP_RET__
