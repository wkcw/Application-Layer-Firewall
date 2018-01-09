#ifndef NETFILTER_SJTU_TCPSOCKET_H
#define NETFILTER_SJTU_TCPSOCKET_H

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <map>
#include <unistd.h>
#include "BaseHandler.h"


#define _BACKLOG_ 5

namespace sjtu {

    typedef struct {
        uint32_t type;
        uint32_t size;
    } packet_head_type;

    /*
     * 一个简单的Tcp工具，支持客户端和服务器，不支持会话管理
     * 服务器需要注册监听，服务器只支持单例
     */
    class TcpSocket
    {

    public:

        /**
         * 构造器
         * @param m_fd 以初始化的fd
         */
        TcpSocket(int m_fd) : m_fd(m_fd) {}

        /**
         * 构造起
         * @param m_addr 目标地址
         * @param m_port 目标端口
         */
        TcpSocket(const std::string &m_addr, uint16_t m_port) : m_addr(m_addr), m_port(m_port), m_threads() {}

        /**
         * 注册回调
         * @param m_pHandler
         */
        void setHandler(BaseHandler *m_pHandler) {
            TcpSocket::m_pHandler = m_pHandler;
        }

        /**
         * 客户端连接服务器
         * @return 0表示成功，否则失败
         */
        int connectToServer()
        {
            m_fd = socket(AF_INET, SOCK_STREAM, 0);
            if (m_fd < 0)
            {
                return -1;
            }

            sockaddr_in _addr = {0};
            _addr.sin_port = htons(m_port);
            _addr.sin_family = AF_INET;
            _addr.sin_addr.s_addr = inet_addr(m_addr.c_str());

            return connect(m_fd, (const sockaddr *)&_addr, sizeof(_addr));
        }

        /**
         * pipe信号的处理入口
         * @param signum 信号类型
         */
        static void signal_callback_handler(int signum) {};

        /**
         * 开始监听并处理新连接，阻塞
         * @param handler 对于所有会话是一个对象处理的，如果连接是有状态的，在对象里面状态管理
         * @return
         */
        int startListening(BaseHandler* handler, bool is_raw = true)
        {
            int m_fd = socket(AF_INET, SOCK_STREAM, 0);
            if (m_fd < 0)
            {
                return -1;
            }
            signal(SIGPIPE, signal_callback_handler);

            sockaddr_in _addr = {0};

            memset(&_addr, 0, sizeof(sockaddr_in));
            _addr.sin_port = htons(m_port);
            _addr.sin_family = AF_INET;
            _addr.sin_addr.s_addr = inet_addr(m_addr.c_str());
            int ret;
            ret = bind(m_fd, (const sockaddr *)&_addr, sizeof(_addr));
            if (ret < 0)
            {
                LOG_ERROR("error binding [code=%d, msg=%s]\n", errno, strerror(errno));
                return -1;
            }

            ret = listen(m_fd, _BACKLOG_);
            if (ret < 0)
                return -3;

            sockaddr _client = {};
            uint32_t _len = sizeof(_client);
            // blocked
            while (true)
            {
                int _newFd = accept(m_fd, &_client, &_len);
                if (_newFd < 0)
                {
                    LOG_ERROR("error accept socket [code=%d, msg=%s]\n", errno, strerror(errno));
                    return -2;
                }

                m_threads[_newFd] = std::thread([this, is_raw, _client, _len] (int fd) {
                    TcpSocket _sock(fd);
                    sockaddr_in client;
                    memcpy(&client, &_client, _len);
                    char _pBuff[2048];
                    int _size;
                    this->m_pHandler->onNewConnection(&client);
                    if (is_raw)
                    {
                        while ((_size = (int)_sock.recvRaw(_pBuff, 2048)) > 0)
                        {
                            std::string rsp = this->m_pHandler->handle(&client, _pBuff, (uint32_t) _size);
                            if (!rsp.empty())
                            {
                                long ret = _sock.sendRaw(rsp.c_str(), rsp.size());
                                if (ret < 0)
                                {
                                    LOG_ERROR("error sending packet [code=%d, msg=%s]\n", errno, strerror(errno));
                                    return -1;
                                }
                            }
                        }
                    }
                    else
                    {
                        while ((_size = _sock.getString(_pBuff)) > 0)
                        {
                            std::string rsp = this->m_pHandler->handle(&client, _pBuff, (uint32_t) _size);
                            if (!rsp.empty())
                            {
                                _sock.putString(rsp.c_str(), rsp.size());
                            }
                        }
                    }
                    if (_size < 0)
                    {
                        LOG_ERROR("error socket [code=%d, msg=%s]\n", errno, strerror(errno));
                    }


                    this->m_pHandler->onLostConnection(&client);

                    close(fd);
                    m_threads[fd].detach();
                }, _newFd);
            }
        }

        /**
         * 释放连接
         * @return 0表示成功
         */
        int release()
        {
            if (m_fd)
                close(m_fd);
            return 0;
        }

        /**
         * 获取一条消息（必须通信两边同时使用此类）
         * @param buff 写入指针
         * @return 0表示成功
         */
        int getString(char *buff)
        {
            packet_head_type _head;
            int ret = getData<packet_head_type>(&_head);
            if (ret < 0)
                return -1;

            ret = (int) recvRaw(buff, _head.size);
            if (ret >= 0)
                return _head.size;
            return -2;
        }

        /**
         * 发送一条消息
         * @param buff 消息首指针
         * @param size 消息长度
         * @return 0表示成功
         */
        int putString(const char *buff, uint32_t size)
        {
            packet_head_type _head;
            _head.size = size;
            _head.type = 0;
            this->sendData<packet_head_type>(&_head);
            return (int) this->sendRaw(buff, size);
        }

        /**
         * 获取一个内存对象
         * @tparam T 任意类型
         * @param data 对象指针
         * @return 读取消息长度
         */
        template <typename T>
        int getData(T* data)
        {
            return (int) recvRaw((char *) data, sizeof(T));
        }

        /**
         * 发送一个内存对象
         * @tparam T 任意类型
         * @param data 对象指针
         * @return 消息长度
         */
        template <typename T>
        int sendData(const T* data)
        {
            return (int) sendRaw((const char *) data, sizeof(T));
        }

        /**
         * 读取原生数据
         * @param buff
         * @param size
         * @return 读取的消息长度
         */
        inline int64_t recvRaw(char *buff, uint32_t size)
        {
            long ret = recv(m_fd, buff, size, 0);
            return ret;
        }

        /**
         * 发送原生数据
         * @param buff
         * @param size
         * @return 写入的消息长度
         */
        inline int64_t sendRaw(const char *buff, uint32_t size)
        {
            return send(m_fd, buff, size, 0);
        }

    private:

        /*
         * Fields
         */
        int                         m_fd;
        uint16_t                    m_port;
        BaseHandler*                m_pHandler;
        std::string                 m_addr;
        std::map<int, std::thread>  m_threads;
    };

}


#endif //NETFILTER_SJTU_TCPSOCKET_H
