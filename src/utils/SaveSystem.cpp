#include "SaveSystem.h"
#include <filesystem>

namespace SaveSystem {
    std::string getSavePath(int slot) {
        std::filesystem::create_directory("saves");
        return "saves/save" + std::to_string(slot) + ".dat";
    }
}