#include "game.h"
#include "projectile.h"
#include "projectile_type.h"

#include <cassert>
#include <cmath>
#include <iostream>

game::game(const int n_ticks, int num_players)
    : m_n_ticks{n_ticks},
      m_v_player(static_cast<unsigned int>(num_players), player()), m_food{1}, m_enemies{1}, m_shelters(3)
{
  for (unsigned int i = 0; i < m_v_player.size(); ++i)
  {
    m_v_player[i] =
        player(300.0 + m_dist_x_pls * i, 400.0, player_shape::rocket);
  }
  // Set color
  {
    int i = 0;
    for (auto &player : m_v_player)
    {
      player.set_color(color(i % 3 == 0 ? 255 : 0, i % 3 == 1 ? 255 : 0,
                             i % 3 == 2 ? 255 : 0));
      ++i;
    }
  }
  // Set shelters
  {
    assert(m_shelters.size() == 3);
    int i = 0;
    for (auto &this_shelter : m_shelters)
    {
      const double angle{2.0 * M_PI * static_cast<double>(i) /
                         static_cast<double>(m_shelters.size())};
      const double mid_x{400.0};
      const double mid_y{300.0};
      const double radius{200.0};
      const double x{mid_x + (std::sin(angle) * radius)};
      const double y{mid_y - (std::cos(angle) * radius)};
      const color c(i % 3 == 0 ? 255 : 0, i % 3 == 1 ? 255 : 0,
                    i % 3 == 2 ? 255 : 0, 128 + 64);
      this_shelter = shelter(x, y, 100.0, c);
      ++i;
    }
  }
}

void add_projectile(game &g, const projectile &p)
{
  g.get_projectiles().push_back(p);
}

int count_n_projectiles(const game &g) noexcept
{
  return static_cast<int>(g.get_projectiles().size());
}

void game::do_action(unsigned int player_index, action_type action)
{

  switch (action)
  {
  case action_type::turn_left:
  {
    get_player(player_index).turn_left();
    break;
  }
  case action_type::turn_right:
  {
    get_player(player_index).turn_right();
    break;
  }
  case action_type::accelerate:
  {
    get_player(player_index).accelerate();
    break;
  }
  case action_type::brake:
  {
    get_player(player_index).brake();
    break;
  }
  case action_type::shoot:
  {
    get_player(player_index).shoot();
    break;
  }
  }
}
double game::get_player_direction( int player_ind)
{
  return get_player(static_cast<unsigned int>(player_ind)).get_direction();
}

double get_player_direction(game g, unsigned int player_ind)
{
  return g.get_player(player_ind).get_direction();
}

void game::set_collision_vector(unsigned int lhs, unsigned int rhs)
{
 m_v_collisions_ind.push_back(lhs);
 m_v_collisions_ind.push_back(rhs);
}

void game::apply_inertia()
{
  for (unsigned int i = 0; i < get_v_player().size(); ++i)
  {
    if (get_player(i).get_speed() > 0)
    {
      // And should this function take some value from environment?
      do_action(i,action_type::brake);
    }
  }
}

void game::move_projectiles()
{
  for (auto &p : m_projectiles)
  {
    p.set_type(projectile_type::rocket);
    p.move();
   }
}

void game::tick()
{

  //if collision abort game
  if(has_collision(*this)) {
    const int first_player_index = get_collision_members(*this)[0];
    const int second_player_index = get_collision_members(*this)[1];
    const player& first_player = m_v_player[first_player_index];
    const player& second_player = m_v_player[second_player_index];
  if (first_player.get_color().get_red() >
     second_player.get_color().get_red())
  {
    m_v_player.erase(m_v_player.begin() + second_player_index);
  }
  else
  {
      m_v_player.erase(m_v_player.begin() + first_player_index);
  }
}


  // Moves the projectiles
  move_projectiles();

  // for now only applies inertia
  apply_inertia();

  // make the player change colors randomly, for fun, just temporary
  for (player &p : m_v_player)
  {
    p.set_color(get_adjacent_color(p.get_color()));
  }

  // players that shoot must generate projectiles
  for (player &p : m_v_player)
  {
    // When a player shoots, 'm_is_shooting' is true for one tick.
    // 'game' reads 'm_is_shooting' and if it is true,
    // it (1) creates a projectile, (2) sets 'm_is_shooting' to false
    if (p.is_shooting())
    {
      // Put the projectile just in front outside of the player
      const double d{p.get_direction()};
      const double x{p.get_x() + (std::cos(d) * p.get_size() * 1.1)};
      const double y{p.get_y() + (std::sin(d) * p.get_size() * 1.1)};
      m_projectiles.push_back(projectile(x, y, d));
    }
    p.stop_shooting();
    assert(!p.is_shooting());
  }

  // and updates m_n_ticks
  ++get_n_ticks();
}

