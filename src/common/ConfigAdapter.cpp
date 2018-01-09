#include <fstream>
#include "ConfigAdapter.h"

using namespace sjtu;

ConfigAdapter::ConfigAdapter()
{
    LOG_INFO("Loading config file...\n");
    std::ifstream ifs;
    ifs.open("./config.json");
    if (!ifs.is_open())
    {
        LOG_ERROR("Ignored config file\n");
    }

    std::string json_err = picojson::parse(m_json, ifs);

    ifs.close();

    if (!json_err.empty())
    {
        LOG_ERROR("json parse error: {%s}\n", json_err.c_str());
        return;
    }

}

ConfigAdapter::~ConfigAdapter() {
//        for (map<string, void*>::iterator it = m_map.begin(); it != m_map.end(); it++)
//        {
//            delete(it->second);
//        }
}