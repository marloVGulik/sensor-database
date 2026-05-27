#include "ui.h"
#include "imtui/imtui-impl-ncurses.h"
#include "imgui/imgui.h"

void SensorUI::initTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    style.GrabRounding = 2.0f;
    style.FrameRounding = 3.0f;

    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
    colors[ImGuiCol_Text] = ImVec4(0.85f, 0.85f, 0.85f, 1.0f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.0f);
    colors[ImGuiCol_Border] = ImVec4(0.30f, 0.30f, 0.30f, 1.0f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.10f, 0.10f, 0.10f, 1.0f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.30f, 0.30f, 0.45f, 1.0f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.50f, 0.80f, 1.0f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.18f, 0.18f, 0.18f, 1.0f);
    colors[ImGuiCol_Button] = ImVec4(0.25f, 0.59f, 0.98f, 1.0f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.65f, 1.0f, 1.0f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.50f, 0.85f, 1.0f);
    colors[ImGuiCol_Header] = ImVec4(0.25f, 0.59f, 0.98f, 0.7f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.30f, 0.65f, 1.0f, 0.8f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.50f, 0.85f, 1.0f);
    colors[ImGuiCol_Separator] = ImVec4(0.30f, 0.30f, 0.30f, 1.0f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.40f, 0.40f, 0.55f, 1.0f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.20f, 0.50f, 0.85f, 1.0f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.25f, 0.59f, 0.98f, 0.3f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.30f, 0.65f, 1.0f, 0.6f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.20f, 0.50f, 0.85f, 0.9f);
    colors[ImGuiCol_Tab] = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.25f, 0.59f, 0.98f, 0.6f);
    colors[ImGuiCol_TabActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.18f, 0.18f, 0.18f, 1.0f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.25f, 0.59f, 0.98f, 1.0f);
}

void SensorUI::render(Database& db, ImTui::TScreen* screen) {
    auto& io = ImGui::GetIO();
    auto& km = io.KeyMap;

    // Escape always works, even when typing in input fields
    if (io.KeysDown[km[ImGuiKey_Escape]]) {
        selected_menu = SensorUI::MENU_SENSORS;
        selected_sensor = -1;
        io.KeysDown[km[ImGuiKey_Escape]] = false;
    }

    // Keyboard navigation (only when not capturing keyboard)
    if (!io.WantCaptureKeyboard) {
        if (io.KeysDown[km[ImGuiKey_UpArrow]]) {
            switch (selected_menu) {
                case SensorUI::MENU_SENSORS:
                    if (selected_sensor > 0) selected_sensor--;
                    break;
                case SensorUI::MENU_GATEWAYS:
                    if (selected_gateway > 0) selected_gateway--;
                    break;
                case SensorUI::MENU_LOCATIONS:
                    if (selected_location > 0) selected_location--;
                    break;
            }
            io.KeysDown[km[ImGuiKey_UpArrow]] = false;
        }
        if (io.KeysDown[km[ImGuiKey_DownArrow]]) {
            switch (selected_menu) {
                case SensorUI::MENU_SENSORS:
                    {
                        auto sensors = db.getSensors();
                        if (selected_sensor > 0 && selected_sensor < (int)sensors.size() - 1)
                            selected_sensor++;
                        else if (selected_sensor < 0)
                            selected_sensor = 0;
                        else if (selected_sensor >= 0 && selected_sensor < (int)sensors.size() - 1)
                            selected_sensor++;
                    }
                    break;
                case SensorUI::MENU_GATEWAYS:
                    {
                        auto gateways = db.getGateways();
                        if (selected_gateway > 0 && selected_gateway < (int)gateways.size() - 1)
                            selected_gateway++;
                        else if (selected_gateway < 0)
                            selected_gateway = 0;
                        else if (selected_gateway >= 0 && selected_gateway < (int)gateways.size() - 1)
                            selected_gateway++;
                    }
                    break;
                case SensorUI::MENU_LOCATIONS:
                    {
                        auto locations = db.getLocations();
                        if (selected_location > 0 && selected_location < (int)locations.size() - 1)
                            selected_location++;
                        else if (selected_location < 0)
                            selected_location = 0;
                        else if (selected_location >= 0 && selected_location < (int)locations.size() - 1)
                            selected_location++;
                    }
                    break;
            }
            io.KeysDown[km[ImGuiKey_DownArrow]] = false;
        }
        if (io.KeysDown[km[ImGuiKey_Enter]] || io.KeysDown[km[ImGuiKey_KeyPadEnter]]) {
            if (selected_menu == SensorUI::MENU_SENSORS && selected_sensor >= 0) {
                selected_menu = SensorUI::MENU_VALUES;
            }
            io.KeysDown[km[ImGuiKey_Enter]] = false;
            io.KeysDown[km[ImGuiKey_KeyPadEnter]] = false;
        }
    }

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(static_cast<float>(screen[0].nx), static_cast<float>(screen[0].ny)), ImGuiCond_Always);

    bool keep_running = true;
    ImGui::Begin("Sensor Dashboard", &keep_running,
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoSavedSettings);

    renderMainMenu();

    switch (selected_menu) {
        case MENU_SENSORS: renderSensors(db); break;
        case MENU_GATEWAYS: renderGateways(db); break;
        case MENU_LOCATIONS: renderLocations(db); break;
        case MENU_VALUES: renderSensorValues(db); break;
        case MENU_ADD_SENSOR: renderAddSensor(db); break;
        case MENU_ADD_GATEWAY: renderAddGateway(db); break;
        case MENU_ADD_LOCATION: renderAddLocation(db); break;
        case MENU_ADD_TYPE: renderAddType(db); break;
        default: break;
    }

    if (flash_message) {
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x / 2 - 120, 5), ImGuiCond_Always);
        ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.20f, 0.59f, 0.98f, 0.95f));
        ImGui::Begin("Flash", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);
        ImGui::Text("%s", flash_text);
        ImGui::End();
        ImGui::PopStyleColor();
    }

  if (!keep_running) {
            running = false;
        }

    ImGui::End();
}

