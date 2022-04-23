/**
 * @file singleton.h
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
#ifndef __SINGLETON_H__
#define __SINGLETON_H__

namespace zcs {
    
template<class T>
class Singleton {
public:
    static T* GetInstance() {
        static T v;
        return &v;
    }
};

} // namespace zcs


#endif //__SINGLETON_H__