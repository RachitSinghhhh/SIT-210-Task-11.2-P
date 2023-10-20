import sys
import os
import tkinter as tk
import pygame
from tkinter import font
from tkinter import *
import serial
from gtts import gTTS

# We imports the GPIO module
import RPi.GPIO as GPIO
# We import the command sleep from time
from time import sleep


def runServo(servopin):
    GPIO.setwarnings(False)
    
    GPIO.setmode(GPIO.BCM)
# Set an output for the PWM Signal
    GPIO.setup(servopin, GPIO.OUT)

# Set up the PWM on pin #servopin at 50Hz
    pwm = GPIO.PWM(servopin, 50)
    pwm.start(0) # Start the servo with 0 duty cycle ( at 0 deg position )
    pwm.ChangeDutyCycle(7) # Tells the servo to turn to the left ( -90 deg position )
    sleep(0.0) # Tells the servo to Delay for 5sec
    pwm.ChangeDutyCycle(7.5) # Tells the servo to turn to the neutral position ( at 0 deg position )
    sleep(0.5) # Tells the servo to Delay for 5sec
    pwm.ChangeDutyCycle(10) # Tells the servo to turn to the right ( +90 deg position )
    sleep(0.5) # Tells the servo to Delay for 5sec
    pwm.stop(0) # Stop the servo with 0 duty cycle ( at 0 deg position )
    GPIO.cleanup() # Clean up all the ports we've used.


def runS():
    if(color==1):
        servopin=19
        runServo(servopin)
        redIn = redIn+1
        updateBoxColor('red')
        text_to_speech("Red box detected")
        print("Red Color")
    elif(color==2):
        servopin=17
        runServo(servopin)
        greenIn = greenIn+1
        text_to_speech("Green box detected")
        updateBoxColor('green')
        print("Green Color")
    elif(color==3):
        servopin=18
        runServo(servopin)
        blueIn = blueIn+1
        text_to_speech("Blue box detected")
        updateBoxColor('blue')
        print("Blue Color")
    else:
        otherIn = otherIn+1
        print("Other Color")

def stop():
    global Start
    Start = False
    exit()


def start():
    global Start
    Start = True


def readData():
    global data,servopin,greenIn,blueIn,redIn,otherIn,boxOut,boxIn,color

    if start:
        try:
            ser = serial.Serial(port='/dev/ttyACM0',baudrate=9600,parity=serial.PARITY_ODD,stopbits=serial.STOPBITS_TWO,bytesize=serial.SEVENBITS)
            data = ser.readline().decode('utf-8').strip()
            print(data)
            rawdata = data.split(",")
            color = int(rawdata[0])
            print(color)
            boxIn = str(rawdata[1])
            boxOut = str(rawdata[2])
            if(color==1):
                servopin=19
                runServo(servopin)
                redIn = redIn+1
                updateBoxColor('red')
                text_to_speech("Red box detected")
                print("Red Color")
            elif(color==2):
                servopin=17
                runServo(servopin)
                greenIn = greenIn+1
                updateBoxColor('green')
                text_to_speech("Green box detected")
                updateBoxColor('green')
                print("Green Color")
            elif(color==3):
                servopin=18
                runServo(servopin)
                blueIn = blueIn+1
                updateBoxColor('blue')
                text_to_speech("Blue box detected")
                updateBoxColor('blue')
                print("Blue Color")
            else:
                otherIn = otherIn+1
                print("Other Color")
                text_to_speech("Other color box detected")
                updateBoxColor('black')
            updateUi()
            
        except Exception as e:
            print(e)
            print("Serial not available")
    root.after(1000, readData)


def updateBoxColor(color):
    canvas.itemconfig(circle, fill=color)


def updateUi():
    global boxIn, redIn, blueIn, greenIn, otherIn, boxOut
    boxInText.config(text=" Box in - " + boxIn)
    boxRedText.config(text=f"Red Box - {redIn}")
    boxBlueText.config(text=f"Blue Box - {blueIn}")
    boxGreenText.config(text=f"Green Box - {greenIn}")
    boxOtherText.config(text=f"Other Box - {otherIn}")


def text_to_speech(text):
    # Initialize gTTS with the text to convert
    speech = gTTS(text)

    # Save the audio file to a temporary file
    speech_file = 'speech.mp3'
    speech.save(speech_file)

    # Play the audio file
    play_audio(speech_file)
    
def play_audio(filename):
    pygame.mixer.init()
    pygame.mixer.music.load(filename)
    pygame.mixer.music.play()
    while pygame.mixer.music.get_busy():
        pygame.time.Clock().tick(10)
    

if __name__ == '__main__':
    servopin = 0
    color=0
    boxIn = 0
    redIn = 0
    blueIn = 0
    greenIn = 0
    otherIn = 0
    boxOut = 0
    Start=False
    
    
    root = tk.Tk()
    root.geometry("500x700")  # width,height
    root.title('Value Plotting')
    global boxInText, boxOutText, boxRedText, boxBlueText, boxGreenText, boxOtherText,canvas
    myfont = tk.font.Font(size=16)
    textProjName = tk.Label(root, text="** Conveyor Belt **", font=('Helvetica bold', 26), height=5)
    boxInText = tk.Label(root, text="Total Count - n/a")
    boxRedText = tk.Label(root, text="Red Box - n/a")
    boxBlueText = tk.Label(root, text="Blue Box - n/a")
    boxGreenText = tk.Label(root, text="Green Box - n/a")
    boxOtherText = tk.Label(root, text="Other Box - n/a")

    # buttons
    exitbtn = tk.Button(text="EXIT", padx=10, pady=10, command=stop,width=15)
    exitbtn['font'] = myfont



    startBtn = tk.Button(text="START", padx=10, pady=10, command=start, width=15)
    startBtn['font'] = myfont

    root.after(2000, readData)
    canvas = tk.Canvas(root, width=200, height=200)
    # Draw a circle on the canvas
    circle = canvas.create_oval(50, 50, 200, 200, fill='white')
    textProjName.pack()
    canvas.pack()
    boxInText.pack()
    boxRedText.pack()
    boxBlueText.pack()
    boxGreenText.pack()
    boxOtherText.pack()
    exitbtn.pack(side='bottom')
    startBtn.pack(side='bottom')
    tk.mainloop()