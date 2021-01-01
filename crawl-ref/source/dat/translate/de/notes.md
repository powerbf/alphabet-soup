# Notes on German Translation


## References

1. https://en.langenscheidt.com/english-german
2. https://www.duden.de/woerterbuch
3. https://dict.leo.org/german-english
4. https://de.wikipedia.org

5. https://www.dnddeutsch.de
6. https://de.classic.wowhead.com/database
7. https://lotr.fandom.com/de/wiki
8. https://harry-potter.fandom.com/de
9. https://harrypotter.fandom.com/de
10. https://rorschachhamster.wordpress.com/2017/12/14/vollstaendige-waffenliste-fuer-swords-wizardry-und-regelideen-dazu/
11. https://elderscrolls.fandom.com/de/wiki
12. http://splitterwiki.de/wiki
13. http://netzhack.de
14. http://prd.5footstep.de/
15. http://www.baldurs-gate.ch/bg2/
16. http://www.knight.privat.t-online.de/KnightsBG2Kits.html

## Specific Terms

The German word Stab means both staff and wand. One can distinguish Zauberstab (magic wand/staff) from Kampfstab (fighting staff), but how do you distinguish a magical staff from a magic wand? Seems like Harry Potter's wand is a Zauberstab, but so is Gandalf's staff. [5] uses Stecken for magical staves.

[5] has might = Stärke, but that means strength, so I went for Macht.  

heroism = Heldenmut [5]  
transmutation = Verwandlung [5]  
experience points = Erfahrungspunkte [5]  
rage = Kampfrausch (lit. battle frenzy) [5]  
abjuration = Bannmagie [5]  
banishment = Verbannung [5]  

conjure = beschwören but also summon = beschwören [3][5]  
In this context we need to distinguish summoning from conjuration.  
For thet reason, I've used summon = herbeirufen.  

melee = Nahkampf [12]  


## Adjectives

DCSS dynamically adds adjectives to things (unidentified items in particular).
This is easy in English because there's no declension of adjectives.
German adjectives, on the other hand, take various endings, and the rules are rather complex...

### Prepositional adjective
no ending

### Weak declension
after definite article (the)

CASE          |MASC |FEM  |NEUT |PLURAL
--------------|-----|-----|-----|------
Nominative    |-e   |-e   |-e   |-en
Accusative    |-en  |-e   |-e   |-en
Dative        |-en  |-en  |-en  |-en
Genitive      |-en  |-en  |-en  |-en

### Mixed declension
after indefinite article (a/an) or possessives (your, etc.)

CASE          |MASC |FEM  |NEUT |PLURAL
--------------|-----|-----|-----|------
Nominative    |-er  |-e   |-es  |-en
Accusative    |-en  |-e   |-es  |-en
Dative        |-en  |-en  |-en  |-en
Genitive      |-en  |-en  |-en  |-en

### Strong declension
when no determiner or after number

CASE          |MASC |FEM  |NEUT |PLURAL
--------------|-----|-----|-----|------
Nominative    |-er  |-e   |-es  |-e
Accusative    |-en  |-e   |-es  |-e
Dative        |-em  |-er  |-em  |-en
Genitive      |-en  |-er  |-en  |-er

Or to put it another way:
-e used for: feminine nom/acc; neuter definite nom/acc; plural nom/acc without determiner
-es used for: neuter nom/acc, except definite
-er used for: masculine nom, except definite; feminine and plural gen without determiner
-en otherwise

Example:

### Weak declension
CASE          |MASC               |FEM               |NEUT                    |PLURAL
--------------|-------------------|------------------|------------------------|------------------
Nominative    |der feine Ring     |die feine Axt     |das feine Schwert       |die feinen Pfeile
Accusative    |den feinen Ring    |die feine Axt     |das feine Schwert       |die feinen Pfeile
Dative        |dem feinen Ring    |der feinen Axt    |dem feinen Schwert      |den feinen Pfeilen
Genitive      |des feinen Rings   |der feinen Axt    |des feinen Schwerts     |der feinen Pfeile

### Mixed declension
CASE          |MASC                |FEM               |NEUT                   |PLURAL
--------------|--------------------|------------------|-----------------------|------------------
Nominative    |ein feiner Ring     |eine feine Axt    |ein feines Schwert     |deine feinen Pfeile
Accusative    |einen feinen Ring   |eine feine Axt    |ein feines Schwert     |deine feinen Pfeile
Dative        |einem feinen Ring   |einer feinen Axt  |einem feinen Schwert   |deinen feinen Pfeilen
Genitive      |eines feinen Rings  |einer feinen Axt  |eines feinen Schwerts  |deinen feinen Pfeile

### Strong declension
CASE          |MASC                |FEM               |NEUT                   |PLURAL
--------------|--------------------|------------------|-----------------------|------------------
Nominative    |feiner Ring         |feine Axt         |feines Schwert         |2 feine Pfeile
Accusative    |feinen Ring         |feine Axt         |feines Schwert         |2 feine Pfeile
Dative        |feinem Ring         |feiner Axt        |feinem Schwert         |2 feinen Pfeilen
Genitive      |feinen Rings        |feiner Axt        |feinen Schwerts        |2 feiner Pfeile

In practice, in DCSS, we see:
- plurals: with number, so strong declension and probably only ever nom/acc (i.e. probably always -e ending)
- singular: with the/a/an/your/etc., so weak or mixed declension, unlikely to see genitive
So in practice, the -e ending will be most common, but -en, -er and -es are also possible.
We're unlikely to ever use the -em ending.
For this reason, I've made the form with the -e ending the default (i.e. no msgctxt).


