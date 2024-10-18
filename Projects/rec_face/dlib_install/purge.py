from firebase_admin import firestore
from firebase_admin import credentials
import firebase_admin

# Initialize Firebase Admin SDK
cred = credentials.Certificate("graphic-charter-415020-3f87c8ff8c15.json")  # Replace with the path to your service account key file
firebase_admin.initialize_app(cred)

# Define a function to delete all documents in a collection
def delete_all_documents(collection_name):
    try:
        db = firestore.client()
        collection_ref = db.collection(collection_name)
        docs = collection_ref.stream()

        # Delete each document in the collection
        for doc in docs:
            doc.reference.delete()
        
        print(f"All documents in collection '{collection_name}' successfully deleted.")
    except Exception as e:
        print(f"Error deleting documents: {e}")

# Call the function with the collection name
collection_name = "attendance_records"
delete_all_documents(collection_name)
