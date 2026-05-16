#pragma once
#include <GLES3/gl3.h>
#include <string>
#include <unordered_map>
#include <android/asset_manager.h>

namespace LoadLib {

struct Texture {
    GLuint id = 0;
    int width = 0;
    int height = 0;
    int channels = 0;
};

class TextureManager {
public:
    static TextureManager& get();
    void setAssetManager(AAssetManager* mgr);
    Texture* loadTexture(const std::string& path);
    void bindTexture(GLuint texId, int slot = 0);
    void cleanup();

private:
    TextureManager() = default;
    ~TextureManager() = default;
    AAssetManager* assetManager = nullptr;
    std::unordered_map<std::string, Texture> textures;
};

}
