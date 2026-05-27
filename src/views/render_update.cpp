#include "../ui.h"
#include "../database.h"
#include "imgui/imgui.h"

// ============================================================
// renderUpdateSensor    - Edit a sensor's name, gateway, location, and linked type
// renderUpdateGateway   - Edit a gateway's name and GPS location
// renderUpdateLocation  - Edit a location's GPS coordinates and info
// renderUpdateType      - Edit a sensor type's name
// ============================================================

/// Render the update sensor form with pre-populated combo boxes.
/// Searches for the current gateway, location, and type indices in the loaded lists.
void SensorUI::renderUpdateSensor(Database& db, const Sensor& sensor) {
    auto gateways = db.getGateways();
    auto locations = db.getLocations();
    auto types = db.getSensorTypes();

    ImGui::TextColored(ImVec4(0.25f, 0.59f, 0.98f, 1.0f), "Update Sensor (ID: %d)", sensor.id);
    ImGui::Separator();

    // Pre-populate the name field with current sensor name
    ImGui::Text("Sensor Name:");
    ImGui::InputText("##update_sensor_name", update_sensor_name, sizeof(update_sensor_name));

    if (!gateways.empty()) {
        std::vector<const char*> items;
        for (auto& g : gateways) items.push_back(g.name.c_str());
        // Find current gateway index by matching IDs
        int current_idx = 0;
        for (int i = 0; i < (int)gateways.size(); i++) {
            if (gateways[i].id == sensor.gateway_id) {
                current_idx = i;
                break;
            }
        }
        add_sensor_gateway_idx = current_idx;
        ImGui::Combo("##update_gateway_combo", &add_sensor_gateway_idx, items.data(), (int)items.size());
    }

    if (!locations.empty()) {
        std::vector<const char*> items;
        for (auto& l : locations) items.push_back(l.info.c_str());
        // Find current location index by matching IDs
        int current_idx = 0;
        for (int i = 0; i < (int)locations.size(); i++) {
            if (locations[i].id == sensor.location_id) {
                current_idx = i;
                break;
            }
        }
        add_sensor_location_idx = current_idx;
        ImGui::Combo("##update_location_combo", &add_sensor_location_idx, items.data(), (int)items.size());
    }

    if (!types.empty()) {
        std::vector<const char*> items;
        for (auto& t : types) items.push_back(t.name.c_str());
        // Find current type index by matching against sensor's linked types
        int current_idx = 0;
        for (int i = 0; i < (int)types.size(); i++) {
            for (auto& st : sensor.types) {
                if (st.id == types[i].id) {
                    current_idx = i;
                    break;
                }
            }
        }
        add_sensor_type_idx = current_idx;
        ImGui::Combo("##update_type_combo", &add_sensor_type_idx, items.data(), (int)items.size());
    }

    // Save button: validate and call update method
    if (ImGui::Button("Save Changes", { 180, 25 })) {
        int gw_id = gateways.empty() ? 0 : gateways[add_sensor_gateway_idx].id;
        int loc_id = locations.empty() ? 0 : locations[add_sensor_location_idx].id;
        bool success = db.updateSensor(sensor.id, update_sensor_name, gw_id, loc_id, sensor.extra_location_info);
        if (success) {
            flash_message = true;
            snprintf(flash_text, sizeof(flash_text), "Sensor '%s' updated successfully", update_sensor_name);
            selected_sensor = -1;
            selected_menu = MENU_SENSORS;
        } else {
            flash_message = true;
            snprintf(flash_text, sizeof(flash_text), "Error updating sensor.");
        }
    }

    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Esc to go back");
}

void SensorUI::renderUpdateGateway(Database& db, const Gateway& gw) {
    ImGui::TextColored(ImVec4(0.25f, 0.59f, 0.98f, 1.0f), "Update Gateway (ID: %d)", gw.id);
    ImGui::Separator();

    // Pre-populate form fields with current gateway data
    ImGui::Text("Gateway Name:");
    ImGui::InputText("##update_gw_name", update_gateway_name, sizeof(update_gateway_name));

    ImGui::Text("Gateway Location (GPS):");
    ImGui::InputText("##update_gw_loc", update_gateway_loc, sizeof(update_gateway_loc));

    // Save button: validate and call update method
    if (ImGui::Button("Save Changes", { 180, 25 })) {
        bool success = db.updateGateway(gw.id, update_gateway_name, update_gateway_loc);
        if (success) {
            flash_message = true;
            snprintf(flash_text, sizeof(flash_text), "Gateway '%s' updated successfully", update_gateway_name);
            selected_gateway = -1;
            selected_menu = MENU_GATEWAYS;
        } else {
            flash_message = true;
            snprintf(flash_text, sizeof(flash_text), "Error updating gateway.");
        }
    }

    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Esc to go back");
}

/// Render the update location form with GPS and info pre-populated.
void SensorUI::renderUpdateLocation(Database& db, const Location& loc) {
    ImGui::TextColored(ImVec4(0.25f, 0.59f, 0.98f, 1.0f), "Update Location (ID: %d)", loc.id);
    ImGui::Separator();

    // Pre-populate form fields with current location data
    ImGui::Text("GPS Coordinates:");
    ImGui::InputText("##update_loc_gps", update_location_gps, sizeof(update_location_gps));

    ImGui::Text("Location Info:");
    ImGui::InputText("##update_loc_info", update_location_info, sizeof(update_location_info));

    // Save button: validate and call update method
    if (ImGui::Button("Save Changes", { 180, 25 })) {
        bool success = db.updateLocation(loc.id, update_location_gps, update_location_info);
        if (success) {
            flash_message = true;
            snprintf(flash_text, sizeof(flash_text), "Location updated successfully");
            selected_location = -1;
            selected_menu = MENU_LOCATIONS;
        } else {
            flash_message = true;
            snprintf(flash_text, sizeof(flash_text), "Error updating location.");
        }
    }

    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Esc to go back");
}

/// Render the update sensor type form with current name pre-populated.
void SensorUI::renderUpdateType(Database& db, const SensorType& type) {
    ImGui::TextColored(ImVec4(0.25f, 0.59f, 0.98f, 1.0f), "Update Type (ID: %d)", type.id);
    ImGui::Separator();

    // Pre-populate the name field with current type name
    strncpy(update_type_name, type.name.c_str(), sizeof(update_type_name) - 1);
    update_type_name[sizeof(update_type_name) - 1] = '\0';

    ImGui::Text("Type Name:");
    ImGui::InputText("##update_type_name", update_type_name, sizeof(update_type_name));

    // Save button: validate and call update method
    if (ImGui::Button("Save Changes", { 180, 25 })) {
        bool success = db.updateSensorType(type.id, update_type_name);
        if (success) {
            flash_message = true;
            snprintf(flash_text, sizeof(flash_text), "Type '%s' updated successfully", update_type_name);
            selected_type = -1;
            selected_menu = MENU_ADD_TYPE;
        } else {
            flash_message = true;
            snprintf(flash_text, sizeof(flash_text), "Error updating type.");
        }
    }

    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Esc to go back");
}
