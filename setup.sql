-- setup.sql: Database schema in 3NF for sensor data storage
-- Creates 6 tables: sensor_types, gateways, locations, sensors, sensor_type_link, sensor_values
-- All tables follow third normal form (3NF) with proper foreign key relationships

-- Sensor type definitions (temperature, humidity, light_intensity, etc.)
CREATE TABLE IF NOT EXISTS sensor_types (
    type_id INTEGER PRIMARY KEY AUTOINCREMENT,  -- Unique type identifier
    type_name TEXT NOT NULL UNIQUE              -- Type name (e.g., "temperature")
);

-- LoRaWAN gateway nodes that collect sensor data
CREATE TABLE IF NOT EXISTS gateways (
    gateway_id INTEGER PRIMARY KEY AUTOINCREMENT,  -- Unique gateway identifier
    gateway_name TEXT NOT NULL UNIQUE,             -- Gateway name (e.g., "Delft Gateway")
    gateway_location TEXT NOT NULL                 -- GPS coordinates of gateway
);

-- Physical sensor installation locations
CREATE TABLE IF NOT EXISTS locations (
    location_id INTEGER PRIMARY KEY AUTOINCREMENT,  -- Unique location identifier
    location_gps TEXT NOT NULL,                     -- GPS coordinates
    location_info TEXT NOT NULL                     -- Human-readable location description
);

-- Individual sensors linked to a gateway and physical location
CREATE TABLE IF NOT EXISTS sensors (
    sensor_id INTEGER PRIMARY KEY AUTOINCREMENT,   -- Unique sensor identifier
    gateway_id INTEGER NOT NULL,                    -- FK: which gateway collects this sensor's data
    sensor_name TEXT NOT NULL,                      -- Sensor name (e.g., "Temp Sensor Dorm A")
    location_id INTEGER NOT NULL,                   -- FK: physical installation location
    extra_location_info TEXT DEFAULT '',            -- Additional installation notes
    FOREIGN KEY (gateway_id) REFERENCES gateways(gateway_id),
    FOREIGN KEY (location_id) REFERENCES locations(location_id)
);

-- Many-to-many link table: a sensor can have multiple types, a type can apply to multiple sensors
CREATE TABLE IF NOT EXISTS sensor_type_link (
    link_id INTEGER PRIMARY KEY AUTOINCREMENT,  -- Unique link identifier
    type_id INTEGER NOT NULL,                    -- FK: sensor type
    sensor_id INTEGER NOT NULL,                  -- FK: sensor
    FOREIGN KEY (type_id) REFERENCES sensor_types(type_id),
    FOREIGN KEY (sensor_id) REFERENCES sensors(sensor_id)
);

-- Time-series sensor readings (values table)
CREATE TABLE IF NOT EXISTS sensor_values (
    id INTEGER PRIMARY KEY AUTOINCREMENT,  -- Unique reading identifier
    sensor_id INTEGER NOT NULL,            -- FK: which sensor this reading belongs to
    timestamp TEXT NOT NULL,               -- ISO format: YYYY-MM-DD HH:MM:SS
    value REAL NOT NULL,                   -- Numeric sensor reading
    FOREIGN KEY (sensor_id) REFERENCES sensors(sensor_id),
    UNIQUE(sensor_id, timestamp)           -- Prevent duplicate readings at same timestamp
);
