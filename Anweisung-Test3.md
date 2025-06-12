# Testanweisungen für den Tester (Rendering-Test)

1. **Anmeldung:**

    *  Du befindest dich im Login-Fenster im Spiel.
    * Gib die folgenden Zugangsdaten ein:
      - **Username:** Tester
      - **Passwort:** tsx!
    * Wähle nun den folgenden Lobby-Namen aus:
      - **Lobbyname:** RaumstationOmega
    * Klicke auf "Looby beitreten".

2. **Spielstart & Orientierung:**

* Du befindest dich nun in einer Raumschiffbasis im Orbit. Von hier aus kannst du das Sonnensystem beobachten.
* Nimm dir einen Moment Zeit, dich umzusehen. Versuche, dich im Raum zu bewegen. Wie gut läuft die Anwendung? Gibt es Ruckler?
* Teste die Kamerasteuerung: Kannst du zoomen, die Ansicht drehen, zwischen verschiedenen Perspektiven wechseln?

3. **Planeten und ihre Bahnen beobachten:**

* Beobachte die Planeten, die sich auf ihren Bahnen bewegen. Wie wirken die Animationen und Bewegungsabläufe?
* Achte auf Details wie Oberflächenstrukturen, Atmosphären oder Monde der Planeten. Wie detailliert sind diese dargestellt?
* Wie viele Planeten oder Himmelskörper kannst du gleichzeitig auf dem Bildschirm sehen, bevor die Performance beeinträchtigt wird?

4. **Raumschiff-Kontrolle testen:**

* Öffne das Raumschiff-Menü und wähle ein verfügbares Raumschiff aus.
* Starte eine Mission zu einem der Planeten. Beobachte, wie sich das Raumschiff durch das Sonnensystem bewegt.
* Achte auf Animationen, Partikeleffekte und Beleuchtung. Wie realistisch wirken diese Effekte?

5. **Rendering-Details und Performance:**

* Wechsle zwischen verschiedenen Kamera-Distanzen (sehr nah an einem Objekt und weit entfernt).
* Beobachte, wie sich die Bildqualität und Performance verändert, wenn mehrere Objekte gleichzeitig im Sichtfeld sind.
* Achte besonders auf Licht- und Schatteneffekte sowie Reflexionen auf verschiedenen Oberflächen.

## 6. Modding & Manipulation 

Du wirst nun das Spiel dynamisch manipulieren – in Echtzeit – über TypeScript-Funktionen.

* Öffne die IDE.

### Modding-Workflow: Planeten

1. **Löschen des Planeten**
    ```ts
    deletePlanet("Mars");
    printGameContext();
    ```
    * Wird er vollständig entfernt? Was passiert mit seiner Umlaufbahn oder Lichtquelle?

2. **Planet hinzufügen**
    ```ts
    addPlanet("Test", [1, 1, 0], 100);
    printGameContext();
    ```
    * Beobachte, ob und wie der Planet "Test" im Spiel erscheint.
    * Welche Texturen werden geladen?

3. **Bewegung des Planeten**
    ```ts
    movePlanet("Test", [10, 10, 10]);
    printGameContext();
    ```
    * Bewegt sich der Planet flüssig?
    * Gibt es visuelle Latenzen oder Glitches?



---

### Modding-Workflow: Raumschiffe

1. **Raumschiff hinzufügen**
    ```ts
    addSpaceShip(1, 1, [1, 2, 0]);
    printGameContext();
    ```
    * Erscheint das Schiff korrekt?
    * Ist die Startposition sichtbar und korrekt beleuchtet?

2. **Ziel setzen (Ziel-Koordinate definieren)**
    ```ts
    setTargetForSpaceShip(1, [5, 5, 5]);
    printGameContext();
    ```
    * Bewegt sich das Raumschiff automatisch zum Ziel?
    * Ist die Flugbahn nachvollziehbar?

3. **Geschwindigkeit verändern**
    ```ts
    setVelocityForSpaceShip(1, [0.5, 0.5, 0.5]);
    ```
    * Wie reagiert das Schiff auf die neue Geschwindigkeit?
    * Beobachte Bewegung und eventuelle Performanceänderungen.

4. **Schiff löschen**
    ```ts
    deleteSpaceShip(1);
    printGameContext();
    ```
    * Wird das Raumschiff sauber entfernt?

5. **Perfomance Test**
   Füge folgenden Code ein und überarbeite ihn nach deinen Vorstelleungen, wenn du möchtest.
   Dieser Code soll ein Paar Planeten erstellen.
   ```ts
   // Hilfsfunktion für zufällige Koordinaten im Bereich von -1000 bis +1000
   function getRandomCoordinate(): [number, number, number] {
       const range = 1000;
       const rand = () => Math.random() * range * 2 - range;
       return [rand(), rand(), rand()];
   }
   
   // Starte den Spawn-Prozess
   function spawnMassivePlanetTest(count: number = 10000) {
       console.log(`Starte Spawning von ${count} Planeten...`);
       for (let i = 0; i < count; i++) {
           const name = `Planet_${i}`;
           const position = getRandomCoordinate();
           const size = Math.random() * 50 + 10; // zufällige Größe zwischen 10 und 60
           addPlanet(name, position, size);
       }
       console.log("Alle Planeten wurden gespawnt.");
       printGameContext();
   }
   
   // Jetzt aufrufen:
   spawnMassivePlanetTest();
   ```
   Schau dir nun das Ergebnis an.
