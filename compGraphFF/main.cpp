#include <GL/glut.h>
#include <vector>
#include <string>
#include <iostream>

struct Item {
    enum Type { DOT, V_LINE, H_LINE, L_DIAGONAL, R_DIAGONAL } type;
    float x, y;
    bool isDragging = false;
};

struct InputSlot {
    float x, y;
    Item::Type placedItem;
    bool hasItem = false;
};

std::vector<Item> draggableItems;
InputSlot grid[9];
std::vector<std::string> createdElements;

int windowWidth = 800, windowHeight = 600;
int draggedIndex = -1;

float slotSize = 50.0f;

// go ve clear duymeleri
float goButtonX = 350, goButtonY = 500, buttonWidth = 100, buttonHeight = 50;
float clearButtonX = 350, clearButtonY = 380; // Below GO button
//-----

//Initial setup function:
//Clears any previous outputs.
//Initializes the 3×3 grid slots with positions
//Adds draggable items (DOT, lines, diagonals) to the screen.
void init() {
    glClearColor(1, 1, 1, 1);

    createdElements.clear(); // evvel yazili nese varsa sil
    createdElements.push_back("New Element:");

    // Positioning 9 input gaps (grid)
    int idx = 0;
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            grid[idx].x = 300 + col * (slotSize + 10);
            grid[idx].y = 250 - row * (slotSize + 10);
            grid[idx].hasItem = false;
            idx++;
        }
    }

    // Draggable items
    draggableItems.push_back({Item::DOT, 100, 100});
    draggableItems.push_back({Item::V_LINE, 160, 100});
    draggableItems.push_back({Item::H_LINE, 220, 100});
    draggableItems.push_back({Item::L_DIAGONAL, 100, 200});
    draggableItems.push_back({Item::R_DIAGONAL, 160, 200});
}

//Draws a gray square for each grid slot or a button
void drawSquare(float x, float y, float size) {
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + size, y);
    glVertex2f(x + size, y + size);
    glVertex2f(x, y + size);
    glEnd();
}


//Draws one of the four line types
void drawLine(float x, float y, Item::Type type) {
    glBegin(GL_LINES);
    if (type == Item::V_LINE) {
        glVertex2f(x + slotSize/2, y);
        glVertex2f(x + slotSize/2, y + slotSize);
    } else if (type == Item::H_LINE) {
        glVertex2f(x, y + slotSize/2);
        glVertex2f(x + slotSize, y + slotSize/2);
    } else if (type == Item::L_DIAGONAL) {
        glVertex2f(x, y);
        glVertex2f(x + slotSize, y + slotSize);
    } else if (type == Item::R_DIAGONAL) {
        glVertex2f(x + slotSize, y);
        glVertex2f(x, y + slotSize);
    }
    glEnd();
}

void drawDot(float x, float y) {
    glPointSize(10);
    glBegin(GL_POINTS);
    glVertex2f(x + slotSize/2, y + slotSize/2);
    glEnd();
}

//Helper function to draw text on the screen using GLUT bitmap fonts
void renderBitmapString(float x, float y, void *font, const char *string) {
    glRasterPos2f(x, y);
    for (const char *c = string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}

//Draws the GO and Clear buttons with colored rectangles and labels
void drawButtons() {
    // Draw GO button
    glColor3f(0.5, 1, 0.5);
    drawSquare(goButtonX, goButtonY, buttonWidth);
    glColor3f(0, 0, 0);
    renderBitmapString(goButtonX + 35, goButtonY + 25, GLUT_BITMAP_HELVETICA_18, "GO");

    // Draw Clear button
    glColor3f(1, 0.5, 0.5);
    drawSquare(clearButtonX, clearButtonY, buttonWidth);
    glColor3f(0, 0, 0);
    renderBitmapString(clearButtonX + 25, clearButtonY + 25, GLUT_BITMAP_HELVETICA_18, "Clear");
}


//This is the main render function:
//Clears the screen.
//Draws the 3×3 input grid with any placed items.
//Draws draggable items.
//Draws buttons.
//Displays a list of detected shapes (e.g., "Big Square", "Torch", etc.).
void draw() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // Draw input grid
    for (int i = 0; i < 9; ++i) {
        glColor3f(0.8, 0.8, 0.8);
        drawSquare(grid[i].x, grid[i].y, slotSize);

        if (grid[i].hasItem) {
            glColor3f(0, 0, 0);
            if (grid[i].placedItem == Item::DOT) drawDot(grid[i].x, grid[i].y);
            else drawLine(grid[i].x, grid[i].y, grid[i].placedItem);
        }
    }

    // Draw draggable items
    for (auto &item : draggableItems) {
        glColor3f(0.2, 0.2, 0.9);
        if (item.type == Item::DOT) drawDot(item.x, item.y);
        else drawLine(item.x, item.y, item.type);
    }

    // GO Button
    glColor3f(0.5, 1, 0.5);
    drawButtons();

    // Created elements list
    glColor3f(0, 0, 0);
    int y = 500;
    for (auto &elem : createdElements) {
        renderBitmapString(650, y, GLUT_BITMAP_HELVETICA_18, elem.c_str());
        y -= 30;
    }

    glutSwapBuffers();
}

