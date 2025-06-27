# Chatbot API Mistral

## Description

Ce projet fournit une API REST en C++ avec Crow qui expose un endpoint `/ask`. Celui-ci permet de :

* Générer des embeddings via Mistral pour une question utilisateur
* Trouver les chunks les plus proches via FAISS
* Construire un prompt contextualisé
* Interroger Mistral 8B latest via une requête HTTP
* Retourner uniquement la réponse générée (champ `answer` en JSON)

Une interface HTML permet d’interagir facilement avec ce chatbot en thème sombre.

---

## Fonctionnalités

* 🔍 Recherche vectorielle contextuelle (embeddings + FAISS)
* 🤖 Génération de réponse par modèle Mistral 8B
* 🧠 API REST simple avec Crow
* 🛡️ Gestion des erreurs et du CORS

---

## Prérequis

* C++17 ou plus récent
* Crow (framework HTTP C++)
* nlohmann/json
* FAISS (Facebook AI Similarity Search)
* libcurl
* Une clé API Mistral définie dans `MISTRAL_API_KEY`

---
## Configuration

* Placez votre fichier `chunks.txt` à la racine du projet : chaque ligne représente un chunk de contexte.
* Définissez votre clé API dans les variables suivante:
- EMB_KEY
- CHAT_KEY

## Utilisation
Par défaut, le serveur écoute sur `http://localhost:18080`

### 2. Requête via curl

```bash
curl -X POST http://localhost:18080/ask \
     -H "Content-Type: application/json" \
     -d '{"question": "Comment ajout un utilisateur ?"}'
```

Réponse attendue :

```json
{ "answer": "Voci la procedure pour ajouter un utilisateur : [...]" }
```

## Structure du projet

```
├── chunks.txt  # Chunks de texte initial pour FAISS (DOC Soins2000)
├── main.cpp    # Code principal C++ (API Crow)
├── README.md   # Documentation
```

---

## Gestion CORS

Un middleware Crow personnalisé ajoute :

```
Access-Control-Allow-Origin: *
Access-Control-Allow-Methods: POST, OPTIONS
Access-Control-Allow-Headers: Content-Type
```

Les requêtes OPTIONS reçoivent un code 204.

---

## Résultat JSON

Réponses normales :

```json
{ "answer": "Texte généré par Mistral" }
```

Erreurs traitées :

```json
{ "error": "Message d'erreur détaillé" }
```

---

