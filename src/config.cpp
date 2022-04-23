#include "config.h"

namespace zcs {

static zcs::Logger::ptr g_logger = ZCS_LOG_NAME("system");

static void ListAllMember(const std::string& perfix,
                          const YAML::Node& node,
                          std::list<std::pair<std::string, const YAML::Node>>& output) {
    if(perfix.find_first_not_of("abcdefghikjlmnopqrstuvwxyz._0123456789") != std::string::npos) {
        ZCS_ERROR(g_logger) << "Config invalid name : " << perfix << " node : " << node;
        return;
    }

    output.push_back(std::make_pair(perfix, node));
    //ZCS_DEBUG(g_logger) << "push back [" << perfix << " : "<< node << "]";
    if(node.IsMap()) {
        for(auto it = node.begin(); it != node.end(); it++) {
            ListAllMember(perfix.empty() ? it->first.Scalar() : perfix + "." + it->first.Scalar(), it->second, output);
        }
    }
    // else if(node.IsSequence()) {
    //     for(size_t i = 0; i < node.size(); i++) {
    //         ListAllMember(perfix.empty() ? node[i].Scalar() : perfix + "." + node[i].Scalar(), node[i], output);
    //     }
    // }
}

ConfigVarBase::ptr Config::LookUpBase(std::string& name) {
    auto it = GetDatas().find(name);
    // ZCS_DEBUG(ZCS_LOG_ROOT()) << "[look up base] name : " << name << ", value :"
    //     << (it == GetDatas().end()) ? " NULL" : it->second->GetName();
    return it == GetDatas().end() ? nullptr : it->second;
}

void Config::LoadFromYaml(const YAML::Node& root) {
    std::list<std::pair<std::string, const YAML::Node>> all_nodes;
    ListAllMember("", root, all_nodes);

    for(auto& it : all_nodes) {
        std::string key = it.first;
        if(key.empty()) {
            continue;
        }

        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        ConfigVarBase::ptr var = LookUpBase(key);

        if(var) {
            if(it.second.IsScalar()) {
                var->FromString(it.second.Scalar());
            } else {
                std::stringstream ss;
                ss << it.second;
                var->FromString(ss.str());
            }
        }
    }
}
    
} // namespace zcs
