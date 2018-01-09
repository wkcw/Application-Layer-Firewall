#include "PackerFilterService.h"

sjtu::RuleManager::Judge *sjtu::PackerFilterService::getJudge() const {
    return m_pJudge;
}

void sjtu::PackerFilterService::setJudge(sjtu::RuleManager::Judge *m_pJudge) {
    PackerFilterService::m_pJudge = m_pJudge;
}
