-- seed.sql: Sample data for sensor database
-- Populates all 6 tables with realistic data for 10 sensors across 2 gateways

-- ============================================================
-- Sensor types (7 types)
-- ============================================================
INSERT OR IGNORE INTO sensor_types (type_name) VALUES ('temperature');
INSERT OR IGNORE INTO sensor_types (type_name) VALUES ('humidity');
INSERT OR IGNORE INTO sensor_types (type_name) VALUES ('light_intensity');
INSERT OR IGNORE INTO sensor_types (type_name) VALUES ('pax_counter');
INSERT OR IGNORE INTO sensor_types (type_name) VALUES ('gps_data');
INSERT OR IGNORE INTO sensor_types (type_name) VALUES ('gateway_info');
INSERT OR IGNORE INTO sensor_types (type_name) VALUES ('time');

-- ============================================================
-- Gateways (2 gateways in Netherlands)
-- ============================================================
INSERT OR IGNORE INTO gateways (gateway_name, gateway_location) VALUES ('Delft Gateway', '52.0115, 4.3571');
INSERT OR IGNORE INTO gateways (gateway_name, gateway_location) VALUES ('Rotterdam Gateway', '51.9244, 4.4777');

-- ============================================================
-- Locations (10 campus locations)
-- ============================================================
INSERT OR IGNORE INTO locations (location_gps, location_info) VALUES ('52.0115, 4.3571', 'Dormitory A - Room 101');
INSERT OR IGNORE INTO locations (location_gps, location_info) VALUES ('52.0120, 4.3580', 'Lab B - Floor 2');
INSERT OR IGNORE INTO locations (location_gps, location_info) VALUES ('52.0125, 4.3590', 'Greenhouse - Campus south');
INSERT OR IGNORE INTO locations (location_gps, location_info) VALUES ('52.0130, 4.3600', 'Classroom 3A - Main building');
INSERT OR IGNORE INTO locations (location_gps, location_info) VALUES ('52.0135, 4.3610', 'Main entrance lobby');
INSERT OR IGNORE INTO locations (location_gps, location_info) VALUES ('52.0140, 4.3620', 'Roof - North tower');
INSERT OR IGNORE INTO locations (location_gps, location_info) VALUES ('52.0145, 4.3630', 'Basement - Server room');
INSERT OR IGNORE INTO locations (location_gps, location_info) VALUES ('52.0150, 4.3640', 'Corridor - Building C');
INSERT OR IGNORE INTO locations (location_gps, location_info) VALUES ('52.0155, 4.3650', 'Library - 1st floor');
INSERT OR IGNORE INTO locations (location_gps, location_info) VALUES ('52.0160, 4.3660', 'Outdoor weather station');

-- ============================================================
-- Sensors (10 total)
-- ============================================================
INSERT OR IGNORE INTO sensors (sensor_name, gateway_id, location_id, extra_location_info) VALUES ('Temp Sensor Dorm A', 1, 1, 'Wall mounted, 1.5m height');
INSERT OR IGNORE INTO sensors (sensor_name, gateway_id, location_id, extra_location_info) VALUES ('Temp Sensor Lab B', 1, 2, 'Near window, south facing');
INSERT OR IGNORE INTO sensors (sensor_name, gateway_id, location_id, extra_location_info) VALUES ('Humidity Sensor Greenhouse', 1, 3, 'Central pole mount');
INSERT OR IGNORE INTO sensors (sensor_name, gateway_id, location_id, extra_location_info) VALUES ('Light Sensor Classroom', 1, 4, 'Ceiling mounted');
INSERT OR IGNORE INTO sensors (sensor_name, gateway_id, location_id, extra_location_info) VALUES ('Pax Counter Main', 2, 5, 'Doorway entry point');
INSERT OR IGNORE INTO sensors (sensor_name, gateway_id, location_id, extra_location_info) VALUES ('GPS Node Roof', 2, 6, 'External GPS antenna');
INSERT OR IGNORE INTO sensors (sensor_name, gateway_id, location_id, extra_location_info) VALUES ('Gateway Info Sensor', 2, 7, 'RSSI and packet stats');
INSERT OR IGNORE INTO sensors (sensor_name, gateway_id, location_id, extra_location_info) VALUES ('Multi-sensor Node 1', 1, 8, 'Temp + humidity + light');
INSERT OR IGNORE INTO sensors (sensor_name, gateway_id, location_id, extra_location_info) VALUES ('Multi-sensor Node 2', 1, 9, 'Temp + humidity + light');
INSERT OR IGNORE INTO sensors (sensor_name, gateway_id, location_id, extra_location_info) VALUES ('Outdoor Weather Station', 2, 10, 'Anemometer + temp + humidity');

