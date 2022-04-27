# 日志模块
```cpp
  Logger 
    |
    |
    |---- LogFormatter
    |
    |
LogAppender
```
- 仿照log4j,有logger,logformatter,logappender实现日志格输出，支持流和格式化的日志输出，自定义配置日志格式
# 配置模块
```cpp
  ConfigVarBase
        |
    ConfigVar
        |
     Config
```
- 通过yaml进行配置(约定优于配置)
  1. 支持标准类型的配置
  2. 支持STL常见容器的配置(vector, list, set, unordered_set, map, unordered_map)
  3. 支持自定义类型的配置(需要先完成对应类型的偏特化LexicalCast),可与STL结合使用
- 对配置的变更注册监听时间，当配置发生变化时通知相应模块
# 线程模块
- 对pthread的简单封装及相应的信号量互斥量
- 各种锁log写入量测试(初步测试)
  | 锁 | 写入速度 |
  | :---: | :---: |
  | Mutex | 30MB/s |
  | SpinLock | 40MB/s |
  | CASLock | 40MB/s |
# 协程模块

# 参考
- [https://github.com/qinguoyi/TinyWebServer](https://github.com/qinguoyi/TinyWebServer) 
- [https://github.com/sylar-yin/sylar](https://github.com/sylar-yin/sylar)
  