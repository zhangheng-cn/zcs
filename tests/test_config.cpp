#include "yaml-cpp/yaml.h"
#include "zcs.h"
#include <iostream>

static zcs::Logger::ptr g_log =  ZCS_LOG_ROOT();
static zcs::Logger::ptr g_system = ZCS_LOG_NAME("system");

zcs::ConfigVar<int>::ptr g_int_config = zcs::Config::LookUp("system.port", (int)8080, "system port");
zcs::ConfigVar<float>::ptr g_float_config = zcs::Config::LookUp("system.value", (float)13.14, "system value");
zcs::ConfigVar<std::vector<int>>::ptr g_int_vec_config = zcs::Config::LookUp("system.int_vec", std::vector<int>{1,2,3}, "system int vec");
zcs::ConfigVar<std::list<int>>::ptr g_int_list_config = zcs::Config::LookUp("system.int_list", std::list<int>{1,2,3}, "system int list");
zcs::ConfigVar<std::set<int>>::ptr g_int_set_config = zcs::Config::LookUp("system.int_set", std::set<int>{1,2,3}, "system int set");
zcs::ConfigVar<std::unordered_set<int>>::ptr g_int_uset_config = zcs::Config::LookUp("system.int_uset", std::unordered_set<int>{1,2,3}, "system int uset");
zcs::ConfigVar<std::map<std::string, int>>::ptr g_int_map_config = zcs::Config::LookUp("system.int_map", std::map<std::string, int>{{"k",1}, {"k2",2}}, "system int map");
zcs::ConfigVar<std::unordered_map<std::string, int>>::ptr g_int_umap_config = zcs::Config::LookUp("system.int_umap", std::unordered_map<std::string, int>{{"k", 10}, {"k2", 20}}, "system int umap");



void print_yaml(YAML::Node node, int level) {
    if(node.IsNull()) {
        ZCS_DEBUG(g_log) << std::string(level * 4, ' ') << "NULL > " << node.Type() << " < " << level;
    } else if(node.IsScalar()) {
        ZCS_DEBUG(g_log) << std::string(level * 4, ' ') << node.Scalar() << " > " << node.Type() << " < " << level;
    } else if(node.IsSequence()) {
        for(size_t i = 0; i < node.size(); i++) {
            ZCS_DEBUG(g_log) << std::string(level * 4, ' ') << i << " > " << node[i].Type() << " < " << level;
            print_yaml(node[i], level + 1);
        }
    } else if(node.IsMap()) {
        for(auto it = node.begin(); it != node.end(); it++) {
            ZCS_DEBUG(g_log) << std::string(level * 4, ' ') << it->first << " > " << it->second.Type() << " < " << level;
            print_yaml(it->second, level + 1);
        }
    }
}

void test_config() {
    ZCS_DEBUG(g_log) << "before system port: " << g_int_config->GetValue();
    ZCS_DEBUG(g_log) << "before system value: " << g_float_config->ToString();
#define XX(g_var, name, perfix) \
    {\
        auto v = g_var->GetValue();\
        for(auto& i : v) { \
            ZCS_DEBUG(g_log) << #perfix " " #name " "<< i; \
        }\
        ZCS_DEBUG(g_log) << g_var->ToString();\
    }

#define XX_M(g_var, name, perfix) \
    {\
        auto v = g_var->GetValue();\
        for(auto& i : v) { \
            ZCS_DEBUG(g_log) << #perfix " " #name " " << i.first << " - " << i.second ; \
        }\
        ZCS_DEBUG(g_log) << g_var->ToString();\
    }

    XX(g_int_vec_config, int_vec, before)
    XX(g_int_list_config, int_list, before)
    XX(g_int_set_config, int_set, before)
    XX(g_int_uset_config, int_uset, before)
    XX_M(g_int_map_config, int_map, before)
    XX_M(g_int_umap_config, int_umap, before)

    YAML::Node root = YAML::LoadFile("/home/zh/code/zcs/tests/log.yml");
    zcs::Config::LoadFromYaml(root);

    ZCS_DEBUG(g_log) << "after system port: " << g_int_config->GetValue();
    ZCS_DEBUG(g_log) << "after system value: " << g_float_config->ToString();


    XX(g_int_vec_config, int_vec, after)
    XX(g_int_list_config, int_list, after)
    XX(g_int_set_config, int_set, after)
    XX(g_int_uset_config, int_uset, after)
    XX_M(g_int_map_config, int_map, after)
    XX_M(g_int_umap_config, int_umap, after)

#undef XX
#undef XX_M
}

