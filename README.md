# IN204-RayTracing

Vous avez plusieurs possibilité pour exécuter ce programme. Vous pouvez configurez les sources de lumière et lesssssssssssss objets présents dans la scène à l'aide d'un script de configuration. Nous vous en fournissons 1 comme exemple et vous laissons libre d'en créer d'autre sur le même modèle. Cette fonctionnalité est disponible uniquement sous windows et Linux

VOus pouvez également modifier directement le main() pour inclure à la main les différents objets et lumières. 

## Exécuter le programme avec un script de configuration

Pour exécuter le programme avec un script de configuration, exécutez simplement l'éxécutable a.out. Vous devrez alors choisir le fichier config1.csv dans votre arborescence de fichier. Une fois sélectionné, le programme s'éxécutera et le résultat sera disponible sous la forme du fichier out.ppm. Si aucune fenêtre vers votre arborescence de fichier ne s'ouvre merci d'utiliser la méthode suivante.

## Exécuter le programme directement depuis le main 

Pour exécuter le programme depuis le main, rendez vous dans la fonction main() du fichier oort.cpp et mettez en commentaire toutes les lignes entre #ifdef __linux__ et #endif. Libre à vous de rajouter à la main les objets et les sources de lumière en suivant le modèle présenté en commentaire du main().