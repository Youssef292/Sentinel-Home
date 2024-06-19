import cv2
from ultralytics import YOLO
import supervision as sv
import os
import time
def detect_weapons_in_video(model_path='best.pt', duration=7):
    """Detects weapons in a video captured from the camera for a specified duration."""
    # Initialize the YOLO model
    model = YOLO(model_path)

    # Open the camera
    cap = cv2.VideoCapture(0)  # Use 0 for the default camera

    if not cap.isOpened():
        print("Error: Could not open camera.")
        return -1

    start_time = time.time()
    weapon_detected = False

    while (time.time() - start_time) < duration:
        ret, frame = cap.read()
        if not ret:
            print("Error: Failed to capture image.")
            break

        with open(os.devnull, 'w') as fnull:  # Optional: suppress model output
            result = model(frame)[0]

        detections = sv.Detections.from_ultralytics(result)

        if any(cls in ['Knife', 'Pistol', 'Gun', 'Knifes', 'Pistols', 'Guns'] for cls in detections['class_name']):
            weapon_detected = True
            break  # Exit loop if a weapon is detected

    # Release the camera
    cap.release()

    return 1 if weapon_detected else 0
