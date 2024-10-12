#include <stdlib.h>
#include <time.h>
#include <cstdlib>
#include <ctime>
#include <time.h>
#include <set>
#include <fstream>
#include <utility>

#include "hlt.hpp"
#include "networking.hpp"

#define PI 3.141596

hlt::GameMap presentMap;

unsigned char normalizeAxe(int a, int maxSize) {
    if (a < 0) 
        return maxSize + a;
    else if (a >= maxSize) 
        return a - maxSize;
    else
        return a;

}

bool test_enemy(unsigned short x , unsigned short y,int direction, unsigned char id) {
    
    return presentMap.getSite(presentMap.getLocation({y,x}, direction)).owner != id;
}

bool is_enemy(unsigned short x , unsigned short y,int direction, unsigned char id) {
    return presentMap.getSite(presentMap.getLocation({y,x}, direction)).owner != id;
}

bool real_enemy(unsigned short x , unsigned short y, unsigned char id) {
    return presentMap.getSite(presentMap.getLocation({y,x}, STILL)).owner != 0 &&
            presentMap.getSite(presentMap.getLocation({y,x}, STILL)).owner != id;
}

unsigned char get_strength(unsigned short x, unsigned short y, int direction) {
    return presentMap.getSite(presentMap.getLocation({y,x}, direction)).strength;
}

bool better_enemy(unsigned char strength, unsigned short x, unsigned short y, int direction, unsigned char id) {
    return is_enemy(x , y, direction, id) && get_strength(x ,y ,direction) <= strength;
}


//sa o verific iar, in caz de erori la minVal = 256!!!
unsigned char poor_enemy(unsigned short x , unsigned short y, unsigned char id) {
    unsigned char minVal = 256;
    hlt::Site enemy;

    for (int i = 1 ; i < 5; i++) {
        enemy = presentMap.getSite({y,x},i);
        if (is_enemy(x,y,i, id) && enemy.strength < minVal) {
            minVal = enemy.strength;
        }
    }

    return minVal;
}

bool hasEnemies(unsigned short x, unsigned short y, unsigned char id) {
    for (int i = 1 ; i < 5;  i++)
        if (is_enemy(x, y, i, id))
            return true;

    return false;
}

int get_zone(int x, int step, int direction, int maxSize) {
    return normalizeAxe(x + step * direction, maxSize);
}

unsigned short get_power_neighbor(unsigned short x, unsigned short y) {
    unsigned short sum = 0;
    for (int i = 1; i < 5; i++) {
        sum += presentMap.getSite(presentMap.getLocation({y,x}, i)).production;
    }
    return sum;
}

hlt::Location find_highest_prod() {
    int highest = 0 , neighbor_mean = 0;
    hlt::Location loc, goodLoc;
    hlt::Site current;

    for (int i = 0 ; i < presentMap.height; i++) {
        for (int j = 0 ; j < presentMap.width; j++) {
            loc = presentMap.getLocation({j , i}, STILL);
            current = presentMap.getSite(loc);
            if (current.production >= highest) {
                if (current.production > highest) {
                    highest = current.production;
                    neighbor_mean = get_power_neighbor(i, j);
                    goodLoc = loc;
                }
                else if (neighbor_mean < get_power_neighbor(i, j)) {
                    neighbor_mean = get_power_neighbor(i, j);
                    goodLoc = loc;
                }
            }
        }
    }

    return goodLoc;
}

int find_target(int x, int y , unsigned char id) {
    int steps = 1, a = 0, b = 0;
    int delta[][2] = {{-1,0}, {0, 1}, {1, 0}, {0 , -1}};
    while (steps < presentMap.width && steps < presentMap.height)
    {
        for (int i = 0; i < 4 ; i++) {
            a = get_zone(x, steps, delta[i][0], presentMap.height);
            b = get_zone(y, steps, delta[i][1], presentMap.width);

            if (is_enemy(a, b, STILL, id)) {
                return i + 1;
            }

        }
        steps++;
    } 
    
    return WEST; //ALES arbitrar
}

int get_angle(float angle) {
    if ((-PI) / 4 <= angle && angle < PI/4)
        return EAST;          
    else if (PI/4 <= angle && angle < (3 * PI )/ 4)
        return SOUTH;
    else if (3 * PI / 4 <= angle && angle < PI || -PI <= angle && angle < -3 * PI / 4)
        return WEST;
    else
        return NORTH; 
}

float get_score(int x, int y, hlt::Location start) {
    hlt::Location loc_zone = presentMap.getLocation({y, x},STILL);
    hlt::Site zone = presentMap.getSite(loc_zone, STILL);
    unsigned char prod = zone.production;
    unsigned char str = zone.strength;
    float distance = presentMap.getDistance(start, loc_zone);

    // return (prod + 0.1) / (str*4 + distance * 10);
    return (sqrt (prod)) / (str / 10.0 + distance * distance);
}

int find_square(int x, int y, unsigned int id) {
    int steps = 1;
    unsigned short a = 0, b = 0;
    int delta[][2] = {{-1,-1}, {-1, 1}, {1, 1}, {1 , -1}};
    int increments[][2] = {{0, 1} , {1, 0}, {0, -1}, {-1 , 0}};
    hlt::Location loc, start, good;
    float angle = 0;
    start = presentMap.getLocation({y, x},STILL);
    float maxScore = -1, score = 0;

    while (steps < (presentMap.width + 1) / 2 && steps < (presentMap.height + 1) / 2)
    {
        for (int j = 0 ; j < 4; j++) {
            for (int i = 0 ; i < 2 * steps; i++) {
                a = normalizeAxe(x + steps * delta[j][0] + i * increments[j][0], presentMap.height);
                b = normalizeAxe(y + steps * delta[j][1] + i * increments[j][1], presentMap.width);
                if (is_enemy(a, b, STILL, id)) {
                    loc = presentMap.getLocation({b, a},STILL);
                    score = get_score(a, b, start);
                    if (score > maxScore) {
                        maxScore = score;
                        good = loc;
                    }
                }
            }
        }
        steps++;
    }

    angle = presentMap.getAngle(start, good);
    return get_angle(angle);
}



