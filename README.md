# BMS_Public
# Sistema de Gestión de Baterías (BMS) para Motocicleta Eléctrica de Competición (120V / 30kW)

Este repositorio contiene el código fuente, los modelos de simulación, el diseño electrónico y las herramientas de validación de un BMS avanzado. El sistema está diseñado específicamente para gestionar el paquete de baterías (30S2P, 120 V, 40 Ah) de una motocicleta eléctrica de competición.

El proyecto implementa una arquitectura modular (Maestro-Esclavo) orientada a maximizar el rendimiento en pista y garantizar la seguridad mediante **balanceo activo inductivo** y un **control térmico dinámico**.

## 📂 Estructura del Repositorio

- **`/firmware`**: Código fuente en C/C++ para el microcontrolador ESP32 (entorno ESP-IDF). Basado en **FreeRTOS**, implementa una arquitectura multitarea que aísla la seguridad crítica de las comunicaciones. Incluye algoritmos de estimación de estados (SoC/SoP), control de contactores, gestión térmica y la pila de comunicaciones CAN Bus (TWAI).
- **`/hardware`**: Esquemáticos de diseño electrónico desarrollados en **KiCad**. Contiene la topología del módulo esclavo, integrando el front-end analógico (AFE) aislado y la etapa de potencia para la ecualización activa (convertidores DC-DC Buck-Boost bidireccionales).
- **`/Simulation`**: Modelos matemáticos y gemelos digitales desarrollados en **MATLAB/Simulink** y **Simscape Electrical**. Se utilizan para la validación teórica de la estrategia térmica en lazo cerrado, la viabilidad del balanceo en alta frecuencia y la respuesta del estimador de estados (EKF).
- **`/dashboard`**: Interfaz gráfica web (HTML/JS/CSS) que utiliza la *Web Serial API*. Actúa como panel de telemetría en tiempo real y permite la inyección de fallos controlados para validar la lógica del microcontrolador (validación Software-in-the-Loop).

## 🚀 Características Principales del Sistema

*   **Determinismo y Seguridad (RTOS)**: Uso de Multiprocesamiento Simétrico (SMP) en el ESP32 para separar el control de la red del vehículo (VCU/Inversor) de las rutinas de protección por hardware, garantizando tiempos de respuesta mínimos ante fallos.
*   **Balanceo Activo de Alta Corriente**: Topología no disipativa capaz de transferir energía entre celdas a >500 mA, evitando el sobrecalentamiento de la placa y maximizando la energía útil en escenarios de competición.
*   **Gestión Térmica Híbrida**: Lógica basada en el modelo termodinámico de Bernardi. Implementa advección proporcional (ventiladores PWM) y restricción predictiva de potencia (Derating de SoP) antes de alcanzar los límites críticos de degradación.
*   **Secuencia de Arranque Segura**: Algoritmo de acoplamiento y circuito de pre-carga automatizado para eliminar la corriente de irrupción (*Inrush Current*) que podría soldar los relés.

## 🛠️ Herramientas Utilizadas

*   **Software Embebido**: ESP-IDF v5.x, FreeRTOS, C/C++.
*   **Diseño Electrónico**: KiCad EDA.
*   **Control y Simulación**: MATLAB, Simulink, Simscape Electrical.
*   **Validación**: Interfaz web desplegable localmente compatible con navegadores Chromium.

---
**Autora:** Selene Masa Muñoz  
*Trabajo Fin de Grado en Ingeniería Electrónica, Robótica y Mecatrónica*  
*Universidad de Sevilla (2026)*BMS repository for PDF
