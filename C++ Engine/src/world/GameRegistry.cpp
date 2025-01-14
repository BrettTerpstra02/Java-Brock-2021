//
// Created by brett on 01/08/22.
//

#include "world/GameRegistry.h"
#include "logging.h"
#include "hashmaps.h"
#include "font.h"
#include <chrono>
#include <thread>
#include <atomic>
#include "window.h"

namespace TD {

    // ---------------{Threads}---------------

    const unsigned int processor_count = std::thread::hardware_concurrency() > 0 ? std::thread::hardware_concurrency() : 4;
    extern volatile bool _isWindowOpen;
    extern std::vector<std::queue<std::pair<std::string, std::string>>> unloadedModels;
    extern std::vector<std::queue<std::pair<std::string, std::string>>> unloadedTextures;
    extern parallel_flat_hash_map<std::string, Display*> displayAllocators;
    extern parallel_node_hash_map<std::string, TD::model*> loadedModels;
    extern parallel_node_hash_map<std::string, TD::Texture> loadedTextures;
    extern bool queuesCreated;
    extern vector<TD::font> fonts;
    extern DefaultLoadingScreenDisplay* defaultLoadDisplay;
    std::vector<std::thread*> modelThreads;

    std::atomic<int> done(0);
    std::atomic<int> modelLoaded(0);

    void runModelThread(int threadID){
        std::queue<std::pair<std::string, std::string>>& modelQueue = unloadedModels[threadID];
        tlog << "Load Thread {" << threadID << "} Beginning Model Loading!";
        // first load all the models
        while (!modelQueue.empty()){
            auto modelToLoad = modelQueue.front();

            std::string ident = modelToLoad.first;
            std::string path = modelToLoad.second;
            auto* model = new TD::model(path);
            loadedModels.insert(std::pair(ident, model));
            defaultLoadDisplay->modelLoaded(ident, path);
            dlog << "Loaded Model " << ident << " From " << path;

            modelQueue.pop();
        }
        tlog << "Load Thread {" << threadID << "} Model Loading Finished!";
        modelLoaded++;
        std::queue<std::pair<std::string, std::string>>& textureQueue = unloadedTextures[threadID];
        tlog << "Load Thread {" << threadID << "} Beginning Texture Loading!";
        // then load all the textures
        while (!textureQueue.empty() || modelLoaded < modelThreads.size()){
            if (!textureQueue.empty()) {
                auto textureToLoad = textureQueue.front();

                std::string ident = textureToLoad.first;
                std::string path = textureToLoad.second;

                if (ident.empty() || path.empty()) { // issue only occurs sometimes, no clue why
                    textureQueue.pop();
                    continue; // TODO: figure out why only sometimes path / ident is null.
                }

                auto *tex = new TD::texture(false, path);
                loadedTextures.insert(std::pair(ident, TD::Texture(tex, DIFFUSE, path)));
                defaultLoadDisplay->textureLoaded(ident, path);
                dlog << "Loaded Texture " << ident << " From " << path;

                textureQueue.pop();
            } else
                std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
        tlog << "Load Thread {" << threadID << "} Finished Texture Loading!";
        // finally then we are done and the loaded assets can be sent to the GPU.
        done++;
    }

    void GameRegistry::createThreadPool() {
        TD::GameRegistry::createQueues();
        for (int i = 0; i < processor_count; i++)
            modelThreads.push_back(new std::thread(runModelThread, i));
    }

    void GameRegistry::deleteThreads() {
        tlog << "Deleting loader threads, complete? : " << loadingComplete();
        for (int i = 0; i < modelThreads.size(); i++) {
            if (modelThreads[i]->joinable())
                modelThreads[i]->join();
            delete (modelThreads[i]);
        }
    }

    bool GameRegistry::loadingComplete() {
        return done >= modelThreads.size();
    }

    void GameRegistry::createQueues() {
        for (int i = 0; i < processor_count; i++) {
            unloadedModels.emplace_back();
            unloadedTextures.emplace_back();
        }
        queuesCreated = true;
    }

    // ---------------{GameRegistry}---------------

    std::vector<void* (*)()> callbacks;

    void GameRegistry::registerRegistrationCallback(void *(*funcion)()) {
        callbacks.push_back(funcion);
    }

    void GameRegistry::registerModel(const std::string& unlocalizedName, const std::string& modelPath) {
        if (!queuesCreated)
            TD::GameRegistry::createQueues();
        int smallest = 1073741824;
        int smallestPos = 0;
        for (int i = 0; i < unloadedModels.size(); i++){
            if (unloadedModels[i].size() < smallest) {
                smallest = unloadedModels[i].size();
                smallestPos = i;
            }
        }
        unloadedModels[smallestPos].push(std::pair(unlocalizedName, modelPath));
        defaultLoadDisplay->modelRegistered(unlocalizedName, modelPath);
    }

    void GameRegistry::registerTexture(const std::string& unlocalizedName, const std::string& texturePath) {
        if (!queuesCreated)
            TD::GameRegistry::createQueues();
        int smallest = 1073741824;
        int smallestPos = 0;
        for (int i = 0; i < unloadedTextures.size(); i++){
            if (unloadedTextures[i].size() < smallest) {
                smallest = unloadedTextures[i].size();
                smallestPos = i;
            }
        }
        unloadedTextures[smallestPos].push(std::pair(unlocalizedName, texturePath));
        defaultLoadDisplay->textureRegisted(unlocalizedName, texturePath);
    }

    void GameRegistry::registerThreaded() {
        for (auto & callback : callbacks){
            callback();
        }
        TD::GameRegistry::createThreadPool();
    }

    void GameRegistry::deleteResources() {
        dlog << "Deleting GameRegistry resources";
        for (auto pair : loadedModels)
            delete(pair.second);
        for (auto pair : loadedTextures)
            delete(pair.second.texture);
        dlog << "GameRegistry resources cleaned up";
    }

    TD::model* GameRegistry::getModel(std::string unlocalizedName) {
        return loadedModels.at(unlocalizedName);
    }

    TD::Texture GameRegistry::getTexture(const std::string& unlocalizedName) {
        return loadedTextures.at(unlocalizedName);
    }

    void GameRegistry::loadToGPU() {
        for (auto pair : loadedModels)
            pair.second->loadToGL();
        for (auto pair : loadedTextures)
            pair.second.texture->loadGLTexture();
    }

    void GameRegistry::registerFont(const std::string& id, std::string path, float size) {
        fonts.emplace_back(id, path, size);
    }

    void GameRegistry::registerDisplayType(const std::string& id, Display *display) {
        displayAllocators.insert(std::pair(std::string(id), display));
    }

    Display *GameRegistry::getDisplayByID(const std::string& id) {
        return displayAllocators.at(id);
    }

} // TD