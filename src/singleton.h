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