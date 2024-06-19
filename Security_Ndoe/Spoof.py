import cv2
import numpy as np
import tflite_runtime.interpreter as tflite

# Load the TFLite Model
interpreter = tflite.Interpreter(model_path="antispoofing_model.tflite")
interpreter.allocate_tensors()

# Get Input and Output Details
input_details = interpreter.get_input_details()
output_details = interpreter.get_output_details()

# Camera Initialization
camera = cv2.VideoCapture(0)  

def process_frame(frame):
    """
    Function to process a single frame and return the model's output.
    """
    # Preprocessing
    resized_frame = cv2.resize(frame, (input_details[0]['shape'][1], input_details[0]['shape'][2]))
    input_data = np.expand_dims(resized_frame, axis=0).astype(np.float32) / 255.0

    # Model Inference
    interpreter.set_tensor(input_details[0]['index'], input_data)
    interpreter.invoke()
    output_data = interpreter.get_tensor(output_details[0]['index'])

    return output_data

while True:
    # Capture Frame
    ret, frame = camera.read()

    # Get Model Output
    output_data = process_frame(frame)

    # Postprocessing and Display Results
    # ... (Interpret 'output_data' based on your model's output)
    if output_data[0][0] < 0.5:
        label = 'Fake'
    else:
        label = 'Real'
    cv2.putText(frame, label, (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.9, (0, 0, 255), 2)

    # ... (Display results on 'frame')
    cv2.imshow("Antispoofing Detection", frame)

    # Exit on Key Press (e.g., 'q')
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Release Camera
camera.release()
cv2.destroyAllWindows()