import pygame as pg
from pygame.locals import *
import os
import random as rd
from ...paths import *
pg.init()

"""
-------------------------BACKEND-------------------------
"""

background_color = (20,20,20)#(250,250,250)#(178,99,27)
(width, height) = (800, 600)
screen = pg.display.set_mode((width, height))

with open(ROOT_DIR/"commFile.txt",'a') as commFileSource:
    commFileSource.write("\n\nPY labrynth.py EXEC SUCESS")
    commFileSource.write("\n\n!+++++++++++LABRYNTH+++++++++++!")

with open(ROOT_DIR/"commFile.txt",'r') as commFileSource:
    commFile=commFileSource.read()

locationOne=commFile.rfind("FLOOR NUMBER: ")
floorNumber=commFile[locationOne+14:commFile.find("\n",locationOne+14)]

pg.display.set_caption('Labrynth Recall (Floor '+floorNumber+')')

screen.fill(background_color)

#update display
pg.display.flip()

#fps counter
clock = pg.time.Clock()


"""
-------------------------MAP-------------------------
"""

localRowsStart=commFile.find("\n" or "\r",commFile.rfind("ROWS, COLS"))+1
localRowsEnd=commFile.find("\n" or "\r",localRowsStart+1)
mapRows=int(commFile[localRowsStart:localRowsEnd])

with open(ROOT_DIR/"commFile.txt",'a') as commFileSource:
    commFileSource.write("\n\nPY Interpret of mapRows index: "+str(mapRows)+", "+str(localRowsStart)+", "+str(localRowsEnd))

localColsStart=localRowsEnd+1
localColsEnd=commFile.find("\n" or "\r",localColsStart+1)
mapCols=int(commFile[localColsStart:localColsEnd])

with open(ROOT_DIR/"commFile.txt",'a') as commFileSource:
    commFileSource.write("\n\nPY Interpret of mapCols index: "+str(mapCols)+", "+str(localColsStart)+", "+str(localColsEnd))

mapCharacters=[]
mapCharIndex=commFile.rfind("Layout Start")+13
for rows in range(0,mapRows):
    col=[]
    for cols in range(0,mapCols):
        col.append(commFile[mapCharIndex:mapCharIndex+1])
        mapCharIndex+=1
    mapCharIndex+=1

    mapCharacters.append(col)

with open(ROOT_DIR/"commFile.txt",'a') as commFileSource:
    commFileSource.write("\n\nPY Interpret of Map:\n"+str(mapCharacters))


"""
Tiles and Player
"""

class Tile(pg.sprite.Sprite):
    def __init__(self, collision, type, name, image="NULL"):
        pg.sprite.Sprite.__init__(self)
        self.collision=collision
        self.type=type
        self.name=name
        temp=pg.image.load(ENV_SPRITE_DIR/image+".png")
        self.image=pg.transform.scale(temp,(100,100))

    def __str__(self):
        return f"{self.name} {self.type} {self.collision}"

    
tileMap={
    'w':Tile(True, 0, "wall","wall1"),
    'd':Tile(False, 1, "open_door","doorOpen1"),
    'D':Tile(True, 1, "closed_door","doorClosed1"),
    'f':Tile(False, 0, "floor","floor1"),
    'l':Tile(True, 1, "switchable_light","light1"),
    'g':Tile(False, 4, "slow_tile","slow1"),
    'T':Tile(True, 3, "wall_trap"),
    '@':Tile(False, 3, "floor_trap"),
    'D':Tile(True, 1, "closed_door"),
    't':Tile(True, 1, "treasure"),
    'S':Tile(False, 5, "safety"),
    'P':Tile(False, 6, "player_character","floor1"),
    'M':Tile(True, 7, "mob_entity"),
    'b':Tile(True,1,"button"),
    'e':Tile(True,1,"currency_exchange"),
    'L':Tile(True,0,"locked_door"),
    'u':Tile(False,0,"unlocked_door"),
    'O':Tile(True,8,"open_chest","chest1")
}

collisionMapRect=[]
for a in range(0,mapRows):
        col=[]
        for b in range(0,mapCols):
            if tileMap[mapCharacters[a][b]].collision==True:
                col.append(pg.Rect(100,100,(b*100),(a*100)))
            else: col.append(pg.Rect(1,1,0,0))
        collisionMapRect.append(col)

