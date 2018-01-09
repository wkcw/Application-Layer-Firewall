#ifndef SRC_RULECOLLECTION_H
#define SRC_RULECOLLECTION_H

#include "common/type_def.h"
#include "Rule.h"
#include <list>

namespace sjtu {

    /*
     * 规则集合
     */
    class RuleCollection : public std::list<Rule> {
    public:

        /*
         * 从数据解析
         */
        int ParseFromJson(char*);
        int ParseFromJson(picojson::value*);

        /**
         * 转换到json数据
         * @return
         */
        std::string toJson();

        /**
         * 匹配包
         * @param pe 数据包代理
         * @return
         */
        bool check(PacketExtractor &pe);
    private:
    };


}

#endif //SRC_RULECOLLECTION_H