void SensorUI::renderMainMenu() {
    const char* menu_items[] = {
        "SENSORS", "GATEWAYS", "LOCATIONS", "VALUES",
        "---", "ADD SENSOR", "ADD GATEWAY", "ADD LOCATION", "ADD TYPE"
    };
    int count = 9;

    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.25f, 0.59f, 0.98f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.30f, 0.65f, 1.0f, 0.7f));

    if (ImGui::BeginTabBar("##MainMenu", ImGuiTabBarFlags_None)) {
        for (int i = 0; i < count; i++) {
            if (menu_items[i][0] == '-') {
                ImGui::Separator();
                continue;
            }
            bool is_active = (selected_menu == static_cast<Menu>(i));
            if (ImGui::BeginTabItem(menu_items[i], &is_active)) {
                selected_menu = static_cast<Menu>(i);
                flash_message = true;
                snprintf(flash_text, sizeof(flash_text), "Viewing: %s", menu_items[i]);
                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();
    }
    ImGui::PopStyleColor(2);
}

void SensorUI::renderSensors(Database& db) {
    auto sensors = db.getSensors();

    if (sensors.empty()) {
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "No sensors found.");
        return;
    }

    ImGui::TextColored(ImVec4(0.25f, 0.59f, 0.98f, 1.0f), "Sensors (%d)", (int)sensors.size());
    ImGui::Separator();

    ImGui::Text("  %-4s  %-25s  %-12s  %-10s  %s", "ID", "Name", "Gateway", "Location", "Types");
    ImGui::Separator();

    for (int i = 0; i < (int)sensors.size(); i++) {
        bool is_selected = (selected_sensor == i);

        std::string type_names;
        for (int j = 0; j < (int)sensors[i].types.size(); j++) {
            if (j > 0) type_names += ", ";
            type_names += sensors[i].types[j].name;
        }

        char buf[256];
        snprintf(buf, sizeof(buf), "  %-4d  %-25s  %-12d  %-10d  %s",
            sensors[i].id,
            sensors[i].name.c_str(),
            sensors[i].gateway_id,
            sensors[i].location_id,
            type_names.c_str());

        ImGui::PushID(i);
        if (is_selected) {
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.25f, 0.59f, 0.98f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        }
        if (ImGui::Selectable(buf, is_selected)) {
            selected_sensor = i;
        }
        if (is_selected) {
            ImGui::PopStyleColor(2);
        }
        ImGui::PopID();
    }

    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Nav: Arrows to select, Enter for values, Esc to change view");
}

