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