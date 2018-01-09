
#include <common/type_def.h>
#include <net/TcpSocket.h>
#include <util/Singleton.h>
#include <common/ConfigAdapter.h>
#include <filter/NetFilterServiceProvider.h>
#include <util/BufferProvider.h>
#include <fcntl.h>
#include <fstream>

static sjtu::PackerFilterService* _filterService;
static sjtu::RuleManager* _ruleManager;
static sjtu::TcpSocket* _tcp;

/*
 * 继承网络处理类
 */
class CHandler : public BaseHandler
{
public:

    /*
     * 简单的数据opt
     */
    enum {
        OPT_NONE            =  -1,  // 无效
        OPT_QUERY_RULES     =  1,   // 获取当前读取配置
        OPT_UPDATE_RULES    =  2,   // 更新当前配置
        OPT_QUERY_LOG       =  3,   // 获取增量日志
        OPT_QUERY_STATUS    =  4,   // 获取当前状态上下文
        OPT_QUERY_CLEAN_LOG =  5    // 获取全量日志
    };

    void onNewConnection(sockaddr_in* client) override {
    }

    /**
     * 数据处理
     * @param client
     * @param buff
     * @param size
     * @return
     */
    std::string handle(sockaddr_in* client, char *buff, uint32_t size) override {
        buff[size] = 0;
        picojson::value v;
        std::string error = picojson::parse(v, buff);

        if (!error.empty())
        {
            LOG_WARNING("Bad request from socket\n");
            return R"({"error_code":2,"error_msg":"bad body"})";
        }

        int opt = getInteger32(v, "opt", -1);
        char *np;
        switch (opt)
        {
            case OPT_QUERY_RULES:
                return _ruleManager->getRules()->toJson();
            case OPT_QUERY_STATUS:
                return _ruleManager->isEnabled() ? R"({"error_code":0,"enabled":1})" : R"({"error_code":0,"enabled":0})";
            case OPT_UPDATE_RULES:
                if (v.contains("data") && v.get("data").is<picojson::array>())
                {
                    int ret = _ruleManager->loadConfigFromJson(v.get("data"));
                    if (ret < 0)
                        return R"({"error_code":5,"error_msg":"rules parse error"})";
                }
                if (v.contains("enabled") && v.get("enabled").is<double>())
                {
                    _ruleManager->setEnabled(v.get("enabled").get<double>() > 0);
                }
                return R"({"error_code":0})";
            case OPT_QUERY_CLEAN_LOG:
                xp = p_sp;
                return picojson::value(std::string(logbuff)).serialize();
            case OPT_QUERY_LOG:
                np = xp;
                xp = p_sp;
                return picojson::value(std::string(np)).serialize();
            default:
                return R"({"error_code":3,"error_msg":"unknown opt"})";
        }

    }
    void onLostConnection(sockaddr_in* client) override {
    }

private:

    char *xp = logbuff;
};

void startFilter() {
    int ret = _filterService->start();
    if (ret < 0)
    {
        LOG_ERROR("error filtering\n");
    }
}

void listenOnSocket() {
    CHandler handler;
    _tcp->setHandler(&handler);
    int ret = _tcp->startListening(NULL); // blocked
    if (ret < 0)
    {
        LOG_ERROR("error listening on socket\n");
    }
}

int main() {
    // 初始化日志
    LOG_INIT();

    // 配置初始化
    sjtu::ConfigAdapter *configAdapter = sjtu::Singleton<sjtu::ConfigAdapter>::GetInstance();

    // 读取服务器网络配置，初始化网络
    std::string server_bind_ip = configAdapter->getConfig<std::string>("ip");
    uint16_t bind_port = (uint16_t) configAdapter->getConfig<double>("port");
    LOG_INFO("listen socket on [ip=%s, bind_port=%d]\n", server_bind_ip.c_str(), bind_port);
    _tcp = new sjtu::TcpSocket(server_bind_ip.c_str(), bind_port);

    // 初始化包过滤队列
    _filterService = sjtu::Singleton<sjtu::NetFilterServiceProvider>::GetInstance();
    _ruleManager = sjtu::Singleton<sjtu::RuleManager>::GetInstance();
    _filterService->setJudge(_ruleManager->getJudge());

    // 读取规则配置
    std::string rules_file = configAdapter->getConfig<std::string>("rules_path");
    LOG_INFO("Loading rule file: %s\n", rules_file.c_str());
    std::ifstream ifs;
    ifs.open(rules_file);
    picojson::value rules_;

    if (!ifs.is_open())
    {
        LOG_ERROR("Ignored config file, use empty rule setting\n");
    }
    else
    {
        std::string error = picojson::parse(rules_, ifs);
        ifs.close();

        if (!error.empty())
        {
            LOG_ERROR("config json parse error: {%s}\n", error.c_str());
            return 1;
        }

        _ruleManager->loadConfigFromJson(rules_);

    }

    // 过滤服务初始化
    int ret = _ruleManager->init();
    if (ret < 0)
    {
        LOG_ERROR("error listening on socket\n");
    }

    // 开启线程
    std::thread _t(&listenOnSocket);

    startFilter();

    if (_t.joinable())
        _t.join();

}
