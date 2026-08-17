# NavGet

**Compact Motorcycle Navigation & Control System**

---

## 📑 Inhaltsverzeichnis / Table of Contents

- [🇩🇪 Deutsche Version](#-deutsch)
- [🇬🇧 English Version](#-english)

---

## 🚧 Projekt-Status / Project Status
**Phase:** Konzept & Prototyp / Concept & Prototype  
**Aktuelles Ziel / Current Goal:** Hardware-Auswahl & Software-Architektur / Hardware Selection & Software Architecture

---

<a id="deutsch"></a>

## 🇩🇪 Deutsch

### Einleitung
Ein kompaktes, modulares Navigations- und Steuergerät für Motorräder, inspiriert vom Beeline Moto, aber mit erweitertem Funktionsumfang und voller Integration gängiger Motorrad-Navigations-Apps.

### Idee
Das System basiert auf einem ESP32 mit rundem Display und Bluetooth-Verbindung zum Smartphone. Die Navigation wird auf dem Handy gestartet, das Gerät zeigt anschließend Turn-by-Turn-Hinweise direkt am Lenker an. Ohne aktive Navigation dient das Display als Geschwindigkeitsanzeige oder Kompass.

Zusätzlich unterstützt das System Smartphone-Funktionen wie Anrufsteuerung, Musiksteuerung und Lautstärkeregelung. Die Bedienung erfolgt über Lenker-Taster mit Single-Press, Double-Press und Long-Press.

### Hardware-Konzept
- **Controller:** ESP32 (voraussichtlich ESP32-S3 für verbesserte BLE-Performance)
- **Display:** Rundes Display (z.B. 1.28" GC9A01), optional mit Touch
- **Konnektivität:** Bluetooth Low Energy (BLE) zum Smartphone
- **Lenkerhalterung:** Zwei modulare Lenkerklemmen
  - Docking-Station für das Displaymodul
  - Bedieneinheit mit physischen Tastern
- **Verbindung:** Kabel zwischen den Klemmen
- **Display-Dock:** Pogo-Pins für kontaktlose Verbindung
- **Stromversorgung:**
  - Integrierter LiPo-Akku im Displaymodul
  - Laden über Dockingstation mit Pogo-Pins
  - **Zusätzlich:** 5V USB-C Versorgung direkt am Lenkerdock für Dauerbetrieb
- **Gehäuse:** Spritzwassergeschützt (IP54+), vibrationsfest

### Software-Architektur
- **ESP32 Firmware:** PlatformIO / ESP-IDF
- **Companion App:** Android (iOS geplant)
- **Integration:** Nutzung der **Android Notification Listener API** zum Abgreifen von Navigations-Hinweisen
- **Kompatible Navi-Apps:**
  - Google Maps
  - Sygic
  - OSMAnd
  - Kurviger
  - Calimoto
- **UI-Framework:** LVGL (Light and Versatile Graphics Library) für das runde Display

### Funktionen
- ✅ Turn-by-Turn-Navigation mit Pfeildarstellung und Entfernungsanzeige
- ✅ **Arrow-Only Mode:** Reine Pfeil-Navigation (nur Richtungspfeil, keine Text-Hinweise) – zeigt immer die Richtung zum nächsten Ziel
- ✅ Geschwindigkeitsanzeige (GPS-basiert)
- ✅ Kompassanzeige im Standby
- ✅ Anrufe annehmen, ablehnen und starten
- ✅ Musiksteuerung (Play/Pause, Vor/Zurück)
- ✅ Lautstärkeregelung
- ✅ Tastenbelegung mit Mehrfachklicks (Single, Double, Long Press)

### Motivation
Bestehende Lösungen wie ChronOS wurden betrachtet, wirken aber für dieses Vorhaben entweder zu umfangreich oder unterstützen nicht alle benötigten Funktionen direkt. Ziel ist eine gezieltere, schlankere Lösung speziell für den Motorrad-Einsatz mit Fokus auf Sicherheit und Ablenkungsfreiheit.

### Roadmap

#### Phase 1: Hardware-Grundlage
- [x] Konzepterstellung & README
- [ ] Auswahl und Bestellung der Hardware-Komponenten
- [ ] Erster Proof-of-Concept: ESP32 mit rundem Display
- [ ] CAD-Entwürfe für Gehäuse und Lenkerhalterung

#### Phase 2: Software-Prototyp
- [ ] LVGL UI-Mockups für rundes Display
- [ ] Android App: Notification Listener API Integration
- [ ] Test mit OSMAnd, Kurviger und Calimoto
- [ ] Bluetooth-Kommunikation ESP32 ↔ App

#### Phase 3: Erste Integration
- [ ] Prototyp der Docking-Station mit Pogo-Pins
- [ ] Stromversorgung: Akku + USB-C Integration
- [ ] Tasten-Matrix und Multi-Click-Erkennung
- [ ] Erstes 3D-gedrucktes Gehäuse

#### Phase 4: Erweiterte Features
- [ ] **Arrow-Only Mode:** Implementierung der reinen Pfeil-Navigation (Bearing-Only)
- [ ] **IMU / Neigungssensor:** Sturzerkennung, automatische Notruffunktion (eCall)
- [ ] **Audio-Out:** Sprachausgabe für Navigationsansagen (z.B. via BT an Headset)
- [ ] OBD2-Integration für echte Fahrzeugdaten (Drehzahl, Motortemperatur)
- [ ] Wetter-Widget über API
- [ ] iOS Companion App

### Lizenz
Dieses Projekt ist unter der **Creative Commons Attribution-NonCommercial-NoDerivatives 4.0 International (CC BY-NC-ND 4.0)** Lizenz veröffentlicht. Siehe [LICENSE](LICENSE) für Details.

Du darfst:
- ✅ Teilen — das Material in jedwedem Format oder Medium vervielfältigen und weiterverbreiten

Unter folgenden Bedingungen:
- 📝 Namensnennung — Du musst angemessene Urheber- und Rechteangaben machen, einen Link zur Lizenz beifügen und angeben, ob Änderungen vorgenommen wurden.
- 🚫 Nicht kommerziell — Du darfst das Material nicht für kommerzielle Zwecke nutzen.
- 🚫 Keine Bearbeitungen — Wenn du das Material remixt, veränderst oder darauf aufbaust, darfst du die bearbeitete Fassung des Materials nicht verbreiten.

### Mitwirken / Contributing
Beiträge, Ideen und Feedback sind herzlich willkommen! Da dieses Projekt unter einer NoDerivatives-Lizenz steht, können Forks nicht direkt als Pull Request gemerged werden, aber ich freue mich über:
- Diskussionen und Vorschläge via Issues
- Hilfe bei der Fehlersuche (Bug Reports)
- CAD-Design-Ideen oder Tipps zur Hardware-Auswahl
- Testing auf verschiedenen Motorrädern

Erstelle einfach ein Issue für deine Vorschläge!

---

<a id="english"></a>

## 🇬🇧 English

### Introduction
A compact, modular navigation and control device for motorcycles, inspired by the Beeline Moto but with an expanded feature set and full integration of popular motorcycle navigation apps.

### Concept
The system is based on an ESP32 with a round display and Bluetooth connection to a smartphone. Navigation is started on the phone, and the device then displays turn-by-turn instructions directly on the handlebar. Without active navigation, the display serves as a speedometer or compass.

Additionally, the system supports smartphone functions such as call control, music control, and volume adjustment. Operation is via handlebar buttons with Single-Press, Double-Press, and Long-Press.

### Hardware Concept
- **Controller:** ESP32 (likely ESP32-S3 for improved BLE performance)
- **Display:** Round display (e.g., 1.28" GC9A01), optionally with touch
- **Connectivity:** Bluetooth Low Energy (BLE) to smartphone
- **Handlebar Mount:** Two modular handlebar clamps
  - Docking station for the display module
  - Control unit with physical buttons
- **Connection:** Cable between clamps
- **Display Dock:** Pogo-Pins for contactless connection
- **Power Supply:**
  - Integrated LiPo battery in the display module
  - Charging via docking station with Pogo-Pins
  - **Additional:** 5V USB-C supply directly at the handlebar dock for continuous operation
- **Enclosure:** Splash-proof (IP54+), vibration-resistant

### Software Architecture
- **ESP32 Firmware:** PlatformIO / ESP-IDF
- **Companion App:** Android (iOS planned)
- **Integration:** Using the **Android Notification Listener API** to capture navigation prompts
- **Compatible Navigation Apps:**
  - Google Maps
  - Sygic
  - OSMAnd
  - Kurviger
  - Calimoto
- **UI Framework:** LVGL (Light and Versatile Graphics Library) for the round display

### Features
- ✅ Turn-by-turn navigation with arrow display and distance indication
- ✅ **Arrow-Only Mode:** Pure arrow navigation (directional arrow only, no text prompts) – always shows the direction to the next destination
- ✅ Speedometer (GPS-based)
- ✅ Compass display in standby
- ✅ Accept, reject, and initiate calls
- ✅ Music control (Play/Pause, Next/Previous)
- ✅ Volume control
- ✅ Button assignment with multi-clicks (Single, Double, Long Press)

### Motivation
Existing solutions like ChronOS have been considered but appear either too complex or do not directly support all required functions for this project. The goal is a more targeted, streamlined solution specifically for motorcycle use, focusing on safety and distraction-free operation.

### Roadmap

#### Phase 1: Hardware Foundation
- [x] Concept creation & README
- [ ] Selection and ordering of hardware components
- [ ] First proof-of-concept: ESP32 with round display
- [ ] CAD designs for enclosure and handlebar mount

#### Phase 2: Software Prototype
- [ ] LVGL UI mockups for round display
- [ ] Android App: Notification Listener API integration
- [ ] Testing with OSMAnd, Kurviger, and Calimoto
- [ ] Bluetooth communication ESP32 ↔ App

#### Phase 3: First Integration
- [ ] Prototype of docking station with Pogo-Pins
- [ ] Power supply: Battery + USB-C integration
- [ ] Button matrix and multi-click detection
- [ ] First 3D-printed enclosure

#### Phase 4: Advanced Features
- [ ] **Arrow-Only Mode:** Implementation of pure arrow navigation (Bearing-Only)
- [ ] **IMU / Tilt Sensor:** Fall detection, automatic emergency call function (eCall)
- [ ] **Audio-Out:** Voice output for navigation prompts (e.g., via BT to headset)
- [ ] OBD2 integration for real vehicle data (RPM, engine temperature)
- [ ] Weather widget via API
- [ ] iOS Companion App

### License
This project is licensed under the **Creative Commons Attribution-NonCommercial-NoDerivatives 4.0 International (CC BY-NC-ND 4.0)** License. See [LICENSE](LICENSE) for details.

You are free to:
- ✅ Share — copy and redistribute the material in any medium or format

Under the following terms:
- 📝 Attribution — You must give appropriate credit, provide a link to the license, and indicate if changes were made.
- 🚫 NonCommercial — You may not use the material for commercial purposes.
- 🚫 NoDerivatives — If you remix, transform, or build upon the material, you may not distribute the modified material.

### Contributing
Contributions, ideas, and feedback are welcome! Since this project is under a NoDerivatives license, forks cannot be directly merged via pull requests, but I am happy to receive:
- Discussions and suggestions via Issues
- Help with debugging (Bug Reports)
- CAD design ideas or hardware selection tips
- Testing on different motorcycles

Simply create an issue for your suggestions!

---

## 📸 Visuals (Coming Soon)
*CAD-Renderings, Schaltpläne und UI-Mockups werden hier hinzugefügt, sobald verfügbar.*  
*CAD renderings, circuit diagrams, and UI mockups will be added here once available.*

---


---

**Viel Spaß beim Tüfteln! / Happy tinkering! 🏍️🔧**
