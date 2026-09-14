#include <PS2X_lib.h>  // Asegúrate de incluir la librería

// Pines de relés
const int re1 = 2; // Arma (X)
const int re2 = 3; // Arma (Círculo)
const int re3 = 4; // Movimiento
const int re4 = 5; // Movimiento
const int re5 = 6; // Movimiento
const int re6 = 7; // Movimiento

// Pines del mando PS2
#define PS2_DAT A0
#define PS2_CMD A1
#define PS2_SEL A2
#define PS2_CLK A3

#define pressures false
#define rumble false

PS2X ps2x;
int error = 0;

int movimientoActual = 0; // 0=ninguno, 1=arriba, 2=abajo, 3=izquierda, 4=derecha
int movimientoAnterior = 0; 

int botonEnclavado = 0;   // 0=ninguno, 5=X, 6=circulo

void apagarRelesMovimiento() {
  digitalWrite(re3, HIGH);
  digitalWrite(re4, HIGH);
  digitalWrite(re5, HIGH);
  digitalWrite(re6, HIGH);
}

void apagarRelesXY() {
  digitalWrite(re1, HIGH);
  digitalWrite(re2, HIGH);
}

void apagarTodosLosReles() {
  apagarRelesXY();
  apagarRelesMovimiento();
}

void setup() {
  Serial.begin(57600);

  pinMode(re1, OUTPUT);
  pinMode(re2, OUTPUT);
  pinMode(re3, OUTPUT);
  pinMode(re4, OUTPUT);
  pinMode(re5, OUTPUT);
  pinMode(re6, OUTPUT);

  apagarTodosLosReles();
  delay(300);

  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);

  if (error == 0) {
    Serial.println("Control PS2 conectado correctamente");
  } else {
    Serial.println("Error conectando control PS2");
  }
}

void loop() {
  if (error != 0) {
    apagarTodosLosReles();
    return;
  }

  ps2x.read_gamepad();

  // ==========================================
  // 1. LÓGICA DEL ARMA (ENCLAVADA) - SIN PARPADEOS
  // ==========================================
  bool cambioArma = false;

  if (ps2x.ButtonPressed(PSB_CROSS)) {
    botonEnclavado = (botonEnclavado == 5) ? 0 : 5;
    cambioArma = true;
  }

  if (ps2x.ButtonPressed(PSB_CIRCLE)) {
    botonEnclavado = (botonEnclavado == 6) ? 0 : 6;
    cambioArma = true;
  }

  // SOLO actualiza los relés del arma si realmente cambió el estado de los botones
  if (cambioArma) {
    apagarRelesXY();
    if (botonEnclavado == 5) digitalWrite(re1, LOW);
    if (botonEnclavado == 6) digitalWrite(re2, LOW);
  }

  // ==========================================
  // 2. LÓGICA DE MOVIMIENTO (MOMENTÁNEO)
  // ==========================================
  movimientoActual = 0;

  if (ps2x.Button(PSB_PAD_UP))    movimientoActual = 1;
  if (ps2x.Button(PSB_PAD_DOWN))  movimientoActual = 2;
  if (ps2x.Button(PSB_PAD_LEFT))  movimientoActual = 3;
  if (ps2x.Button(PSB_PAD_RIGHT)) movimientoActual = 4;

  // SOLO actualiza los relés de movimiento si cambió la dirección
  if (movimientoActual != movimientoAnterior) {
    apagarRelesMovimiento();

    if (movimientoActual == 1) { // Arriba
      digitalWrite(re3, LOW);
      digitalWrite(re6, LOW);
    }
    else if (movimientoActual == 2) { // Abajo
      digitalWrite(re4, LOW);
      digitalWrite(re5, LOW);
    }
    else if (movimientoActual == 3) { // Izquierda
      digitalWrite(re3, LOW);
      digitalWrite(re5, LOW);
    }
    else if (movimientoActual == 4) { // Derecha
      digitalWrite(re4, LOW);
      digitalWrite(re6, LOW);
    }
    
    movimientoAnterior = movimientoActual; // Guardar el estado actual
  }

  delay(30); // Reducido a 30ms para mayor velocidad de respuesta
}