#include "RuleCollection.h"
#include <picojson/picojson.h>

int sjtu::RuleCollection::ParseFromJson(char *s) {
    picojson::value v;
    clear();
    std::string err = picojson::parse(v, std::string(s));
    if (!err.empty())
    {
        LOG_ERROR("error parsing rule collection: %s\n", err.c_str());
        return -1;
    }

    Rule _rule;
    picojson::array& arr = v.get<picojson::array>();

    for (auto it = arr.begin(); it != arr.end(); ++it)
    {
        _rule.ParseFromJson(*it);
        push_back(_rule);
    }

    return 0;
}

bool sjtu::RuleCollection::check(sjtu::PacketExtractor &pe) {
    bool black = false, white = false, monitor = false;
    for (auto _r : *this)
    {
        switch (_r.type)
        {
            case TYPE_BLACK_LIST:
                if (!black)
                    black = _r.check(pe);
                break;
            case TYPE_WHITE_LIST:
                if (!white)
                    white = _r.check(pe);
                break;
            case TYPE_MONITOR_LIST:
                if (!monitor)
                    monitor = _r.check(pe);
                break;
            default:
                break;
        }
    }

    // TODO: log if necessary

    return white || !black;
}

int sjtu::RuleCollection::ParseFromJson(picojson::value *v) {

    Rule _rule;
    picojson::array& arr = v->get<picojson::array>();

    for (auto it = arr.begin(); it != arr.end(); ++it)
    {
        _rule.ParseFromJson(*it);
        push_back(_rule);
    }

    return 0;
}

std::string sjtu::RuleCollection::toJson() {

    picojson::array arr;

    for (auto it : *this)
    {
        arr.push_back(it.toJson());
    }
    picojson::value v;
    v.set<picojson::array>(arr);
    return v.serialize();
}
