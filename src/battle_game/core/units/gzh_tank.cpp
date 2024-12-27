#include "gzh_tank.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

namespace {
uint32_t tank_body_model_index = 0xffffffffu;
uint32_t tank_turret_model_index = 0xffffffffu;
}  // namespace

RachTank::RachTank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Unit(game_core, id, player_id) {
  if (!~tank_body_model_index) {
    auto mgr = AssetsManager::GetInstance();
    {
      /* Tank Body */ 
          std::vector<ObjectVertex> body_vertices = {
              // dark blue 
              {{0.0f, 0.5f}, {0.0f, 0.0f}, {0.055f, 0.203f, 0.531f, 1.0f}},
              {{-1.0f, -1.0f}, {0.0f, 0.0f}, {0.055f, 0.203f, 0.531f, 1.0f}},
              {{-0.64f, -1.0f}, {0.0f, 0.0f}, {0.055f, 0.203f, 0.531f, 1.0f}},
              {{0.18f, 0.23f}, {0.0f, 0.0f}, {0.055f, 0.203f, 0.531f, 1.0f}},
              // light blue
              {{1.0f, -1.0f}, {0.0f, 0.0f}, {0.0f, 0.668f, 0.906f, 1.0f}},
              {{0.5f, -1.0f}, {0.0f, 0.0f}, {0.0f, 0.668f, 0.906f, 1.0f}},
              {{-0.15f, -0.52f}, {0.0f, 0.0f}, {0.0f, 0.668f, 0.906f, 1.0f}},
              {{0.27f, 0.1f}, {0.0f, 0.0f}, {0.0f, 0.668f, 0.906f, 1.0f}},{{0.52f, -0.3f}, {0.0f, 0.0f}, {0.0f, 0.668f, 0.906f, 1.0f}}
          };
          std::vector<uint32_t> body_indices = {
              // Dark blue  (indices 0..3)
              0, 1, 2,    0, 2, 3,
              // Light blue (indices 4..7)
              8, 6, 7,    4,5,8, 4,7,8, 8,6
          };
          const float centerX = 0.0f;
          const float centerY = -1.0f;
          const float radius  = 0.5f;
          const int   segments = 32;
          body_indices.push_back(segments+8);
          // Store the first index of the circle
          unsigned int circleStart = static_cast<unsigned int>(body_vertices.size());
      
          for (int i = 0; i < segments; i++) {
              float theta = (0.6f * static_cast<float>(M_PI)) * (static_cast<float>(i) / segments);
              float px = centerX + radius * std::cos(theta);
              float py = centerY + radius * std::sin(theta);
      
              // Push each circle vertex with your chosen color (example: same light blue)
              body_vertices.push_back({{px, py}, {0.0f, 0.0f}, {0.0f, 0.668f, 0.906f, 1.0f}});
          }
      
          // 4) Create indices for a line loop around the circle
          for (int i = 0; i < segments-1; i++) {
              body_indices.push_back(circleStart-1);
              body_indices.push_back(circleStart + i);
              body_indices.push_back(circleStart + i+1);
          }

    
      tank_body_model_index = mgr->RegisterModel(body_vertices,body_indices);
    }

    {
      /* Tank Turret */
      std::vector<ObjectVertex> turret_vertices;
      std::vector<uint32_t> turret_indices;
      const int precision = 60;
      const float inv_precision = 1.0f / float(precision);
      for (int i = 0; i < precision; i++) {
        auto theta = (float(i) + 0.5f) * inv_precision;
        theta *= glm::pi<float>() * 2.0f;
        auto sin_theta = std::sin(theta);
        auto cos_theta = std::cos(theta);
        turret_vertices.push_back({{sin_theta * 0.15f, cos_theta * 0.15f},
                                   {0.0f, 0.0f},
                                   {0.5f, 0.5f, 0.5f, 1.0f}});
        turret_indices.push_back(i);
        turret_indices.push_back((i + 1) % precision);
        turret_indices.push_back(precision);
      }
      turret_vertices.push_back(
          {{0.0f, 0.0f}, {0.0f, 0.0f}, {0.5f, 0.5f, 0.5f, 1.0f}});
      turret_vertices.push_back(
          {{-0.15f, 0.0f}, {0.0f, 0.0f}, {0.5f, 0.5f, 0.5f, 1.0f}});
      turret_vertices.push_back(
          {{0.15f, 0.0f}, {0.0f, 0.0f}, {0.5f, 0.5f, 0.5f, 1.0f}});
      turret_vertices.push_back(
          {{-0.05f, 2.0f}, {0.0f, 0.0f}, {0.5f, 0.5f, 0.5f, 1.0f}});
      turret_vertices.push_back(
          {{0.05f, 2.0f}, {0.0f, 0.0f}, {0.5f, 0.5f, 0.5f, 1.0f}});
      turret_indices.push_back(precision + 1 + 0);
      turret_indices.push_back(precision + 1 + 1);
      turret_indices.push_back(precision + 1 + 2);
      turret_indices.push_back(precision + 1 + 1);
      turret_indices.push_back(precision + 1 + 2);
      turret_indices.push_back(precision + 1 + 3);
      tank_turret_model_index =
          mgr->RegisterModel(turret_vertices, turret_indices);
    }
  }
}

