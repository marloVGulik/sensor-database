#include "../ui.h"
#include "../database.h"
#include "imgui/imgui.h"

// ============================================================
// renderDeleteConfirm - Modal dialog to confirm deletion of any entity (sensor, gateway, location, type, value)
// ============================================================

/// Render a centered modal dialog asking the user to confirm deletion.
/// Displays the entity type name and ID, with a warning message.
/// Delete is executed on "Yes" button click or cancelled on "Cancel"/Escape.
void SensorUI::renderDeleteConfirm(Database& db) {
    auto& io = ImGui::GetIO();
    auto& km = io.KeyMap;

    // Array of entity type names indexed by delete_confirm_entity value (0-4)
    const char* entity_names[] = {"Sensor", "Gateway", "Location", "Type", "Value"};
    const char* warning = "This action cannot be undone!";

    ImVec2 pos(ImGui::GetIO().DisplaySize.x / 2 - 150, 100);
    ImGui::SetNextWindowPos(pos);
    ImGui::Begin("Delete Confirm", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

    ImGui::TextColored(ImVec4(0.98f, 0.3f, 0.3f, 1.0f), "Delete %s (ID: %d)?", entity_names[delete_confirm_entity], delete_confirm_id);
    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.3f, 1.0f), "%s", warning);
    ImGui::Separator();

    if (ImGui::Button("Yes, Delete", { 120, 25 })) {
        bool success = false;
        switch (delete_confirm_entity) {
            case 0: success = db.deleteSensor(delete_confirm_id) > 0; break;
            case 1: success = db.deleteGateway(delete_confirm_id) > 0; break;
            case 2: success = db.deleteLocation(delete_confirm_id) > 0; break;
            case 3: success = db.deleteSensorType(delete_confirm_id) > 0; break;
            case 4: success = db.deleteSensorValue(delete_confirm_id) > 0; break;
        }
        if (success) {
            flash_message = true;
            snprintf(flash_text, sizeof(flash_text), "%s deleted successfully", entity_names[delete_confirm_entity]);
        } else {
            flash_message = true;
            snprintf(flash_text, sizeof(flash_text), "Error: could not delete %s", entity_names[delete_confirm_entity]);
        }
        show_delete_confirm = false;
        delete_confirm_entity = -1;
        delete_confirm_id = -1;
        selected_sensor = -1;
        selected_gateway = -1;
        selected_location = -1;
        selected_value = -1;
    }

    ImGui::SameLine();
    if (ImGui::Button("Cancel", { 120, 25 })) {
        show_delete_confirm = false;
        delete_confirm_entity = -1;
        delete_confirm_id = -1;
    }

    // Handle Escape for cancel
    if (io.KeysDown[km[ImGuiKey_Escape]]) {
        show_delete_confirm = false;
        delete_confirm_entity = -1;
        delete_confirm_id = -1;
        io.KeysDown[km[ImGuiKey_Escape]] = false;
    }

    ImGui::End();
}
