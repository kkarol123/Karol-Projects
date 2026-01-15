#include "raylib.h"
#include <string>
#include <cstdlib>
#include <vector>
#include <cstring>


float progressGameTimer = 0.0f;
float gameTimeRemaining = 150.0f;
float spawnTimer = 0.0f;
float spawnInterval = 3.0f;
float minSpawnInterval = 0.5f;

float wallPadding = 40.0f;

enum GameState{
    PLAYING, 
    LEVEL_UP,
    WIN,
    GAME_OVER
};
GameState gameState = PLAYING;

//used in the children of the Weapon Class and is here to reduce code duplication
enum Direction {
    DOWN,
    UP,
    LEFT,
    RIGHT
};

//forward class declarations
class Player;
class Weapon;
class SwordWeapon;
class AxeWeapon;
class MagicWeapon;

class Weapon{
    public:
        float attackCooldown;
        float attackDuration;
        int damage;

        Texture2D player_left;
        Texture2D player_right;
        Texture2D player_attack_right;
        Texture2D player_attack_left;
        Texture2D player_weapon;

        Vector2 worldPos;
        bool isOnGround = true;

        Weapon(float attackCooldown, float attackDuration, int damage){
            this->attackCooldown = attackCooldown;
            this->attackDuration = attackDuration;
            this->damage = damage; 
        }

        void LoadTextures(const std::string& path){
            player_left = LoadTexture((path + "_walk_left.png").c_str());
            player_right = LoadTexture((path + "_walk_right.png").c_str());
            player_attack_left = LoadTexture((path + "_attack_left.png").c_str());
            player_attack_right = LoadTexture((path + "_attack_right.png").c_str());
            player_weapon = LoadTexture((path + "_weapon.png").c_str());
        }

        void SetRandomPosition(){
            float scale = 4.0f;
            int maxX = GetScreenWidth() - player_weapon.width * scale;
            int maxY = GetScreenHeight() - player_weapon.height * scale;

            worldPos.x = wallPadding + (int)(rand() % int(maxX-wallPadding));
            worldPos.y = wallPadding + (int)(rand() % int(maxY-wallPadding));
        }

        void Draw(){
            if (isOnGround == false){
                return;
            }
            else{
                float scale = 4.0f;
                Rectangle rectangleBorder = {worldPos.x, worldPos.y, player_weapon.width * scale, player_weapon.height * scale};
                DrawRectangleLinesEx(rectangleBorder, 2, BLACK);
                DrawTextureEx(player_weapon, worldPos, 0, scale, WHITE);
            }
        }

        Rectangle GetPickUpRectangle(){
            float scale = 4.0f;

            return {worldPos.x, worldPos.y, player_weapon.width * scale, player_weapon.height * scale};
        }

        void UnloadTextures(){
            UnloadTexture(player_left);
            UnloadTexture(player_right);
            UnloadTexture(player_attack_left);
            UnloadTexture(player_attack_right);
        }

         //this forces every weapon to define its own attack boxes
        virtual void GetAttackBoxes(Rectangle boxes[], int &count, Player* player) = 0;

        //this is a virtual destructor and is needed to unload the weapon when exiting the game
        virtual ~Weapon() = default;

};



class Player{
    public:
        Direction direction = RIGHT;
        Direction facing = RIGHT; 

        float x;
        float y;
        int speed_x;
        int speed_y;
        
        float size = 50.0f;

        float attackTimer = 0.0f;
        float attackDuration = 0.15f;
        float attackCooldown = 0.25f;

        int health = 6;
        Texture2D player_health;

        float damageCooldown = 0.8f;
        float damageTimer = 0.0f;
        Color color = WHITE;

        Weapon* weapon;

        int level = 1;
        int experience = 0;
        int xpToNextLevel = 100;

        int killsCount = 0;

        Player(float x, float y, int speed_x, int speed_y){
            this->x = x;
            this->y = y;
            this->speed_x = speed_x;
            this->speed_y = speed_y;  
            
            player_health = LoadTexture(("sprites/player/health.png"));
        }