bool has_collision(const game &g) noexcept
{
  const auto n_players = g.get_v_player().size();
  for (unsigned int i = 0; i < n_players; ++i)
  {
    for (unsigned int j = i + 1; j < n_players; ++j)
    {
      if (are_colliding(g.get_player(i), g.get_player(j)))
        {
          return true;
        }
    }
  }
  return false;
}

bool has_collision(const player& pl, const projectile& p)
{
  //Player and projectile are circularal, so use pythagoras
  const double player_radius{pl.get_size()};
  const double projectile_radius{p.get_radius()};
  const double dx = std::abs(p.get_x() - pl.get_x());
  const double dy = std::abs(p.get_y() - pl.get_y());

  if (std::abs(p.get_x() - pl.get_x()) < 0.0001
    && std::abs(p.get_y() - pl.get_y()) < 0.0001)
  {
    return true;
  }
  return false;
}

bool has_collision_with_projectile(const game & g) noexcept
{
  const auto& projectiles = g.get_projectiles();
  if (projectiles.empty()) return false;
  const auto& players = g.get_v_player();

  for (const auto& p : projectiles)
  {
    for (const auto& pl : players)
    {
      if (has_collision(pl, p)) return true;
    }
  }
  return false;
}

std::vector<unsigned int> get_collision_members(const game &g) noexcept
{
  std::vector<unsigned int> v_collisions;
  const auto n_players = g.get_v_player().size();
  for (unsigned int i = 0; i < n_players; ++i)
  {
    for (unsigned int j = i + 1; j < n_players; ++j)
    {
      if (are_colliding(g.get_player(i), g.get_player(j)))
        {
          v_collisions.push_back(i);
          v_collisions.push_back(j);
        }
    }
  }
  return v_collisions;
}

