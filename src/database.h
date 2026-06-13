#pragma once

#include "models.h"
#include <sqlite3.h>
#include <string>
#include <vector>

/// SQLite database wrapper for the sensor data application.
/// Handles database initialization, schema execution, seed data loading,
/// and all CRUD operations using prepared statements.
class Database {
public:
    /// Initialize the database: open the file and execute setup.sql schema.
    /// @param db_path Path to the SQLite database file (creates if not exists)
    /// @param setup_sql_path Path to the SQL schema file (setup.sql)
    /// @return true if initialization succeeded, false on error
    bool init(const std::string& db_path, const std::string& setup_sql_path);

    /// Load sample data from a SQL file into the already-initialized database.
    /// @param seed_sql_path Path to the SQL seed data file (seed.sql)
    /// @return true if loading succeeded, false on error
    bool loadSeedData(const std::string& seed_sql_path);

    // ==================== Query Methods ====================

    /// Retrieve all sensor types from the database.
    /// @return Vector of SensorType structs, ordered by type_id
    std::vector<SensorType> getSensorTypes();

    /// Retrieve all gateways from the database.
    /// @return Vector of Gateway structs, ordered by gateway_id
    std::vector<Gateway> getGateways();

    /// Retrieve all locations from the database.
    /// @return Vector of Location structs, ordered by location_id
    std::vector<Location> getLocations();

    /// Retrieve all sensors with their linked types resolved.
    /// Executes a query on sensors table, then fetches types via sensor_type_link JOIN.
    /// @return Vector of Sensor structs with populated types field
    std::vector<Sensor> getSensors();

    /// Retrieve sensor values, optionally filtered by sensor.
    /// When sensor_id >= 0: returns up to 50 readings for that sensor (most recent first).
    /// When sensor_id < 0: returns up to 100 readings across all sensors (most recent first).
    /// @param sensor_id Sensor to filter by, or -1 for all sensors
    /// @return Vector of SensorValue structs ordered by timestamp DESC
    std::vector<SensorValue> getSensorValues(int sensor_id = -1);

    // ==================== Insert Methods ====================

    /// Insert a new gateway and return its ID.
    /// @param name Gateway name
    /// @param location GPS coordinates string
    /// @return New gateway_id, or -1 on error
    int addGateway(const std::string& name, const std::string& location);

    /// Insert a new location and return its ID.
    /// @param gps GPS coordinates
    /// @param info Human-readable location description
    /// @return New location_id, or -1 on error
    int addLocation(const std::string& gps, const std::string& info);

    /// Insert a new sensor and return its ID.
    /// @param name Sensor name
    /// @param gateway_id FK to gateways table
    /// @param location_id FK to locations table
    /// @param extra Optional additional location info
    /// @return New sensor_id, or -1 on error
    int addSensor(const std::string& name, int gateway_id, int location_id, const std::string& extra = "");

    /// Insert a new sensor type and return its ID.
    /// @param name Type name (must be unique)
    /// @return New type_id, or -1 on error
    int addSensorType(const std::string& name);

    /// Insert a new sensor value reading and return its ID.
    /// @param sensor_id FK to sensors table
    /// @param timestamp ISO-format timestamp string
    /// @param value Numeric sensor reading
    /// @return New value id, or -1 on error
    int addSensorValue(int sensor_id, const std::string& timestamp, double value);

    /// Link a sensor to a type via the sensor_type_link table.
    /// No error handling — silently ignores duplicates (INSERT OR IGNORE).
    /// @param sensor_id FK to sensors table
    /// @param type_id FK to sensor_types table
    void linkSensorToType(int sensor_id, int type_id);

    // ==================== Delete Methods ====================

    /// Delete a sensor and its type links (cascading).
    /// @param sensor_id FK to sensors table
    /// @return 1 if deleted successfully, 0 otherwise
    int deleteSensor(int sensor_id);

    /// Delete a gateway (nullifies FK in sensors first).
    /// @param gateway_id FK to gateways table
    /// @return 1 if deleted successfully, 0 otherwise
    int deleteGateway(int gateway_id);

    /// Delete a location (nullifies FK in sensors first).
    /// @param location_id FK to locations table
    /// @return 1 if deleted successfully, 0 otherwise
    int deleteLocation(int location_id);

    /// Delete a sensor type and its links (cascading).
    /// @param type_id FK to sensor_types table
    /// @return 1 if deleted successfully, 0 otherwise
    int deleteSensorType(int type_id);

    /// Delete a single sensor value reading.
    /// @param value_id FK to sensor_values.id
    /// @return 1 if deleted successfully, 0 otherwise
    int deleteSensorValue(int value_id);

    // ==================== Update Methods ====================

    /// Update a sensor's properties.
    /// @param sensor_id FK to sensors table
    /// @param name New sensor name
    /// @param gateway_id New gateway FK
    /// @param location_id New location FK
    /// @param extra New extra location info
    /// @return true if update succeeded
    bool updateSensor(int sensor_id, const std::string& name, int gateway_id, int location_id, const std::string& extra);

    /// Update a gateway's properties.
    /// @param gateway_id FK to gateways table
    /// @param name New gateway name
    /// @param location New GPS location
    /// @return true if update succeeded
    bool updateGateway(int gateway_id, const std::string& name, const std::string& location);

    /// Update a location's properties.
    /// @param location_id FK to locations table
    /// @param gps New GPS coordinates
    /// @param info New location description
    /// @return true if update succeeded
    bool updateLocation(int location_id, const std::string& gps, const std::string& info);

    /// Update a sensor type's name.
    /// @param type_id FK to sensor_types table
    /// @param name New type name
    /// @return true if update succeeded
    bool updateSensorType(int type_id, const std::string& name);

    /// Update a sensor value reading.
    /// @param value_id FK to sensor_values.id
    /// @param sensor_id New sensor FK
    /// @param timestamp New timestamp
    /// @param value New reading value
    /// @return true if update succeeded
    bool updateSensorValue(int value_id, int sensor_id, const std::string& timestamp, double value);

    // ==================== Utility Methods ====================

    /// Get the raw SQLite database handle.
    /// @warning Use with caution — exposes internal state.
    /// @return Pointer to sqlite3 handle, or nullptr if not initialized
    sqlite3* getHandle();

    /// Read an entire SQL file into a string.
    /// Used internally by init() and loadSeedData() to execute SQL files.
    /// @param db SQLite database handle (unused, kept for future extensibility)
    /// @param path Path to the SQL file
    /// @return SQL content as string, or empty string on error
    static std::string execSQLFile(sqlite3* db, const std::string& path);

private:
    sqlite3* db_ = nullptr; ///< Raw SQLite database connection handle
};
