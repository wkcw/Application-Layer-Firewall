#ifndef SRC_RULE_H
#define SRC_RULE_H

#include <common/type_def.h>
#include <stdint.h>
#include "PacketExtractor.h"
#include <picojson/picojson.h>

namespace sjtu {

    enum {
        TYPE_BLACK_LIST     = 0,
        TYPE_WHITE_LIST     = 1,
        TYPE_MONITOR_LIST   = 2
    };

    /*
     * 规则类
     */
    class Rule {
    public:
        /* fields */
        uint16_t            dest_port;
        uint16_t            src_port;
        uint32_t            src_ip;
        uint32_t            dest_ip;
        time_pass_type      time_pass;
        uint8_t             type;
        uint8_t             protocol;
        uint8_t             sub_protocol;


        static std::string getIcmpString(uint8_t type);
        static uint8_t getIcmpSubType(char* p);
        static std::string getProtocolString(uint8_t type);
        static uint8_t getProtocolType(char* p);

        /**
         * 匹配规则
         * @return 是否匹配成功
         */
        bool check(PacketExtractor &);

        /**
         * 从字符串解析
         * @return
         */
        int ParseFromJson(char *);

        /**
         * 从json解析
         * @return
         */
        int ParseFromJson(picojson::value &);

        /**
         * 协议无关的核对
         * @return
         */
        inline bool leastCheck(PacketExtractor &);

        /**
         * 转换到json
         * @return
         */
        picojson::value toJson();
    };
}


#endif //SRC_RULE_H
