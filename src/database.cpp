#include "database.h"
#include <fstream>
#include <sstream>
#include <iostream>

bool Database::init(const std::string& db_path, const std::string& setup_sql_path) {
    int rc = sqlite3_open(db_path.c_str(), &db_);
    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }

    std::string sql = execSQLFile(db_, setup_sql_path);
    char* err = nullptr;
    rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &err);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << (err ? err : "unknown") << std::endl;
        sqlite3_free(err);
        return false;
    }
    return true;
}

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

sqlite3* Database::getHandle() {
    return db_;
}

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

    // For each sensor, fetch linked types via sensor_type_link and sensor_types
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

int Database::addSensor(const std::string& name, int gateway_id, int location_id, const std::string& extra) {
    const char* sql = "INSERT INTO sensors (sensor_name, gateway_id, location_id, extra_location_info) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return -1;

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, gateway_id);
    sqlite3_bind_int(stmt, 3, location_id);
    sqlite3_bind_text(stmt, 4, extra.c_str(), -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    int id = sqlite3_last_insert_rowid(db_);
    sqlite3_finalize(stmt);
    return id;
}

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
