# Fine-tuning

Il existe plusieurs méthodes pour fine-tuner un modèle Mistral, soit depuis l'interface web, soit avec du code. Dans cet exemple, nous utiliserons Python.

## Documentations
[Documentation Fine-tuning Mistral](https://docs.mistral.ai/guides/finetuning/)

[Choisir correctement les paramètres de l'IA](https://docs.mistral.ai/guides/sampling/)

https://docs.mistral.ai/capabilities/finetuning/text_finetuning/

## Méthode avec l'interface web
Tout d'abord, vous devez vous munir des fichiers suivants :
- Jeu de données d'entraînement
- Fichier de validation

Ensuite, vous devrez vous rendre sur [l'interface de fine-tuning de Mistral](https://console.mistral.ai/build/finetuned-models) et cliquer sur "Affiner le modèle". Uploadez vos deux fichiers, choisissez le modèle (Ministral 3B par exemple), cliquez sur "Suivant" puis sur "Lancer l'entraînement". Au bout de quelques minutes, vous pourrez retrouver votre modèle fine-tuné sur la même interface. Vous pourrez enfin l'utiliser pour créer un agent IA ou autre.

## Méthode en Python

La méthode en Python ne sert qu’à des fins d'automatisation. Elle n'offre pas plus de possibilités. De plus, elle peut échouer si le fichier d'entraînement n'est pas validé. Néanmoins, cette méthode peut être utile.

### Utilisez le code suivant pour fine-tuner un modèle en Python et ajustez les paramètres :
```python
from mistralai import Mistral
import os

api_key = os.environ["MISTRAL_API_KEY"]

client = Mistral(api_key=api_key)

training_data = client.files.upload(
    file={
        "file_name": "training_file.jsonl",
        "content": open("training_file.jsonl", "rb"),
    }
)

validation_data = client.files.upload(
    file={
        "file_name": "validation_file.jsonl",
        "content": open("validation_file.jsonl", "rb"),
    }
)
# create a fine-tuning job
created_jobs = client.fine_tuning.jobs.create(
    model="open-mistral-7b",
    training_files=[{"file_id": training_data.id, "weight": 1}],
    validation_files=[validation_data.id],
    hyperparameters={
        "training_steps": 10,
        "learning_rate":0.0001
    },
    auto_start=True,
#   integrations=[
#       {
#           "project": "finetuning",
#           "api_key": "WANDB_KEY",
#       }
#   ]
)

```

## Paramètres
[Guide de choix des paramètres](https://docs.mistral.ai/guides/sampling/) (Top_p, temperature)
