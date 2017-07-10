#ifndef SERVER_H
#define SERVER_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <set>
#include <vector>
#include <netinet/in.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
using namespace std;

#define MAX_THREAD_NUM 100
#define MAX_LINE_LEN 4096

#define __DEBUG__ 1
#define DEBUG(argv, format...)do{\
    if (__DEBUG__) {\
        fprintf(stderr, argv, ##format);\
    }\
} while(0)

class ChatServer;

typedef void (ChatServer::*p_func)(char *arg, bool is_logged, int connfd, ChatServer *p_session,
        string &usr_name);

class ChatServer{
    public:
        unordered_map<string, p_func> m_func;
        ChatServer(int p) : port(p), cur_connect_num(0), cur_user_num(0)
        {
            m_func["login"] = &ChatServer::login;
            m_func["say"] = &ChatServer::say;
            m_func["logout"] = &ChatServer::logout;
            m_func["look"] = &ChatServer::look;
            m_func["help"] = &ChatServer::help;
        }

        void login(char *arg, bool is_logged, int connfd, ChatServer *p_session,
          string &usr_name);
        void say(char *arg, bool is_logged, int connfd, ChatServer *p_session,
          string &usr_name);
        void logout(char *arg, bool is_logged, int connfd, ChatServer *p_session,
          string &usr_name);
        void look(char *arg, bool is_logged, int connfd, ChatServer *p_session,
          string &usr_name);
        void help(char *arg, bool is_logged, int connfd, ChatServer *p_session,
          string &usr_name);
        void broadcase(char *msg, int msg_len);
        void analyse_cmd(char *buf, char *cmd, char *arg, bool is_logged);
        int run();
        int get_connfd(int connfd_index);
        void destroy_connfd_index();
        int get_valid_connfd_index();
        int hasUser(const string &name);
        void removeUser(int connfd, const string &name);
        void addUser(int connfd, const string &name);
        int initSock();
        static void *talk_thread(void *);
        inline void decrease_thread()
        {
            --cur_thread_num;
            DEBUG("decrease_thread called, now thread_num is %d\n", cur_thread_num);
        }
        inline void increase_thread()
        {
            ++cur_thread_num;
            DEBUG("increase_thread called, now thread_num is %d\n", cur_thread_num);
        }
        inline int get_thread_num()
        {
            DEBUG("get_thread_num called, now thread_num is %d\n", cur_thread_num);
            return cur_thread_num;
        }

    private:
        unordered_map<int, string> m_users;
        set<string> s_users;
        pthread_t thread[MAX_THREAD_NUM];
        struct sockaddr_in servaddr;
        int listenfd;
        int port;
        int connfd_arr[MAX_THREAD_NUM];
        int cur_thread_num;
    };

typedef struct _thread_para_t
{
    ChatServer *p_session;
    int connfd_index;
}thread_para_t;

int setnoblock(int);
#endif

