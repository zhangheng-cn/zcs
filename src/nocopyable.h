/**
 * @file nocopyable.h
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
#ifndef __NOCOPYABLE_H__
#define __NOCOPYABLE_H__

namespace zcs {
    
class Nocopyable {
public:
    Nocopyable() = default;
    ~Nocopyable() = default;
    Nocopyable(const Nocopyable&) = delete;
    Nocopyable& operator=(const Nocopyable&) = delete;
};

} // namespace zcs


#endif //__NOCOPYABLE_H__