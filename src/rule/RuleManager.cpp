#include "RuleManager.h"
#include <netinet/in.h>
#include <arpa/inet.h>

int sjtu::RuleManager::init() {
    if (m_rules == NULL)
    {
        LOG_WARNING("rule_set is not initiated\n");
        m_rules = new RuleCollection;
    }
    enabled = 1;
    return 0;
}

sjtu::RuleManager::RuleManager() :
        m_judge(this), m_rules(0), enabled(false)
{

}

sjtu::RuleCollection *sjtu::RuleManager::getRules() const {
    return m_rules;
}

void sjtu::RuleManager::setEnabled(bool enabled) {
    RuleManager::enabled = enabled;
}

bool sjtu::RuleManager::isEnabled() const {
    return enabled;
}


bool sjtu::RuleManager::Judge::judge(PacketExtractor *packetExtractor)
{

    if (!m_context->isEnabled())
        return true;

    m_context->lockRules();

    bool ret = m_context->m_rules->check(*packetExtractor);

    m_context->unlockRules();

    if (!ret) {
        in_addr _addr;
        _addr.s_addr = packetExtractor->getSrcIp();
        char _p[36];
        strcpy(_p, (const char *) inet_ntoa(_addr));
        _addr.s_addr = packetExtractor->getDestIp();
        strcpy(_p+18, (const char *) inet_ntoa(_addr));
        
        LOG_INFO("blocked packet [%s] %s -> %s", Rule::getProtocolString(packetExtractor->getProtocol()).c_str(), _p, _p+18);
    }

    return ret;
}

sjtu::RuleManager::Judge::Judge(sjtu::RuleManager *m_context) : m_context(m_context)
{

}
