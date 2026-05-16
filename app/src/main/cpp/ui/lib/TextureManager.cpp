#include "TextureManager.h"
#include <android/log.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <android/asset_manager.h>

namespace LoadLib {

TextureManager& TextureManager::get() {
    static TextureManager instance;
    return instance;
}

void TextureManager::setAssetManager(AAssetManager* mgr) {
    assetManager = mgr;
}

Texture* TextureManager::loadTexture(const std::string& path) {
    auto it = textures.find(path);
    if (it != textures.end()) return &it->second;

    if (!assetManager) {
        __android_log_print(ANDROID_LOG_ERROR, "MarkClient", "AssetManager not set");
        return nullptr;
    }

    AAsset* asset = AAssetManager_open(assetManager, path.c_str(), AASSET_MODE_STREAMING);
    if (!asset) {
        __android_log_print(ANDROID_LOG_ERROR, "MarkClient", "Failed to open asset: %s", path.c_str());
        return nullptr;
    }

    off_t len = AAsset_getLength(asset);
    std::vector<unsigned char> buffer(len);
    AAsset_read(asset, buffer.data(), len);
    AAsset_close(asset);

    int w, h, ch;
    unsigned char* data = stbi_load_from_memory(buffer.data(), static_cast<int>(len), &w, &h, &ch, 4);
    if (!data) {
        __android_log_print(ANDROID_LOG_ERROR, "MarkClient", "stbi failed: %s", path.c_str());
        return nullptr;
    }

    Texture tex;
    tex.width = w;
    tex.height = h;
    tex.channels = ch;

    glGenTextures(1, &tex.id);
    glBindTexture(GL_TEXTURE_2D, tex.id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);

    textures[path] = tex;
    __android_log_print(ANDROID_LOG_INFO, "MarkClient", "Loaded texture: %s (%dx%d)", path.c_str(), w, h);
    return &textures[path];
}

void TextureManager::bindTexture(GLuint texId, int slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texId);
}

void TextureManager::cleanup() {
    for (auto& [path, tex] : textures) {
        if (tex.id) glDeleteTextures(1, &tex.id);
    }
    textures.clear();
}

}
