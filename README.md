# Les modèles
Les modèles les plus appropriés pour crée une ia capable de répondre a des questions sur le logiciel Soins2000 semble être:
- Ministral 8B
- Ministral 3B
- Open Mistral Nemo
Ne surtout pas utiliser magistral-medium, je n'ai obtenu que des mauvais résultats.

La méthode approprié pour la compréhension de la documentation n'ai pas le fine tuning mais le RAG (Retrieval-Augmented Generation), référez vous a cette page: https://docs.mistral.ai/guides/rag/

Cependant, le fine-tuning peut être utilisé en plus du RAG pour affiner la forme et le format des réponses. [Voir Fine-tuning](/Fine%20tuning.md)


# Les outils
Dans ce repos vous trouverez les outils suivant:
- Un outils en python permettant de benchmark les modèle mistral et rendre un compte rendu au format MarkDown.
- Un chatbot Python exploitant le principe du RAG. Il commence par convertir la documentation en vecteurs, qu’il stocke ensuite dans une base spécialisée (Faiss). Lorsqu’un utilisateur pose une question, celle-ci est à son tour transformée en vecteurs pour rechercher les passages les plus pertinents dans la base. Le contenu trouvé est alors fourni comme contexte au modèle afin de générer une réponse adaptée.
- Un client en c++ qui envoie les requête des utilisateurs à un serveur et reçoit les réponses.
- Un serveur en c++ qui recoit les question, s'occupe du RAG puis de la génération du prompt pour finir par envoyer le prompt a l'[API Mistral](https://console.mistral.ai/) puis renvoyer la réponse au client. 


