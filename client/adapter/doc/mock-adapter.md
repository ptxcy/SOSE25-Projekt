# Mock HTTP-Adapter

## Übersicht

Der Mock HTTP-Adapter ist eine alternative Implementierung des HTTP-Adapters, die keine externen Abhängigkeiten benötigt. Er ist besonders nützlich für:

1. **Entwicklung ohne Abhängigkeiten**: Ermöglicht das Kompilieren und Testen von Code, der den HTTP-Adapter verwendet, ohne die erforderlichen externen Bibliotheken (`libcurl`, `cpr`, `cppcodec`) zu installieren.

2. **Unabhängiges Testen**: Erlaubt das Testen von Komponenten, die den HTTP-Adapter verwenden, ohne eine Verbindung zum echten Backend herzustellen.

3. **Deterministische Ergebnisse**: Liefert konsistente, vorhersehbare Antworten für Tests und Entwicklung.

## Implementierung

Der Mock-Adapter befindet sich im Verzeichnis `mock/` und implementiert die gleiche API wie der echte HTTP-Adapter:

```cpp
bool createUser(const std::string& username, const std::string& password);
std::optional<std::string> authenticateOnServer(const std::string& username, const std::string& password);
bool createLobby(const std::string& lobbyName, const std::optional<std::string>& lobbyPassword, const std::string& jwtToken);
std::optional<std::string> completeAuthAndLobbyProcess(...);
```

Die Mock-Implementierung gibt für alle Funktionen Erfolgsmeldungen zurück und druckt Debug-Informationen auf der Konsole aus.

## Integration in eigenen Code

### Option 1: Bedingte Kompilierung

Sie können bedingte Kompilierung verwenden, um zwischen dem echten und dem Mock-Adapter zu wechseln:

```cpp
#ifdef USE_MOCK_ADAPTER
    #include "adapter/mock/mock_http_adapter.h"
#else
    #include "adapter/http-adapter.h"
#endif

// Der restliche Code bleibt gleich
```

### Option 2: Direkte Verwendung

Sie können auch direkt den Mock-Adapter importieren, wenn Sie die externen Abhängigkeiten nicht installieren möchten:

```cpp
#include "adapter/mock/mock_http_adapter.h"
```

## Kompilieren und Ausführen

```bash
# Wechseln Sie in das mock-Verzeichnis
cd adapter/mock

# Kompilieren des Mock-Adapters und der Testanwendung
make

# Ausführen des Tests
./mock_test
```

## Vergleich mit dem echten HTTP-Adapter

| Aspekt | Echter HTTP-Adapter | Mock-Adapter |
|--------|---------------------|--------------|
| Externe Abhängigkeiten | `libcurl`, `cpr`, `cppcodec` | Keine |
| Netzwerkkommunikation | Ja, mit dem Backend | Nein, simuliert |
| Rückgabewerte | Abhängig vom Backend | Immer erfolgreich |
| Debugging | Standard-Logging | Ausführliche Konsolenausgabe |
| Verwendungszweck | Produktion | Entwicklung und Tests |
