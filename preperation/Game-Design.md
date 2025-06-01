# Spieldesign: Stellaris-inspiriertes Weltraum-Explorationsspiel

## 1. Spielkonzept

Das Spiel ist ein Weltraum-Explorations- und Ressourcenmanagement-Spiel, das stark von Titeln wie Stellaris inspiriert ist. **Alle Spieler starten auf der Erde.** Ziel ist es, durch den Aufbau von automatisierten Fabriken Ressourcen zu generieren, Technologien zu erforschen und die eigene Basis so schnell wie möglich zu entwickeln, um das Sonnensystem zu erkunden und zu dominieren. **Die Phase auf der Erde ist eine kurze Startphase; der Kern des Spiels liegt im interplanetaren Wettbewerb um Ressourcen und Territorien im Sonnensystem.** Langfristig geht es darum, die Vormachtstellung im Sonnensystem zu erlangen und andere Spieler zu übertreffen.

**Kernmechaniken:**

* **Automatisiertes Ressourcenmanagement:** Aufbau und Optimierung von Fabriken zur Ressourcengenerierung, Verarbeitung und Lagerung.
* Technologieforschung und -entwicklung
* Basisbau und -ausbau (Fabriken, Forschungseinrichtungen, Raumschiffwerften)
* Raumschiffbau und -steuerung
* Planetenexploration und -kolonisierung
* **Wettbewerb:** Wettrennen um Schlüsselpositionen und Ressourcen im Sonnensystem gegen andere Spieler.

## 2. Spielerinteraktionen (Was kann der Spieler tun?)

Der Spieler soll folgende Aktionen ausführen können:

* **Fabriken bauen und verwalten:** Errichten und Verbessern von Gebäuden (Fabriken) zur automatischen Generierung und Verarbeitung von Rohstoffen auf Planeten.
* **Bauen (Basis):** Errichten von Anlagen auf der Heimatbasis (Erde) und später auf anderen Planeten.
  * Gebäude zur Ressourcenverarbeitung (z.B. Schmelzöfen, Raffinerien – als Teil von Fabrikkomplexen)
  * Forschungslabore
  * Raumschiffwerften
  * Lagereinrichtungen
  * Verteidigungsanlagen (optional für spätere Erweiterungen)
* **Forschen:** Freischalten neuer Technologien, Fabriktypen, Gebäude, Raumschiffteile und Abbau-Effizienz-Upgrades.
* **Craften (Bauteile):** Herstellen von Bauteilen für Gebäude und Raumschiffe (keine manuellen Werkzeuge).
* **Raumschiffe entwerfen und bauen:** Aus verschiedenen Modulen (Antrieb, Lager, Waffen – optional, Scanner) Raumschiffe zusammenstellen und bauen.
* **Planeten scannen und bereisen:** Das Sonnensystem erkunden, Planeten nach Ressourcen scannen und mit Raumschiffen anfliegen.
* **Inventar verwalten:** Gesammelte Ressourcen, Werkzeuge und Items organisieren.

## 3. Spielobjekte

### 3.1. Planeten

Jeder Planet im Sonnensystem ist ein einzigartiges Objekt mit spezifischen Eigenschaften.

* **Datenstruktur (JSON `Planet`):**

  ```json
  {
    "name": "string",                 // Name des Planeten (z.B. "Erde", "Mars")
    "typ": "PlanetType",              // (z.B. Gesteinsplanet, Gasriese, Eisplanet)
    "vorkommendeRessourcen": [        // Welche Ressourcen und in welcher Menge
      {
        "rohstoff": "RohstoffTyp",
        "menge": "number",            // Verfügbare Menge oder Dichte
        "abbauSchwierigkeit": "number" // Faktor, der den Abbau beeinflusst
      }
    ],
    "bewohnbar": "boolean",           // Ist der Planet für den Spieler bewohnbar/bebaubar?
    "gebaeudeAufPlanet": [            // Gebäude, die der Spieler auf dem Planeten errichtet hat
      "Gebaeude" // Referenz oder eingebettetes Objekt
    ],
    // Weitere planetenspezifische Eigenschaften:
    "groesse": "number",              // Durchmesser oder Masse
    "temperatur": "number",           // Durchschnittstemperatur
    "atmosphaere": "boolean",         // Vorhandensein der Atmosphäre
    // Position im Sonnensystem
    "position": {
      "x": "number",
      "y": "number",
      "z": "number"
    }
  }
  ```