        void Update(){
            if (IsKeyDown(KEY_W)){
                y -= speed_y;
                direction = UP;
            }
            if (IsKeyDown(KEY_S)){
                y += speed_y;
                direction = DOWN;
            }
            if (IsKeyDown(KEY_A)){
                x -= speed_x;
                direction = LEFT;
                facing = LEFT;
            }
            if (IsKeyDown(KEY_D)){
                x += speed_x;
                direction = RIGHT;
                facing = RIGHT;
            }

            // Cooldown timer
            if (attackTimer > 0.0f) {
                attackTimer -= GetFrameTime();
                if (attackTimer < 0.0f) {
                    attackTimer = 0.0f;
                }
            }
            if (damageTimer > 0.0f){
                damageTimer -= GetFrameTime();
                if (damageTimer < 0.0f){
                    damageTimer = 0.0f;
                    color = WHITE;
                }
            }


            LimitMovement();
        }

        void LimitMovement(){
            float leftWall = wallPadding;
            float topWall = wallPadding;
            float rightWall = GetScreenWidth() - wallPadding - size;

            if (y <= topWall){
                y = topWall;
            }
            if (y + size >= GetScreenHeight()){
                y = GetScreenHeight() - size;
            }
            if (x <= leftWall){
                x = leftWall;
            }
            if (x >= rightWall){
                x = rightWall;
            }
        }

        Rectangle GetRectangle(){
            return {x, y, size, size};
        }

        void Draw(bool attacking){
            Texture2D current;

            if (attacking == false){
                if (facing == LEFT){
                    current = weapon->player_left;
                }
                if (facing == RIGHT){
                    current = weapon->player_right;
                }

            }
            else{
                if (facing == LEFT){
                    current = weapon->player_attack_left;
                }
                if (facing == RIGHT){
                    current = weapon->player_attack_right;
                }
            }

            DrawTextureEx(current, Vector2{x, y}, 0.0f, 2.5f, color);
        }


        void TakeDamage(){
            if (damageTimer == 0.0f){
                health -= 1;
                damageTimer = damageCooldown;
                color = RED;
            }
        }

        void DrawHealth(){
            for (int i = 0; i < health; i++){
                DrawTexture(player_health, GetScreenWidth() - (20 * ((i + 1)*2)) - wallPadding, 43, WHITE);
            }
        }
        
        void UnloadTextures(){
            UnloadTexture(player_health);
        }

        void GainXP(int value){
            experience += value;

            if (experience >= xpToNextLevel){
                experience = 0;
                level++;

                xpToNextLevel += 100;
                gameState = LEVEL_UP;
            }
        }

        void DrawXP(){
            char levelText[16];
            char xpText[16];

            sprintf(levelText, "Level %d", level);
            sprintf(xpText, "Exp: %d/%d", experience, xpToNextLevel);

            DrawText(levelText, wallPadding+10, 42, 42, WHITE);
            DrawText(xpText, GetScreenWidth() / 3 - 20, 42, 42, WHITE);
        }

};



class SwordWeapon: public Weapon{
    public:
        //constructor
        SwordWeapon() : Weapon(0.25f, 0.15f, 1) {
            LoadTextures("sprites/player/player");
        }

        virtual void GetAttackBoxes(Rectangle boxes[], int &count, Player* player){
            count = 0;

            float x = player->x;
            float y = player->y;
            int size = player->size;
            auto direction = player->direction;

            float range = 80;
            float thickness = 50;

            if (direction == RIGHT) {
                boxes[count++] = {x + size, y, range, thickness};
                boxes[count++] = {x, y - range, thickness, range};
                boxes[count++] = {x, y + size, thickness, range};
            }

            if (direction == LEFT) {
                boxes[count++] = {x - range, y, range, thickness};
                boxes[count++] = {x, y - range, thickness, range};
                boxes[count++] = {x, y + size, thickness, range};
            }

            if (direction == UP) {
                boxes[count++] = {x, y - range, thickness, range};
                boxes[count++] = {x - range, y, range, thickness};
                boxes[count++] = {x + size, y, range, thickness};
            }

            if (direction == DOWN) {
                boxes[count++] = {x, y + size, thickness, range};
                boxes[count++] = {x - range, y, range, thickness};
                boxes[count++] = {x + size, y, range, thickness};
            }
        }
};

class AxeWeapon: public Weapon{
    public:
        //constructor
        AxeWeapon() : Weapon(0.45f, 0.15f, 2) {
            LoadTextures("sprites/player/player2");
        }

