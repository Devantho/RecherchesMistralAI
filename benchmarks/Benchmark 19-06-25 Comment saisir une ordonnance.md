# ATTENTION: Ce benchmark est en parti faux car les donnés de la base vectorielles n'expliques pas bien la démarche pour saisir une ordonnance
### System prompt: Répond a la demande de l'utilisateur en utilisant le contexte. N'explique pas ton raisonnement Dans le cas ou tu ne peut pas répondre alors retourne le message suivant: \n Je ne peux pas répondre a votre question, merci de contacter l'assistance Soins2000. 
Adresse e-mail: maintenance@soins2000.com 
Numéro de téléphone: 04 68 42 78 90
### Question = "Comment saisir une ordonnance ?"

Benchmark via un programme python, avec un RAG via FAISS depuis le documentation de Soins2000.
# Modèle: mistral-large-latest   
## Temps d'exécution: 6 secondes  
## Note:   
Pour saisir une ordonnance, suivez ces étapes :  
  
1. Cliquez sur + Ordo.  
2. Si vous connaissez les jours de la semaine pendant lesquels s’effectuent les séances, cochez ces jours en cliquant sur les cases correspondantes.  
3. Si le traitement s’effectue tous les x jours, sélectionnez le nombre de jours (1, 2, 3 ou 4) en cliquant sur la colonne ‘Tous les’.  
4. Si un traitement s’effectue d’une manière plus espacée, cliquez sur ‘…’, et saisissez un nombre de jours, ce nombre deviendra sélectionnable dans la liste.  
5. Si vous voulez ajouter des séances en fin d’ordonnance qui n’ont pas été effectuées à cause de l’absence du patient, saisissez-les manuellement, ou ajoutez des actes aux lignes d’actes, mais dans ce cas, le nombre de séances prévues sera augmenté.  
6. Vous ne pouvez pas modifier une absence patient, vous devez supprimer puis recréer l’absence.  
7. Si vous supprimez une absence patient, et qu’il existe des ordonnances pour ce patient ou la planification est utilisée, les séances concernées seront créées automatiquement.  
  
# Modèle: open-mistral-nemo   
## Temps d'exécution: 2 secondes  
## Note:   
Pour saisir une ordonnance, commencez par cliquer sur "+ Ordo" pour créer une nouvelle ordonnance. Vous pouvez saisir plusieurs ordonnances à la fois. Si vous connaissez les jours de la semaine pendant lesquels s'effectuent les séances, vous devez cocher ces jours en cliquant sur les cases correspondantes. Si le traitement s'effectue tous les x jours, sélectionnez le nombre de jours (1,2,3 ou 4) en cliquant sur la colonne 'Tous les'. Si un traitement s'effectue d'une manière plus espacée, cliquez sur '...', et saisissez un nombre de jour, ce nombre deviendra sélectionnable dans la liste. Si vous voulez ajouter des séances en fin d'ordonnance qui n'ont pas été effectuées à cause de l'absence du patient, saisissez-les manuellement, ou ajoutez des actes aux lignes d'actes, mais dans ce cas le nombre de séances prévues sera augmenté. Vous ne pouvez pas modifier une absence patient, vous devez supprimer puis recréer l'absence. Si vous supprimez une absence patient, et qu'il existe des ordonnances pour ce patient ou la planification est utilisée, les séances concernées seront créées automatiquement.  
  
# Modèle: ministral-8b-latest   
## Temps d'exécution: 1 secondes  
## Note:   
Pour saisir une ordonnance, suivez ces étapes :  
  
