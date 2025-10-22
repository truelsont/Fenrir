// Copyright 2025 Fenrir Project
// Google C++ Style Guide

#ifndef FENRIR_SRC_GODOT_GAME_NODE_HH_
#define FENRIR_SRC_GODOT_GAME_NODE_HH_

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/core/class_db.hpp>
#include "src/state/worldState/worldManager.hh"

namespace fenrir {
namespace godot {

class GameNode : public ::godot::Node {
  GDCLASS(GameNode, ::godot::Node)

 private:
  int tick_count_ = 0;
  fenrir::WorldManager world_manager_;

 protected:
  static void _bind_methods();

 public:
  GameNode();
  ~GameNode();

  // Godot lifecycle
  void _ready() override;
  void _process(double delta) override;

  // Exposed to GDScript
  void hello_world();
  int get_tick_count() const;
  ::godot::Ref<::godot::ImageTexture> get_world_texture();
};

}  // namespace godot
}  // namespace fenrir

#endif  // FENRIR_SRC_GODOT_GAME_NODE_HH_
