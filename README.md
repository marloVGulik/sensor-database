# database_sensor

GUI application for managing sensor data stored in a SQLite database. Built with C++11, SQLite3, and ImGui. Based on GLFW/OpenGL

## Purpose

This application provides a keyboard-navigable interface for viewing and managing sensor data from embedded devices (Raspberry Pi / LoRaWAN nodes). It supports CRUD operations on sensors, gateways, locations, sensor types, and sensor readings.

## Database Schema

The application uses 6 tables in 3NF:

| Table | Description |
|-------|-------------|
| `sensor_types` | Definitions of sensor types (temperature, humidity, etc.) |
| `gateways` | LoRaWAN gateway nodes |
| `locations` | Physical GPS locations |
| `sensors` | Individual sensors linked to gateways and locations |
| `sensor_type_link` | Many-to-many relationship between sensors and types |
| `sensor_values` | Time-series sensor readings |

See `design.md` for the conceptual ER diagram and logical design details.

## Dependencies

- **CMake** >= 3.10
- **SQLite3** development library (`libsqlite3-dev`)
- **ncurses** development library (`libncurses-dev`)
- **imtui** (vendored in `vendor/imtui/`)

## Build Instructions

```bash
mkdir -p build && cd build
cmake ..
make
```

The SQL files (`setup.sql`, `seed.sql`) are automatically copied to the build directory by CMake.

## Usage

```bash
# Use default database path (sensor_data.db in current directory)
./database_sensor
```

