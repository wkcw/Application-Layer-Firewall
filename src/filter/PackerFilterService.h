#ifndef SRC_PACKERFILTERSERVICE_H
#define SRC_PACKERFILTERSERVICE_H


#include <rule/RuleManager.h>

namespace sjtu
{
    /**
     * 包过滤的服务类
     * 只提供最基本的过滤服务，一些控制部分交给judge处理
     */
    class PackerFilterService
    {
    public:
        virtual int start() = 0;

        virtual int stop() = 0;

        virtual int join() = 0; // to join the loop if start() is non-block

        RuleManager::Judge *getJudge() const;

        void setJudge(RuleManager::Judge *m_pJudge);

    private:
        /* fields */
        RuleManager::Judge   *m_pJudge;
    };
}


#endif //SRC_PACKERFILTERSERVICE_H
