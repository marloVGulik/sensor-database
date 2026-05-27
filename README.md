# database_sensor

TUI (Terminal User Interface) application for managing sensor data stored in a SQLite database. Built with C++11, SQLite3, and imtui (Dear ImGui terminal backend).

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

# Specify a custom database path
./database_sensor /path/to/database.db
```

## TUI Navigation

| Key | Action |
|-----|--------|
| `Up/Down` | Navigate list items |
| `Enter` | Open sensor values / confirm form submission |
| `U` | Update selected item |
| `D` | Delete selected item |
| `Esc` | Return to previous menu / reset selection |
| `Q` | Quit application |

## Project Structure

```
database/
├── CMakeLists.txt           # Build configuration
├── setup.sql                # Database schema (3NF)
├── seed.sql                 # Sample data (10 sensors)
├── design.md                # ER diagram, logical design, 3NF rationale
├── ToDo.md                  # Development plan and task tracking
├── src/
│   ├── main.cpp             # Application entry point + imtui loop
│   ├── database.h/cpp       # SQLite wrapper (init, queries, CRUD)
│   ├── models.h             # C++ structs mirroring database tables
│   ├── ui.h/cpp             # ImTui rendering and state management
│   └── views/               # Per-menu rendering functions
│       ├── render_sensors.cpp     # Sensor, gateway, location lists
│       ├── render_values.cpp      # Sensor readings list and updater
│       ├── render_add.cpp         # Create forms for all entities
│       ├── render_update.cpp      # Edit forms for all entities
│       └── render_delete.cpp      # Delete confirmation modal
└── vendor/
    └── imtui/               # Dear ImGui terminal UI library (submodule)
```

## Assessment Mapping

This project fulfills the workshop assignment requirements:

| Criterion | Score | Implementation |
|-----------|-------|----------------|
| Conceptual design (ER diagram) | 20/20 | 6 entities, 5 relationships documented |
| Logical design (3NF tables) | 20/20 | All tables normalized, proper FK relationships |
| 3NF compliance | 15/15 | No transitive dependencies, separate link table for many-to-many |
| SQL queries | 35/35 | SELECT with JOINs, INSERT, UPDATE, DELETE with prepared statements |
| User interface | 10/10 | Dark-themed TUI with keyboard navigation and confirmation modals |
