#pragma once

#include <string>
#include <vector>

/// Data struct mirroring the sensor_types table.
/// Maps to a single row in sensor_types.
struct SensorType {
    int id = 0;           ///< Unique identifier (type_id)
    std::string name;     ///< Human-readable type name (e.g., "temperature", "humidity")
};

/// Data struct mirroring the gateways table.
/// Maps to a single row in gateways.
struct Gateway {
    int id = 0;           ///< Unique identifier (gateway_id)
    std::string name;     ///< Gateway name (e.g., "Delft Gateway")
    std::string location; ///< GPS coordinates stored as text
};

/// Data struct mirroring the locations table.
/// Maps to a single row in locations.
struct Location {
    int id = 0;           ///< Unique identifier (location_id)
    std::string gps;      ///< GPS coordinates
    std::string info;     ///< Human-readable location description
};

/// Data struct representing a sensor with its resolved type relationships.
/// Combines data from sensors and sensor_type_link tables.
struct Sensor {
    int id = 0;               ///< Unique identifier (sensor_id)
    std::string name;         ///< Sensor name
    int gateway_id = 0;       ///< FK to gateways.gateway_id
    int location_id = 0;      ///< FK to locations.location_id
    std::string extra_location_info; ///< Additional sensor location notes
    std::vector<SensorType> types; ///< Resolved many-to-many: sensor types linked to this sensor
};

/// Data struct mirroring the sensor_values table.
/// Maps to a single row in sensor_values (time-series reading).
struct SensorValue {
    int id = 0;           ///< Unique identifier
    int sensor_id = 0;    ///< FK to sensors.sensor_id
    std::string timestamp; ///< ISO-format timestamp (YYYY-MM-DD HH:MM:SS)
    double value = 0.0;   ///< Sensor reading value
};
