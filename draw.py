width = 750
height = 792
linewidth = 2

from graphics import *
from colorsys import hls_to_rgb
from sys import argv

if len(argv) > 1:
    inputstr = argv[1]
else:
    inputstr = input()
if len(inputstr) >= 4 and inputstr[-4:] == ".txt":
    inputstr = inputstr[:-4]

with open(inputstr + ".txt") as f:
    lines = f.readlines()
numbers = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'}
items = []
for i in range(1, len(lines)):
    z = [0, 0, 0, 0]
    j = 1
    while lines[i][j] in numbers:
        z[0] *= 10
        z[0] += int(lines[i][j])
        j += 1
    j += 2
    while lines[i][j] in numbers:
        z[1] *= 10
        z[1] += int(lines[i][j])
        j += 1
    j += 3
    while lines[i][j] in numbers:
        z[2] *= 10
        z[2] += int(lines[i][j])
        j += 1
    j += 2
    while lines[i][j] in numbers:
        z[3] *= 10
        z[3] += int(lines[i][j])
        j += 1
    items.append(z)
if lines[0][-3] == 's':
    for item in items:
        item[2] += item[0]
        item[3] += item[1]
X = set()
for item in items:
    X.add(item[0])
    X.add(item[2])
X = list(X)
X.sort()
Y = set()
for item in items:
    Y.add(item[1])
    Y.add(item[3])
Y = list(Y)
Y.sort()
maxx = 0
maxy = 0
for item in items:
    if maxx < item[2]:
        maxx = item[2]
    if maxy < item[3]:
        maxy = item[3]


n = len(items)
fillcolors = [""] * n
for i in range(len(fillcolors)):
    a = list(hls_to_rgb(i/n, 0.85, 0.3))
    a[0] = hex(int(a[0] * 255 + 0.4))[2:]
    a[1] = hex(int(a[1] * 255 + 0.4))[2:]
    a[2] = hex(int(a[2] * 255 + 0.4))[2:]
    if len(a[0]) == 1:
        a[0] = '0' + a[0]
    if len(a[1]) == 1:
        a[1] = '0' + a[1]
    if len(a[2]) == 1:
        a[2] = '0' + a[2]
    fillcolors[i] = "#" + a[0] + a[1] + a[2]
outcolors = [""] * n
for i in range(len(outcolors)):
    a = list(hls_to_rgb(i/n, 0.5, 1.0))
    a[0] = hex(int(a[0] * 255 + 0.4))[2:]
    a[1] = hex(int(a[1] * 255 + 0.4))[2:]
    a[2] = hex(int(a[2] * 255 + 0.4))[2:]
    if len(a[0]) == 1:
        a[0] = '0' + a[0]
    if len(a[1]) == 1:
        a[1] = '0' + a[1]
    if len(a[2]) == 1:
        a[2] = '0' + a[2]
    outcolors[i] = "#" + a[0] + a[1] + a[2]

if maxx <= width - 80:
    scalex = (width - 80) // maxx
    width = scalex * maxx + 80
else:
    scalex = (width - 80) / maxx
if maxy <= height - 40:
    scaley = (height - 40) // maxy
    height = scaley * maxy + 40
else:
    scaley = (height - 40) / maxy

for item in items:
    item[0] += linewidth * 0.5 / scalex
    item[1] += linewidth * 0.5 / scaley
    item[2] -= linewidth * 0.5 / scalex
    item[3] -= linewidth * 0.5 / scaley


win = GraphWin(inputstr, width, height)
win.setCoords(0, 0, width, height)

Bottom = Line(Point(40, 20), Point(40 + maxx * scalex, 20))
Bottom.setWidth(linewidth)
Bottom.draw(win)
Left = Line(Point(40, 20), Point(40, 40 + maxy * scaley))
Left.setWidth(linewidth)
Left.draw(win)
Right = Line(Point(40 + maxx * scalex, 20), Point(40 + maxx * scalex, 40 + maxy * scaley))
Right.setWidth(linewidth)
Right.draw(win)
i = 0
for x in X:
    line = Line(Point(40 + x * scalex, 17.5), Point(40 + x * scalex, 22.5))
    line.setWidth(linewidth)
    line.draw(win)
    i += 1
    if i % 2:
        txt = Text(Point(40 + x * scalex, 9.5), x)
        txt.draw(win)
    else:
        txt = Text(Point(40 + x * scalex, 30 + maxy * scaley), x)
        txt.draw(win)
    line = Line(Point(40 + x * scalex, 17.5 + maxy * scaley), Point(40 + x * scalex, 22.5 + maxy * scaley))
    line.setWidth(linewidth)
    line.draw(win)
    line = Line(Point(40 + x * scalex, 20), Point(40 + x * scalex, 20 + maxy * scaley))
    line.setWidth(linewidth * 0.25)
    line.draw(win)
for y in Y:
    line = Line(Point(37.5, 20 + y * scaley), Point(42.5, 20 + y * scaley))
    line.setWidth(linewidth)
    line.draw(win)
    txt = Text(Point(19.5, 20 + y * scaley), y)
    txt.draw(win)
    line = Line(Point(maxx * scalex + 37.5, 20 + y * scaley), Point(maxx * scalex + 42.5, 20 + y * scaley))
    line.setWidth(linewidth)
    line.draw(win)
    txt = Text(Point(maxx * scalex + 60.5, 20 + y * scaley), y)
    txt.draw(win)
    line = Line(Point(40, 20 + y * scaley), Point(maxx * scalex + 40, 20 + y * scaley))
    line.setWidth(linewidth * 0.25)
    line.draw(win)

i = 0
for item in items:
    rect = Rectangle(Point(40 + item[0] * scalex, 20 + item[1] * scaley), Point(40 + item[2] * scalex, 20 + item[3] * scaley))
    rect.setWidth(linewidth)
    rect.setOutline(outcolors[i])
    rect.setFill(fillcolors[i])
    rect.draw(win)
    i += 1
    txt = Text(Point(40 + 0.5*(item[0]+item[2]) * scalex, 20 + 0.5*(item[1]+item[3]) * scaley), i)
    txt.draw(win)

imagename = "image.ps"
win.postscript(file=imagename, pagewidth=width*2)
from PIL import Image as NewImage
psimage = NewImage.open(imagename)
psimage.save(inputstr + ".png", subsampling=0, quality=100)
psimage.close()
from os import remove
remove(imagename)

win.getMouse()
win.close()