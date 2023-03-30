#import pygame library under the abbreviation "pg" so I dont have to type "pygame"
import pygame as pg
from ....paths import *
#initialize pygame
pg.init()

#set screen and background values
background_color = (250,250,250)#(178,99,27)
(width, height) = (800, 450)
screen = pg.display.set_mode((width, height))
screen.fill(background_color)

#set "application" name
pg.display.set_caption('Labrynth Recall (Thanks)')

#different sized fonts
bigFont=pg.freetype.Font("Pixellettersfull-BnJ5.ttf", 100)
medFont=pg.freetype.Font("Pixellettersfull-BnJ5.ttf", 50)

#the text to be rendered and their colors
creditsTxt, rect = bigFont.render("Thanks for playing!", (0, 0, 0))
exitText,rect=medFont.render("Press any key to exit",(0,0,0))

#open the file with attribute 'a' (append) and write to it
with open(ROOT_DIR/"commFile.txt",'a') as commFile:
    commFile.write("\n\nPY Thanks Open Success")

#write text to screen
screen.blit(creditsTxt, ((width/2)-(creditsTxt.get_width()/2), 50))
screen.blit(exitText, ((width/2)-(exitText.get_width()/2), 400))

#update display
pg.display.flip()

#keeps program running while it should
running=True

while running:
    
    #gets all events
    for event in pg.event.get():
        # if the event type is the quit button at the top right corner
        if event.type == pg.QUIT:
            #open the file with append and write forced exit
            with open(ROOT_DIR/"commFile.txt",'a') as commFile:
                commFile.write("\nPY Thanks FORCE Exit")
            #stops the while loop
            running=False
            
        #if the event is a keypress
        if event.type==pg.KEYDOWN:
            #open commfile and append to it
            with open(ROOT_DIR/"commFile.txt",'a') as commFile:
                commFile.write("\nPY Thanks Manual Exit")
            #stops the while loop
            running=False

#close window
pg.quit()