        virtual void GetAttackBoxes(Rectangle boxes[], int &count, Player* player){
            count = 0;

            float x = player->x;
            float y = player->y;
            int size = player->size;
            auto direction = player->direction;

            float range = 100;
            float thickness = 50;

            //for the axe weapon, there is only 2 boxes which can collide with enemies instead of 3
            if (direction == RIGHT) {
                boxes[count++] = {x + size, y, range, thickness};
                boxes[count++] = {x, y - range, thickness, range};
            }

            if (direction == LEFT) {
                boxes[count++] = {x - range, y, range, thickness};
                boxes[count++] = {x, y - range, thickness, range};
            }

            if (direction == UP) {
                boxes[count++] = {x, y - range, thickness, range};
                boxes[count++] = {x - range, y, range, thickness};
            }

            if (direction == DOWN) {
                boxes[count++] = {x, y + size, thickness, range};
                boxes[count++] = {x - range, y, range, thickness};
            }
        }
};

class MagicWeapon: public Weapon{
    public:
        //constructor
        MagicWeapon() : Weapon(0.8f, 0.15f, 3) {
            LoadTextures("sprites/player/player3");
        }

        virtual void GetAttackBoxes(Rectangle boxes[], int &count, Player* player){
            count = 0;

            float x = player->x;
            float y = player->y;
            int size = player->size;
            auto direction = player->direction;

            float range = 150;
            float thickness = 50;

            //for the magic weapon, there is only 1 box which can collide with enemies instead of 3
            if (direction == RIGHT) {
                boxes[count++] = {x + size, y, range, thickness};
            }

            if (direction == LEFT) {
                boxes[count++] = {x - range, y, range, thickness};
            }

            if (direction == UP) {
                boxes[count++] = {x, y - range, thickness, range};
            }

            if (direction == DOWN) {
                boxes[count++] = {x, y + size, thickness, range};
            }
        }
};



class Enemy{
    public:
        //constructor
        Direction facing = RIGHT;

        float x;
        float y;
        int speed_x;
        int speed_y;

        float size = 50.0f;

        float attackTimer = 0.0f;
        float attackDuration = 0.15f;
        float attackCooldown = 1.0f;

        Texture2D enemy_left;
        Texture2D enemy_right;
        Texture2D enemy_attack_right;
        Texture2D enemy_attack_left;

        int health;
        float damageCooldown = 0.3f;
        float damageTimer = 0.0f;
        bool isDead = false;
        Color color = WHITE;

        Enemy(float x, float y, int speed_x, int speed_y){
            this->x = x;
            this->y = y;
            this->speed_x = speed_x;
            this->speed_y = speed_y;
        }

        void Update(const Player& player){
            int stopDistance = 5; //small buffer to stop flipping left to right

            if (player.x > x + stopDistance){
                x += speed_x;
                facing = RIGHT;
            }
            else if (player.x < x - stopDistance){
                x -= speed_x;
                facing = LEFT;
            }

            if (player.y > y + stopDistance){
                y += speed_y;
            }
            else if (player.y < y - stopDistance){
                y -= speed_y;
            }

            if (damageTimer > 0.0f){
                damageTimer -= GetFrameTime();
                if (damageTimer < 0.0f){
                    damageTimer = 0.0f;
                    color = WHITE;
                }
            }

            if (attackTimer > 0.0f){
                attackTimer -= GetFrameTime();
                if (attackTimer < 0.0f){
                    attackTimer = 0.0f;
                }
            }
        }

        Rectangle GetRectangle(){
            return {x, y, size, size};
        }

        Rectangle GetAttackBox(){
            float padding = 15.0f;

            return {x - padding, y - padding, size + padding * 2, size + padding * 2};
        }

        void LoadTextures(const std::string& path){
            enemy_left = LoadTexture((path + "_walk_left.png").c_str());
            enemy_right = LoadTexture((path + "_walk_right.png").c_str());
            enemy_attack_left = LoadTexture((path + "_attack_left.png").c_str());
            enemy_attack_right = LoadTexture((path + "_attack_right.png").c_str());
        }

        void UnloadTextures(){
            UnloadTexture(enemy_right);
            UnloadTexture(enemy_left);
            UnloadTexture(enemy_attack_left);
            UnloadTexture(enemy_attack_right);
        }

        void Draw(){
            Texture texture = enemy_right;
            
            bool isAttacking = attackTimer > attackCooldown - attackDuration;

            if (isAttacking == true){
                if (facing == LEFT){
                    texture = enemy_attack_left;
                }
                else{
                    texture = enemy_attack_right;
                }
            }
            else{
                if (facing == LEFT){
                    texture = enemy_left;
                }
                else{
                    texture = enemy_right;
                }
            }

            DrawTextureEx(texture, Vector2{x, y}, 0, 3, color);
        }

