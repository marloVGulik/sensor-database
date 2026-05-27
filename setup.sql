-- setup.sql: Database schema in 3NF for sensor data storage

CREATE TABLE IF NOT EXISTS sensor_types (
    type_id INTEGER PRIMARY KEY AUTOINCREMENT,
    type_name TEXT NOT NULL UNIQUE
);

CREATE TABLE IF NOT EXISTS gateways (
    gateway_id INTEGER PRIMARY KEY AUTOINCREMENT,
    gateway_name TEXT NOT NULL UNIQUE,
    gateway_location TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS locations (
    location_id INTEGER PRIMARY KEY AUTOINCREMENT,
    location_gps TEXT NOT NULL,
    location_info TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS sensors (
    sensor_id INTEGER PRIMARY KEY AUTOINCREMENT,
    gateway_id INTEGER NOT NULL,
    sensor_name TEXT NOT NULL,
    location_id INTEGER NOT NULL,
    extra_location_info TEXT DEFAULT '',
    FOREIGN KEY (gateway_id) REFERENCES gateways(gateway_id),
    FOREIGN KEY (location_id) REFERENCES locations(location_id)
);

CREATE TABLE IF NOT EXISTS sensor_type_link (
    link_id INTEGER PRIMARY KEY AUTOINCREMENT,
    type_id INTEGER NOT NULL,
    sensor_id INTEGER NOT NULL,
    FOREIGN KEY (type_id) REFERENCES sensor_types(type_id),
    FOREIGN KEY (sensor_id) REFERENCES sensors(sensor_id)
);

CREATE TABLE IF NOT EXISTS sensor_values (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    sensor_id INTEGER NOT NULL,
    timestamp TEXT NOT NULL,
    value REAL NOT NULL,
    FOREIGN KEY (sensor_id) REFERENCES sensors(sensor_id)
);
