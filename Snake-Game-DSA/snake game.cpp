#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <windows.h>
#include <string>

using namespace std;

//================ NODE =================//

struct Node
{
    int x, y;
    Node *Link;
};

//================ SNAKE CLASS =================//

class Snake
{
public:

    Node *head;
    Node *tail;
    int length;

    Snake()
    {
        head = NULL;
        tail = NULL;
        length = 0;
    }

    void CreateSnake(int x, int y)
    {
        Node *NewNode = new Node;

        NewNode->x = x;
        NewNode->y = y;
        NewNode->Link = NULL;

        head = tail = NewNode;
        length = 1;
    }

    void Move(char direction, bool grow = false)
    {
        Node *NewHead = new Node;

        if(direction == 'u')
        {
            NewHead->x = head->x;
            NewHead->y = head->y - 1;
        }
        else if(direction == 'd')
        {
            NewHead->x = head->x;
            NewHead->y = head->y + 1;
        }
        else if(direction == 'l')
        {
            NewHead->x = head->x - 1;
            NewHead->y = head->y;
        }
        else
        {
            NewHead->x = head->x + 1;
            NewHead->y = head->y;
        }

        NewHead->Link = head;
        head = NewHead;

        if(grow)
        {
            length++;
            return;
        }

        if(head == tail)
            return;

        Node *temp = head;

        while(temp->Link != tail)
        {
            temp = temp->Link;
        }

        delete tail;

        tail = temp;
        tail->Link = NULL;
    }

    bool Collision()
    {
        Node *temp = head->Link;

        while(temp != NULL)
        {
            if(head->x == temp->x &&
               head->y == temp->y)
                return true;

            temp = temp->Link;
        }

        return false;
    }

    void Draw(char board[20][40])
    {
        int i,j;

        for(i=0;i<20;i++)
        {
            for(j=0;j<40;j++)
            {
                board[i][j]=' ';
            }
        }

        Node *temp=head;

        bool first=true;

        while(temp!=NULL)
        {
            if(first)
            {
                board[temp->y][temp->x]='@';
                first=false;
            }
            else
            {
                board[temp->y][temp->x]='O';
            }

            temp=temp->Link;
        }
    }
};

//================ GLOBAL VARIABLES =================//

int FoodX;
int FoodY;

//================ FOOD =================//

void GenerateFood(int width,int height,Snake &snake)
{
    bool ok;

    do
    {
        ok=true;

        FoodX=rand()%width;
        FoodY=rand()%height;

        Node *temp=snake.head;

        while(temp!=NULL)
        {
            if(temp->x==FoodX &&
               temp->y==FoodY)
            {
                ok=false;
                break;
            }

            temp=temp->Link;
        }

    }while(!ok);
}

//================ CLEAR SCREEN =================//

void ClearScreen()
{
    COORD topLeft={0,0};

    HANDLE console=GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFO screen;

    DWORD written;

    GetConsoleScreenBufferInfo(console,&screen);

    FillConsoleOutputCharacterA(
        console,
        ' ',
        screen.dwSize.X*screen.dwSize.Y,
        topLeft,
        &written
    );

    FillConsoleOutputAttribute(
        console,
        FOREGROUND_RED|
        FOREGROUND_GREEN|
        FOREGROUND_BLUE,
        screen.dwSize.X*screen.dwSize.Y,
        topLeft,
        &written
    );

    SetConsoleCursorPosition(console,topLeft);
}

//================ BOARD =================//

void DisplayBoard(char board[20][40],int score)
{
    int i,j;

    cout<<"Score : "<<score<<endl;
    cout<<endl;

    for(j=0;j<42;j++)
        cout<<"#";

    cout<<endl;

    for(i=0;i<20;i++)
    {
        cout<<"#";

        for(j=0;j<40;j++)
        {
            cout<<board[i][j];
        }

        cout<<"#"<<endl;
    }

    for(j=0;j<42;j++)
        cout<<"#";

    cout<<endl;
}
//================ HIGH SCORE =================//

void SaveScore(string playerName, int score)
{
    ofstream file("highscore.txt", ios::app);

    if(file.is_open())
    {
        file << playerName << " " << score << endl;
        file.close();
    }
}

void DisplayHighestScore()
{
    ifstream file("highscore.txt");

    string line;
    string highestPlayer = "";
    int highestScore = 0;

    while(getline(file, line))
    {
        int pos = line.find_last_of(' ');

        if(pos == string::npos)
            continue;

        string name = line.substr(0, pos);
        int score = atoi(line.substr(pos + 1).c_str());

        if(score > highestScore)
        {
            highestScore = score;
            highestPlayer = name;
        }
    }

    file.close();

    cout << "==========================================" << endl;

    if(highestScore == 0)
    {
        cout << "Highest Score : No Record" << endl;
    }
    else
    {
        cout << "Highest Score : "
             << highestScore
             << "   By : "
             << highestPlayer
             << endl;
    }

    cout << "==========================================" << endl;
}

