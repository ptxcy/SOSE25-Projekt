# Mock HTTP Adapter

Dies ist eine vereinfachte Implementierung des HTTP-Adapters ohne externe Abhängigkeiten (libcurl, cpr, cppcodec). Diese Mock-Implementierung ist nützlich für:

- Entwicklung ohne Installation der externen Abhängigkeiten
- Tests ohne Verbindung zum Backend
- Beispiele für die Verwendung des HTTP-Adapters

## Dateien

- `mock_http_adapter.h`, `mock_http_adapter.cpp`: Mock-Implementierung des HTTP-Adapters
- `test_mock.cpp`: Testprogramm für den Mock-Adapter
- `example.cpp`: Beispielprogramm zur Demonstration der Verwendung des Mock-Adapters
- `Makefile`: Makefile für das Kompilieren und Ausführen der Programme

## Verwendung

```bash
# Kompilieren aller Programme
make

# Ausführen des Tests
make test
# oder direkt
./mock_test

# Ausführen des Beispielprogramms
make run-example
# oder direkt
./example
```

## Integration in eigenen Code

Um den Mock-Adapter in Ihrem eigenen Code zu verwenden:

1. Includieren Sie den Mock-Adapter anstelle des echten Adapters:
   ```cpp
   #include "mock/mock_http_adapter.h"
   ```

2. Kompilieren und linken Sie mit der Mock-Implementierung:
   ```bash
   clang++ -std=c++17 ihr_programm.cpp mock/mock_http_adapter.cpp -o ihr_programm
   ```

## Funktionen

Der Mock-Adapter bietet die gleichen Funktionen wie der echte HTTP-Adapter:

- `createUser`: Simuliert das Erstellen eines Benutzers
- `authenticateOnServer`: Simuliert die Authentifizierung und gibt einen Mock-JWT-Token zurück
- `createLobby`: Simuliert das Erstellen einer Lobby
- `completeAuthAndLobbyProcess`: Führt den gesamten Authentifizierungs- und Lobby-Erstellungsprozess durch

Alle Funktionen geben Erfolgsmeldungen zurück und drucken Debug-Informationen auf die Konsole.