class Item:
    def __init__(self,rate=0.0,name="",value=0,image="NULL"):
        self.rate=rate
        self.name=name
        self.value=value
        temp=pg.image.load(OBJ_SPRITE_DIR/image+".png")
        self.image=pg.transform.scale(temp,(50,50))

    def __str__(self):
        return f"{self.name} {self.value} {self.rate}"

    def eraseData(self):
        self=Item()


torchItem=Item(.2,"Torch",5,"torchItem1")
allItems=[torchItem,Item()]

def randomItem():
    return allItems[rd.randint(0,1)]

class Chest:
    def __init__(self,contents=Item(),isOpen=False):
        self.contents=contents
        self.isOpen=isOpen

    def open(self):
        self.isOpen=True
        self.contents.eraseData()


class Weapon:
    def __init__(self,name,damage,level):
        self.name=name
        self.damage=damage
        self.level=level
    

playerStartX=375
playerStartY=275
for a in range(0,mapRows):
    for b in range(0,mapCols):
        if mapCharacters[a][b]=='P':
            playerStartX=a*100
            playerStartY=b*100

playerImage=pg.image.load(PLAYER_SPRITE_DIR/"player1.png").convert_alpha()
playerImage=pg.transform.scale(playerImage,(50,50))

backgroundX=0
backgroundY=0

stageHeight=mapRows*100
stageWidth=mapCols*100
stagePosX=0
stagePosY=0

playerVelocityX=0
playerVelocityY=0

class Player:
    def __init__(self):
        self.xPos=playerStartX
        self.yPos=playerStartY
        
        self.speed=10
        self.invCap=5
        self.inv=[torchItem]

    def doping(self):
        self.speed+=999
   
player=Player()
playerRect=Rect(28,26,(width/2)-14,(height/2)-13)

def showImage(tileChar,x,y):
    screen.blit(tileMap[tileChar].image,(x,y))

playerRotation=0

running=True

while running:
    #checking for event
    for event in pg.event.get():
        if event.type==pg.QUIT:
            running=False


    

    keys=pg.key.get_pressed()

    if keys[pg.K_UP]:
        playerRotation=0
        #if
        playerVelocityY=-1
    elif keys[pg.K_DOWN]:
        playerRotation=180
        playerVelocityY=1
    else:
        playerVelocityY=0

    if keys[pg.K_RIGHT]:
        playerRotation=270
        playerVelocityX=1
    elif keys[pg.K_LEFT]:
        playerRotation=90
        playerVelocityX=-1
    else:
        playerVelocityX=0
    
    if playerVelocityX!=0 and playerVelocityY!=0:
        playerVelocityY*=.7071
        playerVelocityX*=.7071

    for a in range(0,mapRows):
        for b in range(0,mapCols):
            r=collisionMapRect[a][b]
            if pg.Rect.colliderect(playerRect,r):
                """
                if pg.Rect.collidepoint(playerRect.top,r):
                    playerRect.top=r.bottom
                if pg.Rect.collidepoint(playerRect.left,r):
                    playerRect.left=r.right
                if pg.Rect.collidepoint(playerRect.bottom,r):
                    playerRect.bottom=r.top
                if pg.Rect.collidepoint(playerRect.right,r):
                    playerRect.right=r.left""" 
                

    collideUp=False
    collideLeft=False
    collideDown=False
    collideRight=False

    playerTileX=int(player.xPos/100)
    playerTileY=int(player.yPos/100)
    

    player.xPos+=playerVelocityX*player.speed
    stagePosX-=playerVelocityX*player.speed
    player.yPos+=playerVelocityY*player.speed
    stagePosY-=playerVelocityY*player.speed

        

    screen.fill(background_color)

    for a in range(0,mapRows):
        for b in range(0,mapCols):
            screen.blit(tileMap[mapCharacters[a][b]].image,(b*100+stagePosX,(a*100)+stagePosY))
            
    playerImageRotated=pg.transform.rotate(playerImage,playerRotation)

    screen.blit(playerImageRotated,(375,275))
    dt=clock.tick(120)

    pg.display.flip()

pg.quit()