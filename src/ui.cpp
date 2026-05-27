#include "ui.h"
#include "models.h"
#include "imtui/imtui-impl-ncurses.h"
#include "imgui/imgui.h"

// ============================================================
// View files split across src/views/ for organization:
//   render_sensors.cpp  - renderSensors, renderGateways, renderLocations
//   render_values.cpp   - renderSensorValues, renderUpdateValue
//   render_add.cpp      - renderAddSensor, renderAddGateway, renderAddLocation, renderAddType
//   render_update.cpp   - renderUpdateSensor, renderUpdateGateway, renderUpdateLocation, renderUpdateType
//   render_delete.cpp   - renderDeleteConfirm
// ============================================================

/// Apply a dark color theme to ImGui style.
/// Sets window backgrounds, text colors, button/hover states, and borders
/// to match the terminal TUI aesthetic.
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
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.20f, 0.50f, 0.85f, 1.0f);
    colors[ImGuiCol_Tab] = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.25f, 0.59f, 0.98f, 0.6f);
    colors[ImGuiCol_TabActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.18f, 0.18f, 0.18f, 1.0f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.25f, 0.59f, 0.98f, 1.0f);
}

/// Main render loop: processes input, manages navigation state, and dispatches
/// to the appropriate menu-view function. Called once per frame by main.cpp.
void SensorUI::render(Database& db, ImTui::TScreen* screen) {
    auto& io = ImGui::GetIO();
    auto& km = io.KeyMap;

    // Escape key resets all state and returns to main menu (works even during input)
    if (io.KeysDown[km[ImGuiKey_Escape]]) {
        selected_menu = SensorUI::MENU_SENSORS;
        selected_sensor = -1;
        selected_gateway = -1;
        selected_location = -1;
        selected_type = -1;
        selected_value = -1;
        show_delete_confirm = false;
        flash_message = false;
        io.KeysDown[km[ImGuiKey_Escape]] = false;
    }

    // Keyboard navigation: arrow keys and Enter for menu-only mode (not during text input)
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

    if (show_delete_confirm) {
        renderDeleteConfirm(db);
    } else {
        switch (selected_menu) {
            case MENU_SENSORS: renderSensors(db); break;
            case MENU_GATEWAYS: renderGateways(db); break;
            case MENU_LOCATIONS: renderLocations(db); break;
            case MENU_VALUES: renderSensorValues(db); break;
            case MENU_ADD_SENSOR: renderAddSensor(db); break;
            case MENU_ADD_GATEWAY: renderAddGateway(db); break;
            case MENU_ADD_LOCATION: renderAddLocation(db); break;
            case MENU_ADD_TYPE: renderAddType(db); break;
            case MENU_UPDATE_SENSOR:
                {
                    auto sensors = db.getSensors();
                    if (selected_sensor >= 0 && selected_sensor < (int)sensors.size()) {
                        renderUpdateSensor(db, sensors[selected_sensor]);
                    } else {
                        selected_menu = MENU_SENSORS;
                    }
                }
                break;
            case MENU_UPDATE_GATEWAY:
                {
                    auto gateways = db.getGateways();
                    if (selected_gateway >= 0 && selected_gateway < (int)gateways.size()) {
                        renderUpdateGateway(db, gateways[selected_gateway]);
                    } else {
                        selected_menu = MENU_GATEWAYS;
                    }
                }
                break;
            case MENU_UPDATE_LOCATION:
                {
                    auto locations = db.getLocations();
                    if (selected_location >= 0 && selected_location < (int)locations.size()) {
                        renderUpdateLocation(db, locations[selected_location]);
                    } else {
                        selected_menu = MENU_LOCATIONS;
                    }
                }
                break;
            case MENU_UPDATE_TYPE:
                {
                    auto types = db.getSensorTypes();
                    if (selected_type >= 0 && selected_type < (int)types.size()) {
                        renderUpdateType(db, types[selected_type]);
                    } else {
                        selected_menu = MENU_ADD_TYPE;
                    }
                }
                break;
            case MENU_UPDATE_VALUE:
                {
                    auto sensors = db.getSensors();
                    if (selected_sensor >= 0 && selected_sensor < (int)sensors.size()) {
                        auto values = db.getSensorValues(sensors[selected_sensor].id);
                        if (selected_value >= 0 && selected_value < (int)values.size()) {
                            renderUpdateValue(db, values[selected_value]);
                        } else {
                            selected_menu = MENU_VALUES;
                        }
                    } else {
                        selected_menu = MENU_SENSORS;
                    }
                }
                break;
            default: break;
        }
    }

    if (flash_message) {
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x / 2 - 120, 5), ImGuiCond_Always);
        ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.20f, 0.59f, 0.98f, 0.95f));
        ImGui::Begin("Flash", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);
        ImGui::Text("%s", flash_text);
        ImGui::End();
        ImGui::PopStyleColor();
        static int flash_timer = 0;
        flash_timer++;
        if (flash_timer > 120) {
            flash_message = false;
            flash_timer = 0;
        }
    }

    if (!keep_running) {
        running = false;
    }

    ImGui::End();
}

void SensorUI::renderMainMenu() {
    const char* menu_items[] = {
        "SENSORS", "GATEWAYS", "LOCATIONS", "VALUES",
        "ADD SENSOR", "ADD GATEWAY", "ADD LOCATION", "ADD TYPE"
    };
    int count = 8;

    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.25f, 0.59f, 0.98f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.30f, 0.65f, 1.0f, 0.7f));

    if (ImGui::BeginTabBar("##MainMenu", ImGuiTabBarFlags_None)) {
        for (int i = 0; i < count; i++) {
            if (ImGui::BeginTabItem(menu_items[i], nullptr)) {
                selected_menu = static_cast<Menu>(i);
                flash_message = true;
                snprintf(flash_text, sizeof(flash_text), "Viewing: %s", menu_items[i]);
                ImGui::EndTabItem();
            }
        }
        ImGui::Separator();
        ImGui::EndTabBar();
    }
    ImGui::PopStyleColor(2);
}
