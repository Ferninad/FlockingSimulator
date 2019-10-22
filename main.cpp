#include "common.h"
#include "cmath"
#include "vector"

bool Init();
void CleanUp();
void Run();
void Spawn();
void Draw();
void DrawBoid(int x, int y);
void Flocking();
vector<double> Alignment(int i);
vector<double> Cohesion(int i);
vector<double> Separation(int i);
double ScaleNum(double n, double minN, double maxN, double min, double max);

SDL_Window *window;
SDL_GLContext glContext;
SDL_Surface *gScreenSurface = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_Rect pos;

int screenWidth = 500;
int screenHeight = 500;
int numBoids = 40;
int gridSize = 2;
double perceptionRadius = 25;
const double PI = 3.1415926;
double maxAcc = 1;
double maxVel = 4;
bool space = false;

vector<vector<double>> boids;

bool Init()
{
    if (SDL_Init(SDL_INIT_NOPARACHUTE & SDL_INIT_EVERYTHING) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
        return false;
    }
    else
    {
        //Specify OpenGL Version (4.2)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_Log("SDL Initialised");
    }

    //Create Window Instance
    window = SDL_CreateWindow(
        "Game Engine",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        screenWidth,
        screenHeight,   
        SDL_WINDOW_OPENGL);

    //Check that the window was succesfully created
    if (window == NULL)
    {
        //Print error, if null
        printf("Could not create window: %s\n", SDL_GetError());
        return false;
    }
    else{
        gScreenSurface = SDL_GetWindowSurface(window);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        SDL_Log("Window Successful Generated");
    }
    //Map OpenGL Context to Window
    glContext = SDL_GL_CreateContext(window);

    return true;
}

int main()
{
    //Error Checking/Initialisation
    if (!Init())
    {
        printf("Failed to Initialize");
        return -1;
    }

    // Clear buffer with black background
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    //Swap Render Buffers
    SDL_GL_SwapWindow(window);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    Run();

    CleanUp();
    return 0;
}

void CleanUp()
{
    //Free up resources
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Run()
{
    bool gameLoop = true;
    srand(time(NULL));
    pos.x = 0;
    pos.y = 0;
    pos.w = screenWidth;
    pos.h = screenHeight;
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &pos);
    Spawn();
    Draw();
    SDL_RenderPresent(renderer);
    int time = clock();
    while (gameLoop)
    {   
        int ctime = clock();
        
        if(ctime - time > 160){
            pos.x = 0;
            pos.y = 0;
            pos.w = screenWidth;
            pos.h = screenHeight;
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &pos);
            Draw();
            Flocking();
            SDL_RenderPresent(renderer);
            time = clock();
        }
        
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                gameLoop = false;
            }
            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym){
                    case SDLK_ESCAPE:
                        gameLoop = false;
                        break;
                    default:
                        break;
                }
            }

            if (event.type == SDL_KEYUP)
            {
                switch (event.key.keysym.sym){
                    default:
                        break;
                }
            }
        }
    }
}

