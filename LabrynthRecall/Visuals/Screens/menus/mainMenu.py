#import pygame library
import pygame as pg
from ....paths import *
#init pygame
pg.init()

#setting background color and the actual display (so that it exists)
background_color = (150,150,150)
(width, height) = (800, 450)
screen = pg.display.set_mode((width, height))
pg.display.set_caption('Labrynth Recall (Main Menu)')

screen.fill(background_color)
#the .flip() updates the screen so that it actually shows what you put there
pg.display.flip()


#creating different sized fonts for things
fontSmall = pg.freetype.Font("Pixellettersfull-BnJ5.ttf", 50)
fontBig = pg.freetype.Font("Pixellettersfull-BnJ5.ttf", 100)
fontVerySmall = pg.freetype.Font("Pixellettersfull-BnJ5.ttf", 10)

#creating text for the logo and buttons
titleText,rext=fontBig.render("LABRYNTH RECALL",)

playText,rect=fontSmall.render("Play", (0, 0, 0))
creditsText,rect=fontSmall.render("Credits",(0,0,0))
quitText,rect=fontSmall.render("Quit",(0,0,0))

#subtext
subText,rect=fontVerySmall.render("Use arrow keys to select, enter/return to continue.",(0,0,0))
subTextTwo,rect=fontVerySmall.render("Made by Noah Balderston and Mateo Rose",(0,0,0))

#keeps the while loop going (while the code "shouldnt" kill its self)
running=True

#which box is highlighted
box=1

#the different button "boxes". I didn't want to compute these at run-time
playButton=pg.Rect(width/2-100,150,200,50)
creditButton=pg.Rect(width/2-100,250,200,50)
quitButton=pg.Rect(width/2-100,350,200,50)

while running:
  #detecting "events". 
  for event in pg.event.get():
    #if the "X" button at top-right of window is clicked, then kill the program
    if event.type == pg.QUIT:
      running=False

    #if the event was a key press
    if event.type==pg.KEYDOWN:

      #if the key press was the enter or return key
      if event.key==pg.K_RETURN or event.key==pg.K_KP_ENTER:

        #opening the commFile to write what the enter button did
        with open(ROOT_DIR/"commfile.txt",'a') as commFile:
          if box==1:
            commFile.write("\n\nPY Main Menu PLAY")
          elif box==2:
            commFile.write("\n\nPY Main Menu CREDITS")
          elif box==3:
            commFile.write("\n\nPY Main Menu QUIT")
          else:
            #just for error prevention
            commFile.write("\n\nPY Main Menu 'box' out of bounds on selection")
          
          #kills the program once writing to commFile is done
          running=False

      #if the keypress was the up arrow, then move the "highlight" box up one
      elif event.key==pg.K_UP:
        if box!=1:
          box-=1
        else:
          #change if there are more than 3 options
          box=3
      
      #if the keypress was the down arrow, move "highlight" box down one
      elif event.key==pg.K_DOWN:
        if box!=3:
          box+=1
        else:
          box=1
 
  #drawing the grey rectangles behind the button text (it's the "buttons")
  pg.draw.rect(screen,(50,50,50),playButton)
  pg.draw.rect(screen,(50,50,50),creditButton)
  pg.draw.rect(screen,(50,50,50),quitButton)

  #Labrynth Recall (the big text at the top of the screen)
  screen.blit(titleText,((width/2)-(titleText.get_width()/2), 50))

  #the text on top of the buttons
  screen.blit(playText, ((width/2)-(playText.get_width()/2), 175-(playText.get_height()/2)))
  screen.blit(creditsText, ((width/2)-(creditsText.get_width()/2), 275-(creditsText.get_height()/2)))
  screen.blit(quitText, ((width/2)-(quitText.get_width()/2), 375-(quitText.get_height()/2)))

  #subtext
  screen.blit(subText,(5,430))
  screen.blit(subTextTwo,(5,440))

  #drawing the "highlight" box
  if box==1:
    pg.draw.rect(screen,(25,25,25),(width/2-100,150,200,50),5)
  if box==2:
    pg.draw.rect(screen,(25,25,25),(width/2-100,250,200,50),5)
  if box==3:
    pg.draw.rect(screen,(25,25,25)  ,(width/2-100,350,200,50),5)

  #update screen
  pg.display.flip()
  
#close program
pg.quit()