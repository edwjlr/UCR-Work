import cv2
import dlib
import os
import time

def capture_images(person_name, capture_interval=5):
    cap = cv2.VideoCapture(0)  # Use the default camera
    if not cap.isOpened():
        print("Error: Could not open camera.")
        return
    
    # Create a directory for the person if it doesn't exist
    person_dir = f"./dataset/{person_name}"
    os.makedirs(person_dir, exist_ok=True)
    
    # Initialize dlib's face detector and facial landmark predictor
    detector = dlib.get_frontal_face_detector()
    predictor = dlib.shape_predictor("shape_predictor_68_face_landmarks.dat")
    
    print(f"Capturing images for {person_name}. Press 'q' to stop.")
    
    img_count = 0
    start_time = time.time()
    
    while True:
        ret, frame = cap.read()
        if not ret:
            break
        
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        faces = detector(gray, 0)
        
        for face in faces:
            x1, y1, x2, y2 = face.left(), face.top(), face.right(), face.bottom()
            landmarks = predictor(gray, face)
            
            # Draw facial landmarks
            for n in range(0, 68):
                x = landmarks.part(n).x
                y = landmarks.part(n).y
                cv2.circle(frame, (x, y), 1, (255, 0, 0), -1)
        
        # Display the frame
        cv2.imshow("Capture Images - Press 'q' to quit", frame)
        
        # Automatically save an image at the specified interval if a face is detected
        if time.time() - start_time >= capture_interval and faces:
            img_path = os.path.join(person_dir, f"{person_name}_{img_count}.jpg")
            cv2.imwrite(img_path, frame)
            print(f"Captured {img_path}")
            img_count += 1
            start_time = time.time()  # Reset the timer
        
        # Break the loop when 'q' is pressed
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
    
    cap.release()
    cv2.destroyAllWindows()

def main():
    person_name = input("Enter the name of the person to capture images for: ")
    capture_interval = float(input("Enter the capture interval in seconds (e.g., 5 for every 5 seconds): "))
    capture_images(person_name, capture_interval)

if __name__ == "__main__":
    main()
