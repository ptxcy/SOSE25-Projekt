# Playtest-Plan

## I. Test 1: Spieltest (Startphase auf der Erde)

* **Dauer:** ca. 5-10 Minuten pro Tester.
* **Fokus:**
  * **UI-Verständlichkeit & Bedienbarkeit:**
    * Navigation und Kamerasteuerung (Bewegung, Zoom, Drehung).
    * Prozess des Fabrikbaus (Auswahl, Platzierung).
    * Konfiguration von Fabriken (falls bereits implementiert).
    * Auffindbarkeit und Verständlichkeit von Fortschrittsanzeigen (Ressourcenbestände, Produktionsraten, Ziel "Raumschiff freischalten").
  * **Spielverständnis:**
    * Verstehen die Tester die unmittelbaren Aufgaben in der Startphase?
    * Ist das Ziel, Ressourcen zu sammeln und Fabriken zu bauen, um ein Raumschiff freizuschalten, klar?
  * **Kernmechaniken (Funktionalität):**
    * Funktioniert das Bauen von Fabriken wie vom Spieler erwartet?
    * Ist der grundlegende Kreislauf von Ressourcensammlung und -anzeige nachvollziehbar?
* **Vorbereitung & Credentials (für den Tester bereitzustellen):**
  * **Username:** `TerraPionier`
  * **Passwort:** `Erde2025!`
  * **Lobbyname (falls zutreffend und benötigt):** `StartbasisAlpha`

* **Detaillierte 
* **Protokollierung (durch den Testbeobachter während des Tests):**
  * Wie schnell findet sich der Tester zurecht? (Skala 1-5: sehr schnell - sehr langsam)
  * Verständnis der Aufgabe "Fabrik bauen": (Skala 1-5: sofort verstanden - gar nicht verstanden)
  * Verständnis der Aufgabe "Ressourcen prüfen": (Skala 1-5)
  * Verständnis des Ziels "Raumschiff freischalten": (Skala 1-5)
  * Gab es spezifische UI-Elemente, die Probleme bereiteten? Welche?
  * Zeit bis zur ersten gebauten Fabrik: `[hh:mm:ss]`
  * Direktes Feedback, Zitate und emotionale Reaktionen des Testers.
  * Beobachtete Fehler, Bugs, Performance-Auffälligkeiten.

## II. Test 2: Engine-Test (Modding-Konzepte)

* **Dauer:** ca. 15-20 Minuten
* **Fokus:** Evaluierung der Verständlichkeit und des potenziellen Workflows für geplante Modding-Funktionen (Hinzufügen von Planeten, Anpassen von Ressourcengenerierung). Feedback zum Konzept der Modding-Schnittstelle (z.B. via TypeScript/JSON) sammeln, auch wenn diese noch nicht technisch implementiert ist.
* **Zu testende Aspekte/Konzepte:**
  * Verständlichkeit des Konzepts zum Hinzufügen eines neuen Planeten.
  * Verständlichkeit des Konzepts zur Anpassung von Ressourcengenerierungszeiten.
  * Erwartungen an eine Modding-Schnittstelle (z.B. Struktur von Konfigurationsdateien, benötigte Parameter).
  * Potenzielle Benutzerfreundlichkeit und Hürden der angedachten Modding-Methoden (TypeScript/JSON).
* **Vorbereitung & Setup (spezifisch für Engine-/Modding-Test):**
  * Bereitstellung von Konzeptpapieren, Diagrammen oder Mock-ups, welche die geplante Modding-Schnittstelle illustrieren (z.B. beispielhafte JSON-Strukturen für Planeten, TypeScript-Code-Snippets für Gameplay-Logik-Anpassungen).
  * Ggf. ein einfacher Texteditor-Setup, um hypothetische Modding-Dateien zu betrachten oder zu bearbeiten.
* **Detaillierte Testanweisungen für den Tester (Modding-Konzept):**
  1. **Einführung:** Der Testleiter erklärt kurz die Vision für Modding im Spiel und die aktuell angedachten Konzepte (z.B. Anpassung via JSON-Dateien oder TypeScript-Skripten).
  2. **Konzept 1: Planet hinzufügen:**
     * Sieh dir die bereitgestellten Konzeptmaterialien (z.B. eine Beispiel-JSON-Struktur für einen Planeten) an.
     * Stell dir vor, du möchtest einen neuen Eisplaneten namens "Xylos" mit geringer Schwerkraft und Vorkommen von Wasser-Eis hinzufügen. Welche Informationen müsstest du deiner Meinung nach in der gezeigten Struktur anpassen oder ergänzen? Diskutiere dies mit dem Testleiter.
     * Welche weiteren Eigenschaften oder Konfigurationsmöglichkeiten für einen Planeten wären dir wichtig?
  3. **Konzept 2: Ressourcengenerierung anpassen:**
     * Betrachte das Konzept zur Anpassung von Ressourcengenerierungszeiten (z.B. ein TypeScript-Snippet oder eine weitere JSON-Konfiguration).
     * Angenommen, du möchtest die Generierungszeit für Eisen auf der Erde um 25% beschleunigen. Wie würdest du basierend auf dem Konzept vorgehen? Wo siehst du potenzielle Unklarheiten?
  4. **Allgemeines Feedback zum Modding-Ansatz:**
     * Welche der angedachten Methoden (z.B. JSON vs. TypeScript) empfindest du für solche Anpassungen als zugänglicher oder passender? Warum?
     * Welche Art von Dokumentation oder Hilfestellung wäre für dich unerlässlich, um erfolgreich modden zu können?
     * Siehst du bestimmte Herausforderungen oder Bedenken bezüglich des Moddens auf die gezeigte (geplante) Art und Weise?
* **Protokollierung (durch den Testbeobachter):**
  * Verständnis der präsentierten Modding-Konzepte (Skala 1-5).
  * Einschätzung der erwarteten Benutzerfreundlichkeit der (geplanten) Modding-Schnittstelle durch den Tester.
  * Bevorzugte Modding-Methode (JSON, TypeScript, In-Game-Editor etc.) und Begründung.
  * Konkrete Vorschläge, Ideen und Bedenken des Testers zur Ausgestaltung des Modding-Systems.
  * Fehlende Informationen oder Parameter, die der Tester für wichtig hält.
  * Zitate und direkte Reaktionen des Testers.

## III. Vorbereitung (Allgemein für beide Testteile)

* Sicherstellen, dass Test-Laptop(s) mit aktueller Spiel-/Engine-Version bereitstehen

## IV. Fragebogen (Nach dem Test)

* Ein separater Fragebogen wird nach Abschluss der Tests ausgehändigt (oder per Link/QR-Code bereitgestellt).
* **Zweck:** Systematisches Sammeln von subjektivem Feedback zu den getesteten Aspekten (User Experience, Verständlichkeit, Schwierigkeitsgrad etc.).
* **Dauer:** ca. 5 Minuten.
* **Formate:** Mix aus Multiple Choice, Likert-Skalen und offenen Fragen.
* *Hinweis für Testleiter: QR-Code für den digitalen Fragebogen generieren und bereithalten.*