1. Cliquez sur + Ordo.  
2. Si vous connaissez les jours de la semaine pendant lesquels s'effectuent les séances, cochez ces jours en cliquant sur les cases correspondantes.  
3. Si le traitement s'effectue tous les x jours, sélectionnez le nombre de jours (1, 2, 3 ou 4) en cliquant sur la colonne 'Tous les'.  
4. Si un traitement s'effectue d'une manière plus espacée, cliquez sur '...' et saisissez un nombre de jours, ce nombre deviendra sélectionnable dans la liste.  
5. Si vous voulez ajouter des séances en fin d'ordonnance qui n'ont pas été effectuées à cause de l'absence du patient, saisissez-les manuellement, ou ajoutez des actes aux lignes d'actes, mais dans ce cas le nombre de séances prévues sera augmenté.  
6. Vous ne pouvez pas modifier une absence patient, vous devez supprimer puis recréer l'absence.  
7. Si vous supprimez une absence patient, et qu'il existe des ordonnances pour ce patient ou la planification est utilisée, les séances concernées seront créées automatiquement.  
  
# Modèle: ministral-3b-latest   
## Temps d'exécution: 1 secondes  
## Note:   
Pour saisir une ordonnance, commencez par cliquer sur "+ Ordo" en haut de la fenêtre. Si vous connaissez les jours de la semaine pendant lesquels s'effectuent les séances, cochez ces jours en cliquant sur les cases correspondantes. Si le traitement s'effectue tous les x jours, sélectionnez le nombre de jours (1, 2, 3 ou 4) en cliquant sur la colonne "Tous les". Si un traitement s'effectue d'une manière plus espacée, cliquez sur "..." et saisissez un nombre de jours, ce nombre deviendra sélectionnable dans la liste. Si vous voulez ajouter des séances en fin d'ordonnance qui n'ont pas été effectuées en raison de l'absence du patient, saisissez-les manuellement ou ajoutez des actes aux lignes d'actes, mais dans ce cas, le nombre de séances prévues sera augmenté. Vous ne pouvez pas modifier une absence de patient, vous devez supprimer puis recrécréer l'absence. Si vous supprimez une absence de patient et qu'il existe des ordonnances pour ce patient ou que la planification est utilisée, les séances concernées seront créées automatiquement.  
  
# Modèle: mistral-medium-latest   
## Temps d'exécution: 3 secondes  
## Note:   
Pour saisir une ordonnance, suivez ces étapes :  
  
1. Cliquez sur **+ Ordo**.  
2. Si vous connaissez les jours de la semaine pour les séances, cochez les cases correspondantes.  
3. Si le traitement a lieu tous les *x* jours, sélectionnez le nombre de jours (1, 2, 3 ou 4) dans la colonne **« Tous les »**.  
4. Pour un traitement plus espacé, cliquez sur **« … »** et saisissez un nombre de jours, qui deviendra sélectionnable dans la liste.  
  
Si vous devez ajouter des séances en fin d’ordonnance non effectuées en raison de l’absence du patient, saisissez-les manuellement ou ajoutez des actes aux lignes d’actes (ce qui augmentera le nombre de séances prévues).  
  
*Note* : Vous ne pouvez pas modifier une absence patient ; vous devez la supprimer puis la recréer. Si vous supprimez une absence et qu’il existe des ordonnances ou une planification pour ce patient, les séances concernées seront recréées automatiquement.  
  
# Modèle: magistral-medium-latest 
## Temps d'exécution: 11 secondes  
## Note:  1/10
<think>  
D'abord, je vais lire attentivement le contexte fourni pour comprendre comment saisir une ordonnance. Le contexte semble être une série d'instructions ou de procédures pour un logiciel ou un système de gestion.  
  
En parcourant le texte, je vois une section qui commence par "La saisie d’une ordonnance se déroule comme suit :". Cela semble être le point de départ pour répondre à la question.  
  
