#include "database.h"
#include <fstream>
#include <sstream>
#include <iostream>

/// Initialize the database connection and apply the schema.
/// Opens the SQLite database file, executes setup.sql to create tables,
/// and runs a migration to add a unique index on sensor_values(sensor_id, timestamp).
bool Database::init(const std::string& db_path, const std::string& setup_sql_path) {
    // Open (or create) the SQLite database file
    int rc = sqlite3_open(db_path.c_str(), &db_);
    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }

    // Turn on remote deletion when foreign keys are removed
    sqlite3_exec(db_, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);

    // Read and execute the schema SQL file
    std::string sql = execSQLFile(db_, setup_sql_path);
    char* err = nullptr;
    rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &err);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << (err ? err : "unknown") << std::endl;
        sqlite3_free(err);
        return false;
    }

    // Migration: add UNIQUE constraint on (sensor_id, timestamp) to prevent duplicate readings
    const char* migrate = "CREATE UNIQUE INDEX IF NOT EXISTS idx_sensor_values_unique ON sensor_values(sensor_id, timestamp);";
    rc = sqlite3_exec(db_, migrate, nullptr, nullptr, &err);
    if (rc != SQLITE_OK) {
        std::cerr << "Migration warning: " << (err ? err : "unknown") << std::endl;
        sqlite3_free(err);
    }

    return true;
}

/// Load sample data from seed.sql into the database.
bool Database::loadSeedData(const std::string& seed_sql_path) {
    std::string sql = execSQLFile(db_, seed_sql_path);
    char* err = nullptr;
    int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &err);
    if (rc != SQLITE_OK) {
        std::cerr << "Seed data error: " << (err ? err : "unknown") << std::endl;
        sqlite3_free(err);
        return false;
    }
    return true;
}

/// Read an entire SQL file into a single string.
/// Not suitable for very large files — reads all content into memory.
std::string Database::execSQLFile(sqlite3* db, const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) {
        std::cerr << "Cannot open SQL file: " << path << std::endl;
        return "";
    }
    std::stringstream buf;
    buf << f.rdbuf();
    return buf.str();
}

/// Get the raw SQLite handle for direct use.
sqlite3* Database::getHandle() {
    return db_;
}

/// Fetch all sensor types ordered by ID.
std::vector<SensorType> Database::getSensorTypes() {
    std::vector<SensorType> result;
    const char* sql = "SELECT type_id, type_name FROM sensor_types ORDER BY type_id;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return result;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        SensorType t;
        t.id = sqlite3_column_int(stmt, 0);
        t.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        result.push_back(t);
    }
    sqlite3_finalize(stmt);
    return result;
}

/// Fetch all gateways ordered by ID.
std::vector<Gateway> Database::getGateways() {
    std::vector<Gateway> result;
    const char* sql = "SELECT gateway_id, gateway_name, gateway_location FROM gateways ORDER BY gateway_id;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return result;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Gateway g;
        g.id = sqlite3_column_int(stmt, 0);
        g.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        g.location = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        result.push_back(g);
    }
    sqlite3_finalize(stmt);
    return result;
}

/// Fetch all locations ordered by ID.
std::vector<Location> Database::getLocations() {
    std::vector<Location> result;
    const char* sql = "SELECT location_id, location_gps, location_info FROM locations ORDER BY location_id;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return result;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Location l;
        l.id = sqlite3_column_int(stmt, 0);
        l.gps = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        l.info = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        result.push_back(l);
    }
    sqlite3_finalize(stmt);
    return result;
}

