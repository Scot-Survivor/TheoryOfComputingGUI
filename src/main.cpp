// Dear ImGui: standalone example application for SDL2 + OpenGL
// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

// **DO NOT USE THIS CODE IF YOUR CODE/ENGINE IS USING MODERN OPENGL (SHADERS, VBO, VAO, etc.)**
// **Prefer using the code in the example_sdl2_opengl3/ folder**
// See imgui_impl_sdl2.cpp for details.
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl2.h"
#include "GraphNode.h"
#include "Graph.h"
#include "States.h"
#include <cstdio>
#include <SDL.h>
#include <SDL_opengl.h>



// Main code
int main(int argc, char *argv[])
{
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

    // Setup window
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("NeuralNetworkSim", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(0); // Disable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls


    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL2_Init();

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);

    // Our state
    Graph graph = Graph();
    float node_radius = 20;
    auto *master = new GraphNode(ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), node_radius,  "A");
    graph.add_node(master);

    graph.add_node(
            new GraphNode(ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), node_radius, "B"),
            {0});

    graph.add_node(
            new GraphNode(ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), node_radius, "C"),
            {0, 1});

    graph.add_node(
            new GraphNode(ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), node_radius, "D"),
            {0, 1, 2});

    graph.add_node(
            new GraphNode(ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), node_radius, "E"),
            {2, 3});
    graph.add_node(
            new GraphNode(ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), node_radius, "F"),
            {3});
    graph.add_edge({{3, 0}, {1, 4}});

    auto graph_drawn_state = GraphDrawnState();
    auto selected_node_state = SelectedNodeState();
    bool show_demo_window = false;
    bool show_input_array_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;

            // Check if escape
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE && event.window.windowID == SDL_GetWindowID(window) && selected_node_state.has_box_selected) {
                selected_node_state.has_box_selected = false;
                selected_node_state.has_set_start_pos = false;
                selected_node_state.selected_nodes.clear();
                selected_node_state.selected_nodes_start_positions.clear();
            }
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::Begin("RegexTool", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                                     ImGuiWindowFlags_NoCollapse |ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse |
                                                     ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBackground);
        ImDrawList* draw_list = ImGui::GetWindowDrawList();


        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("Options")) {
                ImGui::MenuItem("Show Demo Window", nullptr, &show_demo_window);
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }


        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);


        if (show_input_array_window) {
            ImGui::Begin("Input Array", &show_input_array_window, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            ImGui::Text("Input Array");
            ImGui::End();
        }

        if (!graph_drawn_state.data_pass) {
            master->set_position({
                                         ImGui::GetIO().DisplaySize.x / 2,
                                         ImGui::GetIO().DisplaySize.y / 2
                                 });
            graph.data_pass();
            graph_drawn_state.data_pass = true;
        }
        graph.draw(draw_list);

        if ( ImGui::IsMouseDoubleClicked( 0 ))
        {
            ImVec2 pos = ImGui::GetMousePos();
            GraphNode* node = graph.get_node(pos);
            selected_node_state.selected_node = node;
            if (selected_node_state.selected_node != nullptr) {
                selected_node_state.is_box_selecting = false;
            }
            else {
                selected_node_state.is_box_selecting = true;
                selected_node_state.p1 = pos;
                draw_list->AddRectFilled(pos, pos, IM_COL32(0, 233, 255, 20));
            }
        } else if (ImGui::IsMouseClicked(0)) {
            if (selected_node_state.selected_node != nullptr) selected_node_state.selected_node = nullptr;
            if (selected_node_state.is_box_selecting && ImGui::IsMouseDown(0)) {
                selected_node_state.has_box_selected = true;
                selected_node_state.p2 = ImGui::GetMousePos();
                selected_node_state.selected_nodes = graph.select_nodes(selected_node_state.p1, selected_node_state.p2);
                draw_list->AddRectFilled(selected_node_state.p1, selected_node_state.p2, IM_COL32(0, 233, 255, 20));
            }
            if (!selected_node_state.is_box_selecting && selected_node_state.has_box_selected) {
                selected_node_state.start_pos = ImGui::GetMousePos();
                selected_node_state.has_set_start_pos = true;
            } else if (!selected_node_state.is_box_selecting && selected_node_state.has_box_selected && selected_node_state.has_set_start_pos) {
                selected_node_state.has_box_selected = false;
                selected_node_state.has_set_start_pos = false;
                selected_node_state.selected_nodes.clear();
                selected_node_state.selected_nodes_start_positions.clear();
            }
        } else if (selected_node_state.is_box_selecting) {
            selected_node_state.p2 = ImGui::GetMousePos();
            draw_list->AddRectFilled(selected_node_state.p1, selected_node_state.p2, IM_COL32(0, 233, 255, 20));
            for (GraphNode* node : graph.select_nodes(selected_node_state.p1, selected_node_state.p2)) {
                if (node != nullptr) {
                    printf("Selected Node: %s\n", node->get_label().c_str());
                    if (std::find(selected_node_state.selected_nodes.begin(), selected_node_state.selected_nodes.end(), node) == selected_node_state.selected_nodes.end()) {
                        selected_node_state.selected_nodes.push_back(node);
                        selected_node_state.selected_nodes_start_positions.push_back(node->get_position());
                    }
                }
            }
        }
        if (selected_node_state.is_box_selecting && !ImGui::IsMouseDown(0)) {
            selected_node_state.is_box_selecting = false;
            selected_node_state.has_box_selected = true;
        } else if (!selected_node_state.is_box_selecting && !selected_node_state.has_box_selected) {
            selected_node_state.selected_nodes.clear();
            selected_node_state.selected_nodes_start_positions.clear();
        }
        if (!selected_node_state.is_box_selecting && selected_node_state.has_box_selected && ImGui::IsMouseDown(0) && selected_node_state.has_set_start_pos) {
            // Move nodes that are selected
            ImVec2 mouse_pos = ImGui::GetMousePos();
            ImVec2 delta = {mouse_pos.x - selected_node_state.start_pos.x, mouse_pos.y - selected_node_state.start_pos.y};
            for (int i = 0; i < selected_node_state.selected_nodes.size(); i++) {
                GraphNode* node = selected_node_state.selected_nodes[i];
                ImVec2 n_start_pos = selected_node_state.selected_nodes_start_positions[i];
                node->set_position({n_start_pos.x + delta.x, n_start_pos.y + delta.y});
            }
        } else if ((!selected_node_state.is_box_selecting && selected_node_state.has_box_selected && !ImGui::IsMouseDown(0) && selected_node_state.has_set_start_pos)){
            selected_node_state.has_set_start_pos = false;
            selected_node_state.has_box_selected = false;
            selected_node_state.selected_nodes.clear();
            selected_node_state.selected_nodes_start_positions.clear();
        }

        if (selected_node_state.selected_node != nullptr) {
            selected_node_state.selected_node->set_position(ImGui::GetMousePos());
        }

        for (GraphNode* node : selected_node_state.selected_nodes) {
            draw_list->AddCircleFilled(node->get_position(), node->get_radius(), IM_COL32(0, 233, 255, 50));
        }

        ImGui::End();

        // Rendering
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        //glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
