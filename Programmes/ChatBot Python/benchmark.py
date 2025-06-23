from mistralai import Mistral
import numpy as np
import faiss
import os
from dotenv import load_dotenv
import time
from datetime import datetime


models = ["mistral-large-latest", "open-mistral-nemo", "ministral-8b-latest", "ministral-3b-latest", "mistral-medium-latest", "mistral-tiny"]
system_prompt="Répond a la demande de l'utilisateur en utilisant le contexte. N'utilise pas tes connaissances préalables. N'explique pas ton raisonnement. Dans le cas ou tu ne peut pas répondre alors retourne le message suivant: \n Je ne peux pas répondre a votre question, merci de contacter l'assistance Soins2000. \n Adresse e-mail: maintenance@soins2000.com Numéro de téléphone: 04 68 42 78 90"
question = "Quelles sont les premières décimales de pi ?"
temperature = 0.4
top_p = 0.1

load_dotenv()
api_key=os.getenv("MISTRAL_API_KEY")
client = Mistral(api_key=api_key)

f = open('documentation.txt', 'r')
text = f.read()
f.close()

benchmark= ""

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

def run_mistral(user_message, model):
    messages = [
        {
            "role": "user", "content": user_message
        }
    ]
    start_time = time.time()
    chat_response = client.chat.complete(
        model=model,
        messages=messages,
        temperature=temperature,
        top_p=top_p,
        timeout_ms=60000
    )
    end_time= time.time()
    execution_time = end_time - start_time
    return [chat_response.choices[0].message.content, execution_time]




question_embeddings = np.array([get_text_embedding(question)])
D, I = index.search(question_embeddings, k=2)  # distance, index
retrieved_chunk = [chunks[i] for i in I.tolist()[0]]


for x in range(len(models)):
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
    process = run_mistral(prompt, model=models[x])
    response = process[0]
    execution_time = process[1]
    benchmark = benchmark + f"\n\n# Modèle: {models[x]} \n## Temps d'exécution: {int(execution_time)} secondes\n## Note: \n{response}"
    print(f"Done : {models[x]}, {x+1} sur {len(models)} en {int(execution_time)}s")

now = datetime.now()
current_date = now.strftime("%d/%m/%Y")
f = open("benchmark.txt", "w")
f.write(f"User prompt: {question} \nSystème prompt: {system_prompt}\n## Température: {temperature}\n## Top_p: {top_p}\n## Date: {current_date}\n## Modèles {models}" + benchmark)
f.close()
