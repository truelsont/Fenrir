// Copyright 2025 Fenrir Project
// Google C++ Style Guide

#ifndef FENRIR_SRC_GODOT_GAME_NODE_HH_
#define FENRIR_SRC_GODOT_GAME_NODE_HH_

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>

namespace fenrir {
namespace godot {

class GameNode : public ::godot::Node {
  GDCLASS(GameNode, ::godot::Node)

 private:
  int tick_count_ = 0;

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
};

}  // namespace godot
}  // namespace fenrir

#endif  // FENRIR_SRC_GODOT_GAME_NODE_HH_
