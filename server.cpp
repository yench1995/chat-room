#include "strtool.h"
#include "server.h"

using namespace std;

int ChatServer::hasUser(const string &name)
{
    return s_users.count(name);
}

void ChatServer::addUser(int connfd, const string &name)
{
    m_users[connfd] = name;
    s_users.insert(name);
}

void ChatServer::removeUser(int connfd, const string &name)
{
    m_users.erase(connfd);
    s_users.insert(name);
}

void ChatServer::destroy_connfd(int connfd_index)
{
    if (connfd_index < MAX_THREAD_NUM)
        connfd_arr[connfd_index] = -1
}

int ChatServer::get_connfd(int connfd_index)
{
    return connfd_index < MAX_THREAD_NUM ? connfd_arr[connfd_index] : -1;
}

int ChatServer::get_valid_connfd_index()
{
    int i = 0;
    for (i = 0; i < MAX_THREAD_NUM; ++i)
        if (get_connfd(i) < 0)
            break;
    return i;
}
void ChatServer::login(char *arg, bool is_logged, int connfd, ChatServer *p_session, string &user_name)
{
    char ret_buf[MAX_LINE_LEN];
    DEBUG("log name[%s]\n", arg);
    if (is_logged)
    {
        say(arg, is_logged, connfd, p_session, user_name);
        return;
    }

    if (hasUser(arg))
    {
        snprintf(ret_buf, MAX_LINE_LEN, "User[%s] already exits. Please try again.%c%c",arg, 30, 0);
        send(connfd, ret_buf, strlen(ret_buf), 0);
    } else {
        addUser(connfd, arg);
        user_name= arg;
        snprintf(ret_buf, MAX_LINE_LEN, "%s joins the room!%c%c", arg, 30, 0);
        broadcase(ret_buf, strlen(ret_buf));
        setLogged(connfd, true);
    }
    DEBUG("ret_buf[%s]\n", ret_buf);
}

void ChatServer::broadcase(char *msg, int msg_len)
{
    int ret = 0;
    if (msg == NULL)
        return;
    for (unordered_map<int, string>::iterator it = m_users.begin(); it != m_users.end(); ++it)
    {
        if((ret = send(it->first, msg, msg_len, 0)) < 0)
            fprintf(stderr, "send message to connfd[%d] failed!\n", it->first);
    }
}

void ChatServer::look(char *arg, bool is_logged, int connfd, ChatServer *p_session, string &user_name)
{
    DEBUG("look\n");
    char ret_buf[MAX_LINE_LEN];
    if (!is_logged)
    {
        snprintf(ret_buf, MAX_LINE_LEN, "You haven't been logged in. Please login using command\"login <name>\" or type \"help\" for help.%c%c", 30, 0);
        int len = strlen(ret_buf);
        DEBUG("ret_buf[%s] len[%d]\n", ret_buf, len);
        send(connfd, ret_buf, strlen(ret_buf), 0);
    } else {
        string str_ret_buf("user list: ");
        char sep[10];
        sprintf(sep, "%c", 30);
        for (auto it = s_users.begin(); it != s_users.end(); ++it)
            str_ret_buf = str_ret_buf + sep + *it;
        send(connfd, str_ret_buf.c_str(), str_ret_buf.size(), 0);
    }
}

void ChatServer::say(char *arg, bool is_logged, int connfd, ChatServer *p_session, string &user_name)
{
    DEBUG("saying. log_flag[%d]\n", is_logged);
    char ret_buf[MAX_LINE_LEN];
    if (is_logged)
    {
        string user_name = m_users[connfd];
        snprintf(ret_buf, MAX_LINE_LEN, "[%s]%s%c%c", user_name.c_str(), arg, 30, 0);
        DEBUG("ret_buf[%s]\n", ret_buf);
        p_session->broadcase(ret_buf, strlen(ret_buf));
    } else {
        snprintf(ret_buf, MAX_LINE_LEN, "You haven't been logged in. Please login using command\"login <name>\" or type \"help\" for help.%c%c", 30, 0);
        int len = strlen(ret_buf);
        DEBUG("ret_buf[%s] len[%d]\n", ret_buf, len);
        send(connfd, ret_buf, strlen(ret_buf), 0);
    }
}

void ChatServer::logout(char *arg, bool is_logged, int connfd, ChatServer *p_session, string &user_name)
{
    if (!is_logged)
    {
        say(arg, is_logged, connfd, p_session, user_name);
        return;
    } else {
        char ret_buf[MAX_LINE_LEN];
        string user = m_users[connfd];
        removeUser(connfd, is_logged);
        snprintf(ret_buf, MAX_LINE_LEN, "[%s] logged out.%c%c", user.c_str(), 30, 0);
        send(connfd, ret_buf, strlen(ret_buf), 0);
        snprintf(ret_buf, MAX_LINE_LEN, "[%s] leaves the room.%c%c", user.c_str(), 30, 0);
        broadcase(ret_buf, strlen(ret_buf));
        setLogged(connfd, false);
    }
}

void ChatServer::help(char *arg, bool is_logged, int connfd, ChatServer *p_session, string &user_name)
{
    char ret_buf[MAX_LINE_LEN];
    snprintf(ret_buf, MAX_LINE_LEN,
            "supported commands:%c\tlogin <name>%c\tlook%c\tlogout%c\thelp%c\tquit%canything else is to send a message%c%c",
            30,30,30,30,30,30,30,0);
    send(connfd, ret_buf, strlen(ret_buf), 0);
}

