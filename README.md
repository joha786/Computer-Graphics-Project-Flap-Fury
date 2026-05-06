# Flap Fury 🎮

A 2D OpenGL-based arcade game inspired by Flappy Bird, developed in C++ using GLUT.  
The game features multiple playable characters, progressive difficulty levels, animated environments, and a dynamic day-night cycle.

---

## 📌 Features

- 🌤️ Dynamic Day & Night Cycle
- ☁️ Animated Clouds and Stars
- 🌸 Decorative Background Elements
- 🐦 5 Unique Playable Characters
- 🚀 Progressive Difficulty System
- 🧱 Pipe Collision Detection (AABB)
- 🎯 Score & Level System
- 💀 Game Over and Restart System
- ✨ Character Animations and Effects
- 🔄 Smooth Object Movement using OpenGL Transformations

---

## 🎮 Levels & Characters

| Level | Character | Difficulty |
|------|------------|------------|
| 1 | Bird | Easy |
| 2 | Rocket | Medium |
| 3 | Butterfly | Medium-Hard |
| 4 | UFO | Hard |
| 5 | Dragon | Extreme |

Each level increases:
- Pipe speed
- Gravity
- Game difficulty

And decreases:
- Pipe gap size

---

## 🛠️ Technologies Used

- C++
- OpenGL
- GLUT (OpenGL Utility Toolkit)
- Windows API

---

## 📂 Project Structure

```bash
Flap-Fury/
│
├── main.cpp
├── README.md
└── assets/ (optional)
```

---

## ⚙️ Requirements

Before running the project, install:

- C++ Compiler (g++)
- OpenGL
- GLUT / FreeGLUT

Recommended IDEs:
- CodeBlocks
- Visual Studio
- Dev-C++
- VS Code with MinGW

---

## ▶️ How to Run

### Using g++

```bash
g++ main.cpp -o FlapFury -lopengl32 -lglu32 -lfreeglut
./FlapFury
```

---

## 🎮 Controls

| Key | Action |
|-----|--------|
| SPACE | Jump / Start / Restart |
| ESC | Exit Game |

---

## 🧠 Core Concepts Used

### OpenGL Transformations
- `glTranslatef()`
- `glRotatef()`
- `glScalef()`

### Animation Techniques
- Wing animation
- Object movement
- Dynamic scaling
- Day-night transition

### Collision Detection
Axis-Aligned Bounding Box (AABB) collision system used for:
- Pipe collision
- Ground collision
- Ceiling collision

### Game Logic
- Scoring system
- Level progression
- Physics simulation
- Gravity and jump mechanics

---

## 🌌 Environment Effects

The game environment includes:

- Animated Sun & Moon
- Dynamic Sky Colors
- Stars appearing at night
- Moving Clouds
- Grass and Flowers
- Smooth Background Transitions

---

## 📈 Difficulty Scaling

The game automatically adjusts:
- Pipe movement speed
- Jump strength
- Gravity
- Pipe gap size

based on the current level.

---

## 🐉 Character System

Each level introduces a new animated character:
- Bird
- Rocket
- Butterfly
- UFO
- Dragon

Characters use:
- Rotation
- Scaling
- Animation effects
- Custom OpenGL drawing primitives

---

## 🔥 Future Improvements

Possible future updates:
- Sound effects and background music
- Power-ups
- High score saving system
- Multiplayer support
- Mobile version
- Texture-based graphics
- Particle effects

---

## 👨‍💻 Author

**Muhammad Joha**
**Shafi Mahmmod**
**Pallab Aich**
**Amir Hamza**
**Mostafizur Rahman**
---

## 📜 License

This project is for educational and academic purposes.