* **Verhalten/Funktionen:**

  * `scanPlanet()`: Gibt Informationen über Ressourcen und Eigenschaften des Planeten zurück.
  * `baueGebaeude(GebaeudeTyp typ, Position pos)`: Erlaubt das Bauen auf dem Planeten.
  * `ressourceAbbauen(RohstoffTyp typ, Werkzeug werkzeug)`: Simuliert den Abbau von Ressourcen.
  * `updatePlanetState()`: Aktualisiert z.B. verbleibende Ressourcenmengen.

### 3.2. Raumschiffe

Raumschiffe dienen dem Transport von Ressourcen und dem Spieler (oder dessen Sonden/Robotern) zwischen Planeten.

* **Datenstruktur (JSON `Raumschiff`):**

  ```json
  {
    "name": "string",
    "typ": "RaumschiffTyp",           // (z.B. Frachter, Erkundungsschiff)
    "huelle": "number",               // Strukturpunkte/Gesundheit
    "geschwindigkeit": "number",      // Maximale Geschwindigkeit im System
    "treibstoffKapazitaet": "number",
    "aktuellerTreibstoff": "number",
    "lagerraum": {                    // Inventar für transportierte Ressourcen/Items
      // Struktur des Inventars, z.B. Array von { "item": "ItemTyp", "menge": "number" }
    },
    "aktuellePositionPlanetId": "string", // ID des Planeten, auf dem es sich befindet (oder im Orbit)
    "zielPlanetId": "string",           // ID des Ziels der aktuellen Reise
    "imFlug": "boolean",
    "position": {
      "x": "number",
      "y": "number",
      "z": "number"
    }
  }
  ```

* **Verhalten/Funktionen:**

  * `starteReise(Planet* ziel)`: Initiiert einen Flug zu einem anderen Planeten.
  * `updateFlug(double deltaTime)`: Bewegt das Schiff, verbraucht Treibstoff.
  * `ladeFracht(RohstoffTyp typ, double menge)`: Lädt Ressourcen ein.
  * `entladeFracht(RohstoffTyp typ, double menge)`: Lädt Ressourcen aus.
  * `scanUmgebung()`: Nutzt Scannermodule, um z.B. nahe Asteroiden oder Ressourcensignaturen zu finden.

### 3.3. Items

Items umfassen Rohstoffe, verarbeitete Materialien und Bauteile. **Manuelle Werkzeuge für den Spieler gibt es nicht; der Abbau erfolgt durch spezialisierte Gebäude/Fabriken.**

#### 3.3.1. Rohstoffe

* **Datenstruktur (JSON `Item` / `Rohstoff`):**

  ```json
  {
    "name": "string",
    "typ": "RohstoffTyp",
    "info": "string" // Platyhalter für kommende Funktionen
  }
  ```

* **Verhalten/Funktionen:**

  * Rohstoffe selbst haben meist kein aktives Verhalten, sondern werden von anderen Objekten manipuliert (abgebaut, transportiert, verarbeitet).
  * `verarbeiteZu(RohstoffTyp zielTyp)`: Funktion in Verarbeitungsgebäuden.

#### 3.3.2. Werkzeuge & Gebäude (als Items im Bau-Kontext) & Bauteile

* **Datenstruktur (JSON `GebaeudeItem` / `Bauteil`):**

  ```json
  // GebaeudeItem (repräsentiert eine platzierbare Fabrik/Gebäude)
  {
    "name": "string",
    "typ": "ItemTyp", // z.B. "GEBAEUDE_ITEM"
    "beschreibung": "string",
    "gebaeudeTyp": "GebaeudeTyp", // Referenziert den Typ der Fabrik/des Gebäudes
    "baukosten": [
      { "rohstoff": "RohstoffTyp", "menge": "number" }
    ]
  }

  // Enum GebaeudeTyp (als String-Konstanten)
  // "MINENKOMPLEX_EISEN", "RAFFINERIE_KUPFER", "FORSCHUNGSLABOR_BASIS", "RAUMSCHIFFWERFT_STANDARD", "SOLARPANEL_FELD", "LAGERHALLE_STANDARD"

  // Bauteil (erbt von Item)
  {
    "name": "string",
    "typ": "ItemTyp", // z.B. "BAUTEIL"
    "beschreibung": "string"
    // z.B. Antriebsdüse, Bohrkopf für Fabriken, Solarzelle
  }
  ```