/// Fetch all sensors with their linked types resolved via sensor_type_link.
/// First query retrieves basic sensor data. Then for each sensor, a second query
/// fetches the associated sensor_types entries through the many-to-many link table.
std::vector<Sensor> Database::getSensors() {
    std::vector<Sensor> result;

    const char* sql = "SELECT s.sensor_id, s.sensor_name, s.gateway_id, s.location_id, s.extra_location_info "
                      "FROM sensors s ORDER BY s.sensor_id;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return result;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Sensor s;
        s.id = sqlite3_column_int(stmt, 0);
        s.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        s.gateway_id = sqlite3_column_int(stmt, 2);
        s.location_id = sqlite3_column_int(stmt, 3);
        s.extra_location_info = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        result.push_back(s);
    }
    sqlite3_finalize(stmt);

    // Resolve many-to-many: fetch linked sensor types for each sensor
    for (auto& s : result) {
        std::stringstream ss;
        ss << "SELECT st.type_id, st.type_name "
           << "FROM sensor_type_link stl "
           << "JOIN sensor_types st ON stl.type_id = st.type_id "
           << "WHERE stl.sensor_id = " << s.id << ";";

        sqlite3_stmt* stmt2;
        if (sqlite3_prepare_v2(db_, ss.str().c_str(), -1, &stmt2, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmt2) == SQLITE_ROW) {
                SensorType t;
                t.id = sqlite3_column_int(stmt2, 0);
                t.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt2, 1));
                s.types.push_back(t);
            }
            sqlite3_finalize(stmt2);
        }
    }

    return result;
}

/// Fetch sensor readings, with different limits based on scope.
/// When sensor_id >= 0: fetch up to 50 most recent readings for that sensor.
/// When sensor_id < 0: fetch up to 100 most recent readings across all sensors.
std::vector<SensorValue> Database::getSensorValues(int sensor_id) {
    std::vector<SensorValue> result;
    sqlite3_stmt* stmt;

    if (sensor_id >= 0) {
        const char* sql = "SELECT id, sensor_id, timestamp, value FROM sensor_values WHERE sensor_id = ? ORDER BY timestamp DESC LIMIT 50;";
        if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, sensor_id);
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                SensorValue v;
                v.id = sqlite3_column_int(stmt, 0);
                v.sensor_id = sqlite3_column_int(stmt, 1);
                v.timestamp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
                v.value = sqlite3_column_double(stmt, 3);
                result.push_back(v);
            }
            sqlite3_finalize(stmt);
        }
    } else {
        const char* sql = "SELECT id, sensor_id, timestamp, value FROM sensor_values ORDER BY timestamp DESC LIMIT 100;";
        if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                SensorValue v;
                v.id = sqlite3_column_int(stmt, 0);
                v.sensor_id = sqlite3_column_int(stmt, 1);
                v.timestamp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
                v.value = sqlite3_column_double(stmt, 3);
                result.push_back(v);
            }
            sqlite3_finalize(stmt);
        }
    }

    return result;
}

/// Insert a new gateway row and return its auto-generated ID.
int Database::addGateway(const std::string& name, const std::string& location) {
    const char* sql = "INSERT INTO gateways (gateway_name, gateway_location) VALUES (?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return -1;

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, location.c_str(), -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    int id = sqlite3_last_insert_rowid(db_);
    sqlite3_finalize(stmt);
    return id;
}

