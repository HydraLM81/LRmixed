#import pygame library
import pygame as pg
#import moviepy for video playback
import moviepy.editor
from moviepy.editor import *
#initialize pygame
pg.init()

#set screen
(screenWidth, screenHeight) = (800, 450)
screen = pg.display.set_mode((screenWidth, screenHeight))

#i think this is here just to prevent things from moving too fast
clock=pg.time.Clock()

#sets "application" name
pg.display.set_caption('Labrynth Recall (opening cutscene)')

#import the video
movie=moviepy.editor.VideoFileClip(r"C:\Users\Noah Balderston\projects\C++\school\LabrynthRecall\Visuals\Screens\cutscenes\videos\soap.mp4")
#edit it to be better sized/timed
movie=movie.resize(height=450)
movie=movie.fx(vfx.speedx,.465)

#open commfile and append 
with open(r'C:\\Users\\Noah Balderston\\projects\\C++\\school\\LabrynthRecall\\commFile.txt','a') as commFile:
    commFile.write("\n\nPY Opening Scene Called")

#shows the video
#movie.preview()

#open commfile and append
with open(r'C:\\Users\\Noah Balderston\\projects\\C++\\school\\LabrynthRecall\\commFile.txt','a') as commFile:
    commFile.write("\nPY Opening Scene FINISHED")

#prevents things from moving too fast
clock.tick(60)

#close window
pg.quit()