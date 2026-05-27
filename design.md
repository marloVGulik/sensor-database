# Database Sensor Program — Design Documentation

A C++ application using SQLite3 as an embedded database and imtui (Dear ImGui terminal UI) for a TUI dashboard to manage sensor data from LoRaWAN nodes.

---

## ER Diagram

```
sensor_types          gateways          locations
┌──────────────┐    ┌──────────────┐    ┌──────────────┐
│ type_id (PK) │    │ gateway_id(PK)│    │location_id(PK)│
│ type_name    │    │gateway_name  │    │ location_gps │
└──────┬───────┘    │gateway_loc   │    │ location_info│
                       └─────────────┘    └──────┬─────┘
                                                  │
          ┌───────────────────────────────────────┤
          │                                       │
┌─────────▼──────────┐     ┌──────────────┐      │
│ sensor_type_link   │     │  sensors     │      │
│ link_id (PK)       │     │ sensor_id(PK)│      │
│ type_id (FK)       │     │ gateway_id(FK)───────┘
│ sensor_id (FK)     │     │ sensor_name  │
└────────────────────┘     │ location_id(FK)
                           │ extra_loc_info│
                           └───────┬──────┘
                                   │
                           ┌───────▼──────────┐
                           │  sensor_values    │
                           │ id (PK)           │
                           │ sensor_id (FK)    │
                           │ timestamp         │
                           │ value             │
                           └───────────────────┘
```

## Relationships

| Relationship | Cardinality | Details |
|---|---|---|
| `gateways` → `sensors` | 1 : N | A gateway can have many sensors; each sensor belongs to one gateway |
| `locations` → `sensors` | 1 : N | A location can have many sensors; each sensor is at one location |
| `sensor_types` ↔ `sensors` | M : N | Resolved via `sensor_type_link` junction table |
| `sensors` → `sensor_values` | 1 : N | Each sensor has many time-series readings |

---

## Logical Design (3NF)

All tables are in Third Normal Form (3NF):

1. **1NF** — All attributes contain atomic (indivisible) values; no repeating groups
2. **2NF** — All non-key attributes are fully dependent on the entire primary key
3. **3NF** — No transitive dependencies; all non-key attributes depend only on the key, the whole key, and nothing but the key

Key design decisions:
- `sensor_types` is separate from `sensors` to avoid repeating sensor type strings
- `sensor_type_link` resolves the many-to-many relationship, enabling a sensor to have multiple types and a type to apply to multiple sensors
- `gateways` and `locations` are normalized into their own tables rather than storing redundant data in `sensors`
- `sensor_values` stores readings as a time-series table with no redundant metadata

---

## API Reference

### `class Database` — SQLite wrapper

| Method | Purpose |
|---|---|
| `init(db_path, setup_sql_path)` | Open database and execute setup.sql schema |
| `loadSeedData(seed_sql_path)` | Load sample data from seed.sql |
| `getSensorTypes()` | SELECT all from sensor_types |
| `getGateways()` | SELECT all from gateways |
| `getLocations()` | SELECT all from locations |
| `getSensors()` | SELECT sensors + JOIN sensor_type_link for types |
| `getSensorValues(sensor_id)` | SELECT sensor_values (LIMIT 50 per sensor, 100 total) |
| `addGateway(name, location)` | INSERT into gateways |
| `addLocation(gps, info)` | INSERT into locations |
| `addSensor(name, gw_id, loc_id, extra)` | INSERT into sensors |
| `addSensorType(name)` | INSERT into sensor_types |
| `addSensorValue(sensor_id, timestamp, value)` | INSERT into sensor_values |
| `linkSensorToType(sensor_id, type_id)` | INSERT into sensor_type_link |
| `deleteSensor(id)` | DELETE sensor_type_link + sensors rows |
| `deleteGateway(id)` | UPDATE sensors FK to NULL, then DELETE gateways row |
| `deleteLocation(id)` | UPDATE sensors FK to NULL, then DELETE locations row |
| `deleteSensorType(id)` | DELETE sensor_type_link + sensor_types rows |
| `deleteSensorValue(id)` | DELETE single sensor_values row |
| `updateSensor(...)` | UPDATE sensors set name, gateway_id, location_id, extra |
| `updateGateway(id, name, location)` | UPDATE gateways set name, location |
| `updateLocation(id, gps, info)` | UPDATE locations set gps, info |
| `updateSensorType(id, name)` | UPDATE sensor_types set name |
| `updateSensorValue(id, sensor_id, timestamp, value)` | UPDATE sensor_values |
| `getHandle()` | Return raw `sqlite3*` for direct access |
| `execSQLFile(db, path)` (static) | Read entire file into std::string |

### `class SensorUI` — Terminal UI state machine

| Field / Method | Purpose |
|---|---|
| `selected_menu` | Current view (enum: MENU_SENSORS, MENU_GATEWAYS, etc.) |
| `selected_sensor`, `selected_gateway`, etc. | Index into the currently loaded list |
| `running` | Main loop flag (set to false to quit) |
| `initTheme()` | Apply dark ImGui style colors |
| `render(db, screen)` | Main per-frame function: processes input, dispatches to view |
| `renderMainMenu()` | Tab bar with 8 menu items |
| `renderSensors()` / `renderGateways()` / `renderLocations()` | List views with selectable rows + Update/Delete |
| `renderSensorValues()` | Readings list for a selected sensor |
| `renderAdd*()` | Forms with InputText + Combo + Save button |
| `renderUpdate*()` | Pre-populated forms with Save Changes |
| `renderDeleteConfirm()` | Centered modal dialog with Yes/Cancel buttons |
| `flash_message` / `flash_text` | Toast notification (auto-dismiss after ~2 seconds) |

---

## Architecture Overview

```
main.cpp
  └── Database::init() + loadSeedData()
  └── SensorUI::initTheme()
  └── while(running) → SensorUI::render(db, screen)
        ├── renderMainMenu()     ← tab bar
        ├── renderSensors()      ← list view
        ├── renderGateways()     ← list view
        ├── renderLocations()    ← list view
        ├── renderSensorValues() ← readings list
        ├── renderAdd*()         ← create forms
        ├── renderUpdate*()      ← edit forms
        └── renderDeleteConfirm()← modal dialog
``` 
