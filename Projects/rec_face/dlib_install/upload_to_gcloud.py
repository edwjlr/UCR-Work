import pickle

# Load face encodings
with open('face_encodings.pkl', 'rb') as f:
    known_face_encodings = pickle.load(f)

# Assuming `known_face_encodings` is a dict with person names as keys and their encodings as values
names = list(known_face_encodings.keys())
encodings = list(known_face_encodings.values())
