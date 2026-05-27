-- seed.sql: Sample data for sensor database

-- Sensor types
INSERT INTO sensor_types (type_name) VALUES ('temperature');
INSERT INTO sensor_types (type_name) VALUES ('humidity');
INSERT INTO sensor_types (type_name) VALUES ('light_intensity');
INSERT INTO sensor_types (type_name) VALUES ('pax_counter');
INSERT INTO sensor_types (type_name) VALUES ('gps_data');
INSERT INTO sensor_types (type_name) VALUES ('gateway_info');
INSERT INTO sensor_types (type_name) VALUES ('time');

-- Gateways (European/Netherlands locations)
INSERT INTO gateways (gateway_name, gateway_location) VALUES ('Delft Gateway', '52.0115, 4.3571');
INSERT INTO gateways (gateway_name, gateway_location) VALUES ('Rotterdam Gateway', '51.9244, 4.4777');

-- Locations
INSERT INTO locations (location_gps, location_info) VALUES ('52.0115, 4.3571', 'Dormitory A - Room 101');
INSERT INTO locations (location_gps, location_info) VALUES ('52.0120, 4.3580', 'Lab B - Floor 2');
INSERT INTO locations (location_gps, location_info) VALUES ('52.0125, 4.3590', 'Greenhouse - Campus south');
INSERT INTO locations (location_gps, location_info) VALUES ('52.0130, 4.3600', 'Classroom 3A - Main building');
INSERT INTO locations (location_gps, location_info) VALUES ('52.0135, 4.3610', 'Main entrance lobby');
INSERT INTO locations (location_gps, location_info) VALUES ('52.0140, 4.3620', 'Roof - North tower');
INSERT INTO locations (location_gps, location_info) VALUES ('52.0145, 4.3630', 'Basement - Server room');
INSERT INTO locations (location_gps, location_info) VALUES ('52.0150, 4.3640', 'Corridor - Building C');
INSERT INTO locations (location_gps, location_info) VALUES ('52.0155, 4.3650', 'Library - 1st floor');
INSERT INTO locations (location_gps, location_info) VALUES ('52.0160, 4.3660', 'Outdoor weather station');

-- Sensors (10 total)
INSERT INTO sensors (sensor_name, gateway_id, location_id, extra_location_info) VALUES ('Temp Sensor Dorm A', 1, 1, 'Wall mounted, 1.5m height');
INSERT INTO sensors (sensor_name, gateway_id, location_id, extra_location_info) VALUES ('Temp Sensor Lab B', 1, 2, 'Near window, south facing');
INSERT INTO sensors (sensor_name, gateway_id, location_id, extra_location_info) VALUES ('Humidity Sensor Greenhouse', 1, 3, 'Central pole mount');
INSERT INTO sensors (sensor_name, gateway_id, location_id, extra_location_info) VALUES ('Light Sensor Classroom', 1, 4, 'Ceiling mounted');
INSERT INTO sensors (sensor_name, gateway_id, location_id, extra_location_info) VALUES ('Pax Counter Main', 2, 5, 'Doorway entry point');
INSERT INTO sensors (sensor_name, gateway_id, location_id, extra_location_info) VALUES ('GPS Node Roof', 2, 6, 'External GPS antenna');
INSERT INTO sensors (sensor_name, gateway_id, location_id, extra_location_info) VALUES ('Gateway Info Sensor', 2, 7, 'RSSI and packet stats');
INSERT INTO sensors (sensor_name, gateway_id, location_id, extra_location_info) VALUES ('Multi-sensor Node 1', 1, 8, 'Temp + humidity + light');
INSERT INTO sensors (sensor_name, gateway_id, location_id, extra_location_info) VALUES ('Multi-sensor Node 2', 1, 9, 'Temp + humidity + light');
INSERT INTO sensors (sensor_name, gateway_id, location_id, extra_location_info) VALUES ('Outdoor Weather Station', 2, 10, 'Anemometer + temp + humidity');

-- Sensor type links
-- Sensor 1: temperature
INSERT INTO sensor_type_link (type_id, sensor_id) VALUES (1, 1);
-- Sensor 2: temperature
INSERT INTO sensor_type_link (type_id, sensor_id) VALUES (1, 2);
-- Sensor 3: humidity
INSERT INTO sensor_type_link (type_id, sensor_id) VALUES (2, 3);
-- Sensor 4: light_intensity
INSERT INTO sensor_type_link (type_id, sensor_id) VALUES (3, 4);
-- Sensor 5: pax_counter
INSERT INTO sensor_type_link (type_id, sensor_id) VALUES (4, 5);
-- Sensor 6: gps_data
INSERT INTO sensor_type_link (type_id, sensor_id) VALUES (5, 6);
-- Sensor 7: gateway_info
INSERT INTO sensor_type_link (type_id, sensor_id) VALUES (6, 7);
-- Sensor 8: temperature, humidity, light_intensity (multi-sensor)
INSERT INTO sensor_type_link (type_id, sensor_id) VALUES (1, 8);
INSERT INTO sensor_type_link (type_id, sensor_id) VALUES (2, 8);
INSERT INTO sensor_type_link (type_id, sensor_id) VALUES (3, 8);
-- Sensor 9: temperature, humidity, light_intensity (multi-sensor)
INSERT INTO sensor_type_link (type_id, sensor_id) VALUES (1, 9);
INSERT INTO sensor_type_link (type_id, sensor_id) VALUES (2, 9);
INSERT INTO sensor_type_link (type_id, sensor_id) VALUES (3, 9);
-- Sensor 10: temperature, humidity, light_intensity (weather station)
INSERT INTO sensor_type_link (type_id, sensor_id) VALUES (1, 10);
INSERT INTO sensor_type_link (type_id, sensor_id) VALUES (2, 10);
INSERT INTO sensor_type_link (type_id, sensor_id) VALUES (3, 10);

