//
// Created by brett on 31/07/22.
//

#ifndef ENGINE_TESTDISPLAY_H
#define ENGINE_TESTDISPLAY_H

#include "../window.h"
#include "../glm.h"
#include "../util.h"
#include "../profiler.h"
#include "../world/World.h"

namespace TD {

    class TestDisplay : public Display {
    private:
        glm::vec3 direction = glm::vec3(0.5f, 1.0f, 0.6f);
        //glm::vec3 direction = glm::vec3(0.2f, 1.0f, 0.3f);
        //glm::vec3 direction = glm::vec3(0.1f, 1.0f, 0.2);
        glm::vec3 color = glm::vec3(255.0/2.0);
        bool dirEnabled = true;

        TD::profiler renderTimer = TD::profiler("Render");
        TD::firstPersonCamera camera;
        const float height = 5;
        TD::random colorRnd = TD::random(0, 1);
        std::vector<TD::Light> lights = {
                TD::Light(glm::vec3(0,height,0),colorRnd.getVec3(),0.07,0.20,65),
                TD::Light(glm::vec3(5,height,0),colorRnd.getVec3(),0.07,0.20,65),
                TD::Light(glm::vec3(0,height,5),colorRnd.getVec3(),0.07,0.20,65),
                TD::Light(glm::vec3(-5,height,0),colorRnd.getVec3(),0.07,0.20,65),
                TD::Light(glm::vec3(0,height,-5),colorRnd.getVec3(),0.07,0.20,65),
                TD::Light(glm::vec3(5,height,5),colorRnd.getVec3(),0.07,0.20,65),
                TD::Light(glm::vec3(-5,height,-5),colorRnd.getVec3(),0.07,0.20,65),
                TD::Light(glm::vec3(25,height,0),colorRnd.getVec3(),0.07,0.20,65),
                TD::Light(glm::vec3(0,height,25),colorRnd.getVec3(),0.07,0.20,65),
                TD::Light(glm::vec3(-25,height,0),colorRnd.getVec3(),0.07,0.20,65),
                TD::Light(glm::vec3(0,height,-25),colorRnd.getVec3(),0.07,0.20,65),
                TD::Light(glm::vec3(25,height,25),colorRnd.getVec3(),0.07,0.20,65),
                TD::Light(glm::vec3(-25,height,-25),colorRnd.getVec3(),0.07,0.20,65),
        };
        TD::World* world = nullptr;
    public:
        TestDisplay(): Display() {}
        explicit TestDisplay(const std::string& name);
        virtual TAG_COMPOUND* onSave();
        virtual void onLoad(TAG_COMPOUND* tag);
        virtual void onSwitch();
        virtual void render();
        virtual void update();
        virtual void onLeave();
        virtual World* getWorld(){return world;}
        virtual Display* allocate(const std::string& name){
            return new TestDisplay(name);
        }
        ~TestDisplay();
    };

} // TD

#endif //ENGINE_TESTDISPLAY_H
