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
- Un outils en python permettant de [benchmark](/Programmes/ChatBot%20Python/benchmark.py) les modèle mistral et rendre un compte rendu au format MarkDown.
- Un [chatbot Python](/Programmes/ChatBot%20Python/) exploitant le principe du RAG. Il commence par convertir la documentation en vecteurs, qu’il stocke ensuite dans une base spécialisée (Faiss). Lorsqu’un utilisateur pose une question, celle-ci est à son tour transformée en vecteurs pour rechercher les passages les plus pertinents dans la base. Le contenu trouvé est alors fourni comme contexte au modèle afin de générer une réponse adaptée.
- Un [client](/Programmes/Chatbot%20client-serveur%20C++/Client/) en c++ qui envoie les requête des utilisateurs à un serveur et reçoit les réponses.
- Un [serveur](/Programmes/Chatbot%20client-serveur%20C++/Serveur/) en c++ qui recoit les question, s'occupe du RAG puis de la génération du prompt pour finir par envoyer le prompt a l'[API Mistral](https://console.mistral.ai/) puis renvoyer la réponse au client.
> Le client/serveur en c++ fournit de moins bonne réponses que le ChatBot Python, cela viens de la méthode RAG qui est différente je pense.
- [Des benchmarks sur différent modèles](/benchmarks/)

# 🚀 Trois moyens d’améliorer les performances d’un modèle IA
![Illustration trois méthodes](https://www.dailydoseofds.com/content/images/size/w1200/2024/11/images_to_frame--5-.png)
## [Fine-tuning](Fine%20tuning.md)
Réentraîner le modèle sur des données spécifiques pour qu’il adopte un ton, un style ou un savoir-faire particulier.

## RAG (Retrieval-Augmented Generation)
Connecter le modèle à une base documentaire externe pour qu’il aille chercher des informations à jour avant de répondre.

## Prompting
Formuler des instructions précises pour orienter la réponse du modèle dans la bonne direction.
[Prompting Guide](https://docs.mistral.ai/guides/prompting_capabilities/)

## 🧩 Une combinaison puissante
Utilisés ensemble, ces trois leviers permettent d’exploiter le plein potentiel d’un modèle IA.
Le fine-tuning lui donne une personnalité ou une spécialisation métier, le RAG garantit que ses réponses s’appuient sur des données fiables et récentes, et le prompting affine en temps réel la forme et le fond de la sortie.
Cette combinaison crée un assistant intelligent, pertinent et réactif, capable de s’adapter à la fois au contexte, à l’actualité et aux besoins de l’utilisateur.

