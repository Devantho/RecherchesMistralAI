# API en python avec Flask

## Modifications
- Séparation du prompt système et du prompt user dans l'envoi de la requête à l'API Mistral

## Fonctionnement
L'api flask recoit une requete POST sur http://mondomaine.fr/ask avec un body json qui contient une clé "question" et une clé "debug".
```json
{
    "question" :"Ma question sur soins 2000 ?",
    "debug":"True"
}
```
Debug ne peut être égale uniquement à "True" ou "False"; dans le cas ou debug est égal à "True" alors l'api retoune le prompt système qui contient le contexte en plus de la réponse à la question.

## Interface
Ce repo contient une [interface web](/Programmes/API%20Python/main.py) sous la forme d'un fichier html qui peut tourner un local sans serveur web. 
