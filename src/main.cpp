#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <filesystem>
#include <map> 

// ImGui and GLFW
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

// Your project headers
#include "database.h"
#include "models.h"

int main() {
    glfwInitHint(GLFW_PLATFORM, GLFW_ANY_PLATFORM);
    if (!glfwInit()) return -1;
    
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE); 

    GLFWwindow* window = glfwCreateWindow(1380, 800, "LoRaWAN Sensor Database Dashboard", NULL, NULL);
    if (!window) return -1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); 

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    float xscale, yscale;
    glfwGetWindowContentScale(window, &xscale, &yscale);
    
    ImFontConfig font_cfg;
    font_cfg.SizePixels = 13.0f * xscale; 
    font_cfg.OversampleH = 2; 
    font_cfg.OversampleV = 2;
    io.Fonts->AddFontDefault(&font_cfg);
    
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(xscale);

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    Database db;
    std::string db_filename = "sensors_data.db";
    bool is_new_db = !std::filesystem::exists(db_filename);

    if (!db.init(db_filename, "setup.sql")) {
        std::cerr << "Database initialization failed. Check if setup.sql exists.\n";
    } else if (is_new_db) {
        db.loadSeedData("seed.sql");
    }

    char gwName[128] = "";      char gwLocation[128] = "";
    char locGps[128] = "";      char locInfo[128] = "";
    char typeName[128] = "";
    
    char sensorName[128] = "";  
    int sensorGwId = 0;         
    int sensorLocId = 0;
    int sensorTypeId = 0; 
    
    int dataSensorId = 0;       char dataTimestamp[64] = "2026-06-13 12:00:00";  double dataValue = 0.0;
    int filterSensorId = -1;

    int editId = -1; 
    char editStr1[128] = "";
    char editStr2[128] = "";
    double editDouble = 0.0;
    int editInt1 = 0;
    int editInt2 = 0; 
    
    // --- DIFFERENTIAL TRACKING FOR SENSOR TYPES ---
    std::map<int, bool> initialTypeSelections;
    std::map<int, bool> editTypeSelections;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;
        ImGui::Begin("DashboardContainer", nullptr, window_flags);
        ImGui::PopStyleVar();

        if (ImGui::BeginTable("DashboardLayout", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchSame)) {
            
            ImGui::TableNextRow();
            
            // ==========================================
            // LEFT COLUMN (Gateways, Locations, Types)
            // ==========================================
            ImGui::TableNextColumn();
            float leftHeight3 = (io.DisplaySize.y / 3.0f) - 12.0f;

            // --- GATEWAYS ---
            ImGui::BeginChild("GatewaysPanel", ImVec2(0, leftHeight3), true);
            ImGui::Text("Gateways"); ImGui::Separator();
            
            ImGui::PushItemWidth(100 * xscale);
            ImGui::InputText("Name##gw", gwName, IM_ARRAYSIZE(gwName)); ImGui::SameLine();
            ImGui::InputText("Loc##gw", gwLocation, IM_ARRAYSIZE(gwLocation)); ImGui::SameLine();
            if (ImGui::Button("Add##gw") && strlen(gwName) > 0) {
                db.addGateway(gwName, gwLocation);
                gwName[0] = '\0'; gwLocation[0] = '\0';
            }
            ImGui::PopItemWidth();

            if (ImGui::BeginTable("GwTbl", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
                ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed); 
                ImGui::TableSetupColumn("Name"); ImGui::TableSetupColumn("Location"); ImGui::TableSetupColumn("Act", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableHeadersRow();
                for (const auto& gw : db.getGateways()) {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn(); ImGui::Text("%d", gw.id);
                    ImGui::TableNextColumn(); ImGui::Text("%s", gw.name.c_str());
                    ImGui::TableNextColumn(); ImGui::Text("%s", gw.location.c_str());
                    
                    ImGui::TableNextColumn(); 
                    ImGui::PushID(gw.id);
                    if (ImGui::Button("Edit")) {
                        editId = gw.id;
                        strncpy(editStr1, gw.name.c_str(), sizeof(editStr1) - 1);
                        strncpy(editStr2, gw.location.c_str(), sizeof(editStr2) - 1);
                        ImGui::OpenPopup("Edit Gateway");
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Del")) db.deleteGateway(gw.id);
                    
                    if (ImGui::BeginPopupModal("Edit Gateway", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                        ImGui::InputText("Name", editStr1, IM_ARRAYSIZE(editStr1));
                        ImGui::InputText("Location", editStr2, IM_ARRAYSIZE(editStr2));
                        if (ImGui::Button("Save")) {
                            db.updateGateway(editId, editStr1, editStr2);
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();
                        ImGui::EndPopup();
                    }
                    ImGui::PopID();
                }
                ImGui::EndTable();
            }
            ImGui::EndChild();

            // --- LOCATIONS ---
            ImGui::BeginChild("LocationsPanel", ImVec2(0, leftHeight3), true);
            ImGui::Text("Locations"); ImGui::Separator();
            
            ImGui::PushItemWidth(100 * xscale);
            ImGui::InputText("GPS##lc", locGps, IM_ARRAYSIZE(locGps)); ImGui::SameLine();
            ImGui::InputText("Info##lc", locInfo, IM_ARRAYSIZE(locInfo)); ImGui::SameLine();
            if (ImGui::Button("Add##lc") && strlen(locGps) > 0) {
                db.addLocation(locGps, locInfo);
                locGps[0] = '\0'; locInfo[0] = '\0';
            }
            ImGui::PopItemWidth();

            if (ImGui::BeginTable("LcTbl", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
                ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed); 
                ImGui::TableSetupColumn("GPS"); ImGui::TableSetupColumn("Info"); ImGui::TableSetupColumn("Act", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableHeadersRow();
                for (const auto& loc : db.getLocations()) {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn(); ImGui::Text("%d", loc.id);
                    ImGui::TableNextColumn(); ImGui::Text("%s", loc.gps.c_str());
                    ImGui::TableNextColumn(); ImGui::Text("%s", loc.info.c_str());
                    
                    ImGui::TableNextColumn(); 
                    ImGui::PushID(loc.id + 10000);
                    if (ImGui::Button("Edit")) {
                        editId = loc.id;
                        strncpy(editStr1, loc.gps.c_str(), sizeof(editStr1) - 1);
                        strncpy(editStr2, loc.info.c_str(), sizeof(editStr2) - 1);
                        ImGui::OpenPopup("Edit Location");
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Del")) db.deleteLocation(loc.id);

                    if (ImGui::BeginPopupModal("Edit Location", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                        ImGui::InputText("GPS", editStr1, IM_ARRAYSIZE(editStr1));
                        ImGui::InputText("Info", editStr2, IM_ARRAYSIZE(editStr2));
                        if (ImGui::Button("Save")) {
                            db.updateLocation(editId, editStr1, editStr2);
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();
                        ImGui::EndPopup();
                    }
                    ImGui::PopID();
                }
                ImGui::EndTable();
            }
            ImGui::EndChild();

            // --- TYPES ---
            ImGui::BeginChild("TypesPanel", ImVec2(0, 0), true);
            ImGui::Text("Sensor Types"); ImGui::Separator();
            
            ImGui::PushItemWidth(150 * xscale);
            ImGui::InputText("Type##ty", typeName, IM_ARRAYSIZE(typeName)); ImGui::SameLine();
            if (ImGui::Button("Add##ty") && strlen(typeName) > 0) {
                db.addSensorType(typeName);
                typeName[0] = '\0';
            }
            ImGui::PopItemWidth();

            if (ImGui::BeginTable("TyTbl", 3, ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
                ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed); 
                ImGui::TableSetupColumn("Name"); ImGui::TableSetupColumn("Act", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableHeadersRow();
                for (const auto& ty : db.getSensorTypes()) {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn(); ImGui::Text("%d", ty.id);
                    ImGui::TableNextColumn(); ImGui::Text("%s", ty.name.c_str());
                    
                    ImGui::TableNextColumn(); 
                    ImGui::PushID(ty.id + 20000);
                    if (ImGui::Button("Edit")) {
                        editId = ty.id;
                        strncpy(editStr1, ty.name.c_str(), sizeof(editStr1) - 1);
                        ImGui::OpenPopup("Edit Type");
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Del")) db.deleteSensorType(ty.id);

                    if (ImGui::BeginPopupModal("Edit Type", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                        ImGui::InputText("Name", editStr1, IM_ARRAYSIZE(editStr1));
                        if (ImGui::Button("Save")) {
                            db.updateSensorType(editId, editStr1);
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();
                        ImGui::EndPopup();
                    }
                    ImGui::PopID();
                }
                ImGui::EndTable();
            }
            ImGui::EndChild();


            // ==========================================
            // RIGHT COLUMN
            // ==========================================
            ImGui::TableNextColumn();
            float rightHeight2 = (io.DisplaySize.y / 2.0f) - 10.0f;

            // --- SENSORS ---
            ImGui::BeginChild("SensorsPanel", ImVec2(0, rightHeight2), true);
            ImGui::Text("Sensors"); ImGui::Separator();

            // Fetch available gateways and locations for the dropdowns
            std::vector<Gateway> allGateways = db.getGateways();
            std::vector<Location> allLocations = db.getLocations();
            std::vector<SensorType> allTypes = db.getSensorTypes();

            ImGui::PushItemWidth(80 * xscale);
            ImGui::InputText("Name##sn", sensorName, IM_ARRAYSIZE(sensorName)); ImGui::SameLine();
            ImGui::PopItemWidth();
            
            // Gateway Dropdown
            std::string gwPreview = "No Gateway";
            for (const auto& gw : allGateways) { if (gw.id == sensorGwId) gwPreview = gw.name; }
            ImGui::PushItemWidth(100 * xscale);
            if (ImGui::BeginCombo("##snGw", gwPreview.c_str())) {
                if (ImGui::Selectable("No Gateway", sensorGwId == 0)) sensorGwId = 0;
                for (const auto& gw : allGateways) {
                    bool is_selected = (sensorGwId == gw.id);
                    if (ImGui::Selectable(gw.name.c_str(), is_selected)) sensorGwId = gw.id;
                    if (is_selected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ImGui::PopItemWidth();
            ImGui::SameLine();

            // Location Dropdown
            std::string locPreview = "No Location";
            for (const auto& loc : allLocations) { if (loc.id == sensorLocId) locPreview = loc.info; }
            ImGui::PushItemWidth(100 * xscale);
            if (ImGui::BeginCombo("##snLoc", locPreview.c_str())) {
                if (ImGui::Selectable("No Location", sensorLocId == 0)) sensorLocId = 0;
                for (const auto& loc : allLocations) {
                    bool is_selected = (sensorLocId == loc.id);
                    if (ImGui::Selectable(loc.info.c_str(), is_selected)) sensorLocId = loc.id;
                    if (is_selected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ImGui::PopItemWidth();
            ImGui::SameLine();
            
            // Type Dropdown
            std::string typePreview = "No Type";
            for (const auto& ty : allTypes) { if (ty.id == sensorTypeId) typePreview = ty.name; }
            ImGui::PushItemWidth(100 * xscale);
            if (ImGui::BeginCombo("##snType", typePreview.c_str())) {
                if (ImGui::Selectable("No Type", sensorTypeId == 0)) sensorTypeId = 0;
                for (const auto& ty : allTypes) {
                    bool is_selected = (sensorTypeId == ty.id);
                    if (ImGui::Selectable(ty.name.c_str(), is_selected)) sensorTypeId = ty.id;
                    if (is_selected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ImGui::PopItemWidth();
            ImGui::SameLine();

            if (ImGui::Button("Add##sn") && strlen(sensorName) > 0) {
                int newId = db.addSensor(sensorName, sensorGwId, sensorLocId);
                if (newId > 0 && sensorTypeId > 0) {
                    db.linkSensorToType(newId, sensorTypeId);
                }
                // Reset inputs after adding
                sensorName[0] = '\0';
                sensorGwId = 0;
                sensorLocId = 0;
                sensorTypeId = 0; 
            }

            if (ImGui::BeginTable("SnTbl", 6, ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable)) {
                ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed); 
                ImGui::TableSetupColumn("Name"); 
                ImGui::TableSetupColumn("GW/Loc", ImGuiTableColumnFlags_WidthFixed); 
                ImGui::TableSetupColumn("Extra Info"); 
                ImGui::TableSetupColumn("Type(s)"); 
                ImGui::TableSetupColumn("Act", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableHeadersRow();
                for (const auto& sn : db.getSensors()) {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn(); ImGui::Text("%d", sn.id);
                    ImGui::TableNextColumn(); ImGui::Text("%s", sn.name.c_str());
                    ImGui::TableNextColumn(); ImGui::Text("%d / %d", sn.gateway_id, sn.location_id);
                    ImGui::TableNextColumn(); ImGui::Text("%s", sn.extra_location_info.c_str());
                    
                    ImGui::TableNextColumn(); 
                    std::string typesStr = "";
                    for (size_t i = 0; i < sn.types.size(); ++i) {
                        typesStr += sn.types[i].name;
                        if (i < sn.types.size() - 1) typesStr += ", ";
                    }
                    if (typesStr.empty()) typesStr = "None";
                    ImGui::Text("%s", typesStr.c_str());
                    
                    ImGui::TableNextColumn(); 
                    ImGui::PushID(sn.id + 30000);
                    
                    if (ImGui::Button("Edit")) {
                        editId = sn.id;
                        strncpy(editStr1, sn.name.c_str(), sizeof(editStr1) - 1);
                        editInt1 = sn.gateway_id;
                        editInt2 = sn.location_id;
                        strncpy(editStr2, sn.extra_location_info.c_str(), sizeof(editStr2) - 1);
                        
                        editTypeSelections.clear();
                        initialTypeSelections.clear();
                        for (const auto& t : sn.types) {
                            editTypeSelections[t.id] = true;
                            initialTypeSelections[t.id] = true;
                        }

                        ImGui::OpenPopup("Edit Sensor");
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Del")) db.deleteSensor(sn.id);

                    if (ImGui::BeginPopupModal("Edit Sensor", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                        ImGui::InputText("Name", editStr1, IM_ARRAYSIZE(editStr1));
                        
                        // Edit Gateway Dropdown
                        std::string editGwPreview = "No Gateway";
                        for (const auto& gw : allGateways) { if (gw.id == editInt1) editGwPreview = gw.name; }
                        if (ImGui::BeginCombo("Gateway", editGwPreview.c_str())) {
                            if (ImGui::Selectable("No Gateway", editInt1 == 0)) editInt1 = 0;
                            for (const auto& gw : allGateways) {
                                bool is_selected = (editInt1 == gw.id);
                                if (ImGui::Selectable(gw.name.c_str(), is_selected)) editInt1 = gw.id;
                                if (is_selected) ImGui::SetItemDefaultFocus();
                            }
                            ImGui::EndCombo();
                        }

                        // Edit Location Dropdown
                        std::string editLocPreview = "No Location";
                        for (const auto& loc : allLocations) { if (loc.id == editInt2) editLocPreview = loc.info; }
                        if (ImGui::BeginCombo("Location", editLocPreview.c_str())) {
                            if (ImGui::Selectable("No Location", editInt2 == 0)) editInt2 = 0;
                            for (const auto& loc : allLocations) {
                                bool is_selected = (editInt2 == loc.id);
                                if (ImGui::Selectable(loc.info.c_str(), is_selected)) editInt2 = loc.id;
                                if (is_selected) ImGui::SetItemDefaultFocus();
                            }
                            ImGui::EndCombo();
                        }

                        ImGui::InputText("Extra Info", editStr2, IM_ARRAYSIZE(editStr2));
                        
                        ImGui::Separator();
                        ImGui::Text("Linked Sensor Types:");
                        for (const auto& ty : allTypes) {
                            bool is_selected = editTypeSelections[ty.id];
                            if (ImGui::Checkbox(ty.name.c_str(), &is_selected)) {
                                editTypeSelections[ty.id] = is_selected;
                            }
                        }
                        ImGui::Separator();

                        if (ImGui::Button("Save")) {
                            db.updateSensor(editId, editStr1, editInt1, editInt2, editStr2);
                            
                            for (const auto& ty : allTypes) {
                                bool was_selected = initialTypeSelections[ty.id];
                                bool is_selected = editTypeSelections[ty.id];
                                
                                if (!was_selected && is_selected) {
                                    db.linkSensorToType(editId, ty.id);
                                } else if (was_selected && !is_selected) {
                                    db.unlinkSensorFromType(editId, ty.id);
                                }
                            }

                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();
                        ImGui::EndPopup();
                    }
                    ImGui::PopID();
                }
                ImGui::EndTable();
            }
            ImGui::EndChild();

            // --- DATA ---
            ImGui::BeginChild("DataPanel", ImVec2(0, 0), true);
            
            std::vector<Sensor> allSensors = db.getSensors();
            std::string previewValue = "All Sensors";
            for (const auto& sn : allSensors) {
                if (sn.id == filterSensorId) previewValue = sn.name + " (ID: " + std::to_string(sn.id) + ")";
            }

            ImGui::PushItemWidth(200 * xscale);
            if (ImGui::BeginCombo("Filter View", previewValue.c_str())) {
                if (ImGui::Selectable("All Sensors", filterSensorId == -1)) filterSensorId = -1;
                for (const auto& sn : allSensors) {
                    bool is_selected = (filterSensorId == sn.id);
                    if (ImGui::Selectable((sn.name + " (ID: " + std::to_string(sn.id) + ")").c_str(), is_selected)) {
                        filterSensorId = sn.id;
                    }
                    if (is_selected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ImGui::PopItemWidth();
            ImGui::Separator();

            ImGui::PushItemWidth(80 * xscale);
            ImGui::InputInt("Sn ID", &dataSensorId); ImGui::SameLine();
            ImGui::PushItemWidth(120 * xscale);
            ImGui::InputText("Time", dataTimestamp, IM_ARRAYSIZE(dataTimestamp)); ImGui::SameLine();
            ImGui::PushItemWidth(80 * xscale);
            ImGui::InputDouble("Val", &dataValue); ImGui::SameLine();
            if (ImGui::Button("Add##dt")) {
                db.addSensorValue(dataSensorId, dataTimestamp, dataValue);
                dataValue = 0.0; 
            }
            ImGui::PopItemWidth(); ImGui::PopItemWidth(); ImGui::PopItemWidth();

            if (ImGui::BeginTable("DtTbl", 5, ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
                ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed); 
                ImGui::TableSetupColumn("Sensor", ImGuiTableColumnFlags_WidthFixed); 
                ImGui::TableSetupColumn("Time"); ImGui::TableSetupColumn("Value"); ImGui::TableSetupColumn("Act", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableHeadersRow();
                for (const auto& val : db.getSensorValues(filterSensorId)) { 
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn(); ImGui::Text("%d", val.id);
                    ImGui::TableNextColumn(); ImGui::Text("%d", val.sensor_id);
                    ImGui::TableNextColumn(); ImGui::Text("%s", val.timestamp.c_str());
                    ImGui::TableNextColumn(); ImGui::Text("%.2f", val.value);
                    
                    ImGui::TableNextColumn(); 
                    ImGui::PushID(val.id + 40000);
                    if (ImGui::Button("Edit")) {
                        editId = val.id;
                        editInt1 = val.sensor_id;
                        strncpy(editStr1, val.timestamp.c_str(), sizeof(editStr1) - 1);
                        editDouble = val.value;
                        ImGui::OpenPopup("Edit Value");
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Del")) db.deleteSensorValue(val.id);

                    if (ImGui::BeginPopupModal("Edit Value", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                        ImGui::InputInt("Sensor ID", &editInt1);
                        ImGui::InputText("Timestamp", editStr1, IM_ARRAYSIZE(editStr1));
                        ImGui::InputDouble("Value", &editDouble);
                        if (ImGui::Button("Save")) {
                            db.updateSensorValue(editId, editInt1, editStr1, editDouble);
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();
                        ImGui::EndPopup();
                    }
                    ImGui::PopID();
                }
                ImGui::EndTable();
            }
            ImGui::EndChild();

            ImGui::EndTable();
        }
        ImGui::End();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.15f, 0.15f, 0.15f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}