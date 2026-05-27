#include "../ui.h"
#include "../database.h"
#include "imgui/imgui.h"

// ============================================================
// renderAddSensor    - Form to create a new sensor (name + combo boxes for gateway/location/type)
// renderAddGateway   - Form to create a new gateway (name + GPS location)
// renderAddLocation  - Form to create a new location (GPS + info)
// renderAddType      - Form to create a new sensor type (name)
// ============================================================

/// Render the "Add Sensor" form with input for name and combo boxes for gateway, location, and type.
void SensorUI::renderAddSensor(Database& db) {
    auto gateways = db.getGateways();
    auto locations = db.getLocations();
    auto types = db.getSensorTypes();

    ImGui::TextColored(ImVec4(0.25f, 0.59f, 0.98f, 1.0f), "Add Sensor");
    ImGui::Separator();

    // Sensor name input field
    ImGui::Text("Sensor Name:");
    ImGui::InputText("##sensor_name", add_sensor_name, sizeof(add_sensor_name));

    // Combo box for gateway selection
    if (!gateways.empty()) {
        std::vector<const char*> items;
        for (auto& g : gateways) items.push_back(g.name.c_str());
        // Clamp index in case gateways were deleted
        if (add_sensor_gateway_idx >= (int)items.size()) add_sensor_gateway_idx = 0;
        ImGui::Combo("##gateway_combo", &add_sensor_gateway_idx, items.data(), (int)items.size());
    }

    // Combo box for location selection
    if (!locations.empty()) {
        std::vector<const char*> items;
        for (auto& l : locations) items.push_back(l.info.c_str());
        // Clamp index in case locations were deleted
        if (add_sensor_location_idx >= (int)items.size()) add_sensor_location_idx = 0;
        ImGui::Combo("##location_combo", &add_sensor_location_idx, items.data(), (int)items.size());
    }

    // Combo box for sensor type selection
    if (!types.empty()) {
        std::vector<const char*> items;
        for (auto& t : types) items.push_back(t.name.c_str());
        // Clamp index in case types were deleted
        if (add_sensor_type_idx >= (int)items.size()) add_sensor_type_idx = 0;
        ImGui::Combo("##type_combo", &add_sensor_type_idx, items.data(), (int)items.size());
    }

    // Save button: validate name is non-empty, insert into database, link type
    if (ImGui::Button("Save Sensor", { 180, 25 })) {
        if (strlen(add_sensor_name) > 0) {
            int gw_id = gateways.empty() ? 0 : gateways[add_sensor_gateway_idx].id;
            int loc_id = locations.empty() ? 0 : locations[add_sensor_location_idx].id;
            int id = db.addSensor(add_sensor_name, gw_id, loc_id);
            if (id > 0) {
                // Link sensor to selected type after insert (need the new ID)
                if (!types.empty()) {
                    int type_id = types[add_sensor_type_idx].id;
                    db.linkSensorToType(id, type_id);
                }
                flash_message = true;
                snprintf(flash_text, sizeof(flash_text), "Sensor '%s' created with ID: %d", add_sensor_name, id);
                add_sensor_name[0] = '\0';
            } else {
                flash_message = true;
                snprintf(flash_text, sizeof(flash_text), "Error adding sensor.");
            }
        } else {
            flash_message = true;
            snprintf(flash_text, sizeof(flash_text), "Please enter a sensor name.");
        }
    }

    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Esc to change view");
}

/// Render the "Add Gateway" form with name and GPS location inputs.
/// Supports both button click and Enter key to submit.
void SensorUI::renderAddGateway(Database& db) {
    auto& io = ImGui::GetIO();
    auto& km = io.KeyMap;

    // Check if Enter was pressed for keyboard shortcut submission
    bool enterPressed = io.KeysDown[km[ImGuiKey_Enter]] || io.KeysDown[km[ImGuiKey_KeyPadEnter]];

    ImGui::TextColored(ImVec4(0.25f, 0.59f, 0.98f, 1.0f), "Add Gateway");
    ImGui::Separator();

    // Gateway name input
    ImGui::Text("Gateway Name:");
    ImGui::InputText("##gateway_name", add_gateway_name, sizeof(add_gateway_name));

    // Gateway location (GPS) input
    ImGui::Text("Gateway Location (GPS):");
    ImGui::InputText("##gateway_loc", add_gateway_loc, sizeof(add_gateway_loc));

    // Enter key shortcut: same logic as the Save button
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

    // Save button with validation
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

    // Clear Enter key state to prevent double submission
    io.KeysDown[km[ImGuiKey_Enter]] = false;
    io.KeysDown[km[ImGuiKey_KeyPadEnter]] = false;

    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Esc to change view");
}

/// Render the "Add Location" form with GPS coordinates and info inputs.
void SensorUI::renderAddLocation(Database& db) {
    auto& io = ImGui::GetIO();
    auto& km = io.KeyMap;

    // Check if Enter was pressed for keyboard shortcut submission
    bool enterPressed = io.KeysDown[km[ImGuiKey_Enter]] || io.KeysDown[km[ImGuiKey_KeyPadEnter]];

    ImGui::TextColored(ImVec4(0.25f, 0.59f, 0.98f, 1.0f), "Add Location");
    ImGui::Separator();

    // GPS coordinates input
    ImGui::Text("GPS Coordinates:");
    ImGui::InputText("##location_gps", add_location_gps, sizeof(add_location_gps));

    // Location info/description input
    ImGui::Text("Location Info:");
    ImGui::InputText("##location_info", add_location_info, sizeof(add_location_info));

    // Enter key shortcut: validate both fields before saving
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

/// Render the "Add Type" form. Shows existing types as reference, then provides
/// an input field and save button for creating a new type.
void SensorUI::renderAddType(Database& db) {
    auto types = db.getSensorTypes();

    ImGui::TextColored(ImVec4(0.25f, 0.59f, 0.98f, 1.0f), "Add Sensor Type");
    ImGui::Separator();

    // Show existing types as reference for the user
    if (!types.empty()) {
        ImGui::Text("Existing types:");
        for (const auto& t : types) {
            ImGui::Text("  - %s (ID: %d)", t.name.c_str(), t.id);
        }
        ImGui::Separator();
    }

    // New type name input
    ImGui::Text("Type Name:");
    ImGui::InputText("##type_name", add_type_name, sizeof(add_type_name));

    // Save button with validation
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

    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Esc to change view");
}
