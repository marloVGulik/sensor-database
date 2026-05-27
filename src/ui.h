#pragma once

#include "database.h"
#include "imtui/imtui.h"

/// Terminal UI class for the sensor dashboard using Dear ImGui (via imtui).
/// Manages all rendering state, keyboard navigation, form inputs, and
/// flash message (toast notification) display for CRUD operations.
class SensorUI {
public:
    /// Menu system: enumerates all possible UI views and forms.
    enum Menu {
        MENU_SENSORS,       ///< List of all sensors
        MENU_GATEWAYS,      ///< List of all gateways
        MENU_LOCATIONS,     ///< List of all locations
        MENU_VALUES,        ///< Sensor readings for selected sensor
        MENU_ADD_SENSOR,    ///< Form to create a new sensor
        MENU_ADD_GATEWAY,   ///< Form to create a new gateway
        MENU_ADD_LOCATION,  ///< Form to create a new location
        MENU_ADD_TYPE,      ///< Form to create a new sensor type
        MENU_UPDATE_SENSOR, ///< Form to edit an existing sensor
        MENU_UPDATE_GATEWAY,///< Form to edit an existing gateway
        MENU_UPDATE_LOCATION,///< Form to edit an existing location
        MENU_UPDATE_TYPE,   ///< Form to edit an existing sensor type
        MENU_UPDATE_VALUE,  ///< Form to edit a sensor reading
        MENU_EXIT           ///< Exit the application
    };

    Menu selected_menu = MENU_SENSORS;  ///< Currently active menu/view
    bool running = true;                ///< Main loop flag (set to false to quit)

    // Selection indices (-1 means nothing selected)
    int selected_sensor = -1;       ///< Index into current sensor list
    int selected_gateway = -1;      ///< Index into gateway list
    int selected_location = -1;     ///< Index into location list
    int selected_type = -1;         ///< Index into type list
    int selected_value = -1;        ///< Index into sensor values list

    // Combo box indices for add/update forms (tracks selected dropdown item)
    int add_sensor_gateway_idx = 0;
    int add_sensor_location_idx = 0;
    int add_sensor_type_idx = 0;

    // Delete confirmation modal state
    bool show_delete_confirm = false;
    int delete_confirm_entity = -1;  ///< 0=sensor, 1=gateway, 2=location, 3=type, 4=value
    int delete_confirm_id = -1;      ///< Primary key of entity to delete

    // Update form text buffers (populated when entering update view)
    char update_sensor_name[128] = "";
    char update_gateway_name[128] = "";
    char update_gateway_loc[128] = "";
    char update_location_gps[128] = "";
    char update_location_info[128] = "";
    char update_type_name[128] = "";
    char update_value_timestamp[128] = "";
    char update_value_str[64] = "";
    double update_value_double = 0.0;

    // Add form text buffers
    char add_value_timestamp[128] = "2026-05-27 12:00:00";
    char add_value_str[64] = "0.0";

    /// Initialize the ImGui dark theme colors and style properties.
    void initTheme();

    /// Main render function called each frame of the main loop.
    /// Handles keyboard input, navigation state, and dispatches to the
    /// appropriate menu-specific render function via selected_menu.
    void render(Database& db, ImTui::TScreen* screen);

private:
    // Add form text buffers (ImGui InputText backing storage)
    char add_gateway_name[128] = "";
    char add_gateway_loc[128] = "";
    char add_location_gps[128] = "";
    char add_location_info[128] = "";
    char add_sensor_name[128] = "";
    char add_sensor_type[128] = "";
    char add_type_name[128] = "";

    // Flash toast notification (auto-dismiss after ~2 seconds)
    bool flash_message = false;
    char flash_text[256] = "";

    void renderMainMenu();
    void renderSensors(Database& db);
    void renderGateways(Database& db);
    void renderLocations(Database& db);
    void renderSensorValues(Database& db);
    void renderAddSensor(Database& db);
    void renderAddGateway(Database& db);
    void renderAddLocation(Database& db);
    void renderAddType(Database& db);
    void renderUpdateSensor(Database& db, const Sensor& sensor);
    void renderUpdateGateway(Database& db, const Gateway& gw);
    void renderUpdateLocation(Database& db, const Location& loc);
    void renderUpdateType(Database& db, const SensorType& type);
    void renderUpdateValue(Database& db, const SensorValue& val);
    void renderDeleteConfirm(Database& db);
    void renderAddValueForm(Database& db, int sensor_id);
};