void ChatServer::analyse_cmd(char *buf, char *cmd, char *arg, bool is_logged)
{
    if (buf == NULL || cmd == NULL || NULL == arg)
        return;
    cmd[0] = arg[0] = 0;
    strip(buf);
    DEBUG("buf after strip: %s\n", buf);
    if (strcasecmp(buf, "help") == 0)
        snprintf(cmd, MAX_LINE_LEN, "%s", buf);
    else if (is_logged && strcasecmp(buf, "logout") == 0)
        snprintf(cmd, MAX_LINE_LEN, "%s", buf);
    else if (!is_logged && strncasecmp(buf, "login ", 6) == 0)
    {
        char *p = strchr(buf, ' ');
        if (p)
        {
            *p++ = 0;
            snprintf(arg, MAX_LINE_LEN, "%s", strip(p));
        }
        snprintf(cmd, MAX_LINE_LEN, "%s", buf);
    }
    else if(is_logged && strcasecmp(buf, "look") == 0)
        snprintf(cmd, MAX_LINE_LEN, "look");
    else
    {
        snprintf(cmd, MAX_LINE_LEN, "say");
        snprintf(arg, MAX_LINE_LEN, "%s", buf);
    }
}

void *ChatServer::talk_thread(void *arg)
{
    thread_para_t *thread_para = static_cast<thread_para_t *>(arg);
    ChatServer *p_session = thread_para->p_session;
    int connfd_index = thread_para->connfd_index;
    int connfd = p_session->connfd_arr[connfd_index];

    p_session->increase_thread();
    char buff[MAX_LINE_LEN];
    char ret_buf[MAX_LINE_LEN + 40];
    int msg_len = 0;
    int ret;
    if ((ret = send(connfd, "Welcome to server!", 100, 0)) < 0)
    {
        fprintf(stderr, "send Welcome failed!\n");
        p_session->destroy_connfd(connfd_index);
        p_session->decrease_thread();
        close(connfd);
        delete thread_para;
        return (void *)1;
    }
    bool is_logged = false;
    char cmd[MAX_LINE_LEN] = "";
    char cmd_arg[MAX_LINE_LEN] = "";
    string user_name;
    while (true)
    {
        if((msg_len = recv(connfd, buff, MAX_LINE_LEN, 0)) == 0)
        {
            fprintf(stderr, "received from client failed!\n");
            break;
        }
        buff[msg_len] = 0;
        printf("connfd_arr[%d] = [%d] received: %s\n", connfd_index, connfd, buff);
        unordered_map<string, p_func>::iterator it = p_session->m_func.find(cmd);
        if (it != p_session->m_func.end())
            (p_session->*(it->second))(cmd_arg, is_logged, connfd, p_session, user_name);
        else
            send(connfd, "cmd error!", 100, 0);
    }
    close(connfd);
    p_session->removeUser(connfd, user_name);
    p_session->destroy_connfd(connfd_index);
    p_session->decrease_thread();
    delete thread_para;

}

int ChatServer::run()
{
    if (initSock() != 0)
    {
        fprintf(stderr, "initSock failed\n");
        return 1;
    }
    int connfd = 0;
    int ret = 0;
    thread_para_t *thread_para;
    while (1)
    {
        if ((connfd = accept(listenfd, (struct sockaddr*)NULL, NULL)) < 0)
        {
            fprintf(stderr, "accept socket error: %s(errno: %d)", strerror(errno), errno);
            continue;
        }
        int connfd_index = get_valid_connfd_index();
        if (connfd_index >= MAX_THREAD_NUM)
        {
            fprintf(stderr, "Too many threads! Please wait.\n");
            char tmp_buf[MAX_LINE_LEN];
            snprintf(tmp_buf, MAX_LINE_LEN, "Room is full. No more than %d people."
                    "Please wait for a moment.%c%c", MAX_THREAD_NUM, 30, 0);
            send(connfd, tmp_buf, strlen(tmp_buf), 0);
            close(connfd);
            continue;
        }
        thread_para = new thread_para_t;
        thread_para->p_session = this;
        thread_para->connfd_index = connfd;
        DEBUG("in function run connfd_index[%d] connfd[%d]\n", connfd_index, connfd);
        connfd_arr[connfd_index] = connfd;
        ret = pthread_create(&thread[connfd_index], NULL, talk_thread, thread_para);
        if (ret != 0)
        {
            fprintf(stderr, "create thread failed!\n");
            destroy_connfd(connfd_index);
            delete thread_para;
            close(connfd);
        }
        else
        {
            fprintf(stderr, "create thread[%d] success!\n", cur_thread_num);
        }
    }
    fprintf(stderr, "run finished!\n");
    return 0;
}

int ChatServer::initSock()
{
    int ret = 0;
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        fprintf(stderr, "create socket error: %s(errno: %d)\n", strerror(errno), errno);
        return 1;
    }

    bzero(%servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    if (bind(listenfd, (struct sockaddr*)%servaddr, sizeof(servaddr)))
    {
        fprintf(stderr, "create socket error: %s(errno: %d)\n", strerror(errno), errno);
        return 2;
    }

    if (listen(listenfd, 10) < 0)
    {
        fprintf(stderr, "listen socket error: %s(errno: %d)\n", strerror(errno), errno);
        return 3;
    }
    fprintf(stderr, "port: %d\ninitSock finished!\n", port);
    return 0;
}