void RachTank::Render() {
  battle_game::SetTransformation(position_, rotation_);
  battle_game::SetTexture(0);
  battle_game::SetColor(game_core_->GetPlayerColor(player_id_));
  battle_game::DrawModel(tank_body_model_index);
  battle_game::SetRotation(turret_rotation_);
  battle_game::DrawModel(tank_turret_model_index);
}

void RachTank::Update() {
  TankMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  Fire();
}

void RachTank::TankMove(float move_speed, float rotate_angular_speed) {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    glm::vec2 offset{0.0f};
    if (input_data.key_down[GLFW_KEY_W]) {
      offset.y += 1.0f;
    }
    if (input_data.key_down[GLFW_KEY_S]) {
      offset.y -= 1.0f;
    }
    float speed = move_speed * GetSpeedScale();
    offset *= kSecondPerTick * speed;
    auto new_position =
        position_ + glm::vec2{glm::rotate(glm::mat4{1.0f}, rotation_,
                                          glm::vec3{0.0f, 0.0f, 1.0f}) *
                              glm::vec4{offset, 0.0f, 0.0f}};
    if (!game_core_->IsBlockedByObstacles(new_position)) {
      game_core_->PushEventMoveUnit(id_, new_position);
    }
    float rotation_offset = 0.0f;
    if (input_data.key_down[GLFW_KEY_A]) {
      rotation_offset += 1.0f;
    }
    if (input_data.key_down[GLFW_KEY_D]) {
      rotation_offset -= 1.0f;
    }
    rotation_offset *= kSecondPerTick * rotate_angular_speed * GetSpeedScale();
    game_core_->PushEventRotateUnit(id_, rotation_ + rotation_offset);
  }
}

void RachTank::TurretRotate() {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    auto diff = input_data.mouse_cursor_position - position_;
    if (glm::length(diff) < 1e-4) {
      turret_rotation_ = rotation_;
    } else {
      turret_rotation_ = std::atan2(diff.y, diff.x) - glm::radians(90.0f);
    }
  }
}

void RachTank::Fire() {
  if (fire_count_down_ == 0) {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        auto velocity = Rotate(glm::vec2{0.0f, 30.0f}, turret_rotation_);
        GenerateBullet<bullet::ReflCannonBall>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity);
        fire_count_down_ = 20;  // Fire interval 1 second.
      }
    }
  }
  if (fire_count_down_) {
    fire_count_down_--;
  }
}

bool RachTank::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return position.x > -0.8f && position.x < 0.8f && position.y > -1.0f &&
         position.y < 1.0f && position.x + position.y < 1.6f &&
         position.y - position.x < 1.6f;
}

const char *RachTank::UnitName() const {
  return "Rachmaninov Tank";
}

const char *RachTank::Author() const {
  return "gzhcharlie";
}
}  // namespace battle_game::unit
