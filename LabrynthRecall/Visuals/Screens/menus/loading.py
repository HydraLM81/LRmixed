import pygame as pg
from ....paths import *
pg.init()

background_color = (250,250,250)#(178,99,27)
(width, height) = (800, 450)
screen = pg.display.set_mode((width, height))
 
pg.display.set_caption('Labrynth Recall')

screen.fill(background_color)

pg.display.flip()


#image creation
image_orig = pg.image.load(IMAGES_DIR/"loadingicon.jpg").convert()
image_edit = pg.transform.smoothscale(image_orig, (175, 175)) 

screen_rect = screen.get_rect()
clock = pg.time.Clock()
image_final = image_edit.copy()
image_rect = image_edit.get_rect(center=screen_rect.center)
angle = 0

font = pg.freetype.Font("Pixellettersfull-BnJ5.ttf", 50)
text_surface, rect = font.render("Loading...", (0, 0, 0))

textWidth=text_surface.get_width()

with open(ROOT_DIR/"commFile.txt") as commFile:
    for line in commFile:
        pass
    last_line = line

running=True

while running:
  
    for event in pg.event.get():
        if event.type == pg.QUIT or last_line=="CPP COMPL":
            running=False
            screen = pg.display.set_mode((1, 1))

    with open(ROOT_DIR/"commFile.txt") as commFile:
        for line in commFile:
            pass
        last_line = line

    image_final = pg.transform.rotate(image_edit, angle)
    image_rect = image_final.get_rect(center=image_rect.center)
    screen.fill(background_color)
    screen.blit(image_final, image_rect)

    screen.blit(text_surface, ((width/2)-(textWidth/2), height-50))

    pg.display.flip()
    clock.tick(40)
    angle -= 9
    
pg.quit()
    