//putem optimiza in functie de minimul inamicilor
int help_ally(int x, int y, unsigned int id) {
    hlt::Site ally, current;
    hlt::Location ally_loc, current_loc;
    int str = 0;

    current_loc = presentMap.getLocation({y , x}, STILL);
    current = presentMap.getSite(current_loc);
    
    //MUUULTE DE modificat aici dar treb sa fug la poli
    for (int i = 0; i < presentMap.height; i++) {
        for (int j = 0 ; j < presentMap.width; j++) {
            ally_loc = presentMap.getLocation({j , i}, STILL);
            ally = presentMap.getSite(ally_loc);
            if (ally.owner == id && hasEnemies(i , j , id)) {
                return get_angle(presentMap.getAngle(current_loc, ally_loc));
                // str = poor_enemy(i, j , id);
                // if (ally.strength + presentMap.getDistance(current_loc, ally_loc) < str) {
                //     if (current.strength + presentMap.getDistance(current_loc, ally_loc) < poor_enemy(x, y, id))
                //         return get_angle(presentMap.getAngle(current_loc, ally_loc));
                // }            
            }
            
        }
    }

    return STILL;
}

double neigh_score(hlt::Location neighbor) {
    hlt::Site current = presentMap.getSite(neighbor);
    return ((double)current.production )/ std::max(1.0, (double)current.strength);
} 


int best_neighbor(int x, int y, unsigned char id) {
    float maxScore = -1;
    int zone = 0;

    hlt::Location current_location = presentMap.getLocation({y,x}, STILL);
    hlt::Location neighbor;

    for (int i = 1 ; i < 5; i++) {
        neighbor = presentMap.getLocation({y, x}, i);
        if (neigh_score(neighbor) >= maxScore && is_enemy(x, y, i, id)) {
            maxScore = neigh_score(neighbor);
            zone = i;
        }
    }

    return zone;
}

int search_enemy(int x, int y, unsigned char id) {
    int steps = 1;
    unsigned short a = 0, b = 0;
    int delta[][2] = {{-1,-1}, {-1, 1}, {1, 1}, {1 , -1}};
    int increments[][2] = {{0, 1} , {1, 0}, {0, -1}, {-1 , 0}};
    hlt::Location loc, start, good;
    float angle = 0;
    start = presentMap.getLocation({y, x},STILL);
    float maxScore = -1, score = 0;

    while (steps < 4)
    {
        for (int j = 0 ; j < 4; j++) {
            for (int i = 0 ; i < 2 * steps; i++) {
                a = normalizeAxe(x + steps * delta[j][0] + i * increments[j][0], presentMap.height);
                b = normalizeAxe(y + steps * delta[j][1] + i * increments[j][1], presentMap.width);
                if (real_enemy(a, b, id)) {
                    loc = presentMap.getLocation({b, a}, STILL);
                    score = presentMap.getSite(loc).production;
                    if (score > maxScore) {
                        maxScore = score;
                        good = loc;
                    }
                }
            }
        }
        steps++;
    }

    if (maxScore == -1)
        return STILL;

    angle = presentMap.getAngle(start, good);
    return get_angle(angle);
}


int main() {

    std::cout.sync_with_stdio(0);

    unsigned char myID;
    
    getInit(myID, presentMap);
    sendInit("MyC++Bot");


    std::set<hlt::Move> moves;
    hlt::Site current, enemy;
    bool moved = false;
    int round = 0, better = 0, next_move = 0;

    while(true) {
        moves.clear();
        round++;
        getFrame(presentMap);

         for(unsigned short a = 0; a < presentMap.height; a++) {
            for(unsigned short b = 0; b < presentMap.width; b++) {
                current = presentMap.getSite({ b, a });
                moved = false;
                if (current.owner == myID) {
                    if (current.strength == 0) {
                        moves.insert({ { b, a }, STILL });
                    } else {
                        //next_move = search_enemy(a,b,myID);
                        next_move = search_enemy(a,b,myID);
                        if (next_move != STILL && current.strength >= current.production * 6 
                        && current.strength >= get_strength(a,b,next_move)) {
                            moves.insert({ { b, a }, next_move});
                            continue;
                        } 

                        if (hasEnemies(a, b, myID)) {
                            
                            better = best_neighbor(a, b, myID);
                            if (better_enemy(current.strength, a, b, better,myID))
                                moves.insert({ { b, a }, better });
                                moved = true;
                            }
                        
                            // for (int i = 1 ; i < 5 ; i++) {
                            //     if (better_enemy(current.strength, a, b, i, myID)) {
                            //         moves.insert({ { b, a }, DIRECTIONS[i]});
                            //         moved = true;
                            //         break;
                            //     }
                            // }

                        if (!moved && !hasEnemies(a, b, myID) && current.strength >= current.production * 6) {
                            moves.insert({ {b,a}, find_square(a, b, myID) });
                        }
                    }
                }
            }
        }
        sendFrame(moves);
    }

    return 0;
}