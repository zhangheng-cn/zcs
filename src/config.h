/**
 * @file config.h
 * @author zhcs (zhanghength@163.com)
 * @brief 要实现自定义类型的配置，需要完成LexicalCast对应的偏特化，然后可以和STL容器一起使用
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
#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <string>
#include <unordered_map>
#include <memory>
#include <boost/lexical_cast.hpp>
#include <yaml-cpp/yaml.h>
#include <list>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <vector>
#include <functional>

#include "log.h"

namespace zcs {

/**
 * @brief 约定优于配置
 * 
 * 
 * @details 
 */
class ConfigVarBase {
public:
    typedef std::shared_ptr<ConfigVarBase> ptr;

    ConfigVarBase(const std::string& name, const std::string& description = "")
        :name_(name)
        ,description_(description) {

    }
    virtual ~ConfigVarBase() {}

    const std::string& GetName() const { return name_;}
    const std::string& GetDescription() const { return description_;}


    virtual std::string ToString() = 0;
    virtual bool FromString(const std::string& val) = 0;

protected:
    std::string name_;
    std::string description_;
};

template<class F, class T>
class LexicalCast {
public:
    T operator()(const F& v) {
        return boost::lexical_cast<T>(v);
    }
};

/**
 * @brief string --> vector
 * 
 * @tparam T 
 * 
 * @details 
 */
