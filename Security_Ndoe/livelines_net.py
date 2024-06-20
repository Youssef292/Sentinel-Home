import cv2
from tensorflow.keras.preprocessing.image import img_to_array
import os
import numpy as np
from tensorflow.keras.models import model_from_json
import tensorflow as tf
import threading

def and_last_three_results(results):
    # Ensure there are at least three results
    if len(results) < 3:
        return None

    # Take the last three results
    last_three = results[-3:]

    # Remove the last three results from the main list to avoid reusing them
    del results[-3:]

    # Perform ANDing operation
    and_result = last_three[0] & last_three[1] & last_three[2]
    return and_result

def input_thread(stop_event):
    input("Press 'q' to stop the program...\n")
    stop_event.set()
def Spoof():
    root_dir = os.getcwd()
    # Load Face Detection Model
    face_cascade = cv2.CascadeClassifier("models/haarcascade_frontalface_default.xml")
    # Load Anti-Spoofing Model graph
    json_file = open('antispoofing_models/antispoofing_model.json', 'r')
    loaded_model_json = json_file.read()
    json_file.close()
    model = model_from_json(loaded_model_json)
    # load antispoofing model weights
    model.load_weights('antispoofing_models/antispoofing_model.h5')
    print("Model loaded from disk")

    results = []  # List to store results

    stop_event = threading.Event()
    thread = threading.Thread(target=input_thread, args=(stop_event,))
    thread.start()
    cnt = 0
    video = cv2.VideoCapture(0)
    while not stop_event.is_set():
        try:
            ret, frame = video.read()
            gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
            faces = face_cascade.detectMultiScale(gray, 1.3, 5)
            for (x, y, w, h) in faces:
                face = frame[y-5:y+h+5, x-5:x+w+5]
                resized_face = cv2.resize(face, (160, 160))
                resized_face = resized_face.astype("float") / 255.0
                resized_face = np.expand_dims(resized_face, axis=0)
                preds = model.predict(resized_face)[0]
                print(preds)
                if preds > 0.5:
                    results.append(0)  # Spoof
                else:
                    results.append(1)  # Real

            # Perform ANDing operation on the last three results if available
            if len(results) >= 3:
                and_result = and_last_three_results(results)
                if and_result is not None:
                    print(f"AND result of last three results: {and_result}")
                cnt +=1
                if cnt == 3:
                    break
        except Exception as e:
            print(f"An error occurred: {e}")  # Print the exception for debugging
            break  # Break the loop if an exception occurs
    # Ensure cleanup
    video.release()
    cv2.destroyAllWindows()
    print("Video capture released, exiting.")
    return and_result

