#ifndef __HTTP_H__
#define __HTTP_H__


#define BUFFER_SZIE 4096

enum CHECK_STATUS {
    CHECK_STATUS_REQUESTLINE = 0,
    CHECK_STATUS_HEADER
};

enum LINE_STATUS {
    LINE_OK = 0,
    LINE_BAD,
    LINE_OPEN
};

enum HTTP_CODE {
    NO_REQUEST,
    GET_REQUEST,
    BAD_REQUEST,
    FORBIDDEN_REQUEST,
    INTERNAL_ERROR,
    CLOSED_CONNECTION
};



LINE_STATUS parse_line(char* buffer, int& checked_index, int& read_index);

HTTP_CODE parse_requestline(char* temp, CHECK_STATUS& check_status);

HTTP_CODE parse_headers(char* temp);

HTTP_CODE parse_content(char* buffer, int& checked_index, CHECK_STATUS& check_status
                ,int& read_index, int& start_line);


#endif //__HTTP_H__