* **Verhalten/Funktionen:**

  * GebäudeItems: Werden beim Bau von Fabriken/Gebäuden "konsumiert".
  * Bauteile: Werden beim Crafting von Raumschiffen/Modulen oder beim Upgrade von Fabriken "konsumiert".

### 3.4. Spielerbasis / Forschungsbaum

Die Progression des Spielers wird durch seine Basis und den Forschungsbaum repräsentiert.

* **Datenstruktur (JSON `SpielerBasis` / `Technologie`):**

  ```json
  // SpielerBasis
  {
    "heimatPlanetId": "string",
    "gebaeude": [ "Gebaeude" ],
    "globalesLager": {
      // Struktur des Inventars, z.B. { "RohstoffTyp": "menge" }
    }
  }

  // Technologie
  {
    "id": "string",
    "name": "string",
    "beschreibung": "string",
    "forschungskosten": [
      { "rohstoff": "RohstoffTyp", "menge": "number" }
    ],
    "voraussetzungen": [ "string" ],
    "erforscht": "boolean"
  }

  // Forschungsbaum: Array von Technologie-Objekten
  // [ { ...Technologie 1... }, { ...Technologie 2... } ]
  ```

* **Verhalten/Funktionen:**

  * Basis: `baueGebaeude()`, `verarbeiteRessourcen()`.
  * Forschung: `starteForschung(Technologie* tech)`, `updateForschung(double deltaTime)`.

## 4. Spielablauf (Grobe Phasen)

1. **Phase: Aufbruch von der Erde (Kurze Startphase)**
   * Spieler (und konkurrierende Spieler) starten auf der Erde mit begrenzten Ressourcen.
   * Fokus: Schnellstmöglicher Aufbau erster automatisierter Basisfabriken (z.B. für Eisen, Kupfer) und eines Forschungslabors.
   * Erforschen grundlegender Technologien: Basis-Energieversorgung, einfache Fabrikationsmethoden, grundlegende Raumfahrt.
   * Ziel: Bau einer Raumschiffwerft und des ersten einfachen Raumschiffs, um die Erde zu verlassen und Ressourcen im nahen Umfeld (Mond, erdnahe Asteroiden) zu sichern. **Diese Phase ist ein Wettlauf gegen die Zeit und andere Akteure.**

2. **Phase: Wettlauf im Sonnensystem**
   * Mit dem ersten Raumschiff nahe Himmelskörper erkunden und dort erste automatisierte Außenposten/Fabriken errichten (Mond, Mars, Asteroidengürtel).
   * Sicherung von Schlüsselressourcen (z.B. Helium-3, seltene Metalle), die für fortschrittliche Technologien benötigt werden.
   * Transport der Ressourcen zwischen den Basen und zur Erde (oder zu spezialisierten Industriezentren).
   * Erforschung von Technologien für bessere Raumschiffe (Reichweite, Geschwindigkeit, Ladekapazität), effizientere Fabriken, fortschrittliche Ressourcenverarbeitung und erste militärische Optionen (zur Verteidigung oder Behinderung von Konkurrenten).
   * Ziel: Etablierung einer dominanten Präsenz auf mehreren Himmelskörpern und Sicherung eines stetigen Ressourceneinkommens, um die Konkurrenz zu überflügeln.

3. **Phase: Dominanz im Sonnensystem (Optional für erste Version, Fokus auf PvP-Konflikt)**
   * Aufbau autarker und spezialisierter Industrie-Kolonien auf verschiedenen Planeten.
   * Entwicklung komplexer Logistiknetzwerke zur optimalen Ressourcenverteilung.
   * Erforschung von Spitzentechnologien (z.B. fortschrittliche Waffensysteme, Terraforming-Ansätze, interstellare Sonden).
   * Direkte Konfrontation oder wirtschaftliche Verdrängung anderer Akteure im Sonnensystem.
   * Ziel: Erlangung der technologischen und wirtschaftlichen (und ggf. militärischen) Vormachtstellung im gesamten Sonnensystem.

## 5. Wichtige Überlegungen

* **Benutzeroberfläche (UI):** Klar und intuitiv, um Ressourcen, Bauoptionen, Forschung und Raumschiffsteuerung zugänglich zu machen.
* **Balance:** Ressourcenverfügbarkeit, Baukosten, Forschungszeiten müssen gut ausbalanciert sein, um einen angenehmen Spielfluss zu gewährleisten.

