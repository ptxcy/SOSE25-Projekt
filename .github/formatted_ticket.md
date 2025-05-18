# Feature Request: Implementierung von HTTP- und Websocket-Adaptern für AuthProxy

## Beschreibung

Wir benötigen zwei Adapter für die Kommunikation mit AuthProxy: einen HTTP-Adapter und einen Websocket-Adapter. Diese sollen die Authentifizierung und die Verbindung zu Lobbys ermöglichen.

## Aufgaben

### HTTP-Adapter

Der HTTP-Adapter soll folgende Anfragen unterstützen:

1. **Benutzer erstellen**
   - `POST http://localhost:8080/user`
   - Payload: `{username, password}`

2. **Authentifizierung**
   - `GET http://localhost:8080/authenticate`
   - Header: Basic Auth (`Basic Base64(username:password)`)

3. **Lobby erstellen**
   - `POST http://localhost:8080/lobbys`
   - Payload: `{lobbyName, optional password}`
   - Header: Auth-Header

### Websocket-Adapter

- Verbindung zu `/calculate` mit dem Auth-Header ermöglichen

## Ablauf der Verbindung

1. Nutzer erstellt einen neuen Benutzer mit `POST http://localhost:8080/user`
2. Nutzer authentifiziert sich mit `GET http://localhost:8080/authenticate` und erhält einen Bearer Token
3. Nutzer erstellt eine Lobby mit `POST http://localhost:8080/lobbys` und überträgt den Auth-Header
4. Nutzer verbindet sich über den Websocket-Adapter mit `http://localhost:8080/calculate` und verwendet dabei den Auth-Header
