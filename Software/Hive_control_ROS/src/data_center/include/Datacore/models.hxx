#ifndef MODELS_HXX
#define MODELS_HXX

#include <string>
#include <memory>
#include <odb/core.hxx>

#pragma db object
class SwarmData {
public:
    SwarmData() {}
    SwarmData(const std::string& node_id, const std::string& data)
        : node_id_(node_id), data_(data) {}

    unsigned long id() const { return id_; }
    const std::string& node_id() const { return node_id_; }
    const std::string& data() const { return data_; }
    
    void set_node_id(const std::string& id) { node_id_ = id; }
    void set_data(const std::string& data) { data_ = data; }

private:
    friend class odb::access;

    #pragma db id auto
    unsigned long id_;
    
    std::string node_id_;
    std::string data_;
};

#endif // MODELS_HXX