-- ============================================================
-- Sensor type links (many-to-many: sensor_id=8,9,10 have 3 types each)
-- ============================================================
INSERT OR IGNORE INTO sensor_type_link (type_id, sensor_id) VALUES (1, 1);   -- Temp Sensor Dorm A -> temperature
INSERT OR IGNORE INTO sensor_type_link (type_id, sensor_id) VALUES (1, 2);   -- Temp Sensor Lab B -> temperature
INSERT OR IGNORE INTO sensor_type_link (type_id, sensor_id) VALUES (2, 3);   -- Humidity Sensor -> humidity
INSERT OR IGNORE INTO sensor_type_link (type_id, sensor_id) VALUES (3, 4);   -- Light Sensor -> light_intensity
INSERT OR IGNORE INTO sensor_type_link (type_id, sensor_id) VALUES (4, 5);   -- Pax Counter -> pax_counter
INSERT OR IGNORE INTO sensor_type_link (type_id, sensor_id) VALUES (5, 6);   -- GPS Node -> gps_data
INSERT OR IGNORE INTO sensor_type_link (type_id, sensor_id) VALUES (6, 7);   -- Gateway Info -> gateway_info
INSERT OR IGNORE INTO sensor_type_link (type_id, sensor_id) VALUES (1, 8);   -- Multi-sensor Node 1 -> temperature
INSERT OR IGNORE INTO sensor_type_link (type_id, sensor_id) VALUES (2, 8);   -- Multi-sensor Node 1 -> humidity
INSERT OR IGNORE INTO sensor_type_link (type_id, sensor_id) VALUES (3, 8);   -- Multi-sensor Node 1 -> light_intensity
INSERT OR IGNORE INTO sensor_type_link (type_id, sensor_id) VALUES (1, 9);   -- Multi-sensor Node 2 -> temperature
INSERT OR IGNORE INTO sensor_type_link (type_id, sensor_id) VALUES (2, 9);   -- Multi-sensor Node 2 -> humidity
INSERT OR IGNORE INTO sensor_type_link (type_id, sensor_id) VALUES (3, 9);   -- Multi-sensor Node 2 -> light_intensity
INSERT OR IGNORE INTO sensor_type_link (type_id, sensor_id) VALUES (1, 10);  -- Outdoor Weather Station -> temperature
INSERT OR IGNORE INTO sensor_type_link (type_id, sensor_id) VALUES (2, 10);  -- Outdoor Weather Station -> humidity
INSERT OR IGNORE INTO sensor_type_link (type_id, sensor_id) VALUES (3, 10);  -- Outdoor Weather Station -> light_intensity

-- ============================================================
-- Sensor values (5 readings per sensor, 2 days x 2x daily)
-- ============================================================

-- Temp Sensor Dorm A (sensor_id=1, type=temperature, range -5 to 35)
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (1, '2026-05-20 08:00:00', 21.5);
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (1, '2026-05-20 12:00:00', 23.0);
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (1, '2026-05-20 16:00:00', 22.8);
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (1, '2026-05-20 20:00:00', 20.5);
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (1, '2026-05-21 08:00:00', 21.0);

-- Temp Sensor Lab B (sensor_id=2, type=temperature, range -5 to 35)
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (2, '2026-05-20 08:00:00', 24.2);
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (2, '2026-05-20 12:00:00', 26.5);
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (2, '2026-05-20 16:00:00', 25.8);
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (2, '2026-05-20 20:00:00', 23.1);
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (2, '2026-05-21 08:00:00', 24.0);

-- Humidity Sensor Greenhouse (sensor_id=3, type=humidity, range 30-95)
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (3, '2026-05-20 08:00:00', 72.0);
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (3, '2026-05-20 12:00:00', 68.5);
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (3, '2026-05-20 16:00:00', 75.2);
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (3, '2026-05-20 20:00:00', 80.1);
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (3, '2026-05-21 08:00:00', 73.5);

-- Light Sensor Classroom (sensor_id=4, type=light_intensity, range 0-100000)
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (4, '2026-05-20 08:00:00', 150.0);
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (4, '2026-05-20 12:00:00', 45000.0);
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (4, '2026-05-20 16:00:00', 32000.0);
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (4, '2026-05-20 20:00:00', 450.0);
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (4, '2026-05-21 08:00:00', 200.0);

-- Pax Counter Main (sensor_id=5, type=pax_counter, range 0-500)
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (5, '2026-05-20 08:00:00', 45);
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (5, '2026-05-20 12:00:00', 312);
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (5, '2026-05-20 16:00:00', 278);
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (5, '2026-05-20 20:00:00', 89);
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (5, '2026-05-21 08:00:00', 34);

-- GPS Node Roof (sensor_id=6, type=gps_data, European coordinates)
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (6, '2026-05-20 08:00:00', 52.0140);
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (6, '2026-05-20 12:00:00', 52.0141);
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (6, '2026-05-20 16:00:00', 52.0140);
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (6, '2026-05-20 20:00:00', 52.0139);
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (6, '2026-05-21 08:00:00', 52.0140);

-- Gateway Info Sensor (sensor_id=7, type=gateway_info)
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (7, '2026-05-20 08:00:00', -65);
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (7, '2026-05-20 12:00:00', -72);
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (7, '2026-05-20 16:00:00', -68);
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (7, '2026-05-20 20:00:00', -70);
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (7, '2026-05-21 08:00:00', -66);

-- Multi-sensor Node 1 (sensor_id=8, types: temp, humidity, light)
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (8, '2026-05-20 08:00:00', 22.1);
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (8, '2026-05-20 12:00:00', 24.5);
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (8, '2026-05-20 16:00:00', 23.8);
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (8, '2026-05-20 20:00:00', 21.2);
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (8, '2026-05-21 08:00:00', 22.0);

-- Multi-sensor Node 2 (sensor_id=9, types: temp, humidity, light)
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (9, '2026-05-20 08:00:00', 19.8);
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (9, '2026-05-20 12:00:00', 21.3);
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (9, '2026-05-20 16:00:00', 20.9);
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (9, '2026-05-20 20:00:00', 18.5);
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (9, '2026-05-21 08:00:00', 19.5);

-- Outdoor Weather Station (sensor_id=10, types: temp, humidity, light)
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (10, '2026-05-20 08:00:00', 16.2);
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (10, '2026-05-20 12:00:00', 19.8);
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (10, '2026-05-20 16:00:00', 18.5);
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (10, '2026-05-20 20:00:00', 15.1);
INSERT OR IGNORE INTO sensor_values (sensor_id, timestamp, value) VALUES (10, '2026-05-21 08:00:00', 16.0);