void Flocking(){
    vector<double> steer;
    vector<double> newBoids;
    for(int i = 0; i < boids.size(); i++){
        vector<double> steer1 = Alignment(i);
        vector<double> steer2 = Cohesion(i);
        vector<double> steer3 = Separation(i);
        double steerx = 0;
        double steery = 0;
        steerx += steer1[0];
        steerx += steer2[0];
        steerx += steer3[0];
        steery += steer1[1];
        steery += steer2[1];
        steery += steer3[1];
        double boidvx = boids[i][3] * cos(boids[i][2]);
        double boidvy = boids[i][3] * sin(boids[i][2]);
        boidvx += steerx;
        boidvy += steery;
        newBoids.push_back(atan2(boidvy, boidvx));
        if(newBoids[i * 2] < 0)
            newBoids[i * 2] += 2 * PI;
        newBoids.push_back(sqrt(pow(boidvx, 2) + pow(boidvy, 2)));
        if(newBoids[i * 2 + 1] > maxVel)
            newBoids[i * 2 + 1] = maxVel;
        // boids[i][2] = atan2(boidvy, boidvx);
        // if(boids[i][2] < 0)
        //     boids[i][2] += 2 * PI;
        // boids[i][3] = sqrt(pow(boidvx, 2) + pow(boidvy, 2));
        // if(boids[i][3] > maxVel)
        //     boids[i][3] = maxVel;
    }
    cout << newBoids.size() << endl;
    for(int i = 0; i < boids.size(); i++){
        boids[i][2] = newBoids[i * 2];
        boids[i][3] = newBoids[i * 2 + 1];
    }
    for(int i = 0; i < boids.size(); i++){
        boids[i][3] = maxVel;
        double velx = boids[i][3] * cos(boids[i][2]);
        double vely = boids[i][3] * sin(boids[i][2]);
        boids[i][0] += velx;
        boids[i][1] += vely;
        if(boids[i][0] < 0)
            boids[i][0] = screenWidth / gridSize;
        else if(boids[i][0] > screenWidth / gridSize)
            boids[i][0] = 0;
        if(boids[i][1] < 0)
            boids[i][1] = screenHeight / gridSize;
        else if(boids[i][1] > screenHeight / gridSize)
            boids[i][1] = 0;
    }
}

vector<double> Alignment(int i){
    double total = 0;
    double totalvx = 0;
    double totalvy = 0;
    for(int j = 0; j < boids.size(); j++){
        if(j != i){
            double d = sqrt(pow(boids[i][0] - boids[j][0], 2) + pow(boids[i][1] - boids[j][1], 2));
            if(d < perceptionRadius){
                totalvx += boids[j][3] * cos(boids[j][2]);
                totalvy += boids[j][3] * sin(boids[j][2]);
                total++;
            }
        }
    }
    double boidvx = boids[i][3] * cos(boids[i][2]);
    double boidvy = boids[i][3] * sin(boids[i][2]);
    if(total > 0){
        double avgvx = totalvx/total;
        double avgvy = totalvy/total;
        double steerx = avgvx - boidvx;
        double steery = avgvy - boidvy;
        double steerAng = atan2(steery, steerx);
        if(steerAng < 0)
            steerAng += 2 * PI;
        steerx = maxAcc * cos(steerAng);
        steery = maxAcc * sin(steerAng);
        vector<double> steer = {steerx, steery};
        return steer;
    }
    else{
        vector<double> steer = {0, 0};
        return steer;
    }
}

vector<double> Cohesion(int i){
    double total = 0;
    double totalx = 0;
    double totaly = 0;
    for(int j = 0; j < boids.size(); j++){
        if(j != i){
            double d = sqrt(pow(boids[i][0] - boids[j][0], 2) + pow(boids[i][1] - boids[j][1], 2));
            if(d < perceptionRadius){
                totalx += boids[j][0];
                totaly += boids[j][1];
                total++;
            }
        }
    }
    if(total > 0){
        double avgx = totalx/total;
        double avgy = totaly/total;
        double steerx = avgx - boids[i][0];
        double steery = avgy - boids[i][1];
        double steerAng = atan2(steery, steerx);
        if(steerAng < 0)
            steerAng += 2 * PI;
        steerx = maxAcc * cos(steerAng);
        steery = maxAcc * sin(steerAng);
        vector<double> steer = {steerx, steery};
        return steer;
    }
    else{
        vector<double> steer = {0, 0};
        return steer;
    }
}

