#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/epoll.h>
#include <signal.h>
#include <fcntl.h>
#include "log.h"
#include "http.h"

zcs::Logger::ptr g_logger = ZCS_LOG_ROOT();

constexpr int max_numer = 65535;

static const char* rt_responses[] = {
    "HTTP/1.1 200 OK\r\n\
Content-Type: text/html\r\n\
\r\n\
<!DOCTYPE html>\n\
<html lang=\"en\">\n\
<head>\n\
    <meta charset=\"UTF-8\" />\n\
    <title>Document</title>\n\
</head>\n\
<body>\n\
    <p>this is http response</p>\n\
</body>\n\
</html>",

    "Something wrong\n"
};

int SetNoBlocking(int fd) {
    int old_op = fcntl(fd, F_GETFL);
    int new_op = old_op | O_NONBLOCK;
    fcntl(fd, new_op);
    return old_op;
}

void AddEpollFD(int epollfd, int fd) {
    epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
    SetNoBlocking(fd);
}

void EpollET() {

}

void EpollLT() {

}

int32_t DealConnection() {

}

int32_t DealSingal() {

}

int32_t DealRecv() {

}

int32_t DealSend() {
    
}





int main(int argc, char* argv[]) {

    //g_logger->ClearAppender();
    g_logger->AddAppender(zcs::LogAppender::ptr(new zcs::FileLogAppender("log.log")));
    ZCS_INFO(g_logger) << "main run";
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0) {
        ZCS_ERROR(g_logger) << "create fd failed!";
    }
    //SetNoBlocking(fd);

    struct sockaddr_in server_addr;

    bzero(&server_addr, sizeof(server_addr));  
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    server_addr.sin_port = htons(7979);


    int ret = bind(fd, (struct sockaddr*) &server_addr, sizeof(server_addr));
    if(ret < 0) {
        ZCS_ERROR(g_logger) << "bind socket failed";
    }
    ret = listen(fd, 10);
    if(ret < 0) {
        ZCS_ERROR(g_logger) << "listen socket failed";
    }
    epoll_event events[max_numer];
    int epoll_fd = epoll_create(5);
    AddEpollFD(epoll_fd, fd);
    SetNoBlocking(epoll_fd);
    int count = 0;
    while(1) {
        ret = epoll_wait(epoll_fd, events, max_numer, -1);
        if(ret < 0) {
            ZCS_ERROR(g_logger) << "epoll failed, ret=" << ret;
        }

        char buf[1024];
        
        for(int i = 0; i < ret; i++) {
            int sockfd = events[i].data.fd;
            if(sockfd == fd) {
                // listen fd 
                struct sockaddr_in client_addr;
                socklen_t client_addr_len = sizeof(client_addr);
                int connfd = accept(fd, (struct sockaddr*) &client_addr, &client_addr_len);

                ZCS_INFO(g_logger) << "new conn fd=" << connfd;
                AddEpollFD(epoll_fd, connfd);
            } else if(events[i].events & EPOLLIN) {
                // epoll in
                // ZCS_INFO(g_logger) << "EPOLLIN event, fd=" << sockfd;
                memset(buf, '\0', 1024);
                int32_t data_read = 0, read_index = 0, checked_index = 0, start_line = 0;
                CHECK_STATUS check_status = CHECK_STATUS_REQUESTLINE;
                while(1) {
                    data_read = recv(sockfd, buf, 1024 - read_index, 0);
                    // 没有读到数据
                    if(data_read < 0) {
                        ZCS_ERROR(g_logger) << "read failed, ret=" << data_read;
                        break;
                    } else if(data_read == 0) {
                        ZCS_INFO(g_logger) << "client closed connection";
                        break;
                    }
                    
                    // 有数据
                    read_index += data_read;
                    // ZCS_DEBUG(g_logger) << "recv buf is : " << std::string(buf);
                    HTTP_CODE ret = parse_content(buf, checked_index, check_status, read_index, start_line);
                    if(ret == NO_REQUEST) {
                        // 
                        continue;
                    } else if(ret == GET_REQUEST) {
                        ZCS_DEBUG_FMT(g_logger, "send data : %s", rt_responses[0]);
                        send(sockfd, rt_responses[0], sizeof(rt_responses[0]), 0);
                        break;
                    } else {
                        send(sockfd, rt_responses[1], sizeof(rt_responses[1]), 0);
                        break;
                    }
                }
                close(sockfd);
                // int
                // ret = recv(sockfd, buf, 1024 - 1, 0);
                // ZCS_DEBUG(g_logger) << "recv num : " << ret;
                // if(ret <= 0) {
                //     close(sockfd);
                //     continue;
                // }
                // ZCS_DEBUG_FMT(g_logger, "recv data is : %s", buf);
                
                // {
                //     int op_case = buf[0] - '0';
                //     ZCS_DEBUG(g_logger) << "case : " << op_case;
                //     switch (op_case) {
                //         case 0:
                //             memset(buf, '\0', 1024);
                //             sprintf(buf, "replay message is one");
                //             break;
                //         case 1:
                //             memset(buf, '\0', 1024);
                //             sprintf(buf, "replay message is two");
                //             break;
                //         case 2:
                //             memset(buf, '\0', 1024);
                //             sprintf(buf, "replay message is three");
                //             break;
                //         default:
                //             memset(buf, '\0', 1024);
                //             sprintf(buf, "replay message is default");
                //             break;
                //     }
                //     // events[i].events &= ~EPOLLIN;
                //     // events[i].events |= EPOLLOUT;
                //     send(sockfd, buf, strlen(buf), 0);
                // }

                
            } else if(events[i].events & EPOLLOUT) {
                ZCS_DEBUG(g_logger) << "send message";
                send(sockfd, buf, strlen(buf), 0);
            } else {
                // other epoll events
                //ZCS_INFO(g_logger) << "other events";
            }
        }
    }

    // struct sockaddr_in client_addr;
    // socklen_t client_addr_len = sizeof(client_addr);
    // ZCS_INFO(g_logger) << "before accpet";
    // int connfd = accept(fd, (struct sockaddr*) &client_addr, &client_addr_len);
    // if(connfd < 0) {
    //     ZCS_ERROR(g_logger) << "accept failed, ret=" <<connfd;
    // } else {
    //     ZCS_INFO(g_logger) << "accept success!, connfd=" << connfd;
    // }

    // close(listen);
    close(fd);
    return 0;
}