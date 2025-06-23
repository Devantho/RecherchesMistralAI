from mistralai import Mistral
import requests
import numpy as np
import faiss
import os
from getpass import getpass
from dotenv import load_dotenv

load_dotenv()
api_key=os.getenv("MISTRAL_API_KEY")
client = Mistral(api_key=api_key)

f = open('documentation.txt', 'r')
text = f.read()
f.close()


chunk_size = 1024
chunks = [text[i:i + chunk_size] for i in range(0, len(text), chunk_size)]
print(len(chunks))

def get_text_embedding(input):
    embeddings_batch_response = client.embeddings.create(
          model="mistral-embed",
          inputs=input
      )
    return embeddings_batch_response.data[0].embedding
text_embeddings = np.array([get_text_embedding(chunk) for chunk in chunks]) # Generate embedded text

# Store text embedding in Faiss as vectors
d = text_embeddings.shape[1]
index = faiss.IndexFlatL2(d)
index.add(text_embeddings)

system_prompt="Répond a la demande de l'utilisateur en utilisant le contexte. N'explique pas ton raisonnemet Dans le cas ou tu ne peut pas répondre alors retourne le message suivant: \n Je ne peux pas répondre a votre question, merci de contacter l'assistance Soins2000. \n Adresse e-mail: maintenance@soins2000.com Numéro de téléphone: 04 68 42 78 90"


def run_mistral(user_message, model="ministral-3b-latest"):
    messages = [
        {
            "role": "user", "content": user_message
        }
    ]
    chat_response = client.chat.complete(
        model=model,
        messages=messages,
        temperature=0.25,
        top_p=0.5
    )
    return (chat_response.choices[0].message.content)


while True:
    question = input("Question > ")
    question_embeddings = np.array([get_text_embedding(question)])
    D, I = index.search(question_embeddings, k=2)  # distance, index
    retrieved_chunk = [chunks[i] for i in I.tolist()[0]]
    prompt = f"""
    Le contexte ce trouve ci dessous.
    ---------------------
    {retrieved_chunk}
    ---------------------
    
    En prenant les informations que je viens de te donner et sans utiliser tes connaissances préalables, répond a la demande.
    Prompt système : {system_prompt}
    Prompt utilisateur: {question}
    Answer:
    """
    response = run_mistral(prompt)
    print(response)