#include "http.h"
#include "log.h"
#include <string.h>

static zcs::Logger::ptr lg = ZCS_LOG_NAME("net");

LINE_STATUS parse_line(char* buffer, int& checked_index, int& read_index) {
    char temp;

    ZCS_DEBUG(lg) << "checked_index : " << checked_index << " read_index: " << read_index << "\n" << std::string(buffer); 
    for(; checked_index < read_index; checked_index++) {
        temp = buffer[checked_index];
        if(temp == '\r') {
            if(checked_index + 1 == read_index) {
                return LINE_OPEN;
            } else if(buffer[checked_index + 1] == '\n') {
                buffer[checked_index++] = '\0';
                buffer[checked_index++] = '\0';
                return LINE_OK;
            }
            return LINE_BAD;
        } else if(temp == '\n') {
            if(checked_index > 1 && buffer[checked_index - 1] == '\r') {
                buffer[checked_index - 1] = '\0';
                buffer[checked_index++] = '\0';
                return LINE_OK;
            }
            return LINE_BAD;
        }
    }

    return LINE_OPEN;
}

HTTP_CODE parse_requestline(char* temp, CHECK_STATUS& check_status) {
    char* url = strpbrk(temp, " \t");
    ZCS_DEBUG_FMT(lg, "url : %s", url);
    if(!url) {
        return BAD_REQUEST;
    }
    *url++ = '\0';
    ZCS_DEBUG_FMT(lg, "url : %s", url);
    char* method = temp;
    if(strcasecmp(method, "GET") == 0) {
        ZCS_DEBUG(lg) << "requeset method is GET";
    } else {
        return BAD_REQUEST;
    }

    url += strspn(url, " \t");
    ZCS_DEBUG_FMT(lg, "url : %s", url);
    char* version = strpbrk(url, " \t");
    if(!version) {
        return BAD_REQUEST;
    }
    *version++ = '\0';
    version += strspn(version, " \t");
    if(strcasecmp(version, "HTTP/1.1") != 0) {
        return BAD_REQUEST;
    }
    ZCS_DEBUG_FMT(lg, "url : %s", url);
    if(strncasecmp(url, "http://", 7) == 0) {
        url += 7;
        url = strchr(url, '/');
    }

    if(!url || url[0] != '/') {
        return BAD_REQUEST;
    }

    ZCS_DEBUG(lg) << "request url is : " << std::string(url);

    check_status = CHECK_STATUS_HEADER;

    return NO_REQUEST;
}

HTTP_CODE parse_headers(char* temp) {
    return GET_REQUEST;
}

HTTP_CODE parse_content(char* buffer, int& checked_index, CHECK_STATUS& check_status
                ,int& read_index, int& start_line) {
    LINE_STATUS line_status = LINE_OK;
    HTTP_CODE rt_code = NO_REQUEST;
    ZCS_DEBUG(lg) << "recv buf : \n" << std::string(buffer);
    while( (line_status = parse_line(buffer, checked_index, read_index)) == LINE_OK) {
        ZCS_DEBUG(lg) << "LINE_OK";
        char* temp = buffer + start_line;
        start_line = checked_index;
        switch(check_status) {
            // 请求行
            case CHECK_STATUS_REQUESTLINE:
                rt_code = parse_requestline(temp, check_status);
                if(rt_code == BAD_REQUEST) {
                    return BAD_REQUEST;
                }
                break;
            // 请求头
            case CHECK_STATUS_HEADER:
                rt_code = parse_headers(temp);
                if(rt_code == BAD_REQUEST) return BAD_REQUEST;
                if(rt_code == GET_REQUEST) return GET_REQUEST;
                break;
            default:
                return INTERNAL_ERROR;
                break;
        }
    }

    if(line_status == LINE_OPEN) {
        return NO_REQUEST;
    }

    return BAD_REQUEST;
}