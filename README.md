# 🎮 Game Engineers want Multiplayer  
## 🚀 Game Engine goes Multiplayer  

---

### 👥 Team

| Name             | Matrikelnummer | E-Mail                           | GitHub Name |
|------------------|----------------|----------------------------------|----------------------------------
| Jonas Kernchen   | 106005         | joke2169@bht-berlin.de           | SneakyChoclate |
| Patrick Kloss    | 107259         | pakl8536@bht-berlin.de           | ptxy |
| Philipp Niclas   | 106932         | phni9317@bht-berlin.de           | SilentNille |
| Lovis Trüstedt   | 103717         | lotr7201@bht-berlin.de           | callidaria |

---

### 🧩 Art des Projekts
> **Multiplayer Game** aus eigener Game Engine

---

### 🛠️ Geplante Technologien

- Datenbank  
- Eigene Game Engine  
- Multiplayer-System  
- Login-System  

#### 🔮 Wunsch-Technologien *(zeitabhängig)*

- Load Balancer  
- Kubernetes  

---

### 📜 Projektbeschreibung

Das Ziel dieses Projekts ist es, ein **plattformunabhängiges Multiplayer-Spiel** zu entwickeln, das auf einer **eigenen Rendering-Engine** basiert und ein **modular strukturiertes Backend** im Stil einer **Microservice-Architektur** bietet.

Die Microservices sorgen für eine **funktionale Trennung** zwischen Multiplayer-Logik und Rechen-Logik. Das erlaubt uns spätere **Skalierbarkeit** und erhöht die **Flexibilität**.  
Wir setzen auf **Docker**, um eine einfache und schnelle Deploybarkeit der Services sicherzustellen.  

> **Zukunftsvision:**  
> Wenn die Zeit es erlaubt, wollen wir **Kubernetes** und **Load Balancer** einbinden, um die **Last optimal zu verteilen**.

---

### 🧱 Technische Basis

Unser Projektkern besteht aus **mehreren unabhängigen Komponenten**.

#### 🎨 Rendering Engine
- **OpenGL 3.3** für grafische Darstellung  
  → Unterstützt auch **schwächere oder Onboard-GPUs**  
  → Kein Einsatz der Fixed-Function Pipeline = mehr Erweiterbarkeit  

#### 🎮 Input Handling
- **SDL2** für bessere Portierbarkeit und flexibles Event-Handling

---

### 📦 Spielinhalt

Das Spiel selbst dient als **Showcase** für unsere Engine.  
Der Content wird **iterativ** während der Entwicklung erstellt.  
Am Ende zählt:  
- Präsentation der **Engine-Funktionalität**  
- **Multiplayer-Anbindung** sichtbar und spielbar machen  
