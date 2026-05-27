#pragma once

#include <string>
#include <vector>

struct SensorType {
    int id = 0;
    std::string name;
};

struct Gateway {
    int id = 0;
    std::string name;
    std::string location;
};

struct Location {
    int id = 0;
    std::string gps;
    std::string info;
};

struct Sensor {
    int id = 0;
    std::string name;
    int gateway_id = 0;
    int location_id = 0;
    std::string extra_location_info;
    std::vector<SensorType> types;
};

struct SensorValue {
    int id = 0;
    int sensor_id = 0;
    std::string timestamp;
    double value = 0.0;
};
