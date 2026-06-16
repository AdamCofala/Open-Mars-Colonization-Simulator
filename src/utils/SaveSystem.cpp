#include "SaveSystem.h"
#include <filesystem>

std::string SaveSystem::getSavePath(int slot) {
    std::filesystem::create_directory("saves");
    return "saves/save" + std::to_string(slot) + ".dat";
}