#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <filesystem>

// ImGui and GLFW
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

// Your project headers
#include "database.h"
#include "models.h"

int main() {
    // 1. Initialize GLFW
    if (!glfwInit()) return -1;
    
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE); 

    // Made the default window larger to accommodate a 2x2 grid
    GLFWwindow* window = glfwCreateWindow(1280, 800, "LoRaWAN Sensor Database Dashboard", NULL, NULL);
    if (!window) return -1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); 

    // 2. Initialize Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    float xscale, yscale;
    glfwGetWindowContentScale(window, &xscale, &yscale);
    io.FontGlobalScale = xscale; 

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // 3. Initialize Database
    Database db;
    std::string db_filename = "sensors_data.db";
    bool is_new_db = !std::filesystem::exists(db_filename);

    if (!db.init(db_filename, "setup.sql")) {
        std::cerr << "Database initialization failed. Check if setup.sql exists.\n";
    } else if (is_new_db) {
        if (!db.loadSeedData("seed.sql")) {
            std::cerr << "Failed to load seed.sql.\n";
        }
    }

    // --- UI State Variables ---
    // Gateways
    char gwName[128] = "";
    char gwLocation[128] = "";
    
    // Sensors
    char sensorName[128] = "";
    int sensorGwId = 0;
    int sensorLocId = 0;
    
    // Sensor Types
    char typeName[128] = "";
    
    // Data (Values)
    int dataSensorId = 0;
    char dataTimestamp[64] = "2026-06-13 12:00:00"; 
    double dataValue = 0.0;

    // 4. Main Render Loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // --- Layout: Fullscreen Background Window ---
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        
        // Remove borders, title bar, and ability to move/resize the main container
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;
        ImGui::Begin("DashboardContainer", nullptr, window_flags);
        ImGui::PopStyleVar();

        // 2x2 Grid setup
        if (ImGui::BeginTable("DashboardGrid", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchSame)) {
            
            // Calculate half height of the screen for the child panels
            float panelHeight = (io.DisplaySize.y / 2.0f) - 15.0f;

            // ==========================================
            // TOP-LEFT: GATEWAYS
            // ==========================================
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::BeginChild("GatewaysPanel", ImVec2(0, panelHeight), true);
            ImGui::Text("Gateways");
            ImGui::Separator();
            
            ImGui::PushItemWidth(100);
            ImGui::InputText("Name##gw", gwName, IM_ARRAYSIZE(gwName)); ImGui::SameLine();
            ImGui::InputText("Loc##gw", gwLocation, IM_ARRAYSIZE(gwLocation)); ImGui::SameLine();
            if (ImGui::Button("Add##gw")) {
                if (strlen(gwName) > 0) {
                    db.addGateway(gwName, gwLocation);
                    gwName[0] = '\0'; gwLocation[0] = '\0';
                }
            }
            ImGui::PopItemWidth();

            if (ImGui::BeginTable("GwTbl", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
                ImGui::TableSetupColumn("ID"); ImGui::TableSetupColumn("Name");
                ImGui::TableSetupColumn("Location"); ImGui::TableSetupColumn("Act");
                ImGui::TableHeadersRow();
                for (const auto& gw : db.getGateways()) {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn(); ImGui::Text("%d", gw.id);
                    ImGui::TableNextColumn(); ImGui::Text("%s", gw.name.c_str());
                    ImGui::TableNextColumn(); ImGui::Text("%s", gw.location.c_str());
                    ImGui::TableNextColumn(); 
                    ImGui::PushID(gw.id);
                    if (ImGui::Button("Del")) db.deleteGateway(gw.id);
                    ImGui::PopID();
                }
                ImGui::EndTable();
            }
            ImGui::EndChild();

            // ==========================================
            // TOP-RIGHT: SENSORS
            // ==========================================
            ImGui::TableNextColumn();
            ImGui::BeginChild("SensorsPanel", ImVec2(0, panelHeight), true);
            ImGui::Text("Sensors");
            ImGui::Separator();

            ImGui::PushItemWidth(80);
            ImGui::InputText("Name##sn", sensorName, IM_ARRAYSIZE(sensorName)); ImGui::SameLine();
            ImGui::InputInt("GW ID", &sensorGwId); ImGui::SameLine();
            ImGui::InputInt("Loc ID", &sensorLocId); ImGui::SameLine();
            if (ImGui::Button("Add##sn")) {
                if (strlen(sensorName) > 0) {
                    db.addSensor(sensorName, sensorGwId, sensorLocId);
                    sensorName[0] = '\0';
                }
            }
            ImGui::PopItemWidth();

            if (ImGui::BeginTable("SnTbl", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
                ImGui::TableSetupColumn("ID"); ImGui::TableSetupColumn("Name");
                ImGui::TableSetupColumn("GW/Loc"); ImGui::TableSetupColumn("Act");
                ImGui::TableHeadersRow();
                for (const auto& sn : db.getSensors()) {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn(); ImGui::Text("%d", sn.id);
                    ImGui::TableNextColumn(); ImGui::Text("%s", sn.name.c_str());
                    ImGui::TableNextColumn(); ImGui::Text("%d / %d", sn.gateway_id, sn.location_id);
                    ImGui::TableNextColumn(); 
                    ImGui::PushID(sn.id + 10000); // Offset ID to prevent button conflicts
                    if (ImGui::Button("Del")) db.deleteSensor(sn.id);
                    ImGui::PopID();
                }
                ImGui::EndTable();
            }
            ImGui::EndChild();

            // ==========================================
            // BOTTOM-LEFT: SENSOR TYPES
            // ==========================================
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::BeginChild("TypesPanel", ImVec2(0, 0), true); // 0,0 fills remaining space
            ImGui::Text("Sensor Types");
            ImGui::Separator();

            ImGui::PushItemWidth(150);
            ImGui::InputText("Type Name", typeName, IM_ARRAYSIZE(typeName)); ImGui::SameLine();
            if (ImGui::Button("Add##ty")) {
                if (strlen(typeName) > 0) {
                    db.addSensorType(typeName);
                    typeName[0] = '\0';
                }
            }
            ImGui::PopItemWidth();

            if (ImGui::BeginTable("TyTbl", 3, ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
                ImGui::TableSetupColumn("ID"); ImGui::TableSetupColumn("Name"); ImGui::TableSetupColumn("Act");
                ImGui::TableHeadersRow();
                for (const auto& ty : db.getSensorTypes()) {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn(); ImGui::Text("%d", ty.id);
                    ImGui::TableNextColumn(); ImGui::Text("%s", ty.name.c_str());
                    ImGui::TableNextColumn(); 
                    ImGui::PushID(ty.id + 20000);
                    if (ImGui::Button("Del")) db.deleteSensorType(ty.id);
                    ImGui::PopID();
                }
                ImGui::EndTable();
            }
            ImGui::EndChild();

            // ==========================================
            // BOTTOM-RIGHT: DATA (VALUES)
            // ==========================================
            ImGui::TableNextColumn();
            ImGui::BeginChild("DataPanel", ImVec2(0, 0), true);
            ImGui::Text("Sensor Data Readings");
            ImGui::Separator();

            ImGui::PushItemWidth(80);
            ImGui::InputInt("Sn ID", &dataSensorId); ImGui::SameLine();
            ImGui::PushItemWidth(120);
            ImGui::InputText("Time", dataTimestamp, IM_ARRAYSIZE(dataTimestamp)); ImGui::SameLine();
            ImGui::PushItemWidth(80);
            ImGui::InputDouble("Val", &dataValue); ImGui::SameLine();
            if (ImGui::Button("Add##dt")) {
                db.addSensorValue(dataSensorId, dataTimestamp, dataValue);
                // Intentionally keeping timestamp so you can enter batches quickly
                dataValue = 0.0; 
            }
            ImGui::PopItemWidth(); ImGui::PopItemWidth(); ImGui::PopItemWidth();

            if (ImGui::BeginTable("DtTbl", 5, ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
                ImGui::TableSetupColumn("ID"); ImGui::TableSetupColumn("Sensor"); 
                ImGui::TableSetupColumn("Time"); ImGui::TableSetupColumn("Value"); ImGui::TableSetupColumn("Act");
                ImGui::TableHeadersRow();
                for (const auto& val : db.getSensorValues(-1)) { // -1 fetches all
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn(); ImGui::Text("%d", val.id);
                    ImGui::TableNextColumn(); ImGui::Text("%d", val.sensor_id);
                    ImGui::TableNextColumn(); ImGui::Text("%s", val.timestamp.c_str());
                    ImGui::TableNextColumn(); ImGui::Text("%.2f", val.value);
                    ImGui::TableNextColumn(); 
                    ImGui::PushID(val.id + 30000);
                    if (ImGui::Button("Del")) db.deleteSensorValue(val.id);
                    ImGui::PopID();
                }
                ImGui::EndTable();
            }
            ImGui::EndChild();

            ImGui::EndTable();
        }
        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.15f, 0.15f, 0.15f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // 5. Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}