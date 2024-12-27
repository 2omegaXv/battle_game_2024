#pragma once
#include "battle_game/core/bullet.h"

namespace battle_game::bullet {
class ReflCannonBall : public Bullet {
 public:
  ReflCannonBall(GameCore *core,
             uint32_t id,
             uint32_t unit_id,
             uint32_t player_id,
             glm::vec2 position,
             float rotation,
             float damage_scale,
             glm::vec2 velocity);
  ~ReflCannonBall() override;
  void Render() override;
  void Update() override;

 private:
  glm::vec2 velocity_{};
  int lifetime =600;
};
}  // namespace battle_game::bullet
