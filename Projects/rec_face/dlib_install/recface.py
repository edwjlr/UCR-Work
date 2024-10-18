import cv2
import dlib
import numpy as np
import face_recognition
import pickle
from google.cloud import firestore
from google.cloud import storage
from datetime import datetime, timedelta
import uuid  # Import uuid module for generating unique identifiers

# Load known face encodings and their names
with open('face_encodings.pkl', 'rb') as f:
    known_face_encodings, known_face_names = pickle.load(f)

# Initialize dlib's face detector and the facial landmark predictor
detector = dlib.get_frontal_face_detector()
predictor_path = 'shape_predictor_68_face_landmarks.dat'
predictor = dlib.shape_predictor(predictor_path)

# Initialize Firestore client
db = firestore.Client()

# Initialize Google Cloud Storage client
storage_client = storage.Client()
bucket = storage_client.bucket('graphic-charter-415020.appspot.com')  # Firebase Storage bucket

# Initialize variables
face_locations = []
face_encodings = []
face_names = []
process_this_frame = True
session_logged_names = set()  # Track logged names for the session

# Dictionary to store last uploaded timestamps for each person
last_uploaded_timestamps = {}

# Function to check if it's time to upload a new image for a person
def should_upload_image(name):
    last_uploaded_time = last_uploaded_timestamps.get(name)
    if last_uploaded_time is None:
        return True
    return datetime.now() - last_uploaded_time > timedelta(hours=1)  

# Start video capture
video_capture = cv2.VideoCapture(0)

while True:
    # Capture frame-by-frame
    ret, frame = video_capture.read()
    if not ret:
        break

    # Convert the frame from BGR color (which OpenCV uses) to RGB color (which face_recognition uses)
    rgb_frame = frame[:, :, ::-1]

    # Resize the frame to 1/4 size for faster face detection processing
    small_rgb_frame = cv2.resize(rgb_frame, (0, 0), fx=0.25, fy=0.25)

    if process_this_frame:
        # Find all the faces and face encodings in the current frame
        face_locations = face_recognition.face_locations(small_rgb_frame, model="hog")
        face_encodings = face_recognition.face_encodings(small_rgb_frame, face_locations)

        face_names = []
        for face_encoding in face_encodings:
            # See if the face is a match for the known face(s)
            matches = face_recognition.compare_faces(known_face_encodings, face_encoding, tolerance=0.4)
            unique_id = str(uuid.uuid4())
            name = unique_id

            # Use the known face with the smallest distance to the new face
            face_distances = face_recognition.face_distance(known_face_encodings, face_encoding)
            best_match_index = np.argmin(face_distances)
            if matches[best_match_index]:
                name = known_face_names[best_match_index]

            face_names.append(name)

            # Check and update Firestore only if the name has not been logged in this session
            if name not in session_logged_names:
                # Firestore update and image upload for recognized faces
                if should_upload_image(name):

                    # Upload face image to Firebase Storage
                    timestamp_str = datetime.now().strftime("%Y%m%d%H%M%S")
                    if name != "Unknown":
                        image_filename = f"{unique_id}_face.jpg"
                    else:
                        image_filename = f"{unique_id}_face.jpg"
                    image_data = cv2.imencode('.jpg', frame)[1].tobytes()
                    blob = bucket.blob(image_filename)
                    blob.upload_from_string(image_data, content_type='image/jpeg')

                    # Update last uploaded timestamp
                    last_uploaded_timestamps[name] = datetime.now()

                # Log entry for recognized faces
                doc_ref = db.collection('log_entries').document()
                doc_ref.set({
                    'name': name,
                    'unique_id' : unique_id,
                    'timestamp': firestore.SERVER_TIMESTAMP
                })

                # Add name to session logged names to avoid re-logging in the same session
                session_logged_names.add(name)

    process_this_frame = not process_this_frame

    # Display results
    for (top, right, bottom, left), name in zip(face_locations, face_names):
        top *= 4
        right *= 4
        bottom *= 4
        left *= 4

        cv2.rectangle(frame, (left, top), (right, bottom), (0, 0, 255), 2)
        cv2.rectangle(frame, (left, bottom + 35), (right, bottom), (0, 0, 255), cv2.FILLED)
        font = cv2.FONT_HERSHEY_DUPLEX
        cv2.putText(frame, name, (left + 6, bottom + 25), font, 0.5, (255, 255, 255), 1)

    cv2.imshow('Video', frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

video_capture.release()
cv2.destroyAllWindows()