        void SpawnOutsideScreen(float margin = 50.0f){
            //pick a random side (top/down/left/right)
            int side = rand() % 4;
            //and then randomly select a position (1-800 i.e. GetScreenHeight/Width)


            if (side == 0){  // left
                x = -margin;
                y = (float)(rand() % GetScreenHeight());
            }
            else if (side == 1){    // right
                x = GetScreenWidth() + margin;
                y = (float)(rand() % GetScreenHeight());
            }
            else if (side == 2){   // top
                x = (float)(rand() % GetScreenWidth());
                y = -margin;
            }
            else{        // bottom
                x = (float)(rand() % GetScreenWidth());
                y = GetScreenHeight() + margin;
            }
        }

        void TakeDamage(int damage){
            if (damageTimer == 0.0f){
                health -= damage;
                damageTimer = damageCooldown;
                color = RED;

                if (health <= 0){
                    isDead = true;
                }
            }
        }
};

class Skeleton: public Enemy{
    public:
        //constructor
        Skeleton() : Enemy(0.0f, 0.0f, 1, 1){
            health = 3;
            SpawnOutsideScreen();

            LoadTextures("sprites/enemy/enemy1");
        }
};

class Slime: public Enemy{
    public:
        //constructor
        Slime() : Enemy(0.0f, 0.0f, 2, 2){
            health = 2;
            SpawnOutsideScreen();

            LoadTextures("sprites/enemy/enemy2");
        }
};

class Bat: public Enemy{
    public:
        //constructor
        Bat() : Enemy(0.0f, 0.0f, 3, 3){
            health = 1;
            SpawnOutsideScreen();

            LoadTextures("sprites/enemy/enemy3");
        }
};


class ExperienceBox{
    public:
        float x;
        float y;
        float size = 20.0f;
        bool collected = false;

        ExperienceBox(float x, float y){
            this->x = x;
            this->y = y;
        }

        Rectangle GetRectangle(){
            return {x, y, size, size};
        }

        void Draw(){
            if (collected == false){
                Color color = {255, 215, 0, 160}; //gold
                DrawRectangleRec(GetRectangle(), color);
                DrawRectangleLinesEx(GetRectangle(), 2, GOLD); //border
            }
        }
};
















void DrawGameTimer(float gameTimeRemaining){
    int totalSeconds = (int)gameTimeRemaining;
    int minutes = totalSeconds/ 60;
    int seconds = totalSeconds % 60;

    char timerText[32];

    sprintf(timerText, "Time Left: %d:", minutes);   //minutes

    if (seconds < 10) {    //add the beginning 0 of any single digit second
        sprintf(timerText + strlen(timerText), "0");
    }
    sprintf(timerText + strlen(timerText), "%d", seconds);

    DrawText(timerText, wallPadding, GetScreenHeight()-50, 42, WHITE);
    DrawText("Controls: WASD", GetScreenWidth() - wallPadding - 330, GetScreenHeight()-50, 42, WHITE);    //whilst here, draw the Controls: WASD text
}

void CalculateEnemyTimerSpawn(float& gameTimeRemaining, float& spawnTimer, float& spawnInterval, float minSpawnInterval, std::vector<Enemy*>& enemies){
    //make the game frame-rate independant which makes the game work the same on all machines
    float deltaTime = GetFrameTime();

    gameTimeRemaining -= deltaTime;
    if (gameTimeRemaining < 0.0f){
        gameTimeRemaining = 0.0f;
    }

    float progress = 1.0f - (gameTimeRemaining / 150.0f);

    //increase the games dificulty to spawn more enemies
    spawnInterval = 3.0f - progress * 2.5f;
    if (spawnInterval < minSpawnInterval){
        spawnInterval = minSpawnInterval;
    }


    //start spawning
    spawnTimer += deltaTime;
    if (spawnTimer >= spawnInterval && gameTimeRemaining > 0.0f){
        spawnTimer = 0.0f;

        int type = rand() % 3;
        if (type == 0){
            enemies.push_back(new Skeleton());
        }
        else if (type == 1){
            enemies.push_back(new Slime());
        }
        else{
            enemies.push_back(new Bat());
        }
    }
}



