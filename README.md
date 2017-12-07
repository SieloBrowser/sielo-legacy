
Platform | Build Status
------------ | -------------
Windows | Non disponible

[![Image officiel de Sielo](https://www.feldrise.com/Sielo/official-sielo.png)](https://www.feldrise.com/Sielo/sielo_setup_1.7.10.exe)
Sielo est un navigateur web que j'ai commencé à développer il y a 1 an et demi. Le navigateur a connu 2 refontes et la dernière date d'il y a quelques mois. Cette fois Sielo a été pensé sur une structure plus stable, plus facile à maintenir de façon à ce qu'il n'y ait plus besoin de refonte.
## Généralité, avancement et originalité
Sielo est aujourd'hui bien avancé. Il possède toutes les fonctionnalités de base d'un navigateur web (navigation par onglets, marque-page, historique, navigation privée, navigation sur le web (oui oui c'est possible :p), etc.) et même quelques fonctionnalités avancées telles qu'un bloqeur de publicités intégré, la gestion des proxy, gestion du cache, la possibilité d'enregistrer ses mots de passe (et de les chiffrer avec un super mot de passe). Sielo utilise le moteur web WebEngine qui vient avec Qt. Mais ce n'est pas tout, Sielo possède aussi des fonctionalités qui lui sont propres et originales : 
### Les espaces d'onglets
C'est **la** fonctionalité qui démarque le plus Sielo. Les espaces d'onglets vous permettent de découper votre fenêtre Sielo en plusieurs espaces, comme si vous aviez plusieurs fenêtre les unes à côté des autres, mais qui interagissent entre elles. Vous pouvez redimensionner les espaces d'onglets, les cacher, les rendre muets... Cela peut permettre, entre autre, d'avoir plusieures sessions, par exemple une session "réseaux sociaux", "musique" et "travail". Ainsi, on peut cacher la musique et la récupérer à tout moment.

Une petite illustration :
![GIF espaces d'onglets](https://www.feldrise.com/Sielo/tabs-space.gif)
### Le bouton flottant
Une autre fonctionalité propre à Sielo. Ce bouton peut-être déplacé n'importe où dans le navigateur pour vous donner accès aux contrôles principaux le plus facilement possible. Il peut aussi se transformer en barre d'outil latérale. Mais une image vaut bien mieux que 1000 mots (pour ceux qui n'aime pas le bouton flottant, vous pouvez utiliser la barre d'outils classique) :
![GIF bouton flottant](https://www.feldrise.com/Sielo/floating-button.gif)
### Des thèmes pour un pouvoir de personnalisation infini
Sielo supporte aussi les thèmes pour complètement personnaliser l'interface. Cela va du bouton flottant aux onglets, en passant par les icônes. Je suis d'ailleurs en train de rédiger un petit tutoriel pour apprendre à faire des thèmes pour Sielo. Ce n'est pas bien compliqué, il suffit de placer les icônes, de les répertorier dans un fichier "theme.index" et de faire un peu de CSS pour personnaliser certains objets tels que les onglets. Les thèmes peuvent d'ailleurs avoir un comportement différent en fonction du système d'exploitation si le créateur le souhaite.

Voici un exemple vous montrant le thème par défaut et sa version sombre :
![GIF des themes](https://www.feldrise.com/Sielo/themes.gif)
## Objectifs
Maintenant que Sielo est construit sur une structure stable, l'objectif est de rendre l'expérience utilisateur encore plus intuitive ! Il reste notamment beaucoup à faire au niveau de la gestion de la souris (par exemple, déplacer un onglet dans un autre espace d'onglets ou même le sortir de la fenêtre uniquement avec la souris). De plus, un système de plugins est déjà écrit, il reste donc à l'activer et créer des plugins. Je réfléchis aussi à la compatibilité avec les "web extension". 

Sielo s'est aussi beaucoup construit sur la base d'idées de personnes comme vous et de testeurs. C'est pour cela que je suis très à l'écoute de toutes vos remarques et de toutes vos idées. D'ailleurs, Sielo intégre aussi un système de commandes qui est une idée apportée par Rein Frost (je vous laisse essayer de rentrer "!easteregg" ou "!witcher [enable/disable]" dans la barre d'adresse :D).

Voilà pour la présentation, Sielo a aussi un [GitHub](https://github.com/Feldrise/SieloNavigateur) sur lequel vous retrouverez le code source. Nous avons aussi [une page où nous réunissons les idées](https://padlet.com/feldrise/j82miccj6zpb) (cette page est assez récente et encore un peu vide). Et enfin, lien que j'ai déjà mis deux fois, [l'open beta 4.1](https://www.feldrise.com/Sielo/sielo_setup_1.7.10.exe) qui est libre au téléchargement ! Pour les utilisateur de Debian ou dérivé, vous pouvez télécharger le paquet [à cette adresse](https://www.feldrise.com/Sielo/Linux/sielo-browser.deb) et pour les utilisateurs de ArchLinux vous trouverez Sielo sur l'AUR.

Je remercie aussi chaleureusement **hotaru70o**, **LavaPower** ([https://github.com/LavaPower](https://github.com/LavaPower)) et **ilearn32** pour leur soutien sans faille qui nous a permis d'arriver jusqu'à cette open beta ! Vraiment merci à eux