void test_game() //!OCLINT tests may be many
{
  // The game has done zero ticks upon startup
  {
    const game g;
    // n_ticks is the number of times the game is displayed on screen
    assert(g.get_n_ticks() == 0);
  }
  // A game has a vector of players
  {
    const game g;
    // The value 1234.5 is irrelevant: just get this to compile
    for (unsigned int i = 0; i < g.get_v_player().size(); ++i)
    {
      assert(g.get_player(i).get_x() > -1234.5);
    }
  }
  // A game has food items
  {
    const game g;
    assert(!g.get_food().empty());
  }
  // A game has enemies
  {
    const game g;
    assert(!g.get_enemies().empty());
  }
  // A game responds to actions: player can turn left
  {
    game g;
    for (unsigned int i = 0; i < g.get_v_player().size(); ++i)
    {
      const double before{g.get_player(i).get_direction()};
      g.do_action(i,action_type::turn_left);
      const double after{g.get_player(i).get_direction()};
      assert(std::abs(before - after) > 0.01); // Should be different
    }
  }
  // A game responds to actions: player can turn right
  {
    game g;
    for (unsigned int i = 0; i < g.get_v_player().size(); ++i)
    {
      const double before{g.get_player(i).get_direction()};
      g.do_action(i, action_type::turn_right);
      const double after{g.get_player(i).get_direction()};
      assert(std::abs(before - after) > 0.01); // Should be different
    }
  }
  // A game responds to actions: player can accelerate
  {
    game g;
    for (unsigned int i = 0; i < g.get_v_player().size(); ++i)
    {
      const double before{g.get_player(i).get_speed()};
      g.do_action(i, action_type::accelerate);
      const double after{g.get_player(i).get_speed()};
      assert(before - after < 0.01); // After should be > than before
    }
  }
  // A game responds to actions: player can break
  {
    game g;
    for (unsigned int i = 0; i < g.get_v_player().size(); ++i)
    {
      // give the player a speed of more than 0
      g.do_action(i, action_type::accelerate);
      const double before{g.get_player(i).get_speed()};
      g.do_action(i, action_type::brake);
      const double after{g.get_player(i).get_speed()};
      assert(before - after > 0.0000000000000001);
      // After should be < than before
    }
  }
  // A game responds to actions: player can shoot
  {
    game g;
    assert(count_n_projectiles(g) == 0);
    g.do_action(0, action_type::shoot);
    // Without a tick, no projectile is formed yet
    assert(count_n_projectiles(g) == 0);
  }
  // A game responds to actions: player can shoot
  {
    game g;
    assert(count_n_projectiles(g) == 0);
    g.do_action(0, action_type::shoot);
    g.tick();
    assert(count_n_projectiles(g) == 1);
  }
  // Projectiles move
  {
    game g;
    g.do_action(0, action_type::shoot);
    g.tick();
    assert(count_n_projectiles(g) == 1);
    const double x_before{g.get_projectiles()[0].get_x()};
    const double y_before{g.get_projectiles()[0].get_y()};
    g.tick();
    const double x_after{g.get_projectiles()[0].get_x()};
    const double y_after{g.get_projectiles()[0].get_y()};
    // coordinats should differ
    assert(std::abs(x_before - x_after) > 0.01 ||
           std::abs(y_before - y_after) > 0.01);
  }
  // Can get a player's direction by using a free function
  {
    const game g;
    for (unsigned int i = 0; i < g.get_v_player().size(); ++i)
    {
      const double a{g.get_player(i).get_direction()};
      const double b{get_player_direction(g, i)};
      assert(std::abs(b - a) < 0.0001);
    }
  }
  // game by default has a mix and max evironment size
  {
    game g;
    assert(g.get_environment().get_max_x() > -56465214.0);
  }

  // A game has enemies
  {
    const game g;
    assert(!g.get_enemies().empty());
  }

  // calling tick updates the counter and

  // callling tick() increases m_n_tick by one
  {
    game g;
    const int before = g.get_n_ticks();
    g.tick();
    assert(g.get_n_ticks() - before == 1);
  }
  // inertia  slows down players
  {
    game g;
    std::vector<double> before_v;
    std::vector<double> after_v;
    for (unsigned int i = 0; i < g.get_v_player().size(); ++i)
    {
      // give the player a speed of more than 0
      g.do_action(i, action_type::accelerate);
      before_v.push_back(g.get_player(i).get_speed());
    }
    g.apply_inertia();
    for (unsigned int i = 0; i < g.get_v_player().size(); ++i)
    {
      after_v.push_back(g.get_player(i).get_speed());
    }
    for (unsigned int i = 0; i < g.get_v_player().size(); ++i)
    {
      assert(before_v[i] - after_v[i] > 0.0000000000000001);
      // After should be < than before
    }
  }

  // players are placed at dist of 300 points
  // along the x axis at initialization
  {
    game g;
    for (unsigned int i = 0; i < (g.get_v_player().size() - 1); ++i)
    {
      assert(g.get_player(i).get_x() - g.get_player(i + 1).get_x() +
                     g.get_dist_x_pls() <
                 0.000001 &&
             g.get_player(i).get_x() - g.get_player(i + 1).get_x() +
                     g.get_dist_x_pls() >
                 -0.000001);
    }
  }

  // In the start of the game no players are colliding
  {
    game g;
    assert(!has_collision(g));
  }
  // two overlapping players signal a collision
  {
    game g;
    g.get_player(1).set_x(g.get_player(0).get_x());
    g.get_player(1).set_y(g.get_player(0).get_y());

    assert(has_collision(g));
  }
  // A collision destroys a player
  {
    game g;
    const auto n_players_before = g.get_v_player().size();
    g.get_player(1).set_x(g.get_player(0).get_x());
    g.get_player(1).set_y(g.get_player(0).get_y());
    assert(has_collision(g));
    g.tick();
    const auto n_players_after = g.get_v_player().size();
    assert(n_players_after < n_players_before);
  }
  // A collision destroy one of the colliding player
  {
    game g;
    const auto n_players_before = g.get_v_player().size();
    g.get_player(1).set_x(g.get_player(0).get_x());
    g.get_player(1).set_y(g.get_player(0).get_y());
    assert(has_collision(g));
    g.tick();
    const auto n_players_after = g.get_v_player().size();
    assert(n_players_after < n_players_before);
    assert(!has_collision(g));
    g.tick();
    const auto n_players_after_after = g.get_v_player().size();
    assert(n_players_after_after == n_players_after);
  }
  //Initially, there is no collision with a projectile
  {
    game g;
    assert(!has_collision_with_projectile(g));
  }
  //If a projectile is put on top of a player, there is a collision
  {
    game g;
    const auto x = g.get_player(0).get_x();
    const auto y = g.get_player(0).get_y();
    add_projectile(g, projectile(x, y));
    assert(!g.get_projectiles().empty());
    assert(has_collision_with_projectile(g));
  }
  //If a projectile is 0.99 of its radius right of a player, there is a collision
  {
    game g;
    const double radius = 12.34;
    const auto x = g.get_player(0).get_x() + (0.99 * radius);
    const auto y = g.get_player(0).get_y();
    const projectile p(x, y, 0.0, projectile_type::rocket, radius);
    add_projectile(g, p);
    assert(!g.get_projectiles().empty());
    assert(has_collision_with_projectile(g));
  }
  //If a projectile is 1.01 of its radius right of a player, there is no collision
  {
    game g;
    const double radius = 12.34;
    const auto x = g.get_player(0).get_x() + (1.01*radius);
    const auto y = g.get_player(0).get_y();
    const projectile p(x, y, 0.0, projectile_type::rocket, radius);
    add_projectile(g, p);
    assert(!g.get_projectiles().empty());
    assert(has_collision_with_projectile(g));
  }

  #ifdef FIX_ISSUE_135
  // In the start of the game, there is no player-food collision
  {
    game g;
    assert(!has_food_collision(g));
  }
  #endif // FIX_ISSUE_135
  #ifdef FIX_ISSUE_139
  //Can modify food items, for example, delete all food items
  {
    game g;
    g.get_food();
    assert(!g.get_food().empty());
    g.get_food().clear();
    assert(g.get_food().empty());
  }
  #endif //FIX_ISSUE_139
  #ifdef FIX_ISSUE_138
  // In the start of the game, there is no player-enemy collision
  {
    game g;
    assert(!has_enemy_collision(g));
  }
  #endif // FIX_ISSUE_138
  //If red eats green then red survives
  {
    game g;
    assert(g.get_player(0).get_color().get_red() == 255);
    assert(g.get_player(1).get_color().get_green() == 255);
    g.get_player(1).set_x(g.get_player(0).get_x());
    g.get_player(1).set_y(g.get_player(0).get_y());
    assert(has_collision(g));
    g.tick();
    assert(g.get_player(0).get_color().get_red() > 250);
  }
  #ifdef FIX_ISSUE_VALENTINES_DAY
  //If green eats blue then green survives
  {
    game g;
    assert(g.get_player(1).get_color().get_green() == 255);
    assert(g.get_player(2).get_color().get_blue() == 255);
    g.get_player(1).set_x(g.get_player(2).get_x());
    g.get_player(1).set_y(g.get_player(2).get_y());
    assert(has_collision(g));
    g.tick();
    assert(g.get_player(1).get_color().get_green() > 250);
  }
  #endif // FIX_ISSUE_VALENTINES_DAY

}


