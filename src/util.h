/**
 * @file util.h
 * @author zhcs (zhanghength@163.com)
 * @brief 
 * @version 0.1
 * @date 2022-04-21
 * 
 * @copyright Copyright (c) 2022 zhanghength@163.com
 * 
 * @pra 修改日志:
 * <table>
 * <tr><th>Date   <th>Version <th>Author  <th>Description
 * <tr><th>2022-04-21 <th>1.0 <th>zhcs    <th>create
 * </table>
 */
#ifndef __UTIL_H__
#define __UTIL_H__


#include <unistd.h>

namespace zcs {

pid_t GetThreadId();
pid_t GetFiberId();

}

#endif //__UTIL_H__