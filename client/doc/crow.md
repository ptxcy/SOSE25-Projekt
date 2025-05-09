# Client Setup mit Crow

Diese Anleitung erklärt, wie du die Entwicklungsumgebung für den C++ Client mit Crow einrichtest.

## Voraussetzungen

1. **C++ Compiler:** Ein C++17-kompatibler Compiler wird benötigt. Das Build-Skript verwendet `clang++`. Unter macOS ist dieser normalerweise mit den Xcode Command Line Tools enthalten.
2. **OpenSSL:** Wird für HTTPS-Verbindungen benötigt.
   - **macOS:** `brew install openssl`
   - **Ubuntu/Debian:** `sudo apt-get install libssl-dev`
3. **Bear:** Wird für die Generierung von `compile_commands.json` verwendet (für bessere IDE-Integration).
   - **macOS:** `brew install bear`
   - **Ubuntu/Debian:** `sudo apt-get install bear`
4. **Unzip:** Wird für die Installation von Crow benötigt.
   - **macOS:** Normalerweise bereits installiert
   - **Ubuntu/Debian:** `sudo apt-get install unzip`

## Installation und Ausführung

### Automatische Installation

1. **Ausführen des Build-Skripts:**
   ```bash
   ./compile_and_run.sh
   ```

   Das Skript führt automatisch folgende Schritte aus:
   - Prüft, ob Crow bereits installiert ist, und installiert es bei Bedarf
   - Kompiliert den Client mit den richtigen Include-Pfaden
   - Führt die Anwendung aus

### Manuelle Installation (falls nötig)

1. **Crow installieren:**
   ```bash
   ./install_crow.sh
   ```

2. **Anwendung kompilieren:**
   ```bash
   clang++ main.cpp -o client_app -std=c++17 -pthread -I./external/include -lssl -lcrypto
   ```

3. **Anwendung ausführen:**
   ```bash
   ./client_app
   ```

## Problembehebung

### Fehler beim Kompilieren

1. **Crow-Header nicht gefunden:**
   - Stelle sicher, dass `install_crow.sh` ausgeführt wurde
   - Überprüfe, ob die Dateien in `./external/include/crow` existieren

2. **OpenSSL-Fehler:**
   - Stelle sicher, dass OpenSSL installiert ist
   - Bei macOS mit Homebrew: `brew link --force openssl`

3. **Linker-Fehler:**
   - Überprüfe die OpenSSL-Pfade in `compile_and_run.sh`

## IDE-Integration

### VS Code

1. Wenn du VS Code verwendest, wird `compile_commands.json` durch das Build-Skript generiert, was die Autovervollständigung und Fehleranzeige verbessert.

2. Empfohlene Extensions:
   - C/C++ Extension (Microsoft)
   - clangd (für verbesserte C++-Unterstützung)

### CLion

Für CLion-Benutzer:

1. Öffne das Projekt in CLion
2. Importiere das `compile_commands.json` File über die Projekteinstellungen

## Verwendung von Crow im Code

```cpp
#include <crow.h>

// Beispiel für einen einfachen HTTP-Request
crow::SimpleApp app;
auto client = app.get_client();
auto response = client.request_async("GET", "http://example.com").get();
```