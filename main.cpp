#include <bits/stdc++.h>
#include <curses.h>
#include <iostream>
#include <vector>

using namespace std;

int posx, posy;
int dirx = 2;
int diry = 0;
vector<pair<int, int>> snakePos = { { posx - 1, posy }, { posx, posy } };
int rows, cols;
int score = 1;
int foodx, foody;
int maxx, maxy, minx, miny;
random_device rd;
bool validFoodPosition = false;

void addBorders()
{
    for (int i = minx - 1; i < maxx; i++) {
        mvaddch(miny - 1, i, ACS_HLINE);
        mvaddch(maxy, i, ACS_HLINE);
    }
    for (int i = miny - 1; i < maxy; i++) {
        mvaddch(i, minx - 1, ACS_VLINE);
        mvaddch(i, maxx, ACS_VLINE);
    }
    mvaddch(miny - 1, minx - 1, ACS_ULCORNER);
    mvaddch(miny - 1, maxx, ACS_URCORNER);
    mvaddch(maxy, minx - 1, ACS_LLCORNER);
    mvaddch(maxy, maxx, ACS_LRCORNER);
    mvaddstr(maxy, (minx + maxx) / 2, ("Score: " + to_string(score)).c_str());
}
void gameOverScreen()
{
    mvaddstr(rows / 2, cols / 2 - 9, "Game Over");
    mvaddstr(rows / 2 + 1, cols / 2 - 9, ("Score: " + to_string(score)).c_str());
    mvaddstr(rows / 2 + 2, cols / 2 - 20, "'r' to restart or 'q' to quit");
}
void printSnake()
{
    for (int i = 0; i < snakePos.size(); i++) {
        if (i == snakePos.size() - 1) {
            mvaddch(snakePos[i].second, snakePos[i].first, '@');
        } else {
            mvaddch(snakePos[i].second, snakePos[i].first, '#');
        }
    }
}
void moveSnake()
{
    snakePos.assign(snakePos.begin() + 1, snakePos.end());
    snakePos.push_back({ posx, posy });
    printSnake();
}

void addFood(uniform_int_distribution<int> distx,
    uniform_int_distribution<int> disty)
{
    if ((posx >= foodx && posx < foodx + 2) && posy == foody) {
        score++;
        validFoodPosition = false;
        while (!validFoodPosition) {
            foodx = distx(rd);
            foody = disty(rd);
            for (pair<int, int> p : snakePos) {
                if (p.first == foodx && p.second == foody) {
                    validFoodPosition = false;
                    break;
                } else {
                    validFoodPosition = true;
                }
            }
        }
        snakePos.insert(snakePos.begin(), snakePos[0]);
    } else {
        mvaddch(foody, foodx, ACS_DIAMOND);
    }
}

int main(int argc, char* argv[])
{
    bool border = false;
    WINDOW* win = initscr();
    getmaxyx(win, rows, cols);
    if (argc == 1) {
        maxx = cols;
        maxy = rows;
        minx = 1;
        miny = 1;
    } else {
        border = true;
        int sizex = atoi(argv[1]);
        int sizey = atoi(argv[2]) / 2;
        maxy = (rows + sizey) / 2;
        miny = (rows - sizey) / 2;
        maxx = (cols + sizex) / 2;
        minx = (cols - sizex) / 2;
    }
    uniform_int_distribution<int> distx(minx + 1, maxx - 1);
    uniform_int_distribution<int> disty(miny + 1, maxy - 1);
    posx = cols / 2;
    posy = rows / 2;
    keypad(win, true);
    nodelay(win, true);
    curs_set(0);
    printSnake();
    int time = 65000;
    bool gameOver = false;
    foodx = distx(rd);
    foody = disty(rd);

    while (!gameOver) {
        int pressed = wgetch(win);
        if (pressed == KEY_LEFT) {
            if (dirx != 2) {
                dirx = -2, diry = 0;
            }
        } else if (pressed == KEY_RIGHT) {
            if (dirx != -2) {
                dirx = 2, diry = 0;
            }
        } else if (pressed == KEY_UP) {
            if (diry != 1) {
                dirx = 0, diry = -1;
            }
        } else if (pressed == KEY_DOWN) {
            if (diry != -1) {
                dirx = 0, diry = 1;
            }
        } else if (pressed == 'w') {
            if (time > 0) {
                time -= 5000;
            }
        } else if (pressed == 's') {
            time += 5000;
        } else if (pressed == 'q')
            break;
        posx += dirx;
        posy += diry;
        erase();

        // Teleport
        if (posx < minx) {
            posx = maxx - 1;
        } else if (posx > maxx - 1) {
            posx = minx;
        } else if (posy < miny) {
            posy = maxy - 1;
        } else if (posy > maxy - 1) {
            posy = miny;
        }
        if (border) {
            addBorders();
        }
        moveSnake();
        addFood(distx, disty);
        for (int i = 0; i < snakePos.size() - 1; i++) {
            if (posx == snakePos[i].first && posy == snakePos[i].second) {
                while (true) {
                    gameOverScreen();
                    int pressed = wgetch(win);
                    if (pressed == 'q') {
                        endwin();
                        return 0;
                    } else if (pressed == 'r') {
                        posx = cols / 2, posy = rows / 2, dirx = 2, diry = 0, score = 1;
                        snakePos = { { posx - 1, posy }, { posx, posy } };
                        break;
                    }
                }
            }
        }
        usleep(time);
    }
    endwin();
    cout << "Score: " << score << endl;
    return 0;
}
