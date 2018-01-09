

#ifndef SRC_TYPE_DEF_H
#define SRC_TYPE_DEF_H

#include <stdio.h>
#include <stdint.h>
#include <vendor/picojson/picojson.h>
#include <mutex>
#include "../monitor/LogService.h"

#define PROTO_STR(name) #name

/*
 * 以下为json数据getter的简单封装
 */
inline uint64_t getInteger(picojson::value &v, std::string key, uint64_t default_)
{
    if (v.contains(key) && v.get(key).is<double>())
    {
        return (uint64_t) v.get(key).get<double>();
    }
    return default_;
}

inline int getInteger32(picojson::value &v, std::string key, int default_)
{
    if (v.contains(key) && v.get(key).is<double>())
    {
        return (int) v.get(key).get<double>();
    }
    return default_;
}

inline std::string getString(picojson::value &v, std::string key, std::string default_)
{
    if (v.contains(key) && v.get(key).is<std::string>())
    {
        return v.get(key).get<std::string>();
    }
    return default_;
}

namespace sjtu
{
}


#endif //SRC_TYPE_DEF_H
