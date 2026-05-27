#pragma once

#include "models.h"
#include <sqlite3.h>
#include <string>
#include <vector>

class Database {
public:
    bool init(const std::string& db_path, const std::string& setup_sql_path);
    bool loadSeedData(const std::string& seed_sql_path);

    std::vector<SensorType> getSensorTypes();
    std::vector<Gateway> getGateways();
    std::vector<Location> getLocations();
    std::vector<Sensor> getSensors();
    std::vector<SensorValue> getSensorValues(int sensor_id = -1);

    int addGateway(const std::string& name, const std::string& location);
    int addLocation(const std::string& gps, const std::string& info);
    int addSensor(const std::string& name, int gateway_id, int location_id, const std::string& extra = "");
    int addSensorType(const std::string& name);
    int addSensorValue(int sensor_id, const std::string& timestamp, double value);
    void linkSensorToType(int sensor_id, int type_id);

    sqlite3* getHandle();
    static std::string execSQLFile(sqlite3* db, const std::string& path);

private:
    sqlite3* db_ = nullptr;
};