class Person {
public:
    std::string name_;
    int32_t age_ = 0;
    bool sex_ = false;

    std::string ToString() const {
        std::stringstream ss;
        ss << "[person : name - " << name_
           << " age - " << age_
           << " sex - " << sex_
           << "]";
        return ss.str();
    }

    bool operator==(const Person& other) const {
        return other.name_ == name_
            && other.age_ == age_
            && other.sex_ == sex_;
    }
};

namespace zcs {

/**
 * @brief 自定义类型Person config的偏特化
 * 
 * @tparam  
 * 
 * @details 
 */
template<>
class LexicalCast<std::string, Person> {
public:
    Person operator()(const std::string& v) {
        YAML::Node node = YAML::Load(v);
        Person p;
        p.name_ = node["name"].as<std::string>();
        p.age_ = node["age"].as<int32_t>();
        p.sex_ = node["sex"].as<bool>();
        return p;
    }
};

template<>
class LexicalCast<Person, std::string> {
public:
    std::string operator()(const Person& p) {
        YAML::Node node;
        node["name"] = p.name_;
        node["age"] = p.age_;
        node["sex"] = p.sex_;
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

}

zcs::ConfigVar<Person>::ptr g_config_person = zcs::Config::LookUp("class.person", Person(), "class person");
zcs::ConfigVar<std::map<std::string, Person> >::ptr g_config_map_person = zcs::Config::LookUp("class.map", std::map<std::string, Person>{}, "class map person");
zcs::ConfigVar<std::map<std::string, std::vector<Person>> >::ptr g_config_map_vec_person = zcs::Config::LookUp("class.map_vec", std::map<std::string, std::vector<Person>>{}, "class map vec person");


void test_class() {

#define XX_MP(g_var, perfix) \
        ZCS_DEBUG(g_log) << perfix << g_var->GetValue().ToString();

#define XX_PM(g_var, perfix) \
    {\
        auto& v = g_var->GetValue(); \
        for(auto it : v) {\
            ZCS_DEBUG(g_log) << perfix << "name : " << it.first << " value : " << it.second.ToString();\
        }\
        ZCS_DEBUG(g_log) << "size : " << v.size();\
    }

    XX_MP(g_config_person, "before class person ")
    XX_PM(g_config_map_person, "before ");
    ZCS_DEBUG(g_log) << "before : " << g_config_map_vec_person->ToString();

    g_config_person->AddListener(10, [](const Person& new_val, const Person& old_val){
        ZCS_DEBUG(g_log) << "[change happend]old : " << old_val.ToString() << " new :" << new_val.ToString();
    });

    YAML::Node root = YAML::LoadFile("/home/zh/code/zcs/tests/test.yml");
    zcs::Config::LoadFromYaml(root);

    XX_MP(g_config_person, "after class person ");
    XX_PM(g_config_map_person, "after ");
    ZCS_DEBUG(g_log) << "after : " << g_config_map_vec_person->ToString();
    ZCS_DEBUG(g_system) << "after : " << g_config_map_vec_person->ToString();
}

void test_log() {
    std::cout << zcs::LoggerMgr::GetInstance()->ToYamlString() << std::endl;
    
    ZCS_INFO(g_log) << "logggggggggg";
    YAML::Node root = YAML::LoadFile("/home/zh/code/zcs/tests/log.yml");
    zcs::Config::LoadFromYaml(root);
    ZCS_INFO(g_log) << "logggggggggg";
    std::cout << zcs::LoggerMgr::GetInstance()->ToYamlString() << std::endl;
}

int main(int argc, char* argv[]) {
    YAML::Node root = YAML::LoadFile("/home/zh/code/zcs/tests/log.yml");
    //ZCS_DEBUG(g_log) << root;
    //test_config();
    //print_yaml(root, 0);
    //test_class();
    test_log();
    return 0;
}