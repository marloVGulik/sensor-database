# ToDo.md - Database Sensor Program Plan

## Overview
Build a C++ application using SQLite3 and imtui (Dear ImGui terminal UI) to store, display, and manage sensor data from embedded devices (Raspberry Pi / LoRaWAN nodes).

## Project Structure
```
database/
├── CMakeLists.txt           # Build configuration
├── setup.sql                # Database schema (3NF)
├── seed.sql                 # Sample data for 10 sensors
├── ToDo.md                  # This file
├── src/
│   ├── main.cpp             # Application entry point + imtui main loop
│   ├── database.h/cpp       # SQLite wrapper (init, queries, CRUD)
│   ├── models.h             # Data structures (Sensor, SensorValue, Gateway, Location, SensorType)
│   └── ui.h/cpp             # imtui rendering functions
└── vendor/
    └── imtui/               # Already present (Dear ImGui terminal UI library)
```

---

## Task 1: Create setup.sql (Database Schema in 3NF)

### What to do
Write a SQL file that creates all 6 tables with proper relationships.

### Tables needed (all in 3NF):

1. **sensor_types** — Type definitions (temperature, humidity, light, etc.)
   - type_id INTEGER PRIMARY KEY AUTOINCREMENT
   - type_name TEXT NOT NULL UNIQUE

2. **gateways** — LoRaWAN gateways
   - gateway_id INTEGER PRIMARY KEY AUTOINCREMENT
   - gateway_name TEXT NOT NULL UNIQUE
   - gateway_location TEXT NOT NULL

3. **locations** — Physical sensor locations (GPS coordinates)
   - location_id INTEGER PRIMARY KEY AUTOINCREMENT
   - location_gps TEXT NOT NULL
   - location_info TEXT NOT NULL

4. **sensors** — Individual sensors linked to gateways and locations
   - sensor_id INTEGER PRIMARY KEY AUTOINCREMENT
   - gateway_id INTEGER NOT NULL (FK → gateways)
   - sensor_name TEXT NOT NULL
   - location_id INTEGER NOT NULL (FK → locations)
   - extra_location_info TEXT DEFAULT ''

5. **sensor_type_link** — Many-to-many between sensors and types
   - link_id INTEGER PRIMARY KEY AUTOINCREMENT
   - type_id INTEGER NOT NULL (FK → sensor_types)
   - sensor_id INTEGER NOT NULL (FK → sensors)

6. **sensor_values** — Time-series readings
   - id INTEGER PRIMARY KEY AUTOINCREMENT
   - sensor_id INTEGER NOT NULL (FK → sensors)
   - timestamp TEXT NOT NULL (ISO format: YYYY-MM-DD HH:MM:SS)
   - value INTEGER NOT NULL

### Why 3NF compliance matters
- Each table has atomic values (1NF)
- All non-key attributes depend on the full primary key (2NF)
- No transitive dependencies between non-key attributes (3NF)
- Separate sensor_types from sensors via sensor_type_link for flexibility

---

## Task 2: Create seed.sql (Sample Data)

### What to do
Populate the database with realistic sample data:

- **sensor_types**: temperature, humidity, light_intensity, pax_counter, gps_data, gateway_info, time
- **gateways**: 2-3 gateways with European locations (Netherlands)
- **locations**: 4-5 locations across campus
- **sensors**: 10 sensors distributed across gateways/locations
- **sensor_type_link**: Link sensors to their types
- **sensor_values**: Generate some sample readings for each sensor

### Sensor examples (10 total):
1. Temperature sensor 1 (Dormitory A)
2. Temperature sensor 2 (Lab B)
3. Humidity sensor 1 (Greenhouse)
4. Light intensity sensor (Classroom)
5. Pax counter (Main entrance)
6. GPS gateway node (Roof)
7. Gateway info sensor (Basement)
8. Multi-sensor node 1 (Corridor)
9. Multi-sensor node 2 (Library)
10. Outdoor weather station

