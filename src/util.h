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
#include <vector>
#include <stdint.h>
#include <string>

namespace zcs {

pid_t GetThreadId();
pid_t GetFiberId();

void Backtrace(std::vector<std::string>& bt, uint32_t size = 64, uint32_t skip = 1);
std::string BacktraceToString(uint32_t size = 64, uint32_t skip = 2, const std::string& perfix = "");


}

#endif //__UTIL_H__