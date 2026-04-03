# NavGet
Compact motorcycle navigation device with Smartphone control 
---
# ESP32 Motorrad-Navi

Ein kompaktes, modulares Navigations- und Steuergerät für Motorräder, inspiriert vom Beeline Moto, aber mit erweitertem Funktionsumfang.

## Idee

Das System basiert auf einem ESP32 mit rundem Display und Bluetooth-Verbindung zum Smartphone. Die Navigation wird auf dem Handy gestartet, das Gerät zeigt anschließend Turn-by-Turn-Hinweise direkt am Lenker an. Ohne aktive Navigation dient das Display als Geschwindigkeitsanzeige.

Zusätzlich soll das System Smartphone-Funktionen wie Anrufsteuerung, Musiksteuerung und Lautstärkeregelung unterstützen. Die Bedienung erfolgt über Lenker-Taster mit Single-Press, Double-Press und Long-Press.

## Hardware-Konzept

- ESP32 als Hauptcontroller
- Rundes Display, optional mit Touch
- Bluetooth-Kopplung mit dem Smartphone
- Zwei Lenkerklemmen:
  - eine für das Displaymodul
  - eine für die Bedieneinheit
- Verbindung zwischen den Klemmen per Kabel
- Pogo-Pins für die Verbindung von Display und Dock
- Integrierter Akku im Displaymodul
- Laden über Dockingstation mit Pogo-Pins
- Spritzwassergeschütztes Gehäuse

## Funktionen

- Turn-by-Turn-Navigation
- Geschwindigkeitsanzeige ohne aktive Navigation
- Anrufe annehmen, ablehnen und starten
- Musiksteuerung
- Lautstärkeregelung
- Tastenbelegung mit Mehrfachklicks

## Motivation

Bestehende Lösungen wie ChronOS wurden bereits betrachtet, wirken aber für dieses Vorhaben entweder zu umfangreich oder unterstützen nicht alle benötigten Funktionen direkt. Ziel ist daher eine gezieltere, schlankere Lösung für den Motorrad-Einsatz.

## Ziel

Ein kompaktes, robustes und erweiterbares Motorrad-Interface, das Navigation und Smartphone-Steuerung sinnvoll kombiniert.