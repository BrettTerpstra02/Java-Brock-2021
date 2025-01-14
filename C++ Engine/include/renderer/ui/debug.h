//
// Created by brett on 21/07/22.
//

#ifndef ENGINE_DEBUG_H
#define ENGINE_DEBUG_H

#include "../../imgui/imgui.h"
#include "../../font.h"
#include "../camera.h"
#include "../gl.h"

namespace TD {

    class DebugTab{
    protected:
        std::string name;
    public:
        virtual void render() {}
        std::string getName() {
            return name;
        }
    };

    class renderable {
    public:
        virtual void render() = 0;
    };

    class debugUI {
    private:
        debugUI(TD::camera* camera) {}
    public:
        static void toggle();
        static void render();
        static void addTab(DebugTab* tab);
        static void deleteTab(DebugTab* tab);
        static void deleteAllTabs();
    };

#ifdef DEBUG_ENABLED
    class Editor {
    private:
        static void renderConsole();
        static void renderInspector();
        static void renderHiearchyScene();
        static void renderMenuBar();
        static void renderGuizmo();
    public:
        // used to be called before the display is setup
        static void setToOpen();
        static void open();
        static void close();
        static void toggle();
        static bool isOpen();
        static void init();
        static void render();
        static void renderGBuffer();
        static void cleanup();
    };
#endif
}


#endif //ENGINE_DEBUG_H
