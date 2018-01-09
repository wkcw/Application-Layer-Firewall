#ifndef NETFILTER_SJTU_LOGSERVICE_H
#define NETFILTER_SJTU_LOGSERVICE_H


/*
 * 简单log的实现
 */

extern char logbuff[10240000];
extern char *p_sp;

#define LOG_ORIG(...)           {printf(__VA_ARGS__);if(p_sp){char __TMP_BUFF[1024];if (p_sp - logbuff > 90000000)p_sp=logbuff; p_sp = p_sp + sprintf(p_sp, __VA_ARGS__);}}

#define LOG_INIT()		p_sp=logbuff;

#define LOG_DEBUG(...)       { \
    LOG_ORIG("[%s %s][DEBUG][%s-%d]\t", __DATE__, __TIME__, __FILE__, __LINE__); \
    LOG_ORIG(__VA_ARGS__); \
    LOG_ORIG("\n"); \
}
#define LOG_INFO(...)        { \
    LOG_ORIG("[%s %s][INFO][%s-%d]\t", __DATE__, __TIME__, __FILE__, __LINE__); \
    LOG_ORIG(__VA_ARGS__); \
    LOG_ORIG("\n"); \
}
#define LOG_WARNING(...)     { \
    LOG_ORIG("[%s %s][WARNING][%s-%d]\t", __DATE__, __TIME__, __FILE__, __LINE__); \
    LOG_ORIG(__VA_ARGS__); \
    LOG_ORIG("\n"); \
}
#define LOG_ERROR(...)       { \
    LOG_ORIG("[%s %s][ERROR][%s-%d]\t", __DATE__, __TIME__, __FILE__, __LINE__); \
    LOG_ORIG(__VA_ARGS__); \
    LOG_ORIG("\n"); \
}


#endif //NETFILTER_SJTU_LOGSERVICE_H
