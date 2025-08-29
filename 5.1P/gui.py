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

def toggle_led():
    selected = led_var.get()
    GPIO.output(RED_PIN, GPIO.HIGH if selected == 'red' else GPIO.LOW)
    GPIO.output(GREEN_PIN, GPIO.HIGH if selected == 'green' else GPIO.LOW)
    GPIO.output(RED2_PIN, GPIO.HIGH if selected == 'red2' else GPIO.LOW)

def exit_program():
    if messagebox.askokcancel("Exit", "Do you want to exit?"):
        GPIO.cleanup()
        root.destroy()
        sys.exit()

root = tk.Tk()
root.title("LED Controller")

led_var = tk.StringVar(value="none")

tk.Label(root, text="Select LED:").pack()
tk.Radiobutton(root, text="Red", variable=led_var, value="red", command=toggle_led).pack(anchor=tk.W)
tk.Radiobutton(root, text="Green", variable=led_var, value="green", command=toggle_led).pack(anchor=tk.W)
tk.Radiobutton(root, text="Red2", variable=led_var, value="red2", command=toggle_led).pack(anchor=tk.W)

tk.Button(root, text="Exit", command=exit_program).pack(pady=10)

root.mainloop()