vector<double> Separation(int i){
    double total = 0;
    double totalx = 0;
    double totaly = 0;
    double steerx = 0;
    double steery = 0;
    for(int j = 0; j < boids.size(); j++){
        if(j != i){
            double d = sqrt(pow(boids[i][0] - boids[j][0], 2) + pow(boids[i][1] - boids[j][1], 2));
            if(d < perceptionRadius){
                double diffx = boids[i][0] - boids[j][0];
                double diffy = boids[i][1] - boids[j][1];
                double ang = atan2(diffy, diffx);
                if(ang < 0)
                    ang += 2 * PI;
                double mag = sqrt(pow(diffx, 2) + pow(diffy, 2));
                mag = mag / (pow(d, 2));
                diffx = mag * cos(ang);
                diffy = mag * sin(ang);
                steerx += diffx;
                steery += diffy;
                total++;
            }
        }
    }
    if(total > 0){
        steerx = steerx / total;
        steery = steery / total;
        double steerAng = atan2(steery, steerx);
        if(steerAng < 0)
            steerAng += 2 * PI;
        steerx = maxAcc * cos(steerAng);
        steery = maxAcc * sin(steerAng);
        vector<double> steer = {steerx, steery};
        return steer;
    }
    else{
        vector<double> steer = {0, 0};
        return steer;
    }
}

void Draw(){
    for(int i = 0; i < boids.size(); i++){
        int x = boids[i][0] * gridSize;
        int y = boids[i][1] * gridSize;
        double ang = (boids[i][2] * 180)/PI;
        pos.x = x;
        pos.y = y;
        pos.w = gridSize;
        pos.h = gridSize;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &pos);
        int xm1 = x - gridSize;
        int xp1 = x + gridSize;
        int ym1 = y - gridSize;
        int yp1 = y + gridSize;
        if(x-gridSize < 0)
            xm1 = screenWidth - gridSize;
        if(x+gridSize > screenWidth - gridSize)
            xp1 = 0;
        if(y-gridSize < 0)
            ym1 = screenHeight - gridSize;
        if(y+gridSize > screenHeight - gridSize)
            yp1 = 0;
        if(ang > 22.5 && ang <= 67.5){
            DrawBoid(xp1, yp1);
            DrawBoid(xm1, y);
            DrawBoid(x, ym1);
        }
        else if(ang > 67.5 && ang <= 112.5){
            DrawBoid(x, yp1);
            DrawBoid(xm1, ym1);
            DrawBoid(xp1, ym1);
        }
        else if(ang > 112.5 && ang <= 157.5){
            DrawBoid(xm1, yp1);
            DrawBoid(x, ym1);
            DrawBoid(xp1, y);
        }
        else if(ang > 157.5 && ang <= 202.5){
            DrawBoid(xm1, y);
            DrawBoid(xp1, ym1);
            DrawBoid(xp1, yp1);
        }
        else if(ang > 202.5 && ang <= 247.5){
            DrawBoid(xm1, ym1);
            DrawBoid(xp1, y);
            DrawBoid(x, yp1);
        }
        else if(ang > 247.5 && ang <= 292.5){
            DrawBoid(x, ym1);
            DrawBoid(xp1, yp1);
            DrawBoid(xm1, yp1);
        }
        else if(ang > 292.5 && ang <= 337.5){
            DrawBoid(xp1, ym1);
            DrawBoid(x, yp1);
            DrawBoid(xm1, y);
        }
        else{
            DrawBoid(xp1, y);
            DrawBoid(xm1, yp1);
            DrawBoid(xm1, ym1);
        }
    }
}

void DrawBoid(int x, int y){
    pos.x = x;
    pos.y = y;
    pos.w = gridSize;
    pos.h = gridSize;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &pos);
}

void Spawn(){
    for(int i = 0; i < numBoids; i++){
        vector<double> temp;
        temp.push_back(rand() % (screenWidth / gridSize));
        temp.push_back(rand() % (screenHeight / gridSize));
        temp.push_back(static_cast<double>(rand()) / RAND_MAX * 2 * PI);
        temp.push_back(maxVel);
        boids.push_back(temp);
    }
}

double ScaleNum(double n, double minN, double maxN, double min, double max){
    return (((n - minN) / (maxN - minN)) * (max - min)) + min;
}