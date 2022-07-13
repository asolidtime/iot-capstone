from array import *
from pyray import *

drawmode = 1

maze = [
    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
    [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
    [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
    [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
    [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
    [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
    [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
    [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
    [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
    [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
    [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
    [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
    [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
    [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
    [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
]

init_window(16 * 32, 16 * 32, "drawing thigy")
set_target_fps(60)

def drawPixel(xpixel, ypixel, color):
    draw_rectangle(xpixel * 32, ypixel * 32, 32, 32, color)

def drawMaze():
    for line in range(16):
        for dot in range(16):
            if (maze[line][dot] == 0):
                drawPixel(line, dot, RAYWHITE)
            elif (maze[line][dot] == 1):
                drawPixel(line, dot, Color(0, 0, 255, 255))
            elif (maze[line][dot] == 2):
                drawPixel(line, dot, Color(255, 0, 0, 255))
            elif (maze[line][dot] == 3):
                drawPixel(line, dot, Color(0, 255, 0, 255))

def drawPixelAtMouse():
    mousex = int(get_mouse_x() / 32)
    mousey = int(get_mouse_y() / 32)
    if mousex > 15 or mousex < 0 or mousey > 15 or mousey < 0:
        return
    
    if drawmode == 0:
        drawPixel(mousex, mousey, Color(0, 0, 0, 50))
    elif drawmode == 1:
        drawPixel(mousex, mousey, Color(0, 0, 255, 255))
    elif drawmode == 2:
        drawPixel(mousex, mousey, Color(255, 0, 0, 255))
    elif drawmode == 3:
        drawPixel(mousex, mousey, Color(0, 255, 0, 255))

    if is_mouse_button_down(MOUSE_BUTTON_LEFT):
        maze[mousex][mousey] = drawmode

def checkDrawmodeChange():
    global drawmode
    if is_key_pressed(KEY_ONE):
        drawmode = 0
    elif is_key_pressed(KEY_TWO):
        drawmode = 1
    elif is_key_pressed(KEY_THREE):
        drawmode = 2
    elif is_key_pressed(KEY_FOUR):
        drawmode = 3
    

def printMaze():
    print("const MazeColors maze[16][16] = {")
    for mazeLine in maze:
        current = "\t{"
        for mazePart in mazeLine:
            if (mazePart == 0):
                current += "g"
            if (mazePart == 1):
                current += "d"
            if (mazePart == 2):
                current += "w"
            if (mazePart == 3):
                current += "t"
            current += ", "
        current = current[:48] + "},"
        print(current)
    print("};")


while not window_should_close():
    begin_drawing()
    clear_background(RAYWHITE)
    drawMaze()
    drawPixelAtMouse()
    checkDrawmodeChange()
    end_drawing()

close_window()
printMaze()