### Data values (metric/European):
- Temperature: -5 to 35 Celsius
- Humidity: 30% to 95%
- Light: 0 to 100000 lux
- Pax counter: 0 to 500 people
- GPS: European coordinates (Netherlands)

---

## Task 3: Create CMakeLists.txt

### What to do
Configure the build system with:
- cmake_minimum_required(VERSION 3.10)
- Find SQLite3 package
- Add imtui as subdirectory
- Create executable from src files
- Link dependencies

### Key dependencies:
- imtui (from vendor/imtui)
- SQLite3 (system package)
- ncurses (for imtui backend)

### CMake structure:
```cmake
cmake_minimum_required(VERSION 3.10)
project(database_sensor)

# C++11 required
set(CMAKE_CXX_STANDARD 11)

# Find SQLite3
find_package(SQLite3 REQUIRED)

# Add imtui subdirectory
add_subdirectory(vendor/imtui)

# Create executable
add_executable(database_sensor
    src/main.cpp
    src/database.cpp
    src/ui.cpp
)

target_include_directories(database_sensor PRIVATE
    src
    ${SQLite3_INCLUDE_DIRS}
    vendor/imtui/include
)

target_link_libraries(database_sensor PRIVATE
    imtui-ncurses
    SQLite3::SQLite3
)
```

---

## Task 4: Create src/models.h

### What to do
Define C++ structs that mirror the database tables:

```cpp
struct SensorType {
    int id;
    std::string name;
};

struct Gateway {
    int id;
    std::string name;
    std::string location;
};

struct Location {
    int id;
    std::string gps;
    std::string info;
};

struct Sensor {
    int id;
    std::string name;
    int gateway_id;
    int location_id;
    std::string extra_location_info;
    std::vector<SensorType> types;  // Many-to-many resolved
};

struct SensorValue {
    int id;
    int sensor_id;
    std::string timestamp;
    int value;
};
```

---

## Task 5: Create src/database.h and database.cpp

### What to do
Build a SQLite wrapper class with these responsibilities:

### database.h interface:
```cpp
class Database {
public:
    // Initialize database from SQL file
    bool init(const std::string& db_path, const std::string& setup_sql_path);
    bool loadSeedData(const std::string& seed_sql_path);
    
    // Get all data
    std::vector<SensorType> getSensorsTypes();
    std::vector<Gateway> getGateways();
    std::vector<Location> getLocations();
    std::vector<Sensor> getSensors();
    std::vector<SensorValue> getSensorValues(int sensor_id = -1);
    
    // CRUD operations
    int addGateway(const std::string& name, const std::string& location);
    int addLocation(const std::string& gps, const std::string& info);
    int addSensor(const std::string& name, int gateway_id, int location_id, const std::string& extra = "");
    int addSensorType(const std::string& name);
    int addSensorValue(int sensor_id, const std::string& timestamp, int value);
    void linkSensorToType(int sensor_id, int type_id);
    
    // Utility
    sqlite3* getHandle();
    static std::string execSQLFile(sqlite3* db, const std::string& path);
    
private:
    sqlite3* db_ = nullptr;
};
```

### Key implementation details:
- Use `sqlite3_open()` to open/create database file
- Use `sqlite3_exec()` to run setup.sql and seed.sql
- Use prepared statements (`sqlite3_prepare_v2`) for queries
- Parse query results into model structs
- Handle errors gracefully with return values

---

## Task 6: Create src/ui.h and ui.cpp

### What to do
Build the imtui/Dear ImGui interface for displaying and managing sensor data.

