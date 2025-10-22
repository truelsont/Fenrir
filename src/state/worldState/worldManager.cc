// Copyright 2025 Fenrir Project
// Google C++ Style Guide

#include "worldManager.hh"
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>

namespace fenrir {

godot::Ref<godot::ImageTexture> WorldManager::createImageTexture() {
    // Create a PackedByteArray from the pixel data
    godot::PackedByteArray byte_array;
    byte_array.resize(width * height * 4);  // RGBA = 4 bytes per pixel
    
    // Copy pixel data into the byte array
    for (size_t i = 0; i < pixels.size(); i++) {
        byte_array[i * 4 + 0] = pixels[i].r;
        byte_array[i * 4 + 1] = pixels[i].g;
        byte_array[i * 4 + 2] = pixels[i].b;
        byte_array[i * 4 + 3] = pixels[i].a;
    }
    
    // Create a Godot Image from the byte array
    // Format: Image::FORMAT_RGBA8 (8-bit RGBA)
    godot::Ref<godot::Image> image = godot::Image::create_from_data(
        width, 
        height, 
        false,  // no mipmaps
        godot::Image::FORMAT_RGBA8, 
        byte_array
    );
    
    // Create an ImageTexture from the Image
    godot::Ref<godot::ImageTexture> texture = godot::ImageTexture::create_from_image(image);
    
    return texture;
}

fenrir::ServiceCodes WorldManager::serializeService() {
    // TODO: Implement serialization
    return fenrir::ServiceCodes::kSERVICE_SERIALIZE_SUCCESS;
}

fenrir::ServiceCodes WorldManager::deserializeService() {
    // TODO: Implement deserialization
    return fenrir::ServiceCodes::kSERVICE_SERIALIZE_SUCCESS;
}

}  // namespace fenrir