-- Sensor values (sample readings with timestamps)
-- Temp Sensor Dorm A (sensor_id=1, type=temperature, range -5 to 35)
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (1, '2026-05-20 08:00:00', 21.5);
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (1, '2026-05-20 12:00:00', 23.0);
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (1, '2026-05-20 16:00:00', 22.8);
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (1, '2026-05-20 20:00:00', 20.5);
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (1, '2026-05-21 08:00:00', 21.0);

-- Temp Sensor Lab B (sensor_id=2, type=temperature, range -5 to 35)
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (2, '2026-05-20 08:00:00', 24.2);
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (2, '2026-05-20 12:00:00', 26.5);
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (2, '2026-05-20 16:00:00', 25.8);
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (2, '2026-05-20 20:00:00', 23.1);
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (2, '2026-05-21 08:00:00', 24.0);

-- Humidity Sensor Greenhouse (sensor_id=3, type=humidity, range 30-95)
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (3, '2026-05-20 08:00:00', 72.0);
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (3, '2026-05-20 12:00:00', 68.5);
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (3, '2026-05-20 16:00:00', 75.2);
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (3, '2026-05-20 20:00:00', 80.1);
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (3, '2026-05-21 08:00:00', 73.5);

-- Light Sensor Classroom (sensor_id=4, type=light_intensity, range 0-100000)
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (4, '2026-05-20 08:00:00', 150.0);
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (4, '2026-05-20 12:00:00', 45000.0);
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (4, '2026-05-20 16:00:00', 32000.0);
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (4, '2026-05-20 20:00:00', 450.0);
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (4, '2026-05-21 08:00:00', 200.0);

-- Pax Counter Main (sensor_id=5, type=pax_counter, range 0-500)
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (5, '2026-05-20 08:00:00', 45);
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (5, '2026-05-20 12:00:00', 312);
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (5, '2026-05-20 16:00:00', 278);
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (5, '2026-05-20 20:00:00', 89);
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (5, '2026-05-21 08:00:00', 34);

-- GPS Node Roof (sensor_id=6, type=gps_data, European coordinates)
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (6, '2026-05-20 08:00:00', 52.0140);
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (6, '2026-05-20 12:00:00', 52.0141);
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (6, '2026-05-20 16:00:00', 52.0140);
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (6, '2026-05-20 20:00:00', 52.0139);
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (6, '2026-05-21 08:00:00', 52.0140);

-- Gateway Info Sensor (sensor_id=7, type=gateway_info)
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (7, '2026-05-20 08:00:00', -65);
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (7, '2026-05-20 12:00:00', -72);
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (7, '2026-05-20 16:00:00', -68);
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (7, '2026-05-20 20:00:00', -70);
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (7, '2026-05-21 08:00:00', -66);

-- Multi-sensor Node 1 (sensor_id=8, types: temp, humidity, light)
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (8, '2026-05-20 08:00:00', 22.1);
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (8, '2026-05-20 12:00:00', 24.5);
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (8, '2026-05-20 16:00:00', 23.8);
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (8, '2026-05-20 20:00:00', 21.2);
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (8, '2026-05-21 08:00:00', 22.0);

-- Multi-sensor Node 2 (sensor_id=9, types: temp, humidity, light)
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (9, '2026-05-20 08:00:00', 19.8);
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (9, '2026-05-20 12:00:00', 21.3);
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (9, '2026-05-20 16:00:00', 20.9);
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (9, '2026-05-20 20:00:00', 18.5);
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (9, '2026-05-21 08:00:00', 19.5);

-- Outdoor Weather Station (sensor_id=10, types: temp, humidity, light)
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (10, '2026-05-20 08:00:00', 16.2);
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (10, '2026-05-20 12:00:00', 19.8);
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (10, '2026-05-20 16:00:00', 18.5);
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (10, '2026-05-20 20:00:00', 15.1);
INSERT INTO sensor_values (sensor_id, timestamp, value) VALUES (10, '2026-05-21 08:00:00', 16.0);
