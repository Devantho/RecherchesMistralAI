# Fine tuning

Il existe plusieurs méthode pour fine tuner un modèle Mistral, soit depuis l'interface web soit avec du code, dans cet exemple nous utiliserons python.

## Documentations
[Fine tuning Documentation Mistral](https://docs.mistral.ai/guides/finetuning/)

[Choisir correctement les paramètres de l'ia](https://docs.mistral.ai/guides/sampling/)

https://docs.mistral.ai/capabilities/finetuning/text_finetuning/

## Méthode avec l'interface web
Tout d'abord vous devez vous munir des fichiers  suivant:
- Jeux de données d'entrainement
- Fichiers de validation

Ensuite, vous devrez vous rendre sur [l'interface de finetuning de mistral](https://console.mistral.ai/build/finetuned-models) et cliquer sur "Affiner le modèle". Uplodez vous deux fichiers, choissisez le modèle (Ministral 3B par exemple) et cliquez sur suivant puis Lancer l'entrainement. Au bout de quelque minute vous pourrez retrouver votre modèle fine-tuné sur [l'interface de finetuning de mistral](https://console.mistral.ai/build/finetuned-models), vous pourrez enfin l'utiliser pour créer un agent ia ou autre.

## Méthode en python

La méthode en python ne sert uniquement dans un but d'automatisation, elle n'offre pas plude possibilités, de plus elle peut échoué si le fichier d'entrainement n'est pas validé. Néanmoins cette méthode peut être utile.

### Utilisez le code suivant pour fine tuné un modèle en python en et ajustez les paramètres.
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
