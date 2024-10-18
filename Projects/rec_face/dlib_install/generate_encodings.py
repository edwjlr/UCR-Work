import face_recognition
import pickle
import os

def generate_encodings(dataset_path):
    known_face_encodings = []
    known_face_names = []

    # Loop through each person in the dataset
    for person_name in os.listdir(dataset_path):
        person_path = os.path.join(dataset_path, person_name)
        if not os.path.isdir(person_path):
            continue

        # Loop through each image for the person
        for image_name in os.listdir(person_path):
            image_path = os.path.join(person_path, image_name)
            image = face_recognition.load_image_file(image_path)
            face_encodings = face_recognition.face_encodings(image)

            if face_encodings:
                known_face_encodings.append(face_encodings[0])
                known_face_names.append(person_name)

    # Save encodings and names
    with open('face_encodings.pkl', 'wb') as f:
        pickle.dump((known_face_encodings, known_face_names), f)

if __name__ == '__main__':
    dataset_path = './dataset'  # Update this path if necessary
    generate_encodings(dataset_path)
    print("Encodings generated and saved.")
