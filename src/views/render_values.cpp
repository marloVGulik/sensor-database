#include "../ui.h"
#include "../database.h"
#include "imgui/imgui.h"

// ============================================================
// renderSensorValues - Display sensor readings list with add/update/delete buttons
// renderUpdateValue  - Edit a single sensor reading (timestamp + value)
// ============================================================

/// Render the sensor values list for the currently selected sensor.
/// Shows ID, timestamp, and value columns with Update/Delete buttons.
void SensorUI::renderSensorValues(Database& db) {
    // Require a sensor to be selected before showing values
    if (selected_sensor < 0) {
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Select a sensor first (use Sensors menu).");
        return;
    }

    auto sensors = db.getSensors();
    if (selected_sensor >= (int)sensors.size()) {
        selected_sensor = 0;
    }

    auto values = db.getSensorValues(sensors[selected_sensor].id);

    // Build comma-separated type display for the selected sensor
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
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Use 'Add Value' button below to insert readings.");
    } else {
        // Column headers for the values table
        ImGui::Text("  %-6s  %-22s  %8s", "ID", "Timestamp", "Value");
        ImGui::Separator();

        for (int i = 0; i < (int)values.size(); i++) {
            bool is_selected = (selected_value == i);

            char buf[256];
            snprintf(buf, sizeof(buf), "  %-6d  %-22s  %8.2f",
                values[i].id,
                values[i].timestamp.c_str(),
                values[i].value);

            ImGui::PushID(i);
            if (is_selected) {
                ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.25f, 0.59f, 0.98f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
            }
            if (ImGui::Selectable(buf, is_selected)) {
                selected_value = i;
            }
            if (is_selected) {
                ImGui::PopStyleColor(2);
            }

            if (is_selected) {
                ImGui::SameLine();
                if (ImGui::Button("Update")) {
                    strncpy(update_value_timestamp, values[i].timestamp.c_str(), sizeof(update_value_timestamp) - 1);
                    update_value_timestamp[sizeof(update_value_timestamp) - 1] = '\0';
                    update_value_double = values[i].value;
                    snprintf(add_value_str, sizeof(add_value_str), "%.2f", values[i].value);
                    selected_menu = MENU_UPDATE_VALUE;
                }
                ImGui::SameLine();
                if (ImGui::Button("Delete")) {
                    show_delete_confirm = true;
                    delete_confirm_entity = 4;
                    delete_confirm_id = values[i].id;
                }
            }

            ImGui::PopID();
        }

        ImGui::Separator();
    }

    // "Add Value" button to open the add-value form
    if (ImGui::Button("Add Value")) {
        selected_value = -1;
        strncpy(add_value_timestamp, "2026-05-27 12:00:00", sizeof(add_value_timestamp) - 1);
        add_value_timestamp[sizeof(add_value_timestamp) - 1] = '\0';
        add_value_str[0] = '0';
        add_value_str[1] = '\0';
        flash_message = true;
        snprintf(flash_text, sizeof(flash_text), "Enter timestamp and value, then press Save Value");
    }
    ImGui::SameLine();
    if (values.empty()) {
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "No readings yet. Click 'Add Value' to start.");
    } else {
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Esc to change view");
    }
}

void SensorUI::renderUpdateValue(Database& db, const SensorValue& val) {
    ImGui::TextColored(ImVec4(0.25f, 0.59f, 0.98f, 1.0f), "Update Value (ID: %d)", val.id);
    ImGui::Separator();

    strncpy(update_value_timestamp, val.timestamp.c_str(), sizeof(update_value_timestamp) - 1);
    update_value_timestamp[sizeof(update_value_timestamp) - 1] = '\0';

    ImGui::Text("Timestamp:");
    ImGui::InputText("##update_val_ts", update_value_timestamp, sizeof(update_value_timestamp));

    snprintf(add_value_str, sizeof(add_value_str), "%.2f", val.value);
    add_value_str[sizeof(add_value_str) - 1] = '\0';

    ImGui::Text("Value:");
    ImGui::InputText("##update_val_value", add_value_str, sizeof(add_value_str));

    if (ImGui::Button("Save Changes", { 180, 25 })) {
        double new_val = 0.0;
        try {
            new_val = std::stod(add_value_str);
        } catch (...) {
            new_val = 0.0;
        }
        bool success = db.updateSensorValue(val.id, val.sensor_id, update_value_timestamp, new_val);
        if (success) {
            flash_message = true;
            snprintf(flash_text, sizeof(flash_text), "Sensor value updated successfully");
            selected_value = -1;
            selected_menu = MENU_VALUES;
        } else {
            flash_message = true;
            snprintf(flash_text, sizeof(flash_text), "Error updating value.");
        }
    }

    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Esc to go back");
}