/// Insert a new location row and return its auto-generated ID.
int Database::addLocation(const std::string& gps, const std::string& info) {
    const char* sql = "INSERT INTO locations (location_gps, location_info) VALUES (?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return -1;

    sqlite3_bind_text(stmt, 1, gps.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, info.c_str(), -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    int id = sqlite3_last_insert_rowid(db_);
    sqlite3_finalize(stmt);
    return id;
}

/// Insert a new sensor row and return its auto-generated ID.
int Database::addSensor(const std::string& name, int gateway_id, int location_id, const std::string& extra) {
    const char* sql = "INSERT INTO sensors (sensor_name, gateway_id, location_id, extra_location_info) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return -1;

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
    
    // Check if valid ID, otherwise bind NULL to satisfy Foreign Key constraints
    if (gateway_id > 0) sqlite3_bind_int(stmt, 2, gateway_id);
    else sqlite3_bind_null(stmt, 2);
    
    if (location_id > 0) sqlite3_bind_int(stmt, 3, location_id);
    else sqlite3_bind_null(stmt, 3);
    
    sqlite3_bind_text(stmt, 4, extra.c_str(), -1, SQLITE_STATIC);
    
    int rc = sqlite3_step(stmt);
    int id = -1;
    
    // Only return an ID if the insert actually succeeded
    if (rc == SQLITE_DONE) {
        id = sqlite3_last_insert_rowid(db_);
    } else {
        std::cerr << "[SQLite Error] Failed to add sensor: " << sqlite3_errmsg(db_) << std::endl;
    }
    
    sqlite3_finalize(stmt);
    return id;
}

/// Insert a new sensor type row and return its auto-generated ID.
int Database::addSensorType(const std::string& name) {
    const char* sql = "INSERT INTO sensor_types (type_name) VALUES (?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return -1;

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    int id = sqlite3_last_insert_rowid(db_);
    sqlite3_finalize(stmt);
    return id;
}

/// Insert a new sensor value reading and return its auto-generated ID.
int Database::addSensorValue(int sensor_id, const std::string& timestamp, double value) {
    const char* sql = "INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return -1;

    sqlite3_bind_int(stmt, 1, sensor_id);
    sqlite3_bind_text(stmt, 2, timestamp.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 3, value);
    sqlite3_step(stmt);
    int id = sqlite3_last_insert_rowid(db_);
    sqlite3_finalize(stmt);
    return id;
}

/// Insert a row into sensor_type_link to associate a sensor with a type.
/// Silently ignores errors (e.g., duplicate link) using CHECK in prepare_v2.
void Database::linkSensorToType(int sensor_id, int type_id) {
    const char* sql = "INSERT INTO sensor_type_link (sensor_id, type_id) VALUES (?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, sensor_id);
        sqlite3_bind_int(stmt, 2, type_id);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
}

/// Delete a sensor and its type links (in that order to respect FK constraints).
int Database::deleteSensor(int sensor_id) {
    const char* sql1 = "DELETE FROM sensor_type_link WHERE sensor_id = ?;";
    sqlite3_stmt* stmt1;
    if (sqlite3_prepare_v2(db_, sql1, -1, &stmt1, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt1, 1, sensor_id);
        sqlite3_step(stmt1);
        sqlite3_finalize(stmt1);
    }

    const char* sql2 = "DELETE FROM sensors WHERE sensor_id = ?;";
    sqlite3_stmt* stmt2;
    if (sqlite3_prepare_v2(db_, sql2, -1, &stmt2, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt2, 1, sensor_id);
        int rc = sqlite3_step(stmt2);
        sqlite3_finalize(stmt2);
        return (rc == SQLITE_DONE) ? 1 : 0;
    }
    return 0;
}

/// Delete a gateway after nullifying its FK references in the sensors table.
int Database::deleteGateway(int gateway_id) {
    const char* sql1 = "UPDATE sensors SET gateway_id = NULL WHERE gateway_id = ?;";
    sqlite3_stmt* stmt1;
    if (sqlite3_prepare_v2(db_, sql1, -1, &stmt1, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt1, 1, gateway_id);
        sqlite3_step(stmt1);
        sqlite3_finalize(stmt1);
    }

    const char* sql2 = "DELETE FROM gateways WHERE gateway_id = ?;";
    sqlite3_stmt* stmt2;
    if (sqlite3_prepare_v2(db_, sql2, -1, &stmt2, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt2, 1, gateway_id);
        int rc = sqlite3_step(stmt2);
        sqlite3_finalize(stmt2);
        return (rc == SQLITE_DONE) ? 1 : 0;
    }
    return 0;
}

/// Delete a location after nullifying its FK references in the sensors table.
int Database::deleteLocation(int location_id) {
    const char* sql1 = "UPDATE sensors SET location_id = NULL WHERE location_id = ?;";
    sqlite3_stmt* stmt1;
    if (sqlite3_prepare_v2(db_, sql1, -1, &stmt1, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt1, 1, location_id);
        sqlite3_step(stmt1);
        sqlite3_finalize(stmt1);
    }

    const char* sql2 = "DELETE FROM locations WHERE location_id = ?;";
    sqlite3_stmt* stmt2;
    if (sqlite3_prepare_v2(db_, sql2, -1, &stmt2, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt2, 1, location_id);
        int rc = sqlite3_step(stmt2);
        sqlite3_finalize(stmt2);
        return (rc == SQLITE_DONE) ? 1 : 0;
    }
    return 0;
}

/// Delete a sensor type and its associated links in sensor_type_link.
int Database::deleteSensorType(int type_id) {
    const char* sql1 = "DELETE FROM sensor_type_link WHERE type_id = ?;";
    sqlite3_stmt* stmt1;
    if (sqlite3_prepare_v2(db_, sql1, -1, &stmt1, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt1, 1, type_id);
        sqlite3_step(stmt1);
        sqlite3_finalize(stmt1);
    }

    const char* sql2 = "DELETE FROM sensor_types WHERE type_id = ?;";
    sqlite3_stmt* stmt2;
    if (sqlite3_prepare_v2(db_, sql2, -1, &stmt2, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt2, 1, type_id);
        int rc = sqlite3_step(stmt2);
        sqlite3_finalize(stmt2);
        return (rc == SQLITE_DONE) ? 1 : 0;
    }
    return 0;
}

/// Delete a single sensor value reading by ID.
int Database::deleteSensorValue(int value_id) {
    const char* sql = "DELETE FROM sensor_values WHERE id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, value_id);
        int rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        return (rc == SQLITE_DONE) ? 1 : 0;
    }
    return 0;
}

