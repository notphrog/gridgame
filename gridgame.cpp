#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <limits>
#include <cstdlib>
#include <ctime>
#include <map>
#include <utility>
#include <algorithm>
#include <random>
#include <chrono>


int damageDealt(int damageDealt, int defense)
{
    float d = (float)defense;
    d*=5;
    d/=100;
    return damageDealt*(1-d);
}

//

int bossDamageDealt(int bossDamageDealt, int attack)
{
    float b = (float)attack;
    b*=5;
    b/=100;
    return bossDamageDealt*(1+b); // major bug fix, thats why the boss took so little damage!!!!!!!
}

//

bool inBounds(int y, int x, const std::vector<std::vector<char>>& grid) {
    return y >= 0 && y < grid.size() &&
           x >= 0 && x < grid[0].size();
}

void paint(std::vector<std::vector<char>>& grid, int y, int x) {
    if (inBounds(y, x, grid)) grid[y][x] = '*';
}

// attacks run whenever boss hp is 1-80 within each hundred (380-301, 280-201, 180-101, 80-1)
// the x00 / 81-99 gaps are the breathers around the checkerboard special
bool inAttackBand(int hp) {
    int m = hp % 100;
    return hp > 0 && m >= 1 && m <= 80;
}

// start of actual game

// int main()
// im gonna do something CRAZY and not use int main()..
bool playGame()
{
   
    std::vector<std::vector<char>> maze;
   
    std::vector<std::pair<int,int>> itemList; // create list of pairs for item coordinates
   
    const int itemCount = 9; //number of items to create
   
   
    char dot = '.';
    char item = 'o';
    char playerCharacter = 'X';
    char bossCharacter = 'V';
    bool allItemsCollected = false;
    int playerHealth = 100;
    int bossHealth = 400;
    char playerAttackInput;
    bool youJustAttackedDidntYou = false;
    int attackTimer = 1;
    int actionCount = 0;
    int afterItemsCollectedMoveTimer = 0;
    int characterQuadrant = 0;
    int characterSide = 0;
    int characterTier = 0;
    bool attackActive;

   
    srand(static_cast<unsigned int>(time(0)));
   
    std::string availableChars = "ab";
    char randomChar;
   
    for(int i = 0; i < 11; i++){
            int randomIndex = rand() % availableChars.length();
            char randomChar = availableChars[randomIndex];
            //std::cout << i;
            //std::cout << randomChar;
    }
   
   
    std::vector<char> c = {dot,dot,dot,dot,dot,dot,dot,dot,dot,dot,dot};
    // std::vector<char> x = {dot,dot,dot,dot,dot,dot,dot,dot,dot,dot,dot};
    // std::vector<char> y = {dot,dot,dot,dot,dot,dot,dot,dot,dot,dot,dot};
    // std::vector<char> z = {dot,dot,dot,dot,dot,dot,dot,dot,dot,dot,dot};
   
    maze.push_back(c);
    maze.push_back(c);
    maze.push_back(c);
    maze.push_back(c);
    maze.push_back(c); // fifth line
    maze.push_back(c);
    maze.push_back(c);
    maze.push_back(c);
    maze.push_back(c);
    maze.push_back(c); // tenth line
    maze.push_back(c);
   
    for (int i = 0; i < itemCount; i++) //generate item positions
    {
        int x = rand()%maze[0].size();
        int y = rand()%maze.size();
        itemList.push_back({y,x});
        //std::cout << itemList[i].first << ", " << itemList[i].second << std::endl;
    }
   
    for (int i = 0; i < itemList.size(); i++) //put them in a list
    // i kinda forgot what this code does :/
    {
        int y = itemList[i].first;
        int x = itemList[i].second;
       
        maze[y][x] = item;
    }
   
    int playerX = 0;
    int playerY = 0;
   
    maze[playerY][playerX] = playerCharacter;

    //these attack and defense variables will have the addAttack and etc added and substracted to them
    int attack = 0; //initialize attack and defense variables, starting at 0 so they at least have a base value
    int defense = 0;


    //get the boss variables. dont move these inside the while loop! or any loop!
    int bossX = 5;
    int bossY = 5;

    int checkerTurnsLeft = 0; // how many more turns the checkerboard stays up
    int nextCheckerAt = 300; // replaces checker200Done and checker100Done (fires at 300, 200, 100)

    int activeAttack = 0; // 0 = nothing running
    int attackStep = 0; // how many frames the running attack has drawn
    int attackCooldown = 0; // idle turns before the next attack can start
    int patchY = 5, patchX = 5; // safe square center
    int patchTY = 5, patchTX = 5; // where the safe square is crawling to
    int bossMoveCounter = 0; // come on


    std::vector<std::pair<int,int>> pendingTiles;   // '!' tiles that go live next turn

    int oldBossX = bossX;
    int oldBossY = bossY;

   // the start of it all..
    while(true) {
    //creating the base 10x10 map
        for(int i = 0; i < maze.size(); ++i){
            for(int j = 0; j < maze[i].size(); ++j){
                std::cout << maze[i][j] << " ";
            }
            std::cout << std::endl;
        }
       
        std::cout << "/------------------------------/" << std::endl;
       
        char inputKey;
        std::cin >> inputKey;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
       
        if(inputKey == 'q'){
            // break;
            return false;
        }

        // if (inputKey == 'c') { // cheat to skip the item phase 
        //     // TODO: REMOVE THIS BEFORE RELEASE. PLEASE.
        //     itemList.clear();
        //     allItemsCollected = true;
        //     for (int i = 0; i < maze.size(); i++)
        //         for (int j = 0; j < maze[i].size(); j++)
        //             if (maze[i][j] == item) maze[i][j] = dot;
        // }
        // DONE THANKS!
       
        int oldPlayerPositionX = playerX;
        int oldPlayerPositionY = playerY;
       
        maze[oldPlayerPositionY][oldPlayerPositionX] = dot;
       
        if (inputKey == 'w') {
            playerY--;
            youJustAttackedDidntYou = false;
            actionCount++;
            attackActive = false;
        }
        else if (inputKey == 's') {
            playerY++;
            youJustAttackedDidntYou = false;
            actionCount++;
            attackActive = false;
        }
        else if (inputKey == 'a') {
            playerX--;
            youJustAttackedDidntYou = false;
            actionCount++;
            attackActive = false;
        }
        else if (inputKey == 'd') {
            playerX++;
            youJustAttackedDidntYou = false;
            actionCount++;
            attackActive = false;
        }
        else if (inputKey == 'e' && allItemsCollected == true) {

            if (inBounds(playerY + 1, playerX, maze)) maze[playerY + 1][playerX] = '+';
            if (inBounds(playerY - 1, playerX, maze)) maze[playerY - 1][playerX] = '+';
            if (inBounds(playerY, playerX + 1, maze)) maze[playerY][playerX + 1] = '+';
            if (inBounds(playerY, playerX - 1, maze)) maze[playerY][playerX - 1] = '+';

            youJustAttackedDidntYou = true;
            attackTimer = 1;
            attackActive = true;
            std::cout << actionCount << std::endl;
        }
        else if (inputKey == 'f') { // feint and do nothing to burn a turn so you can do boss fights better
            std::cout << "you feint..." << std::endl;
            actionCount++;
            attackActive = false;
        }

        // lowk not sure if i need to add f here, but why not gng
        if (inputKey == 'w' || inputKey == 'a' || inputKey == 's' || inputKey == 'd' || inputKey == 'f') {
            for (int i = 0; i < maze.size(); i++){
                for (int j = 0; j < maze.size(); j++) {
                    if (maze[i][j] == '+') {
                        maze[i][j] = '.';
                    }
                }
            }
        }

        //maze collision check
        if (playerY < 0 || playerY >= maze.size() || playerX < 0 || playerX >= maze[0].size()) {
           
            playerX = oldPlayerPositionX;
            playerY = oldPlayerPositionY;
        }
       
        std::random_device dev;
        std::mt19937 rng(dev());
        // distribution in range [1, 6]
        std::uniform_int_distribution<int> dist2(0, 1);

        srand(static_cast<unsigned int>(time(0))); //change the seed from std::random_device dev;

        const std::vector<int> attackPositives{1, 1, 2, 2, 2, 3, 3, 3};
        const std::vector<int> attackNegatives{1, 1, 1, 1, 1, 2, 2};

        const std::vector<int> defensePositives{1, 1, 1, 1, 1, 2, 2, 3};
        const std::vector<int> defenseNegatives{1, 1, 1, 1, 1, 2};

        std::random_device rd;
        //seed the generator
        std::mt19937 generator(std::chrono::system_clock::now().time_since_epoch().count());

        //make the distribution variables
        std::uniform_int_distribution<int> distribution(0, attackPositives.size() - 1);
        std::uniform_int_distribution<int> distribution1(0, attackNegatives.size() - 1);
        std::uniform_int_distribution<int> distribution2(0, defensePositives.size() - 1);
        std::uniform_int_distribution<int> distribution3(0, defenseNegatives.size() - 1);

        //make the index for each number
        int randomAPIndex = distribution(generator);
        int randomANIndex = distribution1(generator);
        int randomDPIndex = distribution2(generator);
        int randomDNIndex = distribution3(generator);

        //the actual random stat numbers
        int addAttack = attackPositives[randomAPIndex];
        int subAttack = attackNegatives[randomANIndex];
        int addDefense = defensePositives[randomDPIndex];
        int subDefense = defenseNegatives[randomDNIndex];
        //
        bool isGood;

        //put the item check here //done!
       for (int i = 0; i < itemList.size(); i++) {
           if (playerX == itemList[i].second && playerY == itemList[i].first) {
               std::cout << "item collected!" << std::endl;

               //make the random number for either buff or debuff
               //std::cout << dist2(rng) << std::endl; //enable this line if you want the number

               int collectedItems = 0;

               if (dist2(rng) == 1) {

                std::cout << "nice! you got a buff!" << std::endl;

                isGood = true;

               } else {

                std::cout << "aw no you got a debuff" << std::endl;

                isGood = false;

               }
               //separator
               //after
               if (isGood == true) {
               
                //std::cout << "we already know its a buff" << std::endl;
                //ignore and dont change whats above this, this is for debugging

                #pragma region addPlayerBuffs

                //std::cout << "add " << addAttack << " to attack" << std::endl;
                //std::cout << "add " << addDefense << " to defense" << std::endl;

                attack = attack + addAttack;
                defense = defense + addDefense;

                #pragma endregion

                //end buff effects logic

               } else if (isGood == false) {

                //std::cout << "we already know its a debuff" << std::endl;
                //ignore and dont change whats above this, this is for debugging

                #pragma region addPlayerDebuffs

                //std::cout << "subtract " << subAttack << " from attack" << std::endl;
                //std::cout << "subtract " << subDefense << " from defense" << std::endl;

                attack = attack - subAttack;
                defense = defense - subDefense;

                #pragma endregion

                //end debuff effects logic

               }
               
               //erases the item after collecting it so you dont get item collected for a past position
               itemList.erase(itemList.begin() + i);

               //to make sure the player doesnt get too overpowered
               if (attack >= 16) {

                attack = 15;
               }

               if (attack < 0 || defense < 0) {
                    attack = 0;
                    defense = 0;
               }
               // add optional negative stat check

                std::cout << "current attack = " << attack << std::endl;
                std::cout << "current defense = " << defense << std::endl;

                //std::cout << itemList.size() << std::endl;

               i--;
           }

           if (itemList.empty()) {

            allItemsCollected = true;
           }
       }


       //boss :D

       oldBossX = bossX;
       oldBossY = bossY;

        //    //get the boss variables. dont move these inside the while loop! or any loop!
        //     int bossX = 5;
        //     int bossY = 5;

        //     int oldBossX = bossX;
        //     int oldBossY = bossY;

        // ^ that is put right above the while loop btw

       if (allItemsCollected == true) {

        maze[bossY][bossX] = bossCharacter; //makes sure the boss renders on top of its attacks when it spawns
        // boss x and y is the current boss position and the old ones are the previous position

        char initialFirstEnter;
        char initialFirstAttack = '*';

        bool bossAttackActive = afterItemsCollectedMoveTimer < 4;

        if (bossAttackActive) {

            for (int x = 0; x < maze[0].size(); x++) {
                if (x != bossX && maze[bossY][x] == dot)
                    maze[bossY][x] = '*';
            }

            for (int y = 0; y < maze.size(); y++) {
                if (y != bossY && maze[y][bossX] == dot)
                    maze[y][bossX] = '*';
            }
        }

        if (playerX == bossX && playerY == bossY) {

            playerX = oldPlayerPositionX;
            playerY = oldPlayerPositionY;
        }

        if (afterItemsCollectedMoveTimer >= 4) {
            for (int y = 0; y < maze.size(); y++) {
                for (int x = 0; x < maze.size(); x++){
                    if (maze[y][x] == '*') {
                        maze[y][x] = '.';
                    }
                }
            }
        }

        // wipe the last turns ! warning squares but pendingTiles still remembers them
        for (int y = 0; y < maze.size(); y++)
            for (int x = 0; x < maze[0].size(); x++)
                if (maze[y][x] == '!') maze[y][x] = '.';


        // does the player health logic and subtracts it from bossHealth but does NOT render it
        // does the boss health logic and subtracts it from bosshealth but does NOT render it
        bossAttackActive = afterItemsCollectedMoveTimer < 4;

        if (inputKey != 'e') {

            afterItemsCollectedMoveTimer++;
            std::cout << afterItemsCollectedMoveTimer << std::endl;
        }
        
        std::cout << "current boss health " << bossHealth << std::endl;
        // clear all * after moving twice after boss fight starts
        if (afterItemsCollectedMoveTimer >= 4) {

            for (int x = 0; x < maze[0].size(); x++) {
                //TODO: RESUME CODE HERE. HERE. HERE IS WHERE YOU LEFT OFF AND YOU ARE DETERMINING WHAT QUADRANT THE CHARACTER IS IN AND BASED ON THAT MOVING THE BARRIERS!!!
                // done: idk what i was doin i dont need to do this
               
            }
        }


        if (playerX <= 4 && playerY <= 4) {

            characterQuadrant = 1;
        } else if (playerX >= 6 && playerY <= 4) {

            characterQuadrant = 2;
        } else if (playerX <= 4 && playerY >= 6) {

            characterQuadrant = 3;
        } else if (playerX >= 6 && playerY >= 6) {

            characterQuadrant = 4;
        } else {

        }

        // #pragma region boss quadrant logic
        // //diagonal towards quad 1 (fully working)
        // if (characterQuadrant == 1) {

        //     // clear old boss position
        //     for (int y = 0; y < maze.size(); y++) {
        //         for (int x = 0; x < maze[0].size(); x++) {
        //             if (maze[y][x] == bossCharacter) {
        //                 maze[y][x] = dot;
        //             }
        //         }
        //     }

        //     // calculate move
        //     int newBossX = bossX - 1;
        //     int newBossY = bossY - 1;

        //     // only commit if valid
        //     if (inBounds(newBossY, newBossX, maze)) {
        //         bossX = newBossX;
        //         bossY = newBossY;
        //     }

        //     // draw boss at final position
        //     maze[bossY][bossX] = bossCharacter;
        // }


        // //diagonal towards quad 2 (fully working)
        // if (characterQuadrant == 2) {
        //     // clear old boss position
        //     for (int y = 0; y < maze.size(); y++) {
        //         for (int x = 0; x < maze[0].size(); x++) {
        //             if (maze[y][x] == bossCharacter) {
        //                 maze[y][x] = dot;
        //             }
        //         }
        //     }

        //     // calculate move
        //     int newBossX = bossX + 1;
        //     int newBossY = bossY - 1;

        //     // only commit if valid
        //     if (inBounds(newBossY, newBossX, maze)) {
        //         bossX = newBossX;
        //         bossY = newBossY;
        //     }

        //     // draw boss at final position
        //     maze[bossY][bossX] = bossCharacter;
        // }


        // //diagonal towards quad 3 (fully working)
        // if (characterQuadrant == 3) {
        //     // clear old boss position
        //     for (int y = 0; y < maze.size(); y++) {
        //         for (int x = 0; x < maze[0].size(); x++) {
        //             if (maze[y][x] == bossCharacter) {
        //                 maze[y][x] = dot;
        //             }
        //         }
        //     }

        //     // calculate move
        //     int newBossX = bossX - 1;
        //     int newBossY = bossY + 1;

        //     // only commit if valid
        //     if (inBounds(newBossY, newBossX, maze)) {
        //         bossX = newBossX;
        //         bossY = newBossY;
        //     }

        //     // draw boss at final position
        //     maze[bossY][bossX] = bossCharacter;
        // }


        // //diagonal towards quad 4 (fully working)
        // if (characterQuadrant == 4) {
        //     // clear old boss position
        //     for (int y = 0; y < maze.size(); y++) {
        //         for (int x = 0; x < maze[0].size(); x++) {
        //             if (maze[y][x] == bossCharacter) {
        //                 maze[y][x] = dot;
        //             }
        //         }
        //     }

        //     // calculate move
        //     int newBossX = bossX + 1;
        //     int newBossY = bossY + 1;

        //     // only commit if valid
        //     if (inBounds(newBossY, newBossX, maze)) {
        //         bossX = newBossX;
        //         bossY = newBossY;
        //     }

        //     // draw boss at final position
        //     maze[bossY][bossX] = bossCharacter;
        // }
        // #pragma endregion

        // int randBossX = dist2(rng);
        // int randBossY = dist2(rng);

        // THE ENTIRE BOSS QUADRANT LOGIC IS DEPRECATED. KEEPING IT AS BACKUP.

        // srand(time(NULL));

        // int randBossY = rand() % 10;
        // int randBossX = rand() % 10;

        // if (bossX == playerX && bossY == playerY) {
        //     maze[bossY][bossX] = dot;

        //     maze[randBossY][randBossX] = bossCharacter;
        // }

        // ALSO DEPRECATED AS IT WAS PART OF THE QUADRANT LOGIC



        // erase the old boss glyph
        for (int y = 0; y < maze.size(); y++)
            for (int x = 0; x < maze[0].size(); x++)
                if (maze[y][x] == bossCharacter) maze[y][x] = dot;

        bossMoveCounter++;
        int moveEvery = (bossHealth > 300) ? 3 : (bossHealth > 150) ? 2 : 1;  // faster the more hurt it is

        // frozen while an attack is running so boss stops moving = dodge now so its easy to predict
        if (activeAttack == 0 && bossMoveCounter >= moveEvery) {
            bossMoveCounter = 0;
            int dx = playerX - bossX;
            int dy = playerY - bossY;

            if (std::abs(dx) + std::abs(dy) > 2) {   // stalk the player, then hold when close
                int newBossX = bossX;
                int newBossY = bossY;
                if (std::abs(dx) >= std::abs(dy)) newBossX += (dx > 0) ? 1 : -1;
                else                              newBossY += (dy > 0) ? 1 : -1;

                if (inBounds(newBossY, newBossX, maze) && !(newBossY == playerY && newBossX == playerX)) {
                    bossX = newBossX;
                    bossY = newBossY;
                }
            }
        }

        maze[bossY][bossX] = bossCharacter;
        // THE ACTUAL REAL BOSS LOGIC ^^^^^



        }

        // if ((inputKey == 'e') && (bossY == playerY - 1 && bossX == playerX) || (bossY == playerY + 1 && bossX == playerX) ) {if ((bossX == playerX - 1 && bossY == playerY) || (bossX == playerX + 1 && bossY == playerY)) {
        //         bossHealth -= bossDamageDealt(20, attack);
        //         for (int i = 0; i < maze.size(); i++) {
        //             for (int j = 0; j < maze.size(); j++) {
        //                 if (maze[i][j] == 'V') {
        //                     maze[i][j] = dot;
        //                 }
        //             }
        //         }
        //         bossX = rand() % 10;
        //         bossY = rand() % 10;

        //         maze[bossY][bossX] = bossCharacter;
        //     }
        // } there was something wrong here 10/22/26

        if (inputKey == 'e' && allItemsCollected &&
            ((bossY == playerY - 1 && bossX == playerX) ||
             (bossY == playerY + 1 && bossX == playerX) ||
             (bossX == playerX - 1 && bossY == playerY) ||
             (bossX == playerX + 1 && bossY == playerY))) {
                bossHealth -= bossDamageDealt(20, attack);
                for (int i = 0; i < maze.size(); i++) {
                    for (int j = 0; j < maze.size(); j++) {
                        if (maze[i][j] == 'V') {
                            maze[i][j] = dot;
                        }
                    }
                }
                do {
                    bossX = rand() % 11;
                    bossY = rand() % 11;
                } while (bossX == playerX && bossY == playerY);

                maze[bossY][bossX] = bossCharacter;
        }

        std::vector<int> attacks = {1, 2, 3, 4, 5, 6};
        // int attackChooser1 = rand() % attacks.size();
        // int attackChooser2 = rand() % attacks.size();
        // if (attackChooser2 == attackChooser1) {
        //     attackChooser2 = attackChooser1 + 1;
        //     if (attackChooser2 > attacks.size()) {

        //     }
        // }
        // int attackChooser3 = rand() % attacks.size();
        // int attackChooser4 = rand() % attacks.size();
        // int attackChooser5 = rand() % attacks.size();
        // int attackChooser6 = rand() % attacks.size();

        //use normalAttack1
        #pragma region normalAttackA
        int i1 = rand() % attacks.size();
        int normalAttack1 = attacks[i1];
        attacks.erase(attacks.begin() + i1);
        #pragma endregion
        //
        //use normalAttack2
        #pragma region normalAttackB
        int i2 = rand() % attacks.size();
        int normalAttack2 = attacks[i2];
        attacks.erase(attacks.begin() + i2);
        #pragma endregion
        // 
        //use normalAttack3
        #pragma region normalAttackC
        int i3 = rand() % attacks.size();
        int normalAttack3 = attacks[i3];
        attacks.erase(attacks.begin() + i3);
        #pragma endregion
        //
        //use normalattack4
        #pragma region normalAttackD
        int i4 = rand() % attacks.size();
        int normalAttack4 = attacks[i4];
        attacks.erase(attacks.begin() + i4);
        #pragma endregion
        //
        //use normalAttack5
        #pragma region normalAttackE
        int i5 = rand() % attacks.size();
        int normalAttack5 = attacks[i5];
        attacks.erase(attacks.begin() + i5);
        #pragma endregion
        //
        //use normalAttack6
        #pragma region normalAttackF
        int i6 = rand() % attacks.size();
        int normalAttack6 = attacks[i6];
        attacks.erase(attacks.begin() + i6);
        #pragma endregion


        // ALL OF THE BLOW GOTTA GO!!!


        // //skipping 200 and 100
        // if ((bossHealth <= 280 && bossHealth >= 261) || (bossHealth <= 260 && bossHealth >= 241) || (bossHealth <= 240 && bossHealth >= 221) || (bossHealth <= 220 && bossHealth >= 201) || (bossHealth <= 180 && bossHealth >= 161) || (bossHealth <= 160 && bossHealth >= 141) || (bossHealth <= 140 && bossHealth >= 121) || (bossHealth <= 120 && bossHealth >= 101) || (bossHealth <= 80 && bossHealth >= 61) || (bossHealth <= 60 && bossHealth >= 41) || (bossHealth <= 40 && bossHealth >= 21) || (bossHealth <= 20 && bossHealth >= 1)) {            
        //     if (normalAttack1 == 1){

        //         for (int i = 0; i < maze.size(); i++){
        //             for (int j = 0; j < maze.size(); j++){

        //             }
        //         }

        //         // if ((bossHealth <= 200 && bossHealth >= 190 ) || (bossHealth <= 100 && bossHealth >= 90 )) {
        //         //     for (int y = 0; y < 11; y++) {
        //         //         for (int x = 0; x < 11; x++) {
        //         //             if ((y + x) % 2 == 0) maze[y][x] = '*';
        //         //                 // checkerboard the entire screen hehe >:)
        //         //         }
        //         //     }
        //         // }

        //         

        //         //most recent point you need to make different attack patterns and make them last for more moves
                
        //     } else if (normalAttack1 == 2) {

        //         // if (bossHealth == 200 || bossHealth == 100) {
        //         //     for (int y = 0; y < 11; y++) {
        //         //         for (int x = 0; x < 11; x++) {
        //         //             if ((y + x) % 2 == 0) maze[y][x] = '*';
        //         //                 // checkerboard the entire screen hehe >:)
        //         //         }
        //         //     }
        //         // }

        //         


        //     } else if (normalAttack1 == 3) {

        //         // if (bossHealth == 200 || bossHealth == 100) {
        //         //     for (int y = 0; y < 11; y++) {
        //         //         for (int x = 0; x < 11; x++) {
        //         //             if ((y + x) % 2 == 0) maze[y][x] = '*';
        //         //                 // checkerboard the entire screen hehe >:)
        //         //         }
        //         //     }
        //         // }

        //     } else if (normalAttack1 == 4) {

                
        //         for (int y = 0; y < 11; y++) {
        //             for (int x = 0; x < 11; x++) {
        //                 if ((y + x) % 2 == 0) maze[y][x] = '*';
        //                     // checkerboard the entire screen hehe >:)
        //             }
        //         }
                

        //     } else if (normalAttack1 == 5) {

        //         // if (bossHealth == 200 || bossHealth == 100) {
        //         //     for (int y = 0; y < 11; y++) {
        //         //         for (int x = 0; x < 11; x++) {
        //         //             if ((y + x) % 2 == 0) maze[y][x] = '*';
        //         //                 // checkerboard the entire screen hehe >:)
        //         //         }
        //         //     }
        //         // }

        //     } else if (normalAttack1 == 6) {

        //         // if (bossHealth == 200 || bossHealth == 100) {
        //         //     for (int y = 0; y < 11; y++) {
        //         //         for (int x = 0; x < 11; x++) {
        //         //             if ((y + x) % 2 == 0) maze[y][x] = '*';
        //         //                 // checkerboard the entire screen hehe >:)
        //         //         }
        //         //     }
        //         // }
                
        //     } else {

        //     }
        // }

        // } else if ((bossHealth <= 200 && bossHealth >= 195 ) || (bossHealth <= 100 && bossHealth >= 95 )) {

        //     // int checkerMoveCounter = 0;

        //     for (int y = 0; y < 11; y++) {
        //         for (int x = 0; x < 11; x++) {
        //             if ((y + x) % 2 == 0) maze[y][x] = '*';
        //                 // checkerboard the entire screen hehe >:)
                        
        //         }
        //     }

        //     // if (inputKey == 'w' || inputKey == 's' || inputKey == 'a' || inputKey == 'd' || inputKey == 'e') {
        //     //     checkerMoveCounter++;
        //     // }

        //     // if (checkerMoveCounter >= 5) {
        //     //     for (int i = 0; i < maze.size(); i++ ) {
        //     //         for (int j = 0; j < maze[i].size(); j++ ){
        //     //             if (maze[i][j] == '*') {
        //     //                 maze[i][j] = '.';
        //     //             }
        //     //         }
        //     //     }
        //     // }

        // }


        // DONE!!! ^^^^

                std::vector<std::vector<char>> beforeAttacks = maze; // snapshot for the telegraph pass

        if (inAttackBand(bossHealth) || activeAttack != 0) { // replaces the giant condition

            // start an attack if nothing is running
            if (activeAttack == 0) {
                if (attackCooldown > 0) {
                    attackCooldown--;
                } else if (afterItemsCollectedMoveTimer >= 4 && normalAttack1 <= 5) {
                    activeAttack = normalAttack1; // rolling a 6 means the boss rests this turn
                    attackStep = 0;
                }
            }

            if (activeAttack == 1) {
                maze[0][0] = '*';
                maze[1][1] = '*';
                maze[2][2] = '*';
                maze[3][3] = '*';
                maze[4][4] = '*';
                maze[5][5] = '*';
                maze[6][6] = '*';
                maze[7][7] = '*';
                maze[8][8] = '*';
                maze[9][9] = '*';
                maze[10][10] = '*';

                maze[0][10] = '*';
                maze[1][9] = '*';
                maze[2][8] = '*';
                maze[3][7] = '*';
                maze[4][6] = '*';
                maze[5][5] = '*';
                maze[6][4] = '*';
                maze[7][3] = '*';
                maze[8][2] = '*';
                maze[9][1] = '*';
                maze[10][0] = '*';

            } else if (activeAttack == 2) {
                // top row of circle, im building it going down instead of a circle pattern
                maze[3][0] = '*';
                maze[4][0] = '*';
                maze[5][0] = '*';
                maze[6][0] = '*';
                maze[7][0] = '*';

                // 2nd (1)
                maze[2][1] = '*';
                maze[3][1] = '*';
                maze[7][1] = '*';
                maze[8][1] = '*';

                // 3rd (2)
                maze[1][2] = '*';
                maze[2][2] = '*';
                maze[8][2] = '*';
                maze[9][2] = '*';

                // 4th (3)
                maze[0][3] = '*';
                maze[1][3] = '*';
                maze[9][3] = '*';
                maze[10][3] = '*';

                // 5th (4)
                maze[0][4] = '*';
                maze[10][4] = '*';

                // 6th (5)
                maze[0][5] = '*';
                maze[10][5] = '*';

                // 7th (6)
                maze[0][6] = '*';
                maze[10][6] = '*';

                // 8th (7)
                maze[0][7] = '*';
                maze[1][7] = '*';
                maze[9][7] = '*';
                maze[10][7] = '*';

                // 9th (8)
                maze[1][8] = '*';
                maze[2][8] = '*';
                maze[8][8] = '*';
                maze[9][8] = '*';

                // 10th (9)
                maze[2][9] = '*';
                maze[3][9] = '*';
                maze[7][9] = '*';
                maze[8][9] = '*';

                // 11th (10)
                maze[3][10] = '*';
                maze[4][10] = '*';
                maze[5][10] = '*';
                maze[6][10] = '*';
                maze[7][10] = '*';

            } else if (activeAttack == 3) {
                // CORNERS: a 2x2 block in each corner (change 2 to 3 for bigger ones)
                for (int dy = 0; dy < 2; dy++) {
                    for (int dx = 0; dx < 2; dx++) {
                        paint(maze, dy, dx);
                        paint(maze, dy, 10 - dx);
                        paint(maze, 10 - dy, dx);
                        paint(maze, 10 - dy, 10 - dx);
                    }
                }

            } else if (activeAttack == 4) {
                // everything lit except a 3x3 patch that crawls 1 tile per turn
                if (attackStep == 0) {
                    patchY = std::clamp(playerY, 1, 9); // first patch is centered on you
                    patchX = std::clamp(playerX, 1, 9);
                    patchTY = 1 + rand() % 9;
                    patchTX = 1 + rand() % 9;
                } else {
                    if (patchX == patchTX && patchY == patchTY) { // arrived so pick a new target
                        patchTY = 1 + rand() % 9;
                        patchTX = 1 + rand() % 9;
                    }
                    if (patchX != patchTX) patchX += (patchTX > patchX) ? 1 : -1;
                    else                   patchY += (patchTY > patchY) ? 1 : -1;
                }

                for (int y = 0; y < 11; y++)
                    for (int x = 0; x < 11; x++)
                        if (std::abs(y - patchY) > 1 || std::abs(x - patchX) > 1)
                            maze[y][x] = '*';

                attackStep++;
                if (attackStep >= 7) { // 7 frames long
                    activeAttack = 0;
                    attackCooldown = 3;
                }

            } else if (activeAttack == 5) {
                // spiral attack, very fancy very tuff
                int litRing = attackStep / 2;
                for (int y = 0; y < 11; y++) {
                    for (int x = 0; x < 11; x++) {
                        int ring = std::min(std::min(y, x), std::min(10 - y, 10 - x));
                        if (ring <= litRing) maze[y][x] = '*';
                    }
                }

                attackStep++;
                if (attackStep >= 8) {
                    activeAttack = 0;
                    attackCooldown = 3;
                }

            } else {

            }

            // single frame attacks are finished after a single draw
            if (activeAttack >= 1 && activeAttack <= 3) {
                activeAttack = 0;
                attackCooldown = 2;   // idle turns before the next attack can start
            }

        }   // closes the band if


        // FULLY REPLACED BY THE BLOCK BELOW!!!!!!!
        // checkerboard special attack muehehehehe
        if (bossHealth <= nextCheckerAt && nextCheckerAt >= 100) {
            checkerTurnsLeft = 5;
            nextCheckerAt -= 100;
        }

        if (checkerTurnsLeft > 0) {
            for (int y = 0; y < 11; y++) {
                for (int x = 0; x < 11; x++) {
                    if ((y + x) % 2 == 0 && !(y == bossY && x == bossX))
                        maze[y][x] = '*';
                }
            }
            checkerTurnsLeft--;
        }

        // telegraph pass
        // anything freshly drawn this turn becomes ! and last turn's ! tiles go live
        std::vector<std::pair<int,int>> newPending;
        for (int y = 0; y < 11; y++) {
            for (int x = 0; x < 11; x++) {
                if (maze[y][x] == '*' && beforeAttacks[y][x] != '*') {
                    if (y == bossY && x == bossX) { maze[y][x] = bossCharacter; continue; }
                    maze[y][x] = '!';
                    newPending.push_back({y, x});
                }
            }
        }
        for (auto& t : pendingTiles) {
            if (t.first == bossY && t.second == bossX) continue;
            maze[t.first][t.second] = '*';
        }
        for (auto& t : newPending)
            if (t.first == playerY && t.second == playerX)
                std::cout << "the tile you're standing on is marked move !!" << std::endl;
        pendingTiles = newPending;

        if (maze[playerY][playerX] == '*') {

            playerHealth -= damageDealt(20, defense);

        }




        std::cout << "current health " << playerHealth << std::endl;

        maze[playerY][playerX] = playerCharacter;

        if (bossHealth <= 0) {
            std::cout << "you win!! press any key to exit! thanks for playing!!!" << std::endl;
            std::cout << "also since you clearly enjoyed MY project enough to win you should check out MY github hehe" << std::endl;
            std::cout << "https://github.com/notphrog" << std::endl;
            char winExit;
            std::cin >> winExit;
            break;
        }
        char deathExit;
        if (playerHealth <= 0) {

            std::cout << "you died! press any key to exit!" << std::endl;
            std::cout << "shameless plug but since you died to MY boss you might as well go to MY github.." << std::endl;
            std::cout << "https://github.com/notphrog" << std::endl;

            std::cin >> deathExit;
            break;
        }
       }
        std::cout << "your code made it to the bottom :3";
        // return 0;
        return true;
    }

int main() 
{
    bool keepPlaying = true;

    while (keepPlaying) {
        keepPlaying = playGame();

        if (keepPlaying) {
            char again;
            std::cout << "play again? (y/n)" << std::endl;
            std::cin >> again;
            if (again != 'y' && again != 'Y') {
                keepPlaying = false;
            }
        }
    }

    return 0;
}