Color WeaponAttackBoxColor(Weapon* weapon, SwordWeapon* sword, AxeWeapon* axe, MagicWeapon* magic){
    if (weapon == sword){
        return (Color){120, 130, 130, 90}; //gray
    }
    if (weapon == axe){
        return (Color){0, 255, 0, 90}; //green
    }
    return (Color){0, 0, 255, 90}; //blue
}



void DrawLevelUpScreen(){
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f)); //hide the game by adding a faint background

    DrawText("LEVEL UP!", GetScreenWidth()/2 - 120, 100, 50, YELLOW);

    DrawText("Choose an upgrade: ", GetScreenWidth()/2 - 180, 180, 30, WHITE);

    DrawText("1 - Increase Damage", GetScreenWidth()/2 - 200, 260, 30, WHITE);
    DrawText("2 - Increase Speed",  GetScreenWidth()/2 - 200, 320, 30, WHITE);
    DrawText("3 - Increase Max Health", GetScreenWidth()/2 - 200, 380, 30, WHITE);
}

void DrawWinScreen(int kills, int level, int time){
    char killsText[64];
    char levelText[64];
    char timeText[64];
    
    const char* youWinText = "YOU WIN!";
    const char* exitText = "Press ESC to exit";

    sprintf(killsText, "Enemies defeated: %d", kills);
    sprintf(levelText, "Level reached: %d", level);
    sprintf(timeText, "Time survived: %d seconds", time);

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f)); //hide the game by adding a faint background

    DrawText(youWinText, (GetScreenWidth() - MeasureText(youWinText, 60))/2, 220, 60, GREEN);

    DrawText(killsText, (GetScreenWidth() - MeasureText(killsText, 30))/2, 310, 30, WHITE);
    DrawText(levelText, (GetScreenWidth() - MeasureText(levelText, 30))/2, 350, 30, WHITE);
    DrawText(timeText, (GetScreenWidth() - MeasureText(timeText, 30))/2, 370, 30, WHITE);

    DrawText(exitText, (GetScreenWidth() - MeasureText(exitText, 30))/2, 420, 30, WHITE);
}

void DrawGameOverScreen(int kills, int level){
    char killsText[64];
    char levelText[64];

    const char* gameOverText = "GAME OVER";
    const char* exitText = "Press ESC to exit";

    sprintf(killsText, "Enemies defeated: %d", kills);
    sprintf(levelText, "Level reached: %d", level);
    
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f)); //hide the game by adding a faint background

    DrawText(gameOverText, (GetScreenWidth() - MeasureText(gameOverText, 60))/2, 220, 60, RED);

    DrawText(killsText, (GetScreenWidth() - MeasureText(killsText, 30))/2, 310, 30, WHITE);
    DrawText(levelText, (GetScreenWidth() - MeasureText(levelText, 30))/2, 350, 30, WHITE);

    DrawText(exitText, (GetScreenWidth() - MeasureText(exitText, 30))/2, 420, 30, WHITE);
}











