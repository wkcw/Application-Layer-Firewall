#ifndef NETFILTER_SJTU_RULEMANAGER_H
#define NETFILTER_SJTU_RULEMANAGER_H


/**
 * the class to manage/parse/deliver filter rules and services
 */
#include <linux/ip.h>
#include <mutex>
#include "PacketExtractor.h"
#include "RuleCollection.h"

namespace sjtu {

    /*
     * 规则过滤的服务类
     */
    class RuleManager {

    public:

        friend class Judge;

        /*
         * 过滤判断的接口
         */
        class Judge {
        public:

            Judge(RuleManager *m_context);

            /**
             * 包过滤的询问接口
             * @param packetExtractor
             * @return
             */
            bool judge(PacketExtractor *packetExtractor);

        private:

            RuleManager* m_context;
        };

        /**
         * 构造器
         */
        RuleManager();

        /**
         * 初始化
         * @return 0表示成功
         */
        int init();

        /**
         * judge getter
         * @return
         */
        Judge* getJudge()
        {
            return &m_judge;
        }

        /**
         * 设置是否过滤生效，如果true，则全部放行
         * @param enabled
         */
        void setEnabled(bool enabled);

        /**
         * 查看过滤生效状态
         * @return
         */
        bool isEnabled() const;

        /**
         * 获取过滤规则集
         * @return
         */
        RuleCollection *getRules() const;

        /**
         * 从数据区解析规则
         * @param s
         * @return
         */
        int loadConfigFromBuffer(char *s)
        {
            auto _rules = new RuleCollection;
            int ret = _rules->ParseFromJson(s);
            if (ret < 0)
                return -1;

            lockRules();

            delete(m_rules);
            m_rules = _rules;

            unlockRules();

            return 0;
        }

        /**
         * 从json解析规则
         * @param v
         * @return
         */
        int loadConfigFromJson(picojson::value &v)
        {
            RuleCollection* _rules = new RuleCollection();
            int ret = _rules->ParseFromJson(&v);
            if (ret < 0)
                return -1;

            lockRules();

            delete(m_rules);
            m_rules = _rules;

            unlockRules();

            return 0;
        }

        /**
         * 阻塞的对象锁
         */
        inline void lockRules()
        {
//            LOG_DEBUG("%d\n", ++_tmp);
            m_mutexRules.lock();
        }

        /**
         * 释放对象锁
         */
        inline void unlockRules()
        {
//            LOG_DEBUG("%d\n", --_tmp);
            m_mutexRules.unlock();
        }

    private:

        /*
         * fields
         */
        Judge               m_judge;
        RuleCollection*     m_rules;
        bool                enabled;
        std::mutex          m_mutexRules;

    };



}


#endif //NETFILTER_SJTU_RULEMANAGER_H
