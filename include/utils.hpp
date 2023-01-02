#include <SFML/Graphics.hpp>

#ifndef UTILS_H
#define UTILS_H

enum Directions{
    north,
    south,
    east,
    west,
    all
};

std::default_random_engine generator;
std::uniform_real_distribution<float> distribution(0.0, 1.0);

bool get_rand_bool(float chance = 0.5){
    return distribution(generator) < chance;
}

float get_rand_float(){
    return distribution(generator);
}


template<typename Base, typename T>
inline bool instanceof(const T *ptr) {
   return dynamic_cast<const Base*>(ptr) != nullptr;
}

class Cell{
    protected:
    sf::Vector2f pos, area;
    sf::RenderWindow *window;
    public:
    Cell(sf::Vector2f pos, sf::RenderWindow *window): pos(pos), area(sf::Vector2f(config::block_edge_size, config::block_edge_size)), window(window) {}
};

class GridCell : public Cell{
    protected:
    sf::RectangleShape shape;
    uint32_t tick_born;
    int grid_height, grid_width;
    public:
    GridCell(sf::Vector2f pos, sf::RenderWindow *window, uint32_t tick_born): Cell(pos, window), grid_height(config::grid_edge_size), grid_width(config::grid_edge_size), tick_born(tick_born){}

    sf::Vector2f get_pos_in_direction(Directions in_direction){
        if (in_direction == all){
            return sf::Vector2f(-1,-1);
        }

        if (in_direction == north){
            return sf::Vector2f(pos.x, (pos.y > 0) ? (pos.y - 1) : grid_height - 1);
        }
        if (in_direction == east){
            return sf::Vector2f((pos.x + 1) < grid_width ? pos.x + 1 : 0, pos.y);
        }
        if (in_direction == south){
            return sf::Vector2f(pos.x, (pos.y + 1) < grid_height ? pos.y + 1: 0);
        }
        if (in_direction == west){
            return sf::Vector2f((pos.x > 0) ? (pos.x - 1) : grid_width - 1, pos.y);
        }
    }
    virtual void step(GridCell **N, GridCell **E, GridCell **S, GridCell **W, int current_tick){
        // Take actions here
    }
    virtual void draw(){
        // Draw Here
    }
};

#endif