### ui.h interface:
```cpp
class SensorUI {
public:
    void render(Database& db, ImTui::TScreen* screen);
    void initTheme();
    
private:
    // State management
    int selected_menu = 0;
    int selected_sensor = -1;
    int selected_gateway = -1;
    int selected_location = -1;
    int selected_type = -1;
    int selected_value = -1;
    
    // Navigation
    enum Menu { MENU_SENSORS, MENU_GATEWAYS, MENU_LOCATIONS, MENU_VALUES, MENU_ADD_SENSOR, MENU_ADD_GATEWAY, MENU_ADD_LOCATION, MENU_ADD_TYPE, MENU_EXIT };
    
    // Helper functions
    void renderMainMenu();
    void renderSensors(Database& db);
    void renderGateways(Database& db);
    void renderLocations(Database& db);
    void renderSensorValues(Database& db);
    void renderAddSensor(Database& db);
    void renderAddGateway(Database& db);
    void renderAddLocation(Database& db);
    void renderAddType(Database& db);
    
    // Input buffers for adding data
    char gateway_name_buf[128] = "";
    char gateway_loc_buf[128] = "";
    char location_gps_buf[128] = "";
    char location_info_buf[128] = "";
    char sensor_name_buf[128] = "";
    char sensor_type_buf[128] = "";
};
```

### Rendering approach:
- Use ImGui tables for structured data display
- Use ImGui listboxes for selection menus
- Use ImGui input text for adding new entries
- Dark theme styling (similar to imtui slack example)

### Color theme (dark):
Use ImVec4 with RGB values normalized to [0,1]:
- Background: (0.15, 0.15, 0.15)
- Window: (0.20, 0.20, 0.20)
- Title: (1.0, 1.0, 1.0)
- Text: (0.85, 0.85, 0.85)
- Highlight: (0.25, 0.59, 0.98)

---

## Task 7: Create src/main.cpp

### What to do
Assemble everything into a working application following the imtui ncurses0 example pattern.

### Application flow:
1. Parse command-line argument for database path (default: `sensor_data.db`)
2. Create ImGui context
3. Initialize imtui ncurses backend
4. Initialize imtui text backend
5. Load database and seed data
6. Enter main loop:
   - Process ncurses input
   - Process text input
   - Create ImGui frame
   - Render UI
   - Draw screen
7. Cleanup on exit

### Main loop structure (based on ncurses0 example):
```cpp
while (true) {
    ImTui_ImplNcurses_NewFrame();
    ImTui_ImplText_NewFrame();
    ImGui::NewFrame();
    
    ui.render(db, screen);
    
    ImGui::Render();
    ImTui_ImplText_RenderDrawData(ImGui::GetDrawData(), screen);
    ImTui_ImplNcurses_DrawScreen();
}
```

### Command line argument handling:
- `./database_sensor [db_path]` — default is "sensor_data.db"
- Pass db_path to Database::init()

---

## Task 8: Build and Test

### What to do
1. Install system dependencies: `sudo apt install libsqlite3-dev libncurses-dev`
2. Create build directory: `mkdir build && cd build`
3. Run cmake: `cmake ..`
4. Build: `make`
5. Run: `./database_sensor` (with optional path argument)
6. Verify all tables are created and populated
7. Test TUI navigation and data entry

---

## Assessment Criteria Mapping

| Criterion | Max Score | How This Plan Addresses It |
|-----------|-----------|---------------------------|
| Conceptual design (ER diagram) | 20 | 6 entities, 5 relationships documented in setup.sql |
| Logical design (3NF tables) | 20 | All tables normalized, proper FK relationships, separate link table |
| 3NF compliance | 15 | No transitive dependencies, atomic values, proper normalization |
| SQL queries | 35 | Rich query set: SELECT with JOINs, INSERT, data retrieval for TUI |
| User interface | 10 | Dark-themed TUI with menus, tables, input forms for CRUD operations |

---

## Estimated Complexity
- **Low**: Database schema and SQL files are straightforward
- **Medium**: SQLite wrapper requires error handling and prepared statements
- **Medium**: imtui/Dear ImGui interface requires learning the widget API
- **High**: Navigation state management (menus, selections, forms) requires careful design

## Risk Areas
1. imtui ncurses backend may have font rendering issues in some terminals
2. ImGui table API may not work well in terminal environments
3. Mouse support may not work in all terminal emulators
4. Cross-compilation for Raspberry Pi may require additional configuration

## Mitigation
- Test on target platform early
- Fall back to text-based rendering if tables don't work
- Keep keyboard navigation as primary input method
