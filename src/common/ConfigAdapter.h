

#ifndef SRC_CONFIGADAPTER_H
#define SRC_CONFIGADAPTER_H

#include "type_def.h"
#include <string>
#include <picojson/picojson.h>

namespace sjtu {

    /**
     * 配置的适配类，直接在这里实现配置
     */
    class ConfigAdapter {
    public:

        /**
         * 构造器，包含读配置文件
         */
        ConfigAdapter();

        virtual ~ConfigAdapter();

        /**
         * 获取配置
         * @tparam T 数据类型
         * @param key 名称
         * @return 数据
         */
        template<class T>
        T getConfig(std::string key) {
            picojson::value &v = m_json.get(key);
            if (!v.is<T>()) {
                LOG_ERROR("Config key not found: %s\n", key.c_str());
                return T();
            }
            return v.get<T>();
        }

    private:
        /*
         * 配置数据的存放，json格式
         */
        picojson::value m_json;
//        json m_jsonConfig;
    };
}



#endif //SRC_CONFIGADAPTER_H