//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 800;

    Rectangle boxes[3];
    int count = 0;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    Texture2D background = LoadTexture("sprites/background/background.png");

    Player player(screenWidth / 2 - 17, screenHeight / 2 - 17, 6, 6);
    SwordWeapon sword;
    AxeWeapon axe;
    MagicWeapon magic;
    
    player.weapon = &sword;
    axe.SetRandomPosition();
    magic.SetRandomPosition();

    std::vector<Enemy*> enemies;
    std::vector<ExperienceBox> expBoxes;

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        if (gameState == PLAYING){
            CalculateEnemyTimerSpawn(gameTimeRemaining, spawnTimer, spawnInterval, minSpawnInterval, enemies);
            
            //update
            player.Update();
            for (Enemy* enemy : enemies){
                enemy->Update(player);
            }

            for (auto& xp : expBoxes){
                if (xp.collected == false && CheckCollisionRecs(player.GetRectangle(), xp.GetRectangle())){
                    xp.collected = true;
                    player.GainXP(25);
                }
            }

            //player takes damage
            Rectangle playerRectangle = player.GetRectangle();
            for (Enemy* enemy : enemies){
                Rectangle enemyRectangle = enemy -> GetRectangle();
                
                if (CheckCollisionRecs(playerRectangle, enemyRectangle)){
                    if (enemy->attackTimer <= 0.0f){
                        enemy->attackTimer = enemy->attackCooldown;
                        player.TakeDamage();

                        if (player.health <= 0){
                            gameState = GAME_OVER;
                        }
                    }
                }
            }


            //game timer
            progressGameTimer = 1.0f - (gameTimeRemaining / 150.0f); 


            //pickup
            if (axe.isOnGround == true && CheckCollisionRecs(playerRectangle, axe.GetPickUpRectangle()) == true && progressGameTimer >= 0.33f){
                player.weapon = &axe;
                axe.isOnGround = false;
            }
            if (magic.isOnGround == true && CheckCollisionRecs(playerRectangle, magic.GetPickUpRectangle()) == true && progressGameTimer >= 0.66f){
                player.weapon = &magic;
                magic.isOnGround = false;
            }


            //attack
            if (player.attackTimer == 0.0f){
                count = 0;
                player.weapon->GetAttackBoxes(boxes, count, &player);

                for (int i = 0; i < count; i++) {
                    for (Enemy* enemy : enemies){
                        if (CheckCollisionRecs(boxes[i], enemy->GetRectangle())) {
                            player.attackTimer = player.weapon->attackCooldown + player.weapon->attackDuration;
                            enemy->TakeDamage(player.weapon->damage);
                            break;
                        }
                    }
                }

                //this is set to loop backwards so there will be no delay/skipping
                for (int i = (int)enemies.size() - 1; i >= 0; i--){
                    if (enemies[i]->isDead == true){
                        ExperienceBox xp(enemies[i]->x, enemies[i]->y);
                        expBoxes.push_back(xp);

                        player.killsCount++;

                        delete enemies[i];  //delete memory
                        enemies.erase(enemies.begin() + i); //remove the enemy from the vector
                    }
                }
            }

            if (gameTimeRemaining <= 0.0f && enemies.empty() == true){
                gameState = WIN;
            }
        }
        else if (gameState == LEVEL_UP){
            if (IsKeyPressed(KEY_ONE)){
                player.weapon->damage += 1;
                gameState = PLAYING;
            }
            else if (IsKeyPressed(KEY_TWO)){
                player.speed_x += 1;
                player.speed_y += 1;
                gameState = PLAYING;
            }
            else if (IsKeyPressed(KEY_THREE)){
                player.health += 1;
                gameState = PLAYING;
            }
        }
        
        
        

        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(GRAY);
            DrawTexture(background, 0, 0, WHITE);

            bool attacking = player.attackTimer > player.attackCooldown;
            player.Draw(attacking);

            //enemy player attack boxes
            //for (Enemy* enemy : enemies) {
                //Rectangle enemyAttackRect = enemy->GetAttackBox();
                //DrawRectangleLinesEx(enemyAttackRect, 2, ORANGE);
            //}

            for (Enemy* enemy : enemies){
                enemy->Draw();
            }

            //draw player attack boxes;
            if (attacking == true){
                Color color = WeaponAttackBoxColor(player.weapon, &sword, &axe, &magic);
                for (int i = 0; i < count; i++) {
                    DrawRectangleRec(boxes[i], color);

                    //border
                    Color border = {color.r, color.g, color.b, 200};
                    DrawRectangleLinesEx(boxes[i], 2, border);
                }
            }
            else{
                for (int i = 0; i < count; i++){
                    DrawRectangleLinesEx(boxes[i], 2, GRAY);
                }
            }

            if (progressGameTimer >= 0.33){   //if the game time has only passed 33% of the timer
                axe.Draw();
            }
            if (progressGameTimer >= 0.66){  //if the game time has only passed 66% of the timer
                magic.Draw();
            }
            
            //draw xp boxes
            for (auto& xp : expBoxes){
                xp.Draw();
            }

            //drawn here at the bottom so enemies do not overlap and hide the timer
            DrawGameTimer(gameTimeRemaining);
            player.DrawHealth();
            player.DrawXP();

            if (gameState == LEVEL_UP){
                DrawLevelUpScreen();
            }
            else if (gameState == WIN){
                DrawWinScreen(player.killsCount, player.level, (int)(150-gameTimeRemaining));
            }
            else if (gameState == GAME_OVER){
                DrawGameOverScreen(player.killsCount, player.level);
            }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    
    player.UnloadTextures();
    player.weapon->UnloadTextures();
    UnloadTexture(background);
    for (Enemy* enemy : enemies) {
        delete enemy;
    }
    enemies.clear();
    
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}