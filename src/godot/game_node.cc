// Copyright 2025 Fenrir Project
// Google C++ Style Guide

#include "src/godot/game_node.hh"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

namespace fenrir {
namespace godot {

GameNode::GameNode() {
  // Constructor
}

GameNode::~GameNode() {
  // Destructor
}

void GameNode::_bind_methods() {
  // Expose methods to GDScript
  ::godot::ClassDB::bind_method(::godot::D_METHOD("hello_world"),
                                 &GameNode::hello_world);
  ::godot::ClassDB::bind_method(::godot::D_METHOD("get_tick_count"),
                                 &GameNode::get_tick_count);
}

void GameNode::_ready() {
  ::godot::UtilityFunctions::print("Fenrir GameNode ready!");
}

void GameNode::_process(double delta) {
  tick_count_++;
}

void GameNode::hello_world() {
  ::godot::UtilityFunctions::print("Hello from Fenrir C++ engine!");
}

int GameNode::get_tick_count() const {
  return tick_count_;
}

}  // namespace godot
}  // namespace fenrir
