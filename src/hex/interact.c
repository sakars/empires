#include "interact.h"
#include "hexgrid.h"
#include <stdio.h>

void interact_with_neighbours(Hex *hex, Hex *neighbours[6]) {
  if (hex == NULL || neighbours == NULL) {
    fprintf(stderr, "Invalid hex or neighbours array\n");
    return;
  }
  // float resource_efficiency = generate_faction_attribute(
  //     hex->faction_id, ATTRIBUTE_RESOURCE_EFFICIENCY);
  // float military_strength =
  //     generate_faction_attribute(hex->faction_id,
  //     ATTRIBUTE_MILITARY_STRENGTH);
  // float population_growth =
  //     generate_faction_attribute(hex->faction_id,
  //     ATTRIBUTE_POPULATION_GROWTH);
  // float military_efficiency = generate_faction_attribute(
  //     hex->faction_id, ATTRIBUTE_MILITARY_EFFICIENCY);
  // float gamblers =
  //     generate_faction_attribute(hex->faction_id, ATTRIBUTE_GAMBLERS);

  // For now, ignore attributes, they will come into play later

  // Pick one attackable neighbour at random and attack
  int attackable_indices[6];
  int attackable_count = 0;
  for (int i = 0; i < 6; i++) {
    if (neighbours[i] != NULL && neighbours[i]->faction_id > 0 &&
        neighbours[i]->faction_id != hex->faction_id &&
        hex->military > neighbours[i]->population * 0.1f &&
        hex->military > neighbours[i]->military * 0.8f) {
      attackable_indices[attackable_count++] = i;
    }
  }
  if (attackable_count > 0) {
    int idx = attackable_indices[rand() % attackable_count];
    Hex *target = neighbours[idx];
    int32_t total_military = hex->military + target->military;
    if (total_military <= 0) {
      // No military to fight with
      fprintf(stderr, "No military to fight with\n");
      return;
    }
    if (rand() % total_military < hex->military) {
      // I win the battle
      target->faction_id = hex->faction_id;
      target->military = hex->military * 0.25f;
      hex->military = hex->military * 0.25f;
      int32_t resources_gained = target->resources * 0.5f;
      hex->resources += resources_gained;
      target->resources -= resources_gained;
    } else {
      // I lose the battle
      hex->military = hex->military * 0.25f;
      target->military = target->military * 0.25f;
    }
  }

  // If my friendly neighbour has less military than me, I can
  // reinforce them
  int32_t reinforcements = hex->military * 0.1f;
  int32_t current_military = hex->military;
  for (int i = 0; i < 6; i++) {
    if (neighbours[i] != NULL && neighbours[i]->faction_id == hex->faction_id &&
        neighbours[i]->military < current_military) {
      if (reinforcements > 0) {
        neighbours[i]->military += reinforcements;
        hex->military -= reinforcements;
        if (neighbours[i]->military > 1000) {
          hex->military +=
              neighbours[i]->military - 1000; // Cap military at 1000
          neighbours[i]->military = 1000;     // Cap military at 1000
        }
      }
    }
  }
  // Do the same with resources, if I have more resources than my
  // friendly neighbour, I can give them some resources
  int32_t resources_to_give = hex->resources * 0.1f;
  int32_t current_resources = hex->resources;
  for (int i = 0; i < 6; i++) {
    if (neighbours[i] != NULL && neighbours[i]->faction_id == hex->faction_id &&
        neighbours[i]->resources < current_resources * 0.5f) {
      if (resources_to_give > 0) {
        neighbours[i]->resources += resources_to_give;
        hex->resources -= resources_to_give;
        if (neighbours[i]->resources > 10000) {
          hex->resources +=
              neighbours[i]->resources - 10000; // Cap resources at 10,000
          neighbours[i]->resources = 10000;     // Cap resources at 10,000
        }
      }
    }
  }
  // if there's no enemy neighbour, reduce military by 10%
  int has_enemy_neighbour = 0;
  for (int i = 0; i < 6; i++) {
    if (neighbours[i] != NULL && neighbours[i]->faction_id > 0 &&
        neighbours[i]->faction_id != hex->faction_id) {
      has_enemy_neighbour = 1;
      break;
    }
  }
  if (!has_enemy_neighbour) {
    hex->military -= hex->military * 0.1f; // Reduce military by 10%
    if (hex->military < 0) {
      hex->military = 0; // Ensure military doesn't go negative
    }
  }
}

void update_hex(Hex *hex) {
  if (hex == NULL) {
    fprintf(stderr, "Invalid hex pointer\n");
    return;
  }
  hex->resources += hex->production; // Add production to resources
  hex->military +=
      hex->population * 0.01f; // Increase military by 1% of population
  if (hex->military < 0) {
    hex->military = 0; // Ensure military doesn't go negative
  }
  if (hex->military > 1000) {
    hex->military = 1000; // Cap military at 1000
  }
  hex->resources -= hex->population; // Each person consumes 1 resource per turn
  hex->resources -=
      hex->military * 1.6f; // Each military unit consumes 1.1 resources
  if (hex->resources < 0) {
    hex->military = hex->military * 0.6f;     // Lose 10% of military
    hex->population = hex->population * 0.6f; // Lose 10% of population
    hex->resources = 0;                       // Resources cannot go negative
    // hex->faction_id = rand() % 40000000;
  }
  hex->population += hex->population * 0.01f; // Increase population by 1%
  if (hex->population > 20000) {
    hex->population = 20000; // Cap population at 20,000
  }
  if (hex->resources < 0) {
    hex->resources = 0; // Ensure resources don't go negative
  }
  // hex->resources += hex->production; // Add production to resources

  // // People consume resources
  // hex->resources -= hex->population; // Each person consumes 1 resource per
  // turn
  // // Military consumes resources
  // hex->resources -=
  //     hex->military * 1.1f; // Each military unit consumes 1.1 resources
  // // If there are no resources, people rebel and military is reduced
  // if (hex->resources < 0) {
  //   hex->military -= hex->military * 0.1f;     // Lose 10% of military
  //   hex->population -= hex->population * 0.1f; // Lose 10% of population
  //   hex->resources = 0;                        // Resources cannot go
  //   negative
  // }
  // hex->population += hex->population * 0.01f; // Increase population by 1%
  // hex->military += hex->military * 0.01f;     // Increase military by 1%
  // if (hex->population > 20000) {
  //   hex->population = 20000; // Cap population at 20,000
  // }
  // if (hex->resources < 0) {
  //   hex->resources = 0; // Ensure resources don't go negative
  // }
  // if (hex->military < 0) {
  //   hex->military = 0; // Ensure military doesn't go negative
  // }
  // if (hex->population < 0) {
  //   hex->population = 0; // Ensure population doesn't go negative
  // }
}