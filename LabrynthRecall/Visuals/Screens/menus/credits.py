#import pygame library
import pygame as pg
from ....paths import *
#initialize pygame
pg.init()

#set background and screen
background_color = (250,250,250)#(178,99,27)
(width, height) = (800, 450)
screen = pg.display.set_mode((width, height))

#sets "application" name
pg.display.set_caption('Labrynth Recall (Credits)')

#different sized fonts
bigFont=pg.freetype.Font("Pixellettersfull-BnJ5.ttf", 100)
medFont=pg.freetype.Font("Pixellettersfull-BnJ5.ttf", 50)
smallFont=pg.freetype.Font("Pixellettersfull-BnJ5.ttf", 30)
VerySmallFont=pg.freetype.Font("Pixellettersfull-BnJ5.ttf", 15)

#render the text
creditsTxt, rect = bigFont.render("Credits", (0, 0, 0))

lineOne,rect=smallFont.render("Made by Noah Balderston and Mateo Rose for a school project.",(0,0,0))
lineTwo,rect=VerySmallFont.render("Design: Both of us",(0,0,0))
lineThree,rect=VerySmallFont.render("Textures and PyGame: Noah",(0,0,0))
lineThreeOne,rect=VerySmallFont.render("PyGame embedding: Noah",(0,0,0))
lineFour,rect=VerySmallFont.render("Backend: Both of us",(0,0,0))
lineFive,rect=VerySmallFont.render("Map generation: Mateo",(0,0,0))
lineSix,rect=VerySmallFont.render("Sound design: Mainly Mateo",(0,0,0))
lineSeven,rect=VerySmallFont.render("More nights spent: Noah 100%",(0,0,0))

exitText,rect=medFont.render("Press any key to exit",(0,0,0))

#set background color
screen.fill(background_color)

#print the text to screen
screen.blit(creditsTxt, ((width/2)-(creditsTxt.get_width()/2), 50))

screen.blit(lineOne, ((width/2)-(lineOne.get_width()/2), 155))
screen.blit(lineTwo, ((width/2)-(lineTwo.get_width()/2), 220))
screen.blit(lineThree, ((width/2)-(lineThree.get_width()/2), 240))
screen.blit(lineThreeOne,((width/2)-(lineThreeOne.get_width()/2), 260))
screen.blit(lineFour, ((width/2)-(lineFour.get_width()/2), 280))
screen.blit(lineFive, ((width/2)-(lineFive.get_width()/2), 300))
screen.blit(lineSix, ((width/2)-(lineSix.get_width()/2), 320))
screen.blit(lineSeven, ((width/2)-(lineSeven.get_width()/2), 360))

screen.blit(exitText, ((width/2)-(exitText.get_width()/2), 400))

#update display
pg.display.flip()

#open commfile and append that credits was opened
with open(ROOT_DIR/"commFile.txt",'a') as commFile:
    commFile.write("\n\nPY Cred Open Success")

#keep the loop running 
running=True

while running:
  
    #get all events
    for event in pg.event.get():
        #if the player closed the window
        if event.type == pg.QUIT:
            #open commfile and write forced exit
            with open(ROOT_DIR/"commFile.txt",'a') as commFile:
                commFile.write("\nPY Cred FORCE Exit")
            #stop loop
            running=False
            
        #if the event was a keypress
        if event.type==pg.KEYDOWN:
            #open commfile and write manual exit
            with open(ROOT_DIR/"commFile.txt",'a') as commFile:
                commFile.write("\nPY Cred Manual Exit")
            #stop loop
            running=False

#close window
pg.quit()