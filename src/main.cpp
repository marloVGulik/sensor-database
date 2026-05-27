#include <iostream>
#include <string>
#include "imtui/imtui.h"
#include "imtui/imtui-impl-ncurses.h"
#include "imtui/imtui-impl-text.h"
#include "database.h"
#include "ui.h"

int main(int argc, char* argv[]) {
    const std::string db_path = (argc > 1) ? argv[1] : "sensor_data.db";

    Database db;
    SensorUI ui;

    if (!db.init(db_path, "setup.sql")) {
        std::cerr << "Failed to initialize database." << std::endl;
        return 1;
    }

    db.loadSeedData("seed.sql");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    auto screen = ImTui_ImplNcurses_Init(true);
    ImTui_ImplText_Init();

    ui.initTheme();

    while (ui.running) {
        bool has_input = ImTui_ImplNcurses_NewFrame();
        ImTui_ImplText_NewFrame();
        ImGui::NewFrame();

        ui.render(db, screen);

        if (ImGui::GetIO().KeysDown[(int)'q']) {
            ui.running = false;
            break;
        }

        ImGui::Render();
        ImTui_ImplText_RenderDrawData(ImGui::GetDrawData(), screen);
        ImTui_ImplNcurses_DrawScreen();
    }

    ImTui_ImplText_Shutdown();
    ImTui_ImplNcurses_Shutdown();
    ImGui::DestroyContext();

    return 0;
}