//Helper function that checks if a point (mouse click) is inside a square area (like a slot or button)
bool inside(float x, float y, float bx, float by, float size) {
    return x > bx && x < bx + size && y > by && y < by + size;
}


//Detects if the user clicked on a draggable item (to start dragging).
//Handles Clear button: empties the grid.
//Handles GO button:
void mouse(int button, int state, int x, int y) {
    float mx = x;
    float my = windowHeight - y;

        // Clear Button click
    if (inside(mx, my, clearButtonX, clearButtonY, buttonWidth)) {
        for (int i = 0; i < 9; ++i) {
            grid[i].hasItem = false;
        }
        glutPostRedisplay();
    }

    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            // Start drag
            for (int i = 0; i < draggableItems.size(); ++i) {
                if (inside(mx, my, draggableItems[i].x, draggableItems[i].y, slotSize)) {
                    draggableItems[i].isDragging = true;
                    draggedIndex = i;
                }
            }

            // GO Button
            if (inside(mx, my, 350, 500, 100)) {
                    createdElements.clear();  // Clear previous results
                    createdElements.push_back("New Element:");
                    // Check for Big Square
                    bool hasH013 = grid[0].hasItem && grid[1].hasItem && grid[2].hasItem &&
                                   grid[6].hasItem && grid[7].hasItem && grid[8].hasItem &&
                                   grid[3].hasItem && grid[5].hasItem;

                    bool isSquare = hasH013 &&
                        grid[0].placedItem == Item::H_LINE &&
                        grid[1].placedItem == Item::H_LINE &&
                        grid[2].placedItem == Item::H_LINE &&
                        grid[6].placedItem == Item::H_LINE &&
                        grid[7].placedItem == Item::H_LINE &&
                        grid[8].placedItem == Item::H_LINE &&
                        grid[3].placedItem == Item::V_LINE &&
                        grid[5].placedItem == Item::V_LINE;

                    if (isSquare) createdElements.push_back("Big Square");

                    // Torch
                    bool isTorch = grid[1].hasItem && grid[4].hasItem && grid[7].hasItem &&
                                   grid[1].placedItem == Item::DOT &&
                                   grid[4].placedItem == Item::V_LINE &&
                                   grid[7].placedItem == Item::V_LINE;

                    if (isTorch) createdElements.push_back("Torch");

                    // -------- New Elements --------
                    // Raqatka
                    if (grid[0].hasItem && grid[2].hasItem && grid[4].hasItem && grid[7].hasItem &&
                        grid[0].placedItem == Item::R_DIAGONAL &&
                        grid[2].placedItem == Item::L_DIAGONAL &&
                        grid[4].placedItem == Item::V_LINE &&
                        grid[7].placedItem == Item::V_LINE) {
                        createdElements.push_back("Raqatka");
                    }

                    // Plane
                    if (grid[0].hasItem && grid[2].hasItem && grid[4].hasItem && grid[6].hasItem && grid[8].hasItem &&
                        grid[0].placedItem == Item::R_DIAGONAL &&
                        grid[8].placedItem == Item::R_DIAGONAL &&
                        grid[2].placedItem == Item::DOT &&
                        grid[4].placedItem == Item::L_DIAGONAL &&
                        grid[6].placedItem == Item::L_DIAGONAL) {
                        createdElements.push_back("Plane");
                    }

                    // Spider
                    if (grid[0].hasItem && grid[2].hasItem && grid[4].hasItem && grid[6].hasItem && grid[8].hasItem &&
                        grid[4].placedItem == Item::DOT &&
                        grid[0].placedItem == Item::R_DIAGONAL &&
                        grid[8].placedItem == Item::R_DIAGONAL &&
                        grid[2].placedItem == Item::L_DIAGONAL &&
                        grid[6].placedItem == Item::L_DIAGONAL) {
                        createdElements.push_back("Spider");
                    }

                    // HaciDur
                    if (grid[1].hasItem && grid[3].hasItem && grid[4].hasItem && grid[5].hasItem && grid[7].hasItem &&
                        grid[1].placedItem == Item::DOT &&
                        grid[4].placedItem == Item::V_LINE &&
                        grid[7].placedItem == Item::V_LINE &&
                        grid[3].placedItem == Item::H_LINE &&
                        grid[5].placedItem == Item::H_LINE) {
                        createdElements.push_back("HaciDur");
                    }

                    // HaciYat
                    if (grid[3].hasItem && grid[6].hasItem && grid[7].hasItem && grid[8].hasItem &&
                        grid[8].placedItem == Item::DOT &&
                        grid[6].placedItem == Item::H_LINE &&
                        grid[7].placedItem == Item::H_LINE &&
                        grid[3].placedItem == Item::R_DIAGONAL) {
                        createdElements.push_back("HaciYat");
                    }

                    // Ucbucaq
                    if (grid[0].hasItem && grid[1].hasItem && grid[2].hasItem && grid[3].hasItem && grid[4].hasItem && grid[6].hasItem &&
                        grid[0].placedItem == Item::V_LINE &&
                        grid[3].placedItem == Item::V_LINE &&
                        grid[1].placedItem == Item::H_LINE &&
                        grid[2].placedItem == Item::L_DIAGONAL &&
                        grid[4].placedItem == Item::L_DIAGONAL &&
                        grid[6].placedItem == Item::L_DIAGONAL) {
                        createdElements.push_back("Ucbucaq");
                    }

                    // Romb
                    if (grid[1].hasItem && grid[3].hasItem && grid[5].hasItem && grid[7].hasItem &&
                        grid[1].placedItem == Item::L_DIAGONAL &&
                        grid[3].placedItem == Item::L_DIAGONAL &&
                        grid[7].placedItem == Item::L_DIAGONAL &&
                        grid[5].placedItem == Item::R_DIAGONAL) {
                        createdElements.push_back("Romb");
                    }

                    // Pistol
                    if (grid[0].hasItem && grid[1].hasItem && grid[2].hasItem && grid[3].hasItem && grid[4].hasItem && grid[6].hasItem &&
                        grid[0].placedItem == Item::V_LINE &&
                        grid[3].placedItem == Item::V_LINE &&
                        grid[6].placedItem == Item::V_LINE &&
                        grid[1].placedItem == Item::H_LINE &&
                        grid[2].placedItem == Item::H_LINE &&
                        grid[4].placedItem == Item::DOT) {
                        createdElements.push_back("Pistol");
                    }
            }
        }

        if (state == GLUT_UP && draggedIndex != -1) {
            // Drop onto grid
            for (int i = 0; i < 9; ++i) {
                if (inside(mx, my, grid[i].x, grid[i].y, slotSize)) {
                    grid[i].hasItem = true;
                    grid[i].placedItem = draggableItems[draggedIndex].type;
                }
            }

            draggableItems[draggedIndex].isDragging = false;
            draggedIndex = -1;
        }
    }

    glutPostRedisplay();
}

//It handles the event when the mouse is moved while a mouse button is pressed (dragging)
void motion(int x, int y) {
    if (draggedIndex != -1) {
        draggableItems[draggedIndex].x = x;
        draggableItems[draggedIndex].y = windowHeight - y;
        glutPostRedisplay();
    }
}

//This function is the reshape callback, called whenever the window is resized
void reshape(int w, int h) {
    windowWidth = w;
    windowHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("OpenGL Puzzle Game");

    init();
    glutDisplayFunc(draw);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutReshapeFunc(reshape);
    glutMainLoop();
    return 0;
}
