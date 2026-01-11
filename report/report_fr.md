# Introduction

Ce travail s'inscrit dans le cadre du programme EIT Digital et du cours High-Performance Computing for Data Science, ce projet vise à analyser en détail un algorithme d’optimisation, à en proposer une version séquentielle, puis à en développer une déclinaison parallélisée adaptée aux environnements HPC. Nous avons choisi d’étudier le Bat Algorithm, une métaheuristique bio-inspirée proposée par Xin-She Yang [[Yang, 2010]](https://arxiv.org/abs/1004.4170), dont le fonctionnement repose sur la modélisation du comportement d’écholocation des chauves-souris.
 L’objectif de cette première phase est de comprendre précisément son mécanisme interne, ses paramètres, et la manière dont il explore l’espace de recherche pour résoudre des problèmes d’optimisation dans lesquels les paramètres à ajuster appartiennent à un espace continu.

Les problèmes d’optimisation continue apparaissent dans de nombreux domaines scientifiques et industriels, et consistent à déterminer, parmi toutes les configurations possibles, celle qui minimise ou maximise une fonction objectif. La structure de cette fonction peut rendre la résolution non triviale, comme le soulignent Nocedal & Wright dans leur ouvrage de référence sur l’optimisation numérique [Numerical Optimization (2ᵉ éd.)](https://www.math.uci.edu/~qnie/Publications/NumericalOptimization.pdf).
 Plusieurs travaux de référence soulignent que les méthodes d’optimisation basées sur le gradient deviennent limitées lorsque la fonction objectif est **non dérivable**, **bruitée**, **non convexe** (donc susceptible de présenter plusieurs minima locaux), ou lorsque la **dimension du problème augmente**.  
Par exemple, Rios & Sahinidis (2013) expliquent que l’absence, l’imprécision ou l’inaccessibilité des dérivées rend les méthodes classiques « *of little or no use* » et montrent empiriquement que « *the ability [...] to obtain good solutions diminishes with increasing problem size* ».  
Ces observations, issues de leur revue approfondie de l’optimisation sans dérivées [Derivative-Free Optimization — A Review of Algorithms and Comparison of Software Implementations](https://scispace.com/pdf/derivative-free-optimization-a-review-of-algorithms-and-2qmveq3j89.pdf), motivent le recours à des approches métaheuristiques ou sans dérivées.  

En tant qu’approche stochastique, le Bat Algorithm intègre des mécanismes aléatoires lui permettant d’explorer efficacement l’espace de recherche. Son fonctionnement repose sur un équilibre entre exploration globale et exploitation locale, ce qui le rend adapté à l’optimisation de fonctions non linéaires ou comportant de multiples minima locaux.

#### Fonctionnement

Le Bat Algorithm repose sur la modélisation simplifiée du comportement d’écholocation des chauves-souris. Dans la nature, celles-ci émettent des impulsions ultrasonores dont la fréquence, l’intensité et le taux d’émission varient en fonction de la proximité d’une cible. Yang transpose ces mécanismes biologiques en concepts algorithmiques : chaque chauve-souris est représentée par une position dans l’espace de recherche, une vitesse associée, une fréquence d’exploration, une loudness (A) qui contrôle l’acceptation des nouvelles solutions, et un pulse rate (r) qui influence la transition entre exploration et exploitation. Au cours de l’itération, ces paramètres sont mis à jour de manière stochastique afin de simuler la manière dont une colonie se rapproche progressivement d’une zone optimale. L’algorithme séquentiel alterne ainsi génération de candidats, évaluation de la fonction objectif et ajustement adaptatif des paramètres pour guider la recherche vers la meilleure solution connue.

Les éléments suivants décrivent les principaux aspects de l’écholocation observés chez les microchauves-souris tels que présentés par Xin-She Yang [[Yang, 2010]](https://arxiv.org/abs/1004.4170).

Les microchauves-souris utilisent un sonar biologique appelé écholocation pour détecter leurs proies, identifier des obstacles et se repérer dans l’obscurité. Elles émettent de courtes impulsions ultrasonores et analysent l’écho renvoyé par les objets environnants. Ce mécanisme leur permet d’estimer avec précision la distance, la direction et la nature de la cible.

**Durée, cadence et fréquence des impulsions**

Chaque impulsion dure seulement quelques millisecondes (jusqu’à 8–10 ms) et présente une fréquence généralement comprise entre 25 kHz et 150 kHz (inaudible pour l’être humain, dont la plage auditive se situe entre environ 20 Hz et 20 kHz).  
En phase de recherche, les microbats émettent environ 10 à 20 impulsions par seconde, puis peuvent atteindre jusqu’à 200 impulsions par seconde lorsqu’elles se rapprochent d’une proie. Cette augmentation est rendue possible par leur système auditif, qui peut analyser un écho en 300 à 400 microsecondes (environ 0,0003 s).

**Intensité sonore et adaptation du signal**

Les impulsions peuvent atteindre 110 dB, un niveau sonore comparable à celui d’un marteau-piqueur. L’intensité n’est toutefois pas constante : lorsqu’elles sont éloignées de leur proie, les microbats émettent des signaux puissants afin d’augmenter leur portée de détection. À l’inverse, lorsqu’elles s’en approchent, elles réduisent progressivement l’intensité pour éviter la saturation auditive et améliorer la précision de la détection.

**Analyse de l’écho : distance, direction et caractéristiques de la cible**

À partir de l’écho réfléchi, les microchauves-souris sont capables d’extraire plusieurs types d’informations. Elles estiment d’abord la distance en mesurant le temps séparant l’émission et la réception du signal, selon la relation

$$
\text{distance} = v \cdot \frac{t}{2}
$$

où \(v\) est la vitesse du son et \(t\) le temps aller-retour de l’onde. Elles déterminent également la direction de la cible grâce à la très faible différence de temps d’arrivée du son entre leurs deux oreilles. Les variations d’intensité de l’écho leur permettent ensuite d’inférer certaines propriétés de l’objet, comme sa taille, sa forme ou sa texture. Enfin, les microbats sont capables de détecter le mouvement d’un insecte en analysant les variations de fréquence dues à l’effet Doppler, une fréquence plus élevée indiquant que la cible s’approche, et une fréquence plus basse qu’elle s’éloigne.

En combinant ces différentes informations, elles construisent une représentation tridimensionnelle de leur environnement et peuvent ainsi éviter des obstacles extrêmement fins, parfois de l’ordre du millimètre.



Le Bat Algorithm repose sur une modélisation simplifiée du comportement observé chez les microbats. Xin-She Yang formalise ce comportement à travers trois hypothèses principales afin d’en tirer un modèle mathématiquement exploitable pour l'optimisation [[Yang, 2010]](https://arxiv.org/abs/1004.4170) :

1. chaque bat virtuelle dispose d’un mécanisme d’évaluation de la qualité d’une position, analogue à la capacité des microbats à identifier une proie via l’analyse de l’écho ; cette évaluation est assurée dans l’algorithme par le calcul de la fonction objectif

2. les bats ajustent leur fréquence, leur vitesse et leur taux d’émission de pulses en fonction de leur proximité d’une solution potentiellement optimale, en analogie avec la modulation des impulsions observée lors de la chasse ;

3. la loudness diminue et le pulse rate augmente au fil des itérations, ce qui modélise le passage d’une phase d’exploration globale à une recherche plus locale autour des meilleures solutions.

Dans ce modèle, chaque bat est représentée par un ensemble de paramètres :

- une position \(x_i\), correspondant à une solution candidate ;
- une vitesse \(v_i\) ;
- une fréquence \(f_i\) ;
- une loudness \(A_i\) ;
- un pulse rate \(r_i\).

Les équations de mise à jour du Bat Algorithm décrites par Yang sont les suivantes.  
La fréquence est mise à jour selon :

**(11.1)** \( f_i = f_{\min} + (f_{\max} - f_{\min}) \, \beta \),

où \( \beta \in [0,1] \) est un nombre aléatoire tiré d’une loi uniforme.

La vitesse est ensuite modifiée en fonction de la position actuelle de la chauve-souris par rapport à la meilleure solution courante \(x_*\) :

**(11.2)** \( v_i^{t+1} = v_i^{t} + (x_i^{t} - x_*) \, f_i \).

La nouvelle position est enfin obtenue par :

**(11.3)** \( x_i^{t+1} = x_i^{t} + v_i^{t+1} \).

Ces trois équations définissent le *global update*, responsable de la phase d’exploration du domaine de recherche.

Pour la recherche locale, Yang introduit un déplacement aléatoire contrôlé par la loudness moyenne \(A^{(t)}\), ce qui donne :

**(11.4)** \( x_{\text{new}} = x_{\text{old}} + \varepsilon \, A^{(t)} \),

où \( \varepsilon \in [-1, 1] \) est un terme aléatoire. D’un point de vue implémentation, comme le souligne Yang (*“From the implementation point of view, it is better to provide a scaling parameter to control the step size”*), cette équation est réécrite en utilisant un bruit gaussien associé à un facteur d’échelle \(\sigma\).


**(11.5)** \( x_{\text{new}} = x_{\text{old}} + \sigma \, \varepsilon_t \, A^{(t)} \),

où \( \varepsilon_t \) est tiré d’une loi normale \(N(0,1)\).  N(0,1)\) et \(\sigma = 0.1\) dans la démonstration fournie par Yang.  
Cette étape constitue le *local search*, qui permet d’explorer le voisinage des meilleures solutions identifiées.

La dynamique d’adaptation de la loudness et du pulse rate est modélisée par :

\[
A_i^{t+1} = \alpha A_i^{t}, \qquad
r_i^{t+1} = r_0 \bigl(1 - e^{-\gamma t}\bigr),
\]

avec \(0 < \alpha < 1\), \(\gamma > 0\) et \(r_0\) un paramètre initial. La loudness décroît donc au cours des itérations, tandis que le pulse rate se rapproche progressivement de \(r_0\).

Les mises à jour de \(f_i\) et \(v_i\) assurent l’exploration globale du domaine en modifiant l’amplitude et la direction des déplacements. La phase d’exploitation locale consiste en un déplacement aléatoire autour de la solution courante de la bat sélectionnée, avec une amplitude contrôlée par la loudness \(A_i\). L’évolution de \(A_i\) et du pulse rate \(r_i\) assure la transition entre exploration et exploitation.

**Algorithm 11.1: Bat Algorithm**

**Data:** Objective function \( f(x) \)  
**Result:** Best or optimal solution

1. Initialize the bat population \( x_i \) and velocities \( v_i \)  \((i = 1, 2, ..., n)\);
2. Initialize frequencies \( f_i \), pulse rates \( r_i \), and loudness \( A_i \);

3. **while** \( t < \text{Max number of iterations} \) **do**
    1. Generate new solutions by adjusting frequency;
    2. Update velocities and positions (solutions) using Eqs. (11.1)–(11.3);
    3. **if** \( \text{rand} > r_i \) **then**
        - Select a solution among the best solutions;
        - Generate a local solution around the selected best solution;
      **end if**
    4. Generate a new solution by flying randomly;
    5. **if** \( \text{rand} < A_i \) **and** \( f(x_i) < f(x_*) \) **then**
        - Accept the new solution;
        - Increase \( r_i \) and reduce \( A_i \);
      **end if**
    6. Rank the bats and find the current best \( x_* \);
4. **end while**


L’algorithme commence par initialiser une population de chauves-souris de taille \(n\), chacune définie par une position \(x_i\), une vitesse \(v_i\) et trois paramètres internes : la fréquence \(f_i\), le taux d’impulsion \(r_i\) et la loudness \(A_i\). À chaque itération, dont la limite est fixée par Max number of iterations, de nouvelles solutions sont générées en ajustant la fréquence, ce qui met à jour la vitesse puis la position de chaque chauve-souris.

À cette étape, deux solutions candidates sont générées : une solution locale construite autour d’une des meilleures solutions actuelles (si la condition \( \text{rand} > r_i \), où rand désigne un nombre aléatoire uniformément distribué dans \([0,1]\), est vérifiée), puis une solution globale obtenue par un mouvement aléatoire (« flying randomly »). Les deux solutions sont évaluées, et la meilleure est retenue pour le test d’acceptation basé sur la loudness \(A_i\).

Ainsi, à chaque itération, une seule solution finale peut être acceptée pour mettre à jour la position \(x_i\). En cas d’amélioration, les paramètres \(A_i\) et \(r_i\) sont mis à jour, et la meilleure solution globale \(x^*\) est actualisée.


**Limites conceptuelles concernant la fréquence \(f_i\) et le pulse rate \(r_i\)**

La lecture approfondie du papier original de Yang nous a conduit à nous interroger sur l'interprétation et l’utilité réelle de certains paramètres du Bat Algorithm, en particulier la fréquence \(f_i\) et le pulse rate \(r_i\), dont le rôle théorique présente des incohérences ou des contradictions avec le comportement biologique qu’ils sont censés modéliser.

1. Interprétation discutable de la fréquence \(f_i\)

Yang relie la fréquence \(f_i\) à la relation physique \(\lambda_i f_i = v_{\text{sound}}\), puis fait varier \(f_i\) pour influencer la mise à jour de la vitesse. Or, dans la nature :

- les microchauves-souris utilisent des fréquences basses pour la recherche à longue portée,
- et des fréquences élevées pour affiner la localisation d’une proie.

Dans l’algorithme, l’effet est inversé : une fréquence élevée entraîne un déplacement de grande amplitude, ce qui correspond à un comportement d’exploration globale plutôt qu’à une localisation fine. De plus, le rôle de \(f_i\) dans les équations pourrait être rempli par n’importe quel coefficient aléatoire ; sa présence semble donc davantage motivée par la métaphore biologique que par une nécessité algorithmique.

2. Condition ambiguë sur le pulse rate \(r_i\)

Le modèle définit \(r_i\) comme un paramètre qui augmente lorsque la bat se rapproche de la solution, ce qui est cohérent avec le comportement réel des microbats qui émettent davantage de pulses près de leur cible. Pourtant, le pseudo-code utilise la condition :

if rand > r_i,

pour déclencher la recherche locale. Or cette condition implique :

- début de l’algorithme : \(r_i\) faible → forte probabilité de recherche locale,
- proximité de la solution : \(r_i\) élevé → faible probabilité de recherche locale.

Ce comportement est donc inverse de l’interprétation biologique, où l’on s’attendrait au contraire à renforcer la recherche locale lorsque la bat s’approche de la cible. Une condition plus cohérente serait 

if rand < r_i,

qui permettrait d’augmenter la fréquence de la recherche locale à mesure que \(r_i\) croît.

Ce sont ces observations qui ont motivé nos recherches sur les incohérences conceptuelles du Bat Algorithm. Celles-ci nous ont conduit à identifier une possible erreur importante dans la mise à jour de la vitesse proposée dans le papier original. En effet, la mise à jour de la vitesse y est formulée comme :

\[
v_i^t = v_i^{t-1} + (x_i^t - x^*)\, f_i,
\]

Or, comme l’a souligné une discussion sur le forum [Mathematics Stack Exchange](https://math.stackexchange.com/questions/3386466/error-in-official-paper-about-bat-algorithm), cette formulation est problématique pour au moins deux raisons : d’une part, l’utilisation de \(x_i^t\) pour calculer \(v_i^t\) crée une dépendance circulaire, puisque la position \(x_i^t\) dépend elle-même de la vitesse. D’autre part, le terme \((x_i^t - x^*)\) oriente la mise à jour dans la direction opposée de la meilleure solution connue, ce qui est contraire au principe même de convergence vers un optimum.

La discussion souligne cependant un point important : dans une implémentation MATLAB publiée par Yang lui-même (dans *Nature-Inspired Optimization Algorithms*, édition 2014), la fréquence est calculée comme :

\[
f_i = f_{\min} + (f_{\min} - f_{\max}) \cdot \text{rand},
\]

soit l’inverse de la formule donnée dans l’article. Comme \(f_{\min} = 0\), cette inversion change le signe de \(f_i\). Dès lors, si l’on utilise la formule de vitesse d’origine :

\[
v_i^t = v_i^{t-1} + (x_{\text{current}} - x_{\text{best}})\, f_i,
\]

l’inversion du signe de \(f_i\) conduit à :

\[
v_i^t = v_i^{t-1} + (x_{\text{best}} - x_{\text{current}})\, f_i,
\]

ce qui rétablit, par effet de compensation, une mise à jour orientée vers la meilleure solution.

Autrement dit, l’erreur dans la définition de la fréquence corrige l’erreur dans la mise à jour de la vitesse. À notre niveau, il nous est difficile de savoir si cette compensation est intentionnelle, accidentelle ou simplement issue d’une divergence entre les différentes versions des publications et ouvrages de Yang. Nous constatons simplement que cette incohérence complique l’interprétation de l’algorithme  et qu’il n’existe pas, dans la littérature de Yang, de position clairement stabilisée sur ce point.


**Implémentation séquentielle : inspirations, choix et différences avec le code MATLAB**

Pour implémenter la version séquentielle du Bat Algorithm, nous nous sommes appuyés sur plusieurs sources, et notamment sur l’implémentation MATLAB proposée par Xin-She Yang dans *Nature-Inspired Optimization Algorithms* (Elsevier, 2014). Cette version du code constitue une référence pratique, car elle explicite non seulement la structure générale de l’algorithme, mais également l’ordre des opérations et l’usage concret des paramètres (mise à jour de la fréquence, gestion de la loudness, conditions d’acceptation, etc.).

Nous avons repris plusieurs éléments de cette implémentation, en particulier :

- la structuration générale de la boucle principale (mise à jour fréquence → vitesse → position → éventuel local search),
- l’utilisation d’un tableau d'individus contenant pour chaque bat sa position, sa vitesse, sa fréquence, sa loudness et son pulse rate,
- la présence d’un local random walk déclenché par un test aléatoire,
- et la mise à jour progressive des paramètres \(A_i\) et \(r_i\) suivant les formules données par Yang.

Cependant, nous avons également pris plusieurs libertés par rapport au code MATLAB, soit pour nous rapprocher davantage du pseudo-code du papier original, soit pour corriger certaines incohérences repérées lors de notre analyse théorique.

1. Correction des équations de fréquence et de vitesse

Comme expliqué dans la section précédente, le code MATLAB utilise une inversion  
\(f = f_{\min} + (f_{\min} - f_{\max}) \cdot \text{rand}\)  
qui change le signe de la fréquence. Cette inversion compense involontairement une erreur présente dans la mise à jour de la vitesse, en rétablissant la direction correcte du mouvement.

Dans notre implémentation, nous avons choisi de corriger explicitement ces incohérences :

- la fréquence est calculée conformément à la formule souhaitée dans le papier,
- la mise à jour de la vitesse utilise une expression cohérente avec l'idée d'attraction vers la meilleure solution :
\[
v_i^t = v_i^{t-1} + (x^* - x_i^{t-1})\, f_i.
\]

Cependant, nous n’avons pas modifié la condition de déclenchement du local search :

`if rand > r_i`

Même si plusieurs analyses issues de la littérature secondaire (notamment des discussions techniques en ligne) soulignent que cette condition est conceptuellement contre-intuitive, Yang utilise systématiquement cette formulation dans son article original, dans ses ouvrages et dans son code MATLAB. Nous n’avons trouvé aucune version officielle où la condition serait inversée.

Dans notre implémentation, ce choix reste cohérent avec notre paramétrisation, car nous initialisons \(r_0 = 1\), ce qui fait décroître \(r_i\) au cours des itérations. Dans cette configuration, la condition `rand > r_i` implique :

- en début de recherche : \(r_i\) élevé → local search rare,
- en fin de recherche : \(r_i\) plus faible → local search plus fréquent.

Ce comportement, bien qu’opposé à l’interprétation biologique (où le pulse rate augmente près de la proie), reste compatible avec une logique d’exploration initiale puis d’exploitation progressive, ce qui correspond au comportement attendu d’un algorithme métaheuristique.



Le pseudo-code présenté dans l’article original de Xin-She Yang semble suggérer trois mécanismes distincts de génération de nouvelles solutions :

- une mise à jour globale (fréquence, vitesse, position),
- une recherche locale conditionnelle,
- un “vol aléatoire” supplémentaire (“flying randomly”).

Cependant, cette structure n’apparaît pas dans l’implémentation MATLAB fournie par Yang dans *Nature-Inspired Optimization Algorithms* (2014). Dans ce code, seules deux solutions potentiellement différentes sont effectivement générées pour une même bat au cours d’une itération :

- une solution globale, obtenue après la mise à jour de la fréquence, de la vitesse et de la position :  
  \[
  S_i = x_i + v_i
  \]

- une solution locale, qui remplace la précédente uniquement si la condition `rand > r` est vérifiée :  
  \[
  S_i = x_{\text{best}} + 0.1 \cdot A \cdot \mathcal{N}(0, I)
  \]

Le code MATLAB n’évalue donc qu’une seule solution par itération, puisque la solution globale est éventuellement écrasée par la solution locale, sans comparaison explicite entre les deux.

Dans notre implémentation séquentielle en C, nous avons choisi d’adopter une approche légèrement différente, plus proche de l’esprit du pseudo-code. Nous conservons en effet deux candidats distincts :

- une solution issue de la mise à jour globale,
- une solution locale (si elle est générée),

puis nous évaluons les deux et retenons la meilleure.

Cette distinction explicite améliore la lisibilité algorithmique et clarifie le rôle respectif du mouvement global et de la recherche locale, tout en restant cohérente avec le fonctionnement général décrit dans l’article de Yang.

**Choix d’implémentation**

L’implémentation séquentielle du Bat Algorithm que nous proposons s’appuie directement sur les éléments fournis dans l’article original de Xin-She Yang (2010), l’implémentation MATLAB publiée dans Nature-Inspired Optimization Algorithms (Elsevier, 2014), ainsi que sur la seconde édition de l’ouvrage (Elsevier, 2020), qui explicite notamment les équations internes et le comportement des paramètres. Notre version en C suit ces sources lorsqu’elles sont précises, et adopte des choix complémentaires lorsque Yang ne détaille pas certains aspects pratiques.

Les équations fondamentales du Bat Algorithm sont présentées dans le livre 2020 sous la forme  
“\(f_i = f_{\min} + (f_{\max} - f_{\min}) \, \beta\) (11.1)”,  
“\(v_i^{t+1} = v_i^t + (x_i^t - x)\, f_i\) (11.2)”,  
“\(x_i^{t+1} = x_i^t + v_i^{t+1}\) (11.3)”.

Nous reprenons directement les équations (11.1) et (11.3). En revanche, pour la mise à jour de la vitesse, nous utilisons la forme orientée vers la meilleure solution, c’est-à-dire \((x - x_i^t)\), conformément aux choix que nous avons déjà explicités et justifiés plus tôt concernant les incohérences de signe présentes dans les ouvrages de Yang.

**Choix de la fréquence** 

Dans les ouvrages de Yang, la fréquence maximale est décrite comme devant rester « de l’ordre de 1 » (“f_max = O(1)”, Yang 2020), sans valeur imposée. Le code MATLAB utilise \(f_{\max} = 2\) à titre d’exemple illustratif. On pense qu'il s’explique par la fonction objectif retenue dans son exemple :

\[
Fun(x) = \sum (x - 2)^2,
\]

Dans cet exemple, l’initialisation uniforme dans un domaine large \([-10,10]^5\) produit le plus souvent des solutions éloignées de l’optimum \(x^* = (2,\dots,2)\). Une fréquence maximale plus élevée augmente alors l’amplitude des déplacements initiaux, ce qui facilite l’exploration du domaine et permet d’atteindre plus rapidement la région contenant l’optimum.

Dans notre cas, l’objectif à optimiser est la fonction :

\[
f(x) = 10 - \sum x_d^2,
\]

qui présente un maximum unique au centre du domaine. Une fréquence excessive produirait des déplacements trop brusques et des oscillations inutiles autour de l’optimum, tandis qu’une fréquence trop faible ralentirait la convergence.

Pour cette raison, nous avons retenu une valeur plus conservatrice :

\[
f_{\max} = 1,
\]

qui reste compatible avec les recommandations générales de Yang tout en offrant une dynamique plus stable dans un domaine borné et symétrique comme \([−5,5]\). Ce choix permet des déplacements suffisamment rapides en début d’exploration, sans compromettre la précision en fin de convergence.

Une alternative intéressante, que nous n’avons pas retenue ici, serait d’utiliser un \(f_{\max}\) adaptatif, par exemple décroissant au cours du temps. Cela offrirait une exploration large au début (fréquence élevée), puis un affinement progressif autour du maximum à mesure que l’algorithme avance. On pense que cette stratégie pourrait améliorer l’équilibre entre exploration et exploitation, aussi bien dans des cas simples comme le nôtre que pour des fonctions plus complexes.



Pour la mise à jour de la loudness et du pulse rate, nous suivons l’équation (11.6) du livre 2020 :  
\(A_i^{t+1} = \alpha A_i^{t},\quad r_i^{t+1} = r_{0i} \left[ 1 - \exp(-\gamma t) \right]\).  
L’auteur ajoute : *“We have used α = 0.97 and γ = 0.1 in our simulations”*. Nous reprenons donc ces valeurs, qui entraînent une décroissance de la loudness et une augmentation progressive du pulse rate au cours des itérations.  
Le choix de \(A_0 = 1\) est également légitimé par le livre : *“For simplicity, we can also use A₀ = 1”*, tandis que pour \(r_0\), Yang indique qu’il peut être pris dans l’intervalle \((0, 1]\). Nous fixons \(R_0 = 1\) conformément aux recommandations de Yang et à la valeur utilisée dans son implémentation MATLAB.

Concernant la taille de la population, le code MATLAB spécifie : *“Population size, typically 10 to 50”*.  
Le livre 2020 confirme cette plage : *“we use n = 10 to 50 virtual bats”*, tandis que le papier de 2010 note que *“sizes between 25 and 50 usually produce better results”*.  
Notre choix de \(NBATS = 30\) correspond donc à la valeur centrale de l’intervalle recommandé.
De même, nous reprenons la valeur \(MAX\_ITERS = 1000\) utilisée dans le code MATLAB. (*“t\_max = 1000”*).

Dans ses exemples, Yang utilise généralement des bornes –10 et +10 : MATLAB fixe  
\(\text{Lb} = -10 * \text{ones}(1,d),\quad \text{Ub} = 10 * \text{ones}(1,d)\),  
et le livre 2020 réitère ce choix dans l’équation (11.9). Il insiste cependant sur le fait qu’il s’agit d’un choix de commodité : *“it is more convenient to impose some simple bounds in practice”*.  
Comme notre fonction objectif est centrée et définie sur un domaine plus restreint, nous ajustons ces bornes à \([-5,5]\), ce qui limite l’exploration inutile et reste adapté à notre fonction objectif.

Pour l’initialisation des solutions, Yang utilise une distribution uniforme dans MATLAB : “Sol(i,:) = Lb + (Ub - Lb) * rand(1,d)”. Dans le livre 2020, il ne prescrit aucune distribution particulière pour cette étape et indique simplement que “Initially, each bat is randomly assigned…”. Le papier de 2010 va dans le même sens en précisant que “Initially, each bat is randomly assigned a frequency which is drawn uniformly from [fmin, fmax]”, ce qui confirme que l’uniforme constitue le choix standard pour l’initialisation. Nous adoptons donc également une initialisation uniforme, adaptée à notre domaine \([-5,5]\), ce qui reste conforme au cadre théorique et cohérent avec l’implémentation MATLAB.

Les ouvrages de Yang ne recommandent aucune structure de données particulière et MATLAB utilise des tableaux séparés. Nous avons choisi de regrouper les informations d’une chauve-souris dans une structure C

Pour la recherche locale, nous utilisons une perturbation gaussienne, comme dans le code MATLAB de Yang où la mise à jour est donnée par “S(i,:) = best + 0.1 · randn(1,d) · A”. Le livre 2020 confirme ce choix en indiquant que “εᵗ is drawn from a Gaussian normal distribution N(0,1)”. Une loi normale génère surtout de petits déplacements autour du best, tout en permettant quelques variations plus larges, ce qui correspond bien au rôle du local search.


Enfin, le livre 2020 distingue explicitement deux mécanismes :  
(1) le *global update* (Eqs. 11.1–11.3), qui correspond au déplacement dirigé par la fréquence et la vitesse,  
et  
(2) le *local search* (Eqs. 11.4–11.5), qui consiste en une perturbation contrôlée autour d’une solution de référence.

Dans le code MATLAB, cependant, le résultat du *global update* est directement remplacé par celui du *local search* lorsque la condition `rand > r` est satisfaite, sans comparaison entre les deux solutions. Par ailleurs, alors que MATLAB utilise une loudness unique et globale, nous introduisons la loudness moyenne telle que proposée dans l’équation (11.5), afin de nous rapprocher davantage du pseudo-code du livre, où le *local search* s’appuie sur un paramètre reflétant l’état global de la population.

Dans notre implémentation, nous traitons ces deux mécanismes de façon distincte : lorsqu’un *local search* est déclenché, nous générons d’un côté la solution issue du *global update* et, de l’autre, la solution issue du *local search*. Les deux sont évaluées, puis seule la meilleure est transmise à la règle d’acceptation. Cette démarche clarifie le rôle respectif du *global update* et du *local search*, conformément à leur présentation comme deux sources de variation séparées dans le livre et dans le papier original.













---

A FAIRE 
- travailler les citations (pas clair)
- inserer le code sequentiel



-----

# Parallel design

#### Analyse du parallélisme dans le Bat Algorithm

Le **Bat Algorithm** est un algorithme métaheuristique à base de population, dans lequel un ensemble d’individus (les chauves-souris) évolue simultanément dans l’espace de recherche.  
À chaque itération, chaque individu met à jour sa position, sa vitesse et ses paramètres internes, puis évalue la fonction objectif associée à sa position courante.

L’analyse de l’algorithme séquentiel montre que la majorité de ces opérations sont **indépendantes pour chaque individu**. En particulier, les étapes suivantes ne présentent pas de dépendances directes entre chauves-souris :

- la mise à jour de la fréquence, de la vitesse et de la position ;
- l’évaluation de la fonction objectif ;
- la mise à jour des paramètres de *loudness* et de *pulse rate*.

Ces opérations constituent la partie la plus coûteuse de l’algorithme en temps de calcul et se prêtent naturellement à une **parallélisation par données**, où différents individus peuvent être traités simultanément sur des unités de calcul distinctes.

Cependant, l’algorithme introduit une **dépendance globale** à travers la meilleure solution trouvée jusqu’à présent, notée \( x_{\text{best}} \). Cette information est utilisée par l’ensemble des individus pour orienter la recherche locale. Par conséquent, bien que l’algorithme soit majoritairement parallélisable, une **synchronisation globale** est nécessaire afin de garantir la cohérence de cette information entre les différentes unités de calcul.

#### Modèle master–worker

Une première stratégie de parallélisation du Bat Algorithm repose sur un **modèle master-worker**, tel que proposé par Noor et al. dans *[Performance of Parallel Distributed Bat Algorithm using MPI on a PC Cluster](https://papers.ssrn.com/sol3/papers.cfm?abstract_id=3559955)*. Cette approche apparaît, de notre point de vue, comme la solution la plus naturelle pour paralléliser le Bat Algorithm dans un environnement MPI distribué.

##### Principe général

Dans ce modèle, la population globale de chauves-souris est répartie entre plusieurs processus travailleurs (workers), tandis qu’un processus distinct joue le rôle de maître (master). Chaque worker exécute localement les étapes du Bat Algorithm séquentiel sur une sous-population, tandis que le master est responsable de la synchronisation globale et de la sélection de la meilleure solution à l’échelle de l’ensemble de la population.

Les auteurs décrivent ce fonctionnement à l’aide d’un pseudo-code intitulé **Parallel-Distributed Bat Algorithm – Master-Worker (PDBA-MW)**, qui met en évidence les différentes responsabilités des workers et du master.

##### Parallel-Distributed Bat Algorithm – Master–Worker (PDBA-MW)

**Initialization**
- Initialize `every_send_time` to 50

**Main Loop**
- For `iteration = 1` to `Max_iteration`:

  - **If (Worker Node)** then:
    - For `i = 1` to `newpopsize`:
      - *Note*: `newpopsize = N / total_workers`
      - Perform Steps 1 to 5 of the sequential algorithm
    - End For

    - If (`iteration == every_send_time`) then:
      - Each `worker_k` sends its best candidate solution and corresponding fitness to the Master
      - Receive the best solution and fitness from the Master node
    - End If

  - **Else if (Master Node)** then:
    - For `k = 1` to `Total_Workers`:
      1. Receive `best_k` from each worker
      2. Compare and choose the best among all received solutions
      3. Send the best solution to each worker
    - End For

- End For

**Termination Condition**
- The process is terminated when the desired accuracy is achieved  
  or when the maximum number of iterations has been reached.

##### Initialisation et paramétrage de la communication

Le pseudo-code introduit un paramètre `every_send_time`, initialisé à une valeur fixée (par exemple 50), qui contrôle la fréquence de communication entre les workers et le master. Ce paramètre vise à espacer les synchronisations entre les processus, afin de limiter des communications trop fréquentes susceptibles d’augmenter le coût global de l’exécution.

##### Traitement parallèle côté workers

À chaque itération, chaque worker exécute le Bat Algorithm sur une sous-population de taille  
`newpopsize = N / total_workers`, où `N` représente la taille totale de la population. Comme indiqué dans le pseudo-code, chaque worker applique localement les étapes principales de l’algorithme séquentiel (mise à jour des positions et des vitesses, génération de solutions candidates, évaluation de la fonction objectif et mise à jour des paramètres internes).

Les opérations réalisées durant cette phase sont effectuées localement par chaque worker, sans échange entre processus. Le traitement indépendant des sous-populations permet ainsi de répartir le calcul sur plusieurs nœuds du cluster.

##### Synchronisation périodique avec le master

Lorsque la condition `iteration == every_send_time` est satisfaite, chaque worker identifie sa meilleure solution locale et l’envoie au processus maître, accompagnée de la valeur de fitness correspondante. Dans l’implémentation proposée par les auteurs, cette étape repose sur des communications point-à-point entre les workers et le master, réalisées à l’aide des primitives MPI `Send/Recv`.

Le master reçoit ensuite les meilleures solutions locales envoyées par les workers, les compare et en extrait la meilleure solution globale. Celle-ci est ensuite diffusée à l’ensemble des workers et utilisée comme \( x_{\text{best}} \) lors des itérations suivantes de l’algorithme.

##### Analyse critique du modèle


Le modèle master–worker adopté introduit une organisation claire et facile à mettre en œuvre, mais repose sur une **synchronisation globale centralisée** autour du processus maître. À chaque phase de synchronisation, le master doit recevoir les meilleures solutions locales de l’ensemble des workers, effectuer une comparaison globale, puis redistribuer la meilleure solution \( x_{\text{best}} \) à tous les processus. Cette organisation implique que le volume de communications géré par le master augmente **linéairement avec le nombre de workers**.

Lorsque le nombre de workers devient important, le processus maître peut ainsi devenir un **point de congestion potentiel**, en particulier du fait de l’utilisation de communications MPI bloquantes de type `Send` et `Recv`, telles qu’indiquées par les auteurs. Dans ce contexte, le temps passé en communication peut progressivement limiter les gains liés à la parallélisation, même si cette situation n’est pas étudiée explicitement dans les expérimentations présentées.

Les résultats expérimentaux montrent néanmoins une amélioration du temps d’exécution et un **speed-up croissant** lorsque la taille du cluster augmente, notamment pour des populations de grande taille. Toutefois, les courbes d’efficacité indiquent également une diminution progressive de l’efficacité lorsque le nombre de nœuds augmente, ce qui peut être interprété comme un effet combiné du coût de communication et de la charge imposée au processus maître.

Par ailleurs, le choix d’un paramètre fixe pour `every_send_time` permet de réduire la fréquence des synchronisations, mais introduit une **dépendance au paramétrage** dont l’impact sur la convergence de l’algorithme n’est pas analysé en détail. Un espacement trop important des synchronisations peut retarder la diffusion de \( x_{\text{best}} \), tandis qu’une synchronisation trop fréquente peut accentuer le coût des communications, en particulier dans un cadre distribué.

Ainsi le modèle master-worker permet une parallélisation directe du Bat Algorithm et assure la diffusion de la meilleure solution globale à l’ensemble des processus. Cependant, cette approche repose sur une synchronisation centralisée qui peut devenir limitante lorsque le nombre de workers augmente. La charge de communication supportée par le processus maître croît alors avec la taille du cluster, ce qui peut affecter la scalabilité globale de l’algorithme.

Ces limitations ont conduit à l’étude de stratégies alternatives visant à réduire la centralisation des communications. Parmi celles-ci, les modèles basés sur des sous-populations évoluant de manière largement indépendante proposent une organisation plus distribuée, susceptible de diminuer les coûts de synchronisation et d’améliorer la scalabilité sur des architectures parallèles de plus grande taille.


#### Modèle par sous-populations indépendantes (Island Model)

Le modèle par sous-populations indépendantes, également appelé *Island Model*, repose sur une division de la population globale en plusieurs groupes évoluant de manière autonome. Cette stratégie est notamment adoptée par Tsai et al., qui proposent une version parallélisée du Bat Algorithm intégrant une stratégie de communication périodique entre sous-populations.

##### Les étapes du modèle par sous-populations

##### Initialisation

La population initiale de chauves-souris est générée puis divisée en \( G \) sous-populations. Chaque sous-groupe est initialisé indépendamment à l’aide du Bat Algorithm.

Un ensemble d’itérations \( R \) est défini afin de déterminer les moments où la stratégie de communication sera appliquée. La position d’une chauve-souris est notée \( X_{ij}^t \), où \( i \) désigne l’individu au sein du sous-groupe \( j \), et \( t \) l’itération courante. L’algorithme démarre avec \( t = 1 \).

##### Évaluation

À chaque itération, la fonction objectif \( f(X_{ij}^t) \) est évaluée pour toutes les chauves-souris de chaque sous-population. Cette étape est réalisée localement, sans échange d’information entre les groupes.

##### Mise à jour

Les vitesses et les positions des chauves-souris sont mises à jour à l’aide des équations du Bat Algorithm original. Chaque sous-population évolue de manière indépendante et conserve sa propre meilleure solution locale, sans connaissance directe des solutions trouvées par les autres groupes.

##### Stratégie de communication

La stratégie de communication est activée uniquement à certaines itérations définies par l’ensemble  
\( R = \{ R_1, 2R_1, 3R_1, \dots \} \).

Lorsqu’une itération de communication est atteinte, chaque sous-groupe sélectionne les \( k \) meilleures chauves-souris selon la valeur de fitness. Ces solutions sont ensuite copiées vers le sous-groupe voisin \( g_{(p+1) \bmod G} \), où elles remplacent le même nombre de solutions les moins performantes.

Ce mécanisme permet une diffusion progressive de l’information entre sous-populations, sans synchronisation globale ni processus centralisé.

##### Terminaison

Les étapes précédentes sont répétées jusqu’à l’atteinte d’un critère d’arrêt, tel qu’un nombre maximal d’itérations ou l’atteinte d’une valeur cible de la fonction objectif. La meilleure valeur obtenue ainsi que la position correspondante sont alors enregistrées comme solution finale.

##### Limites du modèle par sous-populations indépendantes

Le modèle par sous-populations indépendantes proposé par Tsai et al. permet de réduire la fréquence des communications et d’éviter une synchronisation globale centralisée. Toutefois, cette organisation introduit certaines limites du point de vue de la dynamique algorithmique.

En particulier, la meilleure solution globale n’est pas partagée en continu entre l’ensemble des individus. Chaque sous-population évolue avec sa propre meilleure solution locale, et l’information issue des autres groupes n’est intégrée que de manière périodique via les migrations. Cette diffusion retardée peut ralentir la convergence, notamment lorsque certaines sous-populations explorent des régions peu prometteuses de l’espace de recherche.

Par ailleurs, cette stratégie repose sur plusieurs paramètres supplémentaires, tels que le nombre de sous-populations, la fréquence des communications et le nombre de solutions migrées. Le choix de ces paramètres influence fortement l’équilibre entre exploration et exploitation, sans que leur impact soit analysé de manière approfondie.

Ainsi, bien que ce modèle favorise la diversité et la scalabilité, il s’éloigne du comportement du Bat Algorithm séquentiel et introduit des compromis importants entre qualité de convergence, rapidité de diffusion de l’information et complexité de paramétrage.

Dans la suite de ce travail, nous proposons une approche visant à tirer parti de la parallélisation tout en limitant les modifications apportées au fonctionnement original de l’algorithme.