Voici les étapes mentionnées pour saisir une ordonnance :  
1. Cliquez sur "+ Ordo en" (le texte semble être coupé ici, mais on peut supposer que c'est "+ Ordo" ou quelque chose de similaire).  
2. Ensuite, il y a des instructions sur la façon de spécifier les jours de la semaine pour les séances ou la fréquence des traitements. Par exemple, cocher les jours de la semaine ou sélectionner un nombre de jours pour la fréquence du traitement.  
3. Il y a aussi des instructions pour ajouter des séances en fin d'ordonnance si le patient était absent, soit en les saisissant manuellement, soit en ajoutant des actes aux lignes d'actes.  
4. Il est mentionné que vous ne pouvez pas modifier une absence de patient, vous devez supprimer puis recréer l'absence.  
5. Enfin, il est indiqué que si vous supprimez une absence de patient et qu'il existe des ordonnances pour ce patient ou que la planification est utilisée, les séances concernées seront créées automatiquement.  
  
Cependant, le texte semble être incomplet ou coupé à certains endroits, ce qui peut rendre certaines parties un peu difficiles à comprendre. Mais en me basant sur les informations disponibles, je peux résumer les étapes pour saisir une ordonnance.  
  
Pour résumer, les étapes pour saisir une ordonnance semblent être les suivantes :  
6. Cliquez sur "+ Ordo" (ou quelque chose de similaire, le texte est incomplet ici).  
7. Spécifiez les jours de la semaine pour les séances ou la fréquence des traitements.  
8. Ajoutez des séances en fin d'ordonnance si nécessaire, soit manuellement, soit en ajoutant des actes aux lignes d'actes.  
9. Gérez les absences des patients en les supprimant et en les recréant si nécessaire.  
  
Mais comme le texte est incomplet, je ne suis pas sûr de toutes les étapes. Cependant, je peux essayer de formuler une réponse basée sur les informations disponibles.  
  
Voici une réponse possible :  
  
Pour saisir une ordonnance, commencez par cliquer sur "+ Ordo". Ensuite, spécifiez les jours de la semaine pour les séances ou la fréquence des traitements. Vous pouvez ajouter des séances en fin d'ordonnance si nécessaire, soit manuellement, soit en ajoutant des actes aux lignes d'actes. Pour les absences des patients, vous devez supprimer et recréer l'absence si nécessaire.  
  
Mais comme le texte est incomplet, je ne suis pas sûr de toutes les étapes. Peut-être que la réponse devrait être plus générale ou indiquer que certaines informations sont manquantes.  
  
Cependant, le prompt système indique que si je ne peux pas répondre, je dois retourner un message spécifique. Comme le texte est incomplet et que certaines parties sont manquantes, je ne suis pas sûr de pouvoir donner une réponse complète et précise.  
  
Donc, peut-être que la meilleure réponse serait de dire que je ne peux pas répondre à la question en raison du texte incomplet et de diriger l'utilisateur vers l'assistance.  
  
Mais avant de décider, je vais relire le texte pour voir si je peux extraire plus d'informations.  
  
En relisant, je vois que la première partie du texte parle de l'ajout d'un utilisateur, ce qui n'est pas pertinent pour la question sur la saisie d'une ordonnance. La partie pertinente commence par "La saisie d’une ordonnance se déroule comme suit :".  
  
Voici le texte pertinent :  
  
"La saisie d’une ordonnance se déroule comme suit :  
  - Cliquez sur + Ordo en"  
  
Le reste du texte semble être coupé ou incomplet. Il y a des instructions sur la gestion des séances et des absences, mais la première étape pour saisir une ordonnance est incomplète.  
  
Donc, en raison de l'incomplétude du texte, je ne peux pas fournir une réponse complète et précise à la question "Comment saisir une ordonnance ?".  
  
Par conséquent, je devrais retourner le message indiquant que je ne peux pas répondre à la question et diriger l'utilisateur vers l'assistance.  
  
</think>  
  
Je ne peux pas répondre à votre question, merci de contacter l'assistance Soins2000.  
  
Adresse e-mail: maintenance@soins2000.com  
Numéro de téléphone: 04 68 42 78 90  
  
\boxed{Je ne peux pas répondre à votre question, merci de contacter l'assistance Soins2000.}