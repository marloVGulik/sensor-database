#pragma once

#include "database.h"
#include "imtui/imtui.h"

class SensorUI {
public:
    enum Menu {
        MENU_SENSORS,
        MENU_GATEWAYS,
        MENU_LOCATIONS,
        MENU_VALUES,
        MENU_ADD_SENSOR,
        MENU_ADD_GATEWAY,
        MENU_ADD_LOCATION,
        MENU_ADD_TYPE,
        MENU_EXIT
    };

    Menu selected_menu = MENU_SENSORS;
    bool running = true;

    int selected_sensor = -1;
    int selected_gateway = -1;
    int selected_location = -1;
    int selected_type = -1;

    void render(Database& db, ImTui::TScreen* screen);
    void initTheme();

private:
    char add_gateway_name[128] = "";
    char add_gateway_loc[128] = "";
    char add_location_gps[128] = "";
    char add_location_info[128] = "";
    char add_sensor_name[128] = "";
    char add_sensor_type[128] = "";
    char add_type_name[128] = "";

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
};