template<class T>
class LexicalCast<std::string, std::vector<T> > {
public:
    std::vector<T> operator()(const std::string& v) {
        YAML::Node node = YAML::Load(v);
        std::vector<T> vec;
        std::stringstream ss;
        for(size_t i = 0; i < node.size(); i++) {
            ss.str("");
            ss << node[i];
            vec.push_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

/**
 * @brief vector --> string
 * 
 * @tparam T 
 * 
 * @details 
 */
template<class T>
class LexicalCast<std::vector<T>, std::string> {
public:
    std::string operator()(const std::vector<T>& v) {
        YAML::Node node(YAML::NodeType::Sequence);
        for(auto& i : v) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};


template<class T>
class LexicalCast<std::string, std::set<T> > {
public:
    std::set<T> operator()(const std::string& v) {
        YAML::Node node = YAML::Load(v);
        std::set<T> vec;
        std::stringstream ss;
        for(size_t i = 0; i < node.size(); i++) {
            ss.str("");
            ss << node[i];
            vec.insert(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

template<class T>
class LexicalCast<std::set<T>, std::string> {
public:
    std::string operator()(const std::set<T>& v) {
        YAML::Node node(YAML::NodeType::Sequence);
        for(auto& i : v) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template<class T>
class LexicalCast<std::string, std::unordered_set<T> > {
public:
    std::unordered_set<T> operator()(const std::string& v) {
        YAML::Node node = YAML::Load(v);
        std::unordered_set<T> vec;
        std::stringstream ss;
        for(size_t i = 0; i < node.size(); i++) {
            ss.str("");
            ss << node[i];
            vec.insert(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

template<class T>
class LexicalCast<std::unordered_set<T>, std::string> {
public:
    std::string operator()(const std::unordered_set<T>& v) {
        YAML::Node node(YAML::NodeType::Sequence);
        for(auto& i : v) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template<class T>
class LexicalCast<std::string, std::list<T> > {
public:
    std::list<T> operator()(const std::string& v) {
        YAML::Node node = YAML::Load(v);
        std::list<T> vec;
        std::stringstream ss;
        for(size_t i = 0; i < node.size(); i++) {
            ss.str("");
            ss << node[i];
            vec.push_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

template<class T>
class LexicalCast<std::list<T>, std::string> {
public:
    std::string operator()(const std::list<T>& v) {
        YAML::Node node(YAML::NodeType::Sequence);
        for(auto& i : v) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template<class T>
class LexicalCast<std::string, std::map<std::string, T> > {
public:
    std::map<std::string, T> operator()(const std::string& v) {
        YAML::Node node = YAML::Load(v);
        std::map<std::string, T> vec;
        std::stringstream ss;
        for(auto it = node.begin(); it != node.end(); it++) {
            ss.str("");
            ss << it->second;
            vec.insert(std::make_pair(it->first.Scalar(), LexicalCast<std::string, T>()(ss.str())));
        }
        return vec;
    }
};

template<class T>
class LexicalCast<std::map<std::string, T>, std::string> {
public:
    std::string operator()(const std::map<std::string, T>& v) {
        YAML::Node node(YAML::NodeType::Map);
        for(auto& i : v) {
            node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
            //node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template<class T>
class LexicalCast<std::string, std::unordered_map<std::string, T> > {
public:
    std::unordered_map<std::string, T> operator()(const std::string& v) {
        YAML::Node node = YAML::Load(v);
        std::unordered_map<std::string, T> vec;
        std::stringstream ss;
        for(auto it = node.begin(); it != node.end(); it++) {
            ss.str("");
            ss << it->second;
            vec.insert(std::make_pair(it->first.Scalar(), LexicalCast<std::string, T>()(ss.str())));
        }
        return vec;
    }
};

template<class T>
class LexicalCast<std::unordered_map<std::string, T>, std::string> {
public:
    std::string operator()(const std::unordered_map<std::string, T>& v) {
        YAML::Node node(YAML::NodeType::Map);
        for(auto& i : v) {
            node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
            //node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};


template<class T, class FromStr = LexicalCast<std::string, T>
                ,class ToStr = LexicalCast<T, std::string> >
class ConfigVar : public ConfigVarBase {
public:
    typedef std::shared_ptr<ConfigVar> ptr;
    typedef std::function<void(const T& new_val, const T& old_val)> on_change_cb;


    ConfigVar(const std::string& name, const T& default_value, const std::string& descripiton = "") 
        :ConfigVarBase(name, descripiton)
        ,val_(default_value) {
        }
    
    std::string ToString() override {
        try {
            // return boost::lexical_cast< >(val_);
            return ToStr()(val_);
        } catch (std::exception& e) {
            ZCS_ERROR(ZCS_LOG_ROOT()) << "Configvar::ToString exception "
                << e.what() << " convert : " << typeid(val_).name() << " to string" << " name=" << name_;
        }
        return "";
    }

    bool FromString(const std::string& val) override {
        try {
            // val_ = boost::lexical_cast<T>(val);
            SetValue(FromStr()(val));
            return true;
        } catch (std::exception& e) {
            ZCS_ERROR(ZCS_LOG_ROOT()) << "Configvar::FromString exception "
                << e.what() << " convert : string to" << typeid(val_).name() << " name=" << name_ << " - val=" << val;
        }
        return false;
    }

    const T GetValue() { return val_;}
    void SetValue(const T& v) {
        if(val_ == v) {
            return;
        }
        for(auto& i : cbs_) {
            i.second(v, val_);
        }
        val_ = v;
    }

    void AddListener(uint64_t id, on_change_cb cb) {
        cbs_[id] = cb;
    }

    void DelListener(u_int64_t id) {
        cbs_.erase(id);
    }

    void ClearListener() {
        cbs_.clear();
    }

public:
    T val_;
    std::map<uint64_t, on_change_cb> cbs_;
};

class Config {
public:
    typedef std::unordered_map<std::string, ConfigVarBase::ptr> ConfigVarMap;

    /**
     * @brief 
     * 
     * @tparam T 
     * @param [in] name 
     * @param [in] default_value 
     * @param [in] description 
     * @return ConfigVar<T>::ptr 
     * 
     * @details 
     */
    template<class T>
    static typename ConfigVar<T>::ptr LookUp(const std::string& name,
        const T& default_value, const std::string& description = "") {
            // find
            auto it = GetDatas().find(name);
            if(it != GetDatas().end()) {
                auto tmp = std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
                if(tmp) {
                    ZCS_INFO(ZCS_LOG_ROOT()) << "LookUp name: " << name << "exists";
                    return tmp;  
                } else {
                    // TODO
                    return nullptr;
                }
            }
            // not find 
            if(name.find_first_not_of("abcdefghijklmnopqrstuvwxyz._0123456789") != std::string::npos) {
                ZCS_ERROR(ZCS_LOG_ROOT()) << "Config::LookUp name invalid " << name;
                throw std::invalid_argument(name); 
            }

            typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_value, description));
            //ZCS_DEBUG(ZCS_LOG_ROOT()) << "name : " << name << " value : " << v->ToString();
            GetDatas()[name] = v;
            return v;
    }

    static ConfigVarBase::ptr LookUpBase(std::string& name);

    static void LoadFromYaml(const YAML::Node& root);
private:
    /**
     * @brief Get the Datas object
     * 
     * @return ConfigVarMap& 
     * 
     * @details 使用静态的方法去获取静态数据，防止在其他在其他静态数据访问该数据产生初始化顺序问题
     */
    static ConfigVarMap& GetDatas() {
        static ConfigVarMap datas_;
        return datas_;
    }
};


} // namespace zcs


#endif //__CONFIG_H__