import cv2
import dlib
import numpy as np
import face_recognition
import pickle

# Load known face encodings and their names
with open('face_encodings.pkl', 'rb') as f:
    known_face_encodings, known_face_names = pickle.load(f)

# Initialize dlib's face detector and the facial landmark predictor
detector = dlib.get_frontal_face_detector()
predictor_path = 'shape_predictor_68_face_landmarks.dat'  # Ensure this path is correct
predictor = dlib.shape_predictor(predictor_path)

# Initialize variables
face_locations = []
face_encodings = []
face_names = []
process_this_frame = True

# Start video capture
video_capture = cv2.VideoCapture(0)

while True:
    # Capture frame-by-frame
    ret, frame = video_capture.read()
    if not ret:
        break

    # Convert the frame from BGR color (which OpenCV uses) to RGB color (which dlib uses)
    rgb_frame = frame[:, :, ::-1]

    # Resize the frame to 1/4 size for faster face detection processing
    small_rgb_frame = cv2.resize(rgb_frame, (0, 0), fx=0.25, fy=0.25)

    # Only process every other frame to save time
    if process_this_frame:
        # Find all the faces and face encodings in the current frame
        face_locations = face_recognition.face_locations(small_rgb_frame, model="hog")
        face_encodings = face_recognition.face_encodings(small_rgb_frame, face_locations)

        face_names = []
        for face_encoding in face_encodings:
            # See if the face is a match for the known face(s)
            matches = face_recognition.compare_faces(known_face_encodings, face_encoding, tolerance=0.6)
            name = "Unknown"

            # Use the known face with the smallest distance to the new face
            face_distances = face_recognition.face_distance(known_face_encodings, face_encoding)
            best_match_index = np.argmin(face_distances)
            if matches[best_match_index]:
                name = known_face_names[best_match_index]

            face_names.append(name)

    process_this_frame = not process_this_frame

    # Display the results
    for (top, right, bottom, left), name in zip(face_locations, face_names):
        # Scale back up face locations since the frame was scaled to 1/4 size for faster face detection processing
        top *= 4
        right *= 4
        bottom *= 4
        left *= 4

        # Draw a box around the face
        cv2.rectangle(frame, (left, top), (right, bottom), (0, 0, 255), 2)

        # Draw a label with a name below the face
        cv2.rectangle(frame, (left, bottom + 35), (right, bottom), (0, 0, 255), cv2.FILLED)
        font = cv2.FONT_HERSHEY_DUPLEX
        cv2.putText(frame, name, (left + 6, bottom + 25), font, 0.5, (255, 255, 255), 1)

    # Display the resulting frame
    cv2.imshow('Video', frame)

    # Break from the loop on pressing 'q'
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Release the capture
video_capture.release()
cv2.destroyAllWindows()
