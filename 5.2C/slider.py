import tkinter as tk
from tkinter import messagebox
import RPi.GPIO as GPIO
import sys

RED_PIN = 17
GREEN_PIN = 27
RED2_PIN = 22

GPIO.setmode(GPIO.BCM)
GPIO.setup(RED_PIN, GPIO.OUT)
GPIO.setup(GREEN_PIN, GPIO.OUT)
GPIO.setup(RED2_PIN, GPIO.OUT)

pwm_red = GPIO.PWM(RED_PIN, 100)
pwm_green = GPIO.PWM(GREEN_PIN, 100)
pwm_red2 = GPIO.PWM(RED2_PIN, 100)

pwm_red.start(0)
pwm_green.start(0)
pwm_red2.start(0)

def update_red_intensity(value):
    pwm_red.ChangeDutyCycle(int(value))

def update_green_intensity(value):
    pwm_green.ChangeDutyCycle(int(value))

def update_red2_intensity(value):
    pwm_red2.ChangeDutyCycle(int(value))

def exit_program():
    if messagebox.askokcancel("Exit", "Do you want to exit?"):
        pwm_red.stop()
        pwm_green.stop()
        pwm_red2.stop()
        GPIO.cleanup()
        root.destroy()
        sys.exit()

root = tk.Tk()
root.title("LED PWM Controller")

tk.Label(root, text="Red Intensity:").pack()
red_slider = tk.Scale(root, from_=0, to=100, orient=tk.HORIZONTAL, command=update_red_intensity)
red_slider.pack()

tk.Label(root, text="Green Intensity:").pack()
green_slider = tk.Scale(root, from_=0, to=100, orient=tk.HORIZONTAL, command=update_green_intensity)
green_slider.pack()

tk.Label(root, text="Red2 Intensity:").pack()
red2_slider = tk.Scale(root, from_=0, to=100, orient=tk.HORIZONTAL, command=update_red2_intensity)
red2_slider.pack()

tk.Button(root, text="Exit", command=exit_program).pack(pady=10)

root.mainloop()