Dieser Plan dient als Grundlage und kann im Laufe der Entwicklung weiter verfeinert und detailliert werden.

## 6. Technologiebaum und Errungenschaften (Entwurf)

Der Technologiebaum ist in Ären oder Stufen unterteilt, die den Fortschritt des Spielers widerspiegeln. Jede Technologie schaltet neue Gebäude, Fabrik-Upgrades, Raumschiffmodule, Rezepte oder globale Boni frei.

### 6.1. Technologie-Ären (Beispiel)

* **Ära 1: Planetare Industrialisierung (Fokus Erde)**
  * **Grundlegende Energieerzeugung:** (Schaltet Solarpanel I, Kohlekraftwerk – falls Kohle als Ressource existiert)
    * *Errungenschaft: "Erleuchtung"* - Erstes Energienetzwerk stabilisiert.
  * **Automatisierte Erzgewinnung:** (Schaltet Minenkomplex I für Eisen, Kupfer)
    * *Errungenschaft: "Die Erde bebt"* - Erste vollautomatische Mine errichtet.
  * **Basis-Raumfahrt:** (Schaltet Raumschiffwerft I, Chemische Triebwerke I, Kleiner Frachter Rumpf)
    * *Errungenschaft: "Den Staub abgeschüttelt"* - Erstes Raumschiff gestartet.
  * **Grundlegende Materialverarbeitung:** (Schaltet Raffinerie I für Stahl, Kupferplatten)
* **Ära 2: Interplanetare Expansion**
  * **Verbesserte Raumantriebe:** (Schaltet Ionentriebwerke, verbesserte Treibstoffeffizienz)
    * *Errungenschaft: "Sternenfahrer"* - Mars erreicht.
  * **Kolonisierungstechniken:** (Schaltet Basis-Habitate für Außenposten, Lebenserhaltung I)
    * *Errungenschaft: "Neue Heimat"* - Ersten Außenposten außerhalb der Erde errichtet.
  * **Spezialisierte Ressourcenextraktion:** (Schaltet Gas-Sammler für Helium-3 vom Mond/Gasriesen, Asteroiden-Bohrer)
    * *Errungenschaft: "Schätze des Vakuums"* - Helium-3 Abbau gestartet.
  * **Fortgeschrittene Elektronik:** (Schaltet Forschungslabor II, verbesserte Scanner, Fabriksteuerungen I)
* **Ära 3: Systemweite Kontrolle**
  * **Großraumfrachter und Logistik:** (Schaltet Große Frachter Rümpfe, Logistikzentren zur Ressourcenverteilung)
    * *Errungenschaft: "Meister der Lieferketten"* - 5 Planeten gleichzeitig mit Ressourcen versorgen.
  * **Autonome Fabrikkomplexe:** (Schaltet Fabrik-Upgrades für Selbstoptimierung, reduzierte Wartung)
  * **Planetare Verteidigungssysteme:** (Schaltet Orbitale Geschützplattformen, Planetare Schilde – falls Konflikt implementiert wird)
    * *Errungenschaft: "Festung Planet"* - Einen Planeten vollständig mit Verteidigungsanlagen gesichert.
  * **Exotische Materialwissenschaft:** (Erforschung seltener Ressourcen und deren Verarbeitung für High-End-Module)
* **Ära 4: Vorstoß ins Unbekannte (Optional/Zukunft)**
  * **Interstellare Sonden:** (Erste Schritte zur Erkundung jenseits des Sonnensystems)
  * **Terraforming-Grundlagen:** (Experimentelle Technologien zur Umweltanpassung)
    * *Errungenschaft: "Gärtner der Galaxis"* - Erste erfolgreiche Terraforming-Maßnahme.

### 6.2. Beispiel-Errungenschaften (Allgemein)

* **"Monopolist":** Kontrolliere 50% einer bestimmten Ressource im Sonnensystem.
* **"Industrie-Titan":** Besitze 10 Fabriken der höchsten Stufe.
* **"Wissenschaftlicher Durchbruch":** Erforsche alle Technologien einer Ära.
* **"Der Unaufhaltsame":** Erreiche jeden Planeten im Sonnensystem.
* **"Erster auf dem Mars":** Sei der erste Spieler, der eine Basis auf dem Mars errichtet.
* **"Alle Systeme Grün":** Erreiche eine positive Energiebilanz von +1000 Energieeinheiten.

