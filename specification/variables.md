## Variables

Les variables sont des emplacement mémoire désignées par un nom et pouvant contenir une valeur. Certaines variables ne peuvent pas être modifiées, on les appelle *constantes*.

### Nom

Le nom d'une variable est constitué d'une suite de caractères, au minimum un. Les caractères acceptés sont `_`, `a-z`, `A-Z`, `0-9` et tous les caractères unicode au dessus de `0xc0` (`À`). Le premier caractère ne peut pas être un chiffre. Les exemples suivant sont des noms de variable valides :
```js
var x; var monÉtat2000; var snake_case; var 你好世界; var 🐼;
```

### Valeur et type

Une variable peut contenir une valeur. Si une variable n'a pas de valeur, elle est dite *vide* et a le type **`jamais`**, qui indique l'absence de valeur. Si elle a une valeur, elle a le type de cette valeur.

### Constantes

Une variable peut être déclarée comme constante. On parle dans ce cas de *variable constante* même si c'est un oxymore, ou simplement *constante*. La valeur d'une constante lui ai donné à sa création et ne peut plus du tout être changée par la suite. On dit qu'elle sont *immutables* ou *imuables*.

### Globales

Les variables globales sont des variables persistantes d'une éxécution du programme à une autre. Elles sont également accessibles partout dans le programme.

#### Déclaration d'une globale

Une variable globale se déclare comme une variable normale, mais en utilisant le mot-clé **`global`** au lieu de **`var`** :

```php
global exemple = 1234
```

#### Déclaration dans le bloc principal

Une globale doit être déclarée dans le bloc principal du programme, il est impossible de le faire dans une fonction, ou dans un sous-bloc :

```php
{
    global x // Erreur : impossible de déclarer une globale ailleurs que dans le bloc principal
}
function() {
    global y // Erreur : impossible de déclarer une globale ailleurs que dans le bloc principal
}
```

### Constantes globales

Une variable peut être à la fois globale et constante, dans ce cas on la déclare à l'aide du mot-clé **`const`**. Il est conseillé d'utiliser un nom en majuscules pour les constantes globales :

```js
const MA_CONSTANTE = 1234
```

### Création

#### Unicité des noms de variable

Il est impossible de déclarer une variable ayant le même nom qu'une variable déjà existante dans le bloc courant. Dans ce cas, une erreur sera renvoyée indiquant que le nom est déjà pris.

#### Création d'une variable

Une constante se déclare à l'aide du mot-clé **`var`**.

Création d'une variable vide :
```js
var exemple
```
Création d'une variable avec une valeur :
```js
var exemple = 1234
```

#### Création d'une constante

Une constante se déclare à l'aide du mot-clé **`let`**. Il est interdit de créer une constante sans lui donner une valeur, une constante vide.

Création d'une constante avec une valeur :
```js
let constante = 1234
```

### Lecture d'une variable

#### Accès à une variable
Pour accéder à la valeur contenue dans la variable, il suffit d'écrire son nom :
```js
var exemple = 1234
print(exemple) // Affiche 1234
```

#### Accès à une variable vide
Si l'on tente d'accéder à la valeur d'une variable vide, qui a le type **`jamais`**, on récupère la valeur **`null`** et un avertissement est renvoyé pour chaque endroit où une tentative d'accès est constatée :
```js
var exemple
print(exemple) // Affiche null
//    ^^^^^^^ Avertissement : "Accès à la variable 'exemple' qui est vide"
```

### Portée des variables

#### Variable déclarée dans un bloc

Une variable est créée dans un bloc et est accessible dans ce bloc après sa définition. Quand ce bloc se ferme, la variable n'existe plus. Un accès à une variable qui n'existe plus produit une erreur :
```js
{
    var exemple = 1234
    print(exemple) // Affiche 1234
}
print(exemple) // Erreur : la variable exemple n'existe pas
```

#### Capture d'une variable par une fonction

Une variable peut être utilisée dans une fonction tant que la variable est définie avant la fonction. Dans ce cas la fonction gardera automatiquement une référence vers cette variable avec elle. On parle de *capture* :
```js
var exemple = 1234
var f = function(x) {
    return x + exemple
}
print(f(10)) // Affiche 1244
exemple += 1000
print(f(10)) // Affiche 2244
```

#### Durée de vie d'une capture

Une capture existe tant que sa fonction existe, et ce même si la variable capturée d'origine n'existe plus :
```js
var f = {
    var exemple = 12
    function(x) {
        return x * exemple
    }
}
// La variable exemple n'existe plus à cette ligne
// Mais la fonction f existe, et en elle une référence à la variable exemple
print(f(10)) // Affiche 120
```


### Modification d'une variable

#### Modification simple d'une variable

La valeur d'une variable non constante peut être modifiée à l'aide d'opérateurs ou de fonctions spécifiques :
```js
var exemple = 1234
exemple += 100
print(exemple) // Affiche 1334
```

#### Immutabilité des constantes

Les constantes ne sont en aucun cas modifiables. En essayant de le faire, une erreur sera renvoyée :
```js
let exemple = 1234
exemple += 100 // Erreur : impossible de modifier la constante "exemple"
```

#### Changement de type : mutation

Une variable non constante peut changer de type de valeur, on appelle cet évenement une *mutation*.

```js
var exemple = 1234
print(exemple) // Affiche 1234
exemple = "bonjour"
print(exemple) // Affiche "bonjour"
```

#### Mutation d'une variable dans une branche

Une mutation peut survenir dans une branche, dans ce cas, le type de la variable lorsque l'embranchement est terminé est automatiquement recalculé et la variable est convertie vers le nouveau type :

```js
var exemple = 12 // entier
if (...) {
    exemple = 'bonjour' // texte
}
exemple // type : entier | texte
```

#### Mutation d'une variable dans une boucle

Dans le cas des boucles, le type final de la variable est calculé après la simulation d'un tour de boucle et la variable est convertie vers ce type juste avant la boucle :
```js
var exemple = [] // Type : tableau[]
// Au cours de la boucle, la variable exemple subit une mutation vers le type liste<entier>,
// elle est donc convertie vers liste<entier> avant la boucle
for (var i in [1, 2, 3]) {
    exemple.push(i)
}
print(exemple) // Affiche [1, 2, 3], Type : liste<entier>
```
