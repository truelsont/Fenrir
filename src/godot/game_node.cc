// Copyright 2025 Fenrir Project
// Google C++ Style Guide

#include "src/godot/game_node.hh"

#include <boost/log/trivial.hpp>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

namespace fenrir {
namespace godot {

GameNode::GameNode() {
  // Constructor
  BOOST_LOG_TRIVIAL(info) << "GameNode constructor called";
}

GameNode::~GameNode() {
  // Destructor
}

void GameNode::_bind_methods() {
  ::godot::ClassDB::bind_method(::godot::D_METHOD("hello_world"),
                                 &GameNode::hello_world);
  ::godot::ClassDB::bind_method(::godot::D_METHOD("get_tick_count"),
                                 &GameNode::get_tick_count);
  ::godot::ClassDB::bind_method(::godot::D_METHOD("get_world_texture"),
                                 &GameNode::get_world_texture);
  ::godot::ClassDB::bind_method(::godot::D_METHOD("get_world_texture_viewport", "x", "y", "width", "height"),
                                 &GameNode::get_world_texture_viewport);
}

void GameNode::_ready() {
  BOOST_LOG_TRIVIAL(info) << "Fenrir GameNode ready!";
  ::godot::UtilityFunctions::print("Fenrir GameNode ready!");
}

void GameNode::_process(double delta) {
  tick_count_++;
}

void GameNode::hello_world() {
  BOOST_LOG_TRIVIAL(info) << "hello_world() called - tick_count: " << tick_count_;
  ::godot::UtilityFunctions::print("Hello from Fenrir C++ engine!");
}

int GameNode::get_tick_count() const {
  return tick_count_;
}

::godot::Ref<::godot::ImageTexture> GameNode::get_world_texture() {
  fenrir::WorldManager::rendering_options_t rendering_options;
  rendering_options.map_mode = fenrir::WorldManager::kOWNER;
  rendering_options.view_port_x = 0;
  rendering_options.view_port_y = 0;
  rendering_options.view_port_width = 1280;
  rendering_options.view_port_height = 720;
  return world_manager_.createImageTexture(rendering_options);
}

::godot::Ref<::godot::ImageTexture> GameNode::get_world_texture_viewport(
    int x, int y, int width, int height) {
  fenrir::WorldManager::rendering_options_t rendering_options;
  rendering_options.map_mode = fenrir::WorldManager::kPROVINCE_DEBUG; 
  rendering_options.view_port_x = x;
  rendering_options.view_port_y = y;
  rendering_options.view_port_width = width;
  rendering_options.view_port_height = height;
  return world_manager_.createImageTexture(rendering_options);
}

}  // namespace godot
}  // namespace fenrir
