# nex.pod
A compact, interactive multi-mode smart device built using the XIAO ESP32-S3. This project combines a digital clock, virtual desk pet, mini games, and productivity tools into a single OLED-based system controlled with just two buttons.

## ✨ Features

### 🐾 Desk Pet Mode

* Animated virtual pet displayed on OLED
* Time-based behavior (morning, afternoon, evening, night)
* Mood system that changes based on interaction
* Idle detection (pet reacts if ignored)
* Random animations and sound feedback

---

### ⏰ Clock Mode

* Real-time clock using RTC module
* Displays time and date
* Clean and minimal interface

---

### 🎮 Game Mode

* Lightweight 2-button mini games
* Fast and responsive gameplay
* Designed for low-resource OLED systems

---

### ⏱ Stopwatch

* Start/Stop and Reset functionality
* Simple and accurate time tracking

---

### 📚 Study Timer (Pomodoro)

* 25-minute focus sessions
* 5-minute break cycles
* Buzzer alerts for session completion

---

### ⚙️ Settings

* Toggle sound (buzzer)
* Reset pet mood
* Basic time configuration

---

## 🎮 Controls

* **Button 1 (D6):** Scroll / Next
* **Button 2 (D7):** Select / Back
* **Long Press:** Open main menu

---

## 🛠 Hardware Used

* XIAO ESP32-S3
* SSD1306 OLED Display (128x64, I2C)
* DS1307 RTC Module
* 2 Push Buttons
* Buzzer

---

## 🔌 Pin Configuration

| Component | Pin |
| --------- | --- |
| SDA       | D4  |
| SCL       | D5  |
| Button 1  | D6  |
| Button 2  | D7  |
| Buzzer    | D2  |

---

## 🧠 How It Works

The system runs on a **mode-based architecture**, where each feature (Pet, Clock, Game, etc.) operates as a separate module.
A simple UI system allows navigation using only two buttons.

The desk pet uses **time-based logic and randomness** to simulate behavior, creating the illusion of intelligence without requiring additional sensors.

---

## 🚀 Why This Project?

This project demonstrates:

* Embedded UI design with minimal input
* Multi-functional system architecture
* Real-time processing using RTC
* Interactive behavior simulation
* Efficient use of limited hardware

---

## 📸 Future Improvements

* Add sensors (LDR, MPU6050) for real-world interaction
* More games and animations
* Battery-powered portable version
* Bluetooth/WiFi connectivity

---
![Platform](https://img.shields.io/badge/Platform-ESP32-blue)
![Status](https://img.shields.io/badge/Status-Development-orange)
![UI](https://img.shields.io/badge/UI-OLED-lightgrey)

---

## 📌 Author

Built by **Ekam Wadhwa**
A student exploring embedded systems, UI design, and interactive hardware projects.

---

## ⭐ If you like this project

Give it a star and feel free to fork or contribute!