void SensorUI::renderGateways(Database& db) {
    auto gateways = db.getGateways();

    if (gateways.empty()) {
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "No gateways found.");
        return;
    }

    ImGui::TextColored(ImVec4(0.25f, 0.59f, 0.98f, 1.0f), "Gateways (%d)", (int)gateways.size());
    ImGui::Separator();

    ImGui::Text("  %-4s  %-25s  %s", "ID", "Name", "Location (GPS)");
    ImGui::Separator();

    for (int i = 0; i < (int)gateways.size(); i++) {
        bool is_selected = (selected_gateway == i);

        char buf[256];
        snprintf(buf, sizeof(buf), "  %-4d  %-25s  %s",
            gateways[i].id,
            gateways[i].name.c_str(),
            gateways[i].location.c_str());

        ImGui::PushID(i);
        if (is_selected) {
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.25f, 0.59f, 0.98f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        }
        if (ImGui::Selectable(buf, is_selected)) {
            selected_gateway = i;
        }
        if (is_selected) {
            ImGui::PopStyleColor(2);
        }
        ImGui::PopID();
    }

    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Nav: Arrows to select, Enter to view sensors, Esc to change view");
}

void SensorUI::renderLocations(Database& db) {
    auto locations = db.getLocations();

    if (locations.empty()) {
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "No locations found.");
        return;
    }

    ImGui::TextColored(ImVec4(0.25f, 0.59f, 0.98f, 1.0f), "Locations (%d)", (int)locations.size());
    ImGui::Separator();

    ImGui::Text("  %-4s  %-25s  %s", "ID", "GPS", "Info");
    ImGui::Separator();

    for (int i = 0; i < (int)locations.size(); i++) {
        bool is_selected = (selected_location == i);

        char buf[256];
        snprintf(buf, sizeof(buf), "  %-4d  %-25s  %s",
            locations[i].id,
            locations[i].gps.c_str(),
            locations[i].info.c_str());

        ImGui::PushID(i);
        if (is_selected) {
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.25f, 0.59f, 0.98f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        }
        if (ImGui::Selectable(buf, is_selected)) {
            selected_location = i;
        }
        if (is_selected) {
            ImGui::PopStyleColor(2);
        }
        ImGui::PopID();
    }

    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Nav: Arrows to select, Enter to view sensors, Esc to change view");
}

void SensorUI::renderSensorValues(Database& db) {
    if (selected_sensor < 0) {
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Select a sensor first (use Sensors menu).");
        return;
    }

    auto sensors = db.getSensors();
    if (selected_sensor >= (int)sensors.size()) {
        selected_sensor = 0;
    }

    auto values = db.getSensorValues(sensors[selected_sensor].id);

    std::string sensor_type_display;
    for (auto& t : sensors[selected_sensor].types) {
        if (!sensor_type_display.empty()) sensor_type_display += ", ";
        sensor_type_display += t.name;
    }

    ImGui::TextColored(ImVec4(0.25f, 0.59f, 0.98f, 1.0f),
        "Sensor Values: %s (%s)",
        sensors[selected_sensor].name.c_str(),
        sensor_type_display.c_str());
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Sensor ID: %d  |  %d readings",
        sensors[selected_sensor].id, (int)values.size());
    ImGui::Separator();

    if (values.empty()) {
        ImGui::Text("No readings for this sensor.");
        return;
    }

    ImGui::Text("  %-6s  %-22s  %s", "ID", "Timestamp", "Value");
    ImGui::Separator();

    for (int i = 0; i < (int)values.size(); i++) {
        ImGui::Text("  %-6d  %-22s  %.2f",
            values[i].id,
            values[i].timestamp.c_str(),
            values[i].value);
    }

    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Navigation: Esc to change view");
}

