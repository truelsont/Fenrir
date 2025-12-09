#include "map_mode_selector.hh"

namespace fenrir {
namespace ui {

MapModeSelector::MapModeSelector(MapModeRegistry& registry, Logger& logger)
    : registry_(registry), logger_(logger), hovered_index_(-1), hover_time_(0.0f) {
}

void MapModeSelector::initialize(int screen_width, int screen_height) {
  buildButtons(screen_width, screen_height);
  if (!buttons_.empty()) {
    switchMode(buttons_[0].mode_id);
  }
  logger_.Log("MapModeSelector initialized with " + std::to_string(buttons_.size()) + " modes");
}

void MapModeSelector::buildButtons(int screen_width, int screen_height) {
  auto mode_ids = registry_.getAllIds();
  
  const int btn_width = 60;
  const int btn_height = 28;
  const int spacing = 4;
  const int cols = 8;
  const int rows = (mode_ids.size() + cols - 1) / cols;
  const int start_x = screen_width - (cols * (btn_width + spacing)) - 10;
  const int start_y = screen_height - (rows * (btn_height + spacing)) - 10;
  
  for (size_t i = 0; i < mode_ids.size() && i < 16; i++) {
    auto strategy = registry_.create(mode_ids[i]);
    if (!strategy) continue;
    
    int row = i / cols;
    int col = i % cols;
    
    MapModeButton btn;
    btn.mode_id = mode_ids[i];
    btn.label = strategy->getShortName();
    btn.description = strategy->getDescription();
    btn.bounds = {
      static_cast<float>(start_x + col * (btn_width + spacing)),
      static_cast<float>(start_y + row * (btn_height + spacing)),
      static_cast<float>(btn_width),
      static_cast<float>(btn_height)
    };
    
    buttons_.push_back(btn);
  }
}

void MapModeSelector::update(float delta) {
  Vector2 mouse = GetMousePosition();
  int old_hovered = hovered_index_;
  hovered_index_ = -1;
  
  for (size_t i = 0; i < buttons_.size(); i++) {
    if (CheckCollisionPointRec(mouse, buttons_[i].bounds)) {
      hovered_index_ = i;
      break;
    }
  }
  
  if (hovered_index_ != old_hovered) {
    hover_time_ = 0.0f;
  } else if (hovered_index_ >= 0) {
    hover_time_ += delta;
  }
  
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && hovered_index_ >= 0) {
    switchMode(buttons_[hovered_index_].mode_id);
  }
}

void MapModeSelector::draw() {
  for (size_t i = 0; i < buttons_.size(); i++) {
    const auto& btn = buttons_[i];
    bool is_active = (btn.mode_id == current_mode_id_);
    bool is_hovered = (static_cast<int>(i) == hovered_index_);
    
    Color bg_color = is_active ? BLUE : (is_hovered ? GRAY : DARKGRAY);
    Color text_color = (is_active || is_hovered) ? WHITE : LIGHTGRAY;
    
    DrawRectangleRec(btn.bounds, bg_color);
    DrawRectangleLinesEx(btn.bounds, 1, BLACK);
    
    int text_width = MeasureText(btn.label.c_str(), 12);
    DrawText(
      btn.label.c_str(),
      btn.bounds.x + (btn.bounds.width - text_width) / 2,
      btn.bounds.y + 8,
      12,
      text_color
    );
    
    if (is_hovered && hover_time_ > 0.5f) {
      Rectangle tooltip = {
        btn.bounds.x,
        btn.bounds.y - 45,
        200,
        40
      };
      DrawRectangleRec(tooltip, Fade(BLACK, 0.8f));
      DrawRectangleLinesEx(tooltip, 1, WHITE);
      DrawText(btn.description.c_str(), tooltip.x + 5, tooltip.y + 5, 12, WHITE);
    }
  }
}

void MapModeSelector::switchMode(const std::string& mode_id) {
  current_mode_id_ = mode_id;
  current_strategy_ = registry_.create(mode_id);
  if (current_strategy_) {
    logger_.Log("Switched to map mode: " + current_strategy_->getName());
  }
}

}  // namespace ui
}  // namespace fenrir
