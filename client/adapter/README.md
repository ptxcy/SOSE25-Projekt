# HTTP und WebSocket Adapter

Dieses Verzeichnis enthält die Adapter für HTTP- und WebSocket-Verbindungen des SOSE25-Projekts.

## Struktur

- `http-adapter.h`, `http-adapter.cpp`: HTTP-Adapter für die Kommunikation mit dem Backend.
- `websocket-adapter.h`, `websocket-adapter.cpp`: WebSocket-Adapter für die Echtzeit-Kommunikation.
- `message-pack-helper.h`: Hilfsfunktionen für MessagePack.
- `doc/`: Dokumentation zum Adapter.
- `test/`: Tests für den Adapter.
- `mock/`: Mock-Implementierungen der Adapter für die Entwicklung ohne externe Abhängigkeiten.

## HTTP-Adapter

Der HTTP-Adapter bietet Funktionen für:
- Benutzererstellung
- Authentifizierung
- Lobby-Erstellung

### Abhängigkeiten

Der HTTP-Adapter benötigt:
- `libcurl`
- `cpr` (C++ Requests - ein Wrapper für libcurl)
- `cppcodec` (für Base64-Kodierung)

### Kompilieren des HTTP-Adapters

```bash
# In adapter/ ausführen
make
```

## Mock-Adapter

Für die Entwicklung ohne Installation der externen Abhängigkeiten steht ein Mock-Adapter zur Verfügung. Weitere Informationen finden Sie in der [Mock-Adapter-README](mock/README.md).

## WebSocket-Adapter

Der WebSocket-Adapter bietet Funktionen für die Echtzeit-Kommunikation mit dem Backend über WebSockets. Die Implementierung ist in `websocket-adapter.h` und `websocket-adapter.cpp` zu finden.

### Abhängigkeiten

Der WebSocket-Adapter benötigt:
- `cpprestsdk` (C++ REST SDK - für WebSocket-Kommunikation)
- `msgpack-c` (für die MessagePack-Serialisierung)

### MessagePack-Unterstützung

Der WebSocket-Adapter unterstützt die Kommunikation mit dem Calculation Unit-Service über das MessagePack-Format:

1. `message-pack-helper.h` stellt Hilfsfunktionen zur Serialisierung und Deserialisierung von Daten bereit.
2. Der WebSocket-Client kann binäre MessagePack-Daten über die `send_binary_message`-Methode senden.
3. Die `sendMessagePackToCalculationUnit`-Funktion vereinfacht das Senden von MessagePack-Daten an den Calculation Unit-Service.

### Beispiel

Ein Beispiel für die Verwendung des WebSocket-Adapters mit MessagePack finden Sie in `test/messagepack_example.cpp`.

```bash
# Beispiel kompilieren
cd adapter/test
./build_messagepack_example.sh

# Beispiel ausführen (JWT-Token als Parameter übergeben)
./messagepack_example "your_jwt_token"
```