## 7. Zusammenfassung der Spielobjekte

Eine Liste der zentralen Spielobjekte:

* **Planeten:**
  * Erde (Startplanet)
  * Mond
  * Mars
  * Venus
  * Merkur
  * Jupiter (und Monde wie Europa, Ganymed)
  * Saturn (und Monde wie Titan, Enceladus)
  * Uranus
  * Neptun
  * Asteroidengürtel (als Ressourcenzone oder viele kleine Objekte)
  * Zwergplaneten (z.B. Ceres, Pluto)
* **Raumschiffe (Typen):**
  * Erkundungsschiff (Scanner, schnell, geringe Kapazität)
  * Frachter (großer Laderaum, langsamer)
  * Kolonieschiff (zum Aufbau neuer Basen)
  * Bauschiff (zum Errichten von Orbitalstrukturen oder großen planetaren Anlagen)
  * (Optional) Kriegsschiffe (verschiedene Klassen für Verteidigung/Angriff)
* **Gebäude/Fabriken (Beispiele für Typen):**
  * **Ressourcengewinnung:**
    * Minenkomplex (für Erze wie Eisen, Kupfer, Silizium etc. auf Gesteinsplaneten)
    * Gas-Extraktor (für Helium-3, Wasserstoff etc. von Gasriesen oder Atmosphären)
    * Wasser-Eis-Bohrer (auf Eisplaneten oder Monden)
    * Asteroiden-Abbaustation (im Asteroidengürtel oder auf einzelnen Asteroiden)
  * **Ressourcenverarbeitung:**
    * Schmelze/Raffinerie (für Metalle, Legierungen)
    * Chemiefabrik (für Treibstoff, Polymere etc.)
    * Elektronikfabrik (für Komponenten, Chips)
  * **Energieerzeugung:**
    * Solarpanel-Farm
    * Fusionsreaktor (spätere Technologie, benötigt z.B. Helium-3)
    * Geothermalkraftwerk (auf vulkanisch aktiven Planeten)
  * **Forschung & Entwicklung:**
    * Forschungslabor (Stufen I, II, III)
    * Technologie-Archiv
  * **Raumfahrt & Logistik:**
    * Raumschiffwerft (Stufen I, II, III für verschiedene Schiffsgrößen/-typen)
    * Orbitaldock (Reparatur, Aufrüstung von Schiffen)
    * Logistikzentrum (zur automatisierten Ressourcenverteilung zwischen Planeten)
    * Massenbeschleuniger (für schnellen Ressourcentransport zwischen zwei Punkten – teuer, späte Technologie)
  * **Lagerung:**
    * Lagerhalle (für Rohstoffe und verarbeitete Güter)
    * Treibstoffdepot
  * **Kolonisierung & Basis:**
    * Habitat/Basis-Modul (Grundstruktur für neue Kolonien)
    * Lebenserhaltungssystem
  * **(Optional) Verteidigung:**
    * Planetare Geschütztürme
    * Orbitale Verteidigungsplattformen
    * Schildgeneratoren
* **Raumschiffmodule (Beispiele):**
  * Antriebe (Chemisch, Ionen, Plasma, Fusionsantrieb)
  * Scanner (Planetenscanner, Ressourcenscanner, Tiefenradar)
  * Laderaum-Erweiterungen
  * Schildgeneratoren (für Schiffe)
  * (Optional) Waffensysteme (Laser, Raketen, Kinetische Geschütze)
  * Abbaumodule (für spezialisierte Abbauschiffe, z.B. Asteroidenbohrer)
  * Kolonisierungsmodul (für Kolonieschiffe)
* **Rohstoffe (Beispiele):**
  * **Basis:** Eisen, Kupfer, Silizium, Kohlenstoff, Wasser
  * **Fortgeschritten:** Aluminium, Titan, Uran, Helium-3, Seltene Erden
  * **Verarbeitet:** Stahl, Kupferplatten, Elektronische Komponenten, Treibstoff, Polymere, Supraleiter
* **Spielerbasis:** Repräsentiert die Gesamtheit der Besitztümer und Infrastruktur eines Spielers.
* **Technologien:** Abstrakte Objekte im Forschungsbaum, die neue Fähigkeiten, Gebäude oder Einheiten freischalten.
