#include "battle_game/core/bullets/canBall_refl.h"

#include "battle_game/core/game_core.h"
#include "battle_game/core/particles/particles.h"

namespace battle_game::bullet {
ReflCannonBall::ReflCannonBall(GameCore *core,
                       uint32_t id,
                       uint32_t unit_id,
                       uint32_t player_id,
                       glm::vec2 position,
                       float rotation,
                       float damage_scale,
                       glm::vec2 velocity)
    : Bullet(core, id, unit_id, player_id, position, rotation, damage_scale),
      velocity_(velocity) {
}

void ReflCannonBall::Render() {
  SetTransformation(position_, rotation_, glm::vec2{0.05f});
  SetColor(glm::vec4{0.0f,0.0f,0.0f,1.0f});
  SetTexture(BATTLE_GAME_ASSETS_DIR "textures/particle3.png");
  DrawModel(0);
}

void ReflCannonBall::Update() {
  auto p = position_ + velocity_ * kSecondPerTick;
  lifetime--;
  bool should_die = false;
  if(!lifetime) should_die = true;
  int ty = game_core_->IsBlockedByObstaclesY(p);
  int tx = game_core_->IsBlockedByObstaclesX(p);
  if (ty){
    velocity_.y = -velocity_.y;
    if(ty==2) position_ = p;
    return;
  }
  if (tx){
    velocity_.x = -velocity_.x;
    if(tx==2) position_ = p;
    return;
  }
  position_ = p;
  auto &units = game_core_->GetUnits();
  for (auto &unit : units) {
    if (unit.first == unit_id_) {
      continue;
    }
    if (unit.second->IsHit(position_)) {
      game_core_->PushEventDealDamage(unit.first, id_, damage_scale_ * 15.0f);
      should_die = true;
    }
  }

  if (should_die) {
    game_core_->PushEventRemoveBullet(id_);
  }
}

ReflCannonBall::~ReflCannonBall() {
  for (int i = 0; i < 5; i++) {
    game_core_->PushEventGenerateParticle<particle::Smoke>(
        position_, rotation_, game_core_->RandomInCircle() * 2.0f, 0.2f,
        glm::vec4{0.0f, 0.0f, 0.0f, 1.0f}, 3.0f);
  }
}
}  // namespace battle_game::bullet
