from weapon_detection import detect_weapons_in_video
from livelines_net import Spoof
from facial_req import Face_Recognition
from SIM800 import *
from gpiozero import MotionSensor
from gpiozero.pins.pigpio import PiGPIOFactory
from time import sleep
Police_Station = "+201552229799"

# Create a PiGPIOFactory instance
factory = PiGPIOFactory()

# Define the PIR sensor input pin using the PiGPIOFactory
 # Use GPIO pin 4
pir = MotionSensor(21, pin_factory=factory)

while True:
    if pir.motion_detected:
        print("Motion")
        Weapon_result = detect_weapons_in_video(model_path='best.pt', duration=7)
        if Weapon_result:
            print ("Emergency")
            make_call(Police_Station)
            sleep(3)
            ser.close()
        else:
            print("No Weapon")
            print("==================")
            spoofing = Spoof()
            Facial = Face_Recognition()
            if spoofing:
                print("Real Person")
            else:
                print("Spoof")

            if Facial:
                print("Hi")
            else:
                print("Unknown")
    else :
        print("No Motion")
    sleep(1)

exit()