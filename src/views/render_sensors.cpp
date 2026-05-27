#include "../ui.h"
#include "../database.h"
#include "imgui/imgui.h"

// ============================================================
// renderSensors - List all sensors with update/delete buttons
// renderGateways  - List all gateways with update/delete buttons
// renderLocations - List all locations with update/delete buttons
// ============================================================

/// Render the sensors list view: displays all sensors in a selectable list with
/// Update/Delete buttons for the currently selected item.
void SensorUI::renderSensors(Database& db) {
    auto sensors = db.getSensors();

    if (sensors.empty()) {
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "No sensors found.");
        return;
    }

    ImGui::TextColored(ImVec4(0.25f, 0.59f, 0.98f, 1.0f), "Sensors (%d)", (int)sensors.size());
    ImGui::Separator();

    // Column headers for the sensor list
    ImGui::Text("  %-4s  %-20s  %-10s  %-8s  %s", "ID", "Name", "Gateway", "Location", "Types");
    ImGui::Separator();

    for (int i = 0; i < (int)sensors.size(); i++) {
        bool is_selected = (selected_sensor == i);

        // Build comma-separated list of linked sensor types
        std::string type_names;
        for (int j = 0; j < (int)sensors[i].types.size(); j++) {
            if (j > 0) type_names += ", ";
            type_names += sensors[i].types[j].name;
        }

        char buf[256];
        snprintf(buf, sizeof(buf), "  %-4d  %-20s  %-10d  %-8d  %s",
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

        if (is_selected) {
            ImGui::SameLine();
            if (ImGui::Button("Update")) {
                strncpy(update_sensor_name, sensors[i].name.c_str(), sizeof(update_sensor_name) - 1);
                update_sensor_name[sizeof(update_sensor_name) - 1] = '\0';
                add_sensor_gateway_idx = 0;
                add_sensor_location_idx = 0;
                auto gateways = db.getGateways();
                for (int gi = 0; gi < (int)gateways.size(); gi++) {
                    if (gateways[gi].id == sensors[i].gateway_id) {
                        add_sensor_gateway_idx = gi;
                        break;
                    }
                }
                add_sensor_type_idx = 0;
                selected_menu = MENU_UPDATE_SENSOR;
            }
            ImGui::SameLine();
            if (ImGui::Button("Delete")) {
                show_delete_confirm = true;
                delete_confirm_entity = 0;
                delete_confirm_id = sensors[i].id;
            }
        }

        ImGui::PopID();
    }

   ImGui::Separator();
    // Navigation help text (gray)
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Nav: Arrows to select, Enter for values, U=Update, D=Delete, Esc to change view");
}

/// Render the gateways list view (same pattern as renderSensors).
void SensorUI::renderGateways(Database& db) {
    auto gateways = db.getGateways();

    if (gateways.empty()) {
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "No gateways found.");
        return;
    }

    ImGui::TextColored(ImVec4(0.25f, 0.59f, 0.98f, 1.0f), "Gateways (%d)", (int)gateways.size());
    ImGui::Separator();

    // Column headers for gateway list
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

        if (is_selected) {
            ImGui::SameLine();
            if (ImGui::Button("Update")) {
                strncpy(update_gateway_name, gateways[i].name.c_str(), sizeof(update_gateway_name) - 1);
                update_gateway_name[sizeof(update_gateway_name) - 1] = '\0';
                strncpy(update_gateway_loc, gateways[i].location.c_str(), sizeof(update_gateway_loc) - 1);
                update_gateway_loc[sizeof(update_gateway_loc) - 1] = '\0';
                selected_menu = MENU_UPDATE_GATEWAY;
            }
            ImGui::SameLine();
            if (ImGui::Button("Delete")) {
                show_delete_confirm = true;
                delete_confirm_entity = 1;
                delete_confirm_id = gateways[i].id;
            }
        }

        ImGui::PopID();
    }

    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Nav: Arrows to select, Enter to view sensors, Esc to change view");
}

/// Render the locations list view (same pattern as renderSensors and renderGateways).
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

        if (is_selected) {
            ImGui::SameLine();
            if (ImGui::Button("Update")) {
                strncpy(update_location_gps, locations[i].gps.c_str(), sizeof(update_location_gps) - 1);
                update_location_gps[sizeof(update_location_gps) - 1] = '\0';
                strncpy(update_location_info, locations[i].info.c_str(), sizeof(update_location_info) - 1);
                update_location_info[sizeof(update_location_info) - 1] = '\0';
                selected_menu = MENU_UPDATE_LOCATION;
            }
            ImGui::SameLine();
            if (ImGui::Button("Delete")) {
                show_delete_confirm = true;
                delete_confirm_entity = 2;
                delete_confirm_id = locations[i].id;
            }
        }

        ImGui::PopID();
    }

    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Nav: Arrows to select, Enter to view sensors, Esc to change view");
}
