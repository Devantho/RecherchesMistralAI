# --- api_server.py ---
from flask import Flask, request, jsonify
from flask_cors import CORS
from mistralai import Mistral
import numpy as np
import faiss
import os
import re
from dotenv import load_dotenv

# Configuration
load_dotenv()
api_key = os.getenv("MISTRAL_API_KEY")
client = Mistral(api_key=api_key)

# Initialisation Flask
app = Flask(__name__)
CORS(app)

# Chargement et préparation des données
with open('documentation.txt', 'r', encoding='utf-8') as f:
    text = f.read()

paragraphs = [p.strip() for p in text.split('\n\n') if len(p.strip()) > 50]

# Fusion de paragraphes courts
target_min_len = 80
target_max_len = 300
chunks = []
buffer = ""
for para in paragraphs:
    if len(buffer) + len(para) < target_max_len:
        buffer += para + "\n"
    else:
        if len(buffer.strip()) > target_min_len:
            chunks.append(buffer.strip())
            buffer = para + "\n"
        else:
            buffer += para + "\n"
if buffer.strip():
    chunks.append(buffer.strip())

# Embedding & indexation

def get_text_embedding(input):
    response = client.embeddings.create(model="mistral-embed", inputs=input)
    return response.data[0].embedding

text_embeddings = np.array([get_text_embedding(chunk) for chunk in chunks])
d = text_embeddings.shape[1]
index = faiss.IndexFlatL2(d)
index.add(text_embeddings)

# Endpoint API
@app.route('/ask', methods=['POST'])
def ask():
    data = request.json
    question = data.get('question', '')
    debug = data.get('debug', '')
    if not question:
        return jsonify({'error': 'Question required'}), 400

    question_embedding = np.array([get_text_embedding(question)])
    D, I = index.search(question_embedding, k=2)
    retrieved_chunks = [chunks[i] for i in I[0]]

    system_prompt = """
    Vous êtes l’assistant de Soins2000, spécialisé dans la gestion des utilisateurs.  
    – N’utilisez pas vos connaissances préalables : fiez-vous **exclusivement** aux informations fournies dans le contexte.  
    – Répondez **uniquement** en Markdown, sans exposer votre raisonnement.  
    – Si la demande ne peut être satisfaite, renvoyez **exactement** ce message :
    - Encadre les mots importants comme ceci **exemple** (deux * de chaque coté)

    Je ne peux pas répondre à votre question, merci de contacter l’assistance Soins2000.  
    Adresse e-mail : maintenance@soins2000.com  
    Numéro de téléphone : 04 68 42 78 90"""

    contexte = f"""
    Le contexte se trouve ci-dessous :\n
    ---------------------\n
    {retrieved_chunks[0]}\n
    ---------------------
    """

    system_prompt += f"\n{contexte}"



    messages = [{"role": "system", "content": system_prompt},{"role": "user", "content": question}]
    chat_response = client.chat.complete(
        model="ministral-8b-latest",
        messages=messages,
        temperature=0.2,
        top_p=0.2
    )
    response = chat_response.choices[0].message.content

    debug = bool(debug)
    if debug:
        response += f"\n\n**DEBUG Prompt Système** :{system_prompt}\n"
        return jsonify({'response': response})
    else:
        return jsonify({'response': response})



# Lancement serveur local
if __name__ == '__main__':
    app.run(debug=True)