//================ PAUSE =================//

void PauseGame()
{
    ClearScreen();

    cout << endl;
    cout << "*********** GAME PAUSED ***********" << endl;
    cout << endl;
    cout << "Press P to Continue..." << endl;

    while(true)
    {
        if(_kbhit())
        {
            char ch = _getch();

            if(ch == 'p' || ch == 'P')
                break;
        }

        Sleep(100);
    }
}

//================ GAME OVER =================//

bool GameOver(int score, string playerName)
{
    SaveScore(playerName, score);

    ClearScreen();

    cout << endl;
    cout << "======================================" << endl;
    cout << "             GAME OVER                " << endl;
    cout << "======================================" << endl;
    cout << endl;

    cout << "Player : " << playerName << endl;
    cout << "Score  : " << score << endl;

    cout << endl;
    DisplayHighestScore();

    cout << endl;
    cout << "Press R to Restart" << endl;
    cout << "Press ESC to Exit" << endl;

    while(true)
    {
        if(_kbhit())
        {
            char ch = _getch();

            if(ch == 'r' || ch == 'R')
                return true;

            if(ch == 27)
                return false;
        }

        Sleep(100);
    }
}

//================ RESET GAME =================//

void ResetGame(Snake &snake, int &score, char &direction)
{
    while(snake.head != NULL)
    {
        Node *temp = snake.head;
        snake.head = snake.head->Link;
        delete temp;
    }

    snake.head = NULL;
    snake.tail = NULL;
    snake.length = 0;

    snake.CreateSnake(20,10);

    GenerateFood(40,20,snake);

    score = 0;
    direction = 'r';
}

//================ SPEED =================//

int GetSpeed(int score)
{
    int speed = 150;

    speed = 150 - (score * 5);

    if(speed < 50)
        speed = 50;

    return speed;
}

//================ WALL COLLISION =================//

bool WallCollision(Snake &snake)
{
    if(snake.head->x < 0)
        return true;

    if(snake.head->x >= 40)
        return true;

    if(snake.head->y < 0)
        return true;

    if(snake.head->y >= 20)
        return true;

    return false;
}

//================ FOOD CHECK =================//

bool EatFood(Snake &snake)
{
    if(snake.head->x == FoodX &&
       snake.head->y == FoodY)
        return true;

    return false;
}
//================ MAIN FUNCTION =================//

int main()
{
    srand(time(NULL));

    Snake snake;

    char board[20][40];

    string playerName;

    cout << "==========================================" << endl;
    cout << "          SNAKE GAME (DEV C++)            " << endl;
    cout << "==========================================" << endl;

    DisplayHighestScore();

    cout << endl;
    cout << "Enter Player Name : ";
    getline(cin, playerName);

    snake.CreateSnake(20,10);

    GenerateFood(40,20,snake);

    char direction='r';

    int score=0;

    while(true)
    {
        ClearScreen();

        snake.Draw(board);

        board[FoodY][FoodX]='*';

        DisplayBoard(board,score);

        cout<<endl;
        cout<<"Controls : W A S D"<<endl;
        cout<<"Pause : P"<<endl;
        cout<<"Exit : ESC"<<endl;

        //---------------- Keyboard ----------------//

        if(_kbhit())
        {
            char key=_getch();

            if(key=='w' || key=='W')
            {
                if(direction!='d')
                    direction='u';
            }

            else if(key=='s' || key=='S')
            {
                if(direction!='u')
                    direction='d';
            }

            else if(key=='a' || key=='A')
            {
                if(direction!='r')
                    direction='l';
            }

            else if(key=='d' || key=='D')
            {
                if(direction!='l')
                    direction='r';
            }

            else if(key=='p' || key=='P')
            {
                PauseGame();
            }

            else if(key==27)
            {
                SaveScore(playerName,score);
                break;
            }
        }

        //---------------- Check Next Position ----------------//

        int nextX = snake.head->x;
        int nextY = snake.head->y;

        if(direction=='u')
            nextY--;

        else if(direction=='d')
            nextY++;

        else if(direction=='l')
            nextX--;

        else if(direction=='r')
            nextX++;

        bool grow=false;

        if(nextX==FoodX && nextY==FoodY)
            grow=true;

        //---------------- Move Snake ----------------//

        snake.Move(direction,grow);

        //---------------- Ate Food ----------------//

        if(grow)
        {
            score++;
            GenerateFood(40,20,snake);
        }

        //---------------- Wall Collision ----------------//

        if(WallCollision(snake))
        {
            if(GameOver(score,playerName))
            {
                ResetGame(snake,score,direction);
                continue;
            }
            else
            {
                break;
            }
        }

        //---------------- Self Collision ----------------//

        if(snake.Collision())
        {
            if(GameOver(score,playerName))
            {
                ResetGame(snake,score,direction);
                continue;
            }
            else
            {
                break;
            }
        }

        Sleep(GetSpeed(score));
    }

    return 0;
}