// Unlink sensor from type
void Database::unlinkSensorFromType(int sensor_id, int type_id) {
    const char* sql = "DELETE FROM sensor_type_link WHERE sensor_id = ? AND type_id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, sensor_id);
        sqlite3_bind_int(stmt, 2, type_id);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
}

/// Update a sensor's properties.
bool Database::updateSensor(int sensor_id, const std::string& name, int gateway_id, int location_id, const std::string& extra) {
    const char* sql = "UPDATE sensors SET sensor_name = ?, gateway_id = ?, location_id = ?, extra_location_info = ? WHERE sensor_id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
        
        // Handle Foreign Keys here as well
        if (gateway_id > 0) sqlite3_bind_int(stmt, 2, gateway_id);
        else sqlite3_bind_null(stmt, 2);
        
        if (location_id > 0) sqlite3_bind_int(stmt, 3, location_id);
        else sqlite3_bind_null(stmt, 3);
        
        sqlite3_bind_text(stmt, 4, extra.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 5, sensor_id);
        
        int rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            std::cerr << "[SQLite Error] Failed to update sensor: " << sqlite3_errmsg(db_) << std::endl;
        }
        
        sqlite3_finalize(stmt);
        return (rc == SQLITE_DONE);
    }
    return false;
}

/// Update a gateway's name and location.
bool Database::updateGateway(int gateway_id, const std::string& name, const std::string& location) {
    const char* sql = "UPDATE gateways SET gateway_name = ?, gateway_location = ? WHERE gateway_id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, location.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 3, gateway_id);
        int rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        return (rc == SQLITE_DONE);
    }
    return false;
}

/// Update a location's GPS coordinates and info description.
bool Database::updateLocation(int location_id, const std::string& gps, const std::string& info) {
    const char* sql = "UPDATE locations SET location_gps = ?, location_info = ? WHERE location_id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, gps.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, info.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 3, location_id);
        int rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        return (rc == SQLITE_DONE);
    }
    return false;
}

/// Update a sensor type's name.
bool Database::updateSensorType(int type_id, const std::string& name) {
    const char* sql = "UPDATE sensor_types SET type_name = ? WHERE type_id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, type_id);
        int rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        return (rc == SQLITE_DONE);
    }
    return false;
}

/// Update a sensor value's sensor link, timestamp, and reading value.
bool Database::updateSensorValue(int value_id, int sensor_id, const std::string& timestamp, double value) {
    const char* sql = "UPDATE sensor_values SET sensor_id = ?, timestamp = ?, value = ? WHERE id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, sensor_id);
        sqlite3_bind_text(stmt, 2, timestamp.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 3, value);
        sqlite3_bind_int(stmt, 4, value_id);
        int rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        return (rc == SQLITE_DONE);
    }
    return false;
}
