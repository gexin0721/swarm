#ifndef DATACORE_H
#define DATACORE_H

#include <string>
#include <memory>
#include <odb/database.hxx>
#include <odb/sqlite/database.hxx>

class Datacore {
public:
    Datacore(const std::string& db_path = "swarm.db");
    ~Datacore();

    // Initialize database
    bool init();
    
    // Database operations
    bool saveSwarmData(const std::string& node_id, const std::string& data);
    
    // Get database instance
    std::shared_ptr<odb::database> getDatabase() { return db_; }

private:
    std::string db_path_;
    std::shared_ptr<odb::database> db_;
};

#endif // DATACORE_H
