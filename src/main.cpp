#include <iostream>
#include <string>
#include "imtui/imtui.h"
#include "imtui/imtui-impl-ncurses.h"
#include "imtui/imtui-impl-text.h"
#include "database.h"
#include "ui.h"

/// Application entry point.
/// Parses optional command-line argument for database path, initializes
/// SQLite database, creates ImGui context, and runs the main TUI loop.
int main(int argc, char* argv[]) {
    // Use command-line argument for db path, or default to "sensor_data.db"
    const std::string db_path = (argc > 1) ? argv[1] : "sensor_data.db";

    Database db;
    SensorUI ui;

    // Initialize SQLite database (creates tables if they don't exist)
    if (!db.init(db_path, "setup.sql")) {
        std::cerr << "Failed to initialize database." << std::endl;
        return 1;
    }

    // Load sample sensor data
    db.loadSeedData("seed.sql");

    // Initialize ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Initialize imtui terminal backend (true = mouse support)
    auto screen = ImTui_ImplNcurses_Init(true);
    ImTui_ImplText_Init();

    // Apply dark theme colors
    ui.initTheme();

    // Main event loop: render UI each frame until running == false
    while (ui.running) {
        bool has_input = ImTui_ImplNcurses_NewFrame();
        ImTui_ImplText_NewFrame();
        ImGui::NewFrame();

        // Render the full dashboard UI
        ui.render(db, screen);

        // 'q' key exits the application
        if (ImGui::GetIO().KeysDown[(int)'q']) {
            ui.running = false;
            break;
        }

        ImGui::Render();
        ImTui_ImplText_RenderDrawData(ImGui::GetDrawData(), screen);
        ImTui_ImplNcurses_DrawScreen();
    }

    // Cleanup: destroy ImGui context and terminal backends
    ImTui_ImplText_Shutdown();
    ImTui_ImplNcurses_Shutdown();
    ImGui::DestroyContext();

    return 0;
}