void SensorUI::renderAddGateway(Database& db) {
    auto& io = ImGui::GetIO();
    auto& km = io.KeyMap;
    bool enterPressed = io.KeysDown[km[ImGuiKey_Enter]] || io.KeysDown[km[ImGuiKey_KeyPadEnter]];

    ImGui::TextColored(ImVec4(0.25f, 0.59f, 0.98f, 1.0f), "Add Gateway");
    ImGui::Separator();

    ImGui::Text("Gateway Name:");
    ImGui::InputText("##gateway_name", add_gateway_name, sizeof(add_gateway_name));

    ImGui::Text("Gateway Location (GPS):");
    ImGui::InputText("##gateway_loc", add_gateway_loc, sizeof(add_gateway_loc));

    if (enterPressed && strlen(add_gateway_name) > 0 && strlen(add_gateway_loc) > 0) {
        int id = db.addGateway(add_gateway_name, add_gateway_loc);
        if (id > 0) {
            flash_message = true;
            snprintf(flash_text, sizeof(flash_text), "Gateway added with ID: %d", id);
            add_gateway_name[0] = '\0';
            add_gateway_loc[0] = '\0';
        } else {
            flash_message = true;
            snprintf(flash_text, sizeof(flash_text), "Error adding gateway.");
        }
    }

    if (ImGui::Button("Save Gateway", { 180, 25 })) {
        if (strlen(add_gateway_name) > 0 && strlen(add_gateway_loc) > 0) {
            int id = db.addGateway(add_gateway_name, add_gateway_loc);
            if (id > 0) {
                flash_message = true;
                snprintf(flash_text, sizeof(flash_text), "Gateway added with ID: %d", id);
                add_gateway_name[0] = '\0';
                add_gateway_loc[0] = '\0';
            } else {
                flash_message = true;
                snprintf(flash_text, sizeof(flash_text), "Error adding gateway.");
            }
        } else {
            flash_message = true;
            snprintf(flash_text, sizeof(flash_text), "Please fill in all fields.");
        }
    }

    io.KeysDown[km[ImGuiKey_Enter]] = false;
    io.KeysDown[km[ImGuiKey_KeyPadEnter]] = false;

    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Esc to change view");
}

void SensorUI::renderAddLocation(Database& db) {
    auto& io = ImGui::GetIO();
    auto& km = io.KeyMap;
    bool enterPressed = io.KeysDown[km[ImGuiKey_Enter]] || io.KeysDown[km[ImGuiKey_KeyPadEnter]];

    ImGui::TextColored(ImVec4(0.25f, 0.59f, 0.98f, 1.0f), "Add Location");
    ImGui::Separator();

    ImGui::Text("GPS Coordinates:");
    ImGui::InputText("##location_gps", add_location_gps, sizeof(add_location_gps));

    ImGui::Text("Location Info:");
    ImGui::InputText("##location_info", add_location_info, sizeof(add_location_info));

    if (enterPressed && strlen(add_location_gps) > 0 && strlen(add_location_info) > 0) {
        int id = db.addLocation(add_location_gps, add_location_info);
        if (id > 0) {
            flash_message = true;
            snprintf(flash_text, sizeof(flash_text), "Location added with ID: %d", id);
            add_location_gps[0] = '\0';
            add_location_info[0] = '\0';
        } else {
            flash_message = true;
            snprintf(flash_text, sizeof(flash_text), "Error adding location.");
        }
    }

    if (ImGui::Button("Save Location", { 180, 25 })) {
        if (strlen(add_location_gps) > 0 && strlen(add_location_info) > 0) {
            int id = db.addLocation(add_location_gps, add_location_info);
            if (id > 0) {
                flash_message = true;
                snprintf(flash_text, sizeof(flash_text), "Location added with ID: %d", id);
                add_location_gps[0] = '\0';
                add_location_info[0] = '\0';
            } else {
                flash_message = true;
                snprintf(flash_text, sizeof(flash_text), "Error adding location.");
            }
        } else {
            flash_message = true;
            snprintf(flash_text, sizeof(flash_text), "Please fill in all fields.");
        }
    }

    io.KeysDown[km[ImGuiKey_Enter]] = false;
    io.KeysDown[km[ImGuiKey_KeyPadEnter]] = false;

    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Esc to change view");
}

void SensorUI::renderAddSensor(Database& db) {
    auto& io = ImGui::GetIO();
    auto& km = io.KeyMap;
    bool enterPressed = io.KeysDown[km[ImGuiKey_Enter]] || io.KeysDown[km[ImGuiKey_KeyPadEnter]];

    auto gateways = db.getGateways();
    auto locations = db.getLocations();
    auto types = db.getSensorTypes();

    ImGui::TextColored(ImVec4(0.25f, 0.59f, 0.98f, 1.0f), "Add Sensor");
    ImGui::Separator();

    ImGui::Text("Sensor Name:");
    ImGui::InputText("##sensor_name", add_sensor_name, sizeof(add_sensor_name));

    if (!gateways.empty()) {
        static int gw_idx = 0;
        std::vector<const char*> items;
        for (auto& g : gateways) items.push_back(g.name.c_str());
        if (gw_idx >= (int)items.size()) gw_idx = 0;
        ImGui::Combo("##gateway_combo", &gw_idx, items.data(), (int)items.size());
    }

    if (!locations.empty()) {
        static int loc_idx = 0;
        std::vector<const char*> items;
        for (auto& l : locations) items.push_back(l.info.c_str());
        if (loc_idx >= (int)items.size()) loc_idx = 0;
        ImGui::Combo("##location_combo", &loc_idx, items.data(), (int)items.size());
    }

    if (!types.empty()) {
        static int type_idx = 0;
        std::vector<const char*> items;
        for (auto& t : types) items.push_back(t.name.c_str());
        if (type_idx >= (int)items.size()) type_idx = 0;
        ImGui::Combo("##type_combo", &type_idx, items.data(), (int)items.size());
    }

    if (enterPressed) {
        if (strlen(add_sensor_name) > 0) {
            flash_message = true;
            snprintf(flash_text, sizeof(flash_text), "Sensor '%s' created successfully", add_sensor_name);
            add_sensor_name[0] = '\0';
        } else {
            flash_message = true;
            snprintf(flash_text, sizeof(flash_text), "Please enter a sensor name.");
        }
    }

    if (ImGui::Button("Save Sensor", { 180, 25 })) {
        if (strlen(add_sensor_name) > 0) {
            flash_message = true;
            snprintf(flash_text, sizeof(flash_text), "Sensor '%s' created successfully", add_sensor_name);
            add_sensor_name[0] = '\0';
        } else {
            flash_message = true;
            snprintf(flash_text, sizeof(flash_text), "Please enter a sensor name.");
        }
    }

    io.KeysDown[km[ImGuiKey_Enter]] = false;
    io.KeysDown[km[ImGuiKey_KeyPadEnter]] = false;

    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Esc to change view");
}

void SensorUI::renderAddType(Database& db) {
    auto& io = ImGui::GetIO();
    auto& km = io.KeyMap;
    bool enterPressed = io.KeysDown[km[ImGuiKey_Enter]] || io.KeysDown[km[ImGuiKey_KeyPadEnter]];

    auto types = db.getSensorTypes();

    ImGui::TextColored(ImVec4(0.25f, 0.59f, 0.98f, 1.0f), "Add Sensor Type");
    ImGui::Separator();

    if (!types.empty()) {
        ImGui::Text("Existing types:");
        for (const auto& t : types) {
            ImGui::Text("  - %s (ID: %d)", t.name.c_str(), t.id);
        }
        ImGui::Separator();
    }

    ImGui::Text("Type Name:");
    ImGui::InputText("##type_name", add_type_name, sizeof(add_type_name));

    if (enterPressed) {
        if (strlen(add_type_name) > 0) {
            int id = db.addSensorType(add_type_name);
            if (id > 0) {
                flash_message = true;
                snprintf(flash_text, sizeof(flash_text), "Type '%s' added with ID: %d", add_type_name, id);
                add_type_name[0] = '\0';
            } else {
                flash_message = true;
                snprintf(flash_text, sizeof(flash_text), "Error adding type (may already exist).");
            }
        } else {
            flash_message = true;
            snprintf(flash_text, sizeof(flash_text), "Please enter a type name.");
        }
    }

    if (ImGui::Button("Save Type", { 180, 25 })) {
        if (strlen(add_type_name) > 0) {
            int id = db.addSensorType(add_type_name);
            if (id > 0) {
                flash_message = true;
                snprintf(flash_text, sizeof(flash_text), "Type '%s' added with ID: %d", add_type_name, id);
                add_type_name[0] = '\0';
            } else {
                flash_message = true;
                snprintf(flash_text, sizeof(flash_text), "Error adding type (may already exist).");
            }
        } else {
            flash_message = true;
            snprintf(flash_text, sizeof(flash_text), "Please enter a type name.");
        }
    }

    io.KeysDown[km[ImGuiKey_Enter]] = false;
    io.KeysDown[km[ImGuiKey_KeyPadEnter]] = false;

    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Esc to change view");
}
