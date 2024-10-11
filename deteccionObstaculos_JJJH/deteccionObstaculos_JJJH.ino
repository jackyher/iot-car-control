// Motor derecho
int in1 = 14;
int in2 = 27;
int enA = 13; // Enable para la velocidad del motor A

// Motor izquierdo
int in3 = 26;
int in4 = 25;
int enB = 33; // Enable para la velocidad del motor B

// Pines de sensor ultrasónico
int trig = 32;
int echo = 35;
long duration;
int distance;

int distanciaMin = 40; //Distancia mínima para detectar obstáculo

void setup() {
  // Configurar los pines como salida
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(enA, OUTPUT);

  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(enB, OUTPUT);

  // Configurar los pines del ultrasónico
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  // Inicializamos con el carrito detenido
  controlCarrito(0, 1000, 0, 0);

  Serial.begin(9600);
}

// Matriz de movimientos
int movimientos[5][4] = {
  {0, 0, 0, 0}, // 0 Detenerse
  {1, 0, 0, 1}, // 1 Avanzar
  {0, 1, 1, 0}, // 2 Retroceder
  {1, 0, 1, 0}, // 3 Giro derecha
  {0, 1, 0, 1}, // 4 Giro izquierda
};

// Función para controlar los motores
void controlCarrito(int moveIndex, int time, int speedA, int speedB) {
  // Motor A
  digitalWrite(in1, movimientos[moveIndex][0]);
  digitalWrite(in2, movimientos[moveIndex][1]);
  analogWrite(enA, (moveIndex == 0) ? 0 : speedA); // Si es detenerse, velocidad 0

  // Motor B
  digitalWrite(in3, movimientos[moveIndex][2]);
  digitalWrite(in4, movimientos[moveIndex][3]);
  analogWrite(enB, (moveIndex == 0) ? 0 : speedB); // Si es detenerse, velocidad 0

  delay(time);
}

// Función para medir la distancia del ultrasónico
int medirDistancia() {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  duration = pulseIn(echo, HIGH);
  distance = duration * 0.034 / 2;

  if (distance < 3) {
    distance = distanciaMin; // Obstáculo muy cerca
  }

  return distance;
}

void loop() {
  // Avanzar
  controlCarrito(1, 200, 255, 255);
  distance = medirDistancia();
  Serial.println(distance);

  // Verificar si hay un obstáculo delante
  if (distance <= distanciaMin) {
    controlCarrito(0, 500, 0, 0); // Detenerse
    delay(300);
    
    // Probar un giro progresivo a la derecha
    for (int i = 0; i < 3; i++) {
      controlCarrito(3, 500, 255, 255); // Girar a la derecha
      delay(300);
      
      // Medir distancia después de girar
      distance = medirDistancia();
      if (distance > distanciaMin) {
        break; // Si no hay obstáculo, salir del bucle
      }
    }

    // Si aún hay un obstáculo, probar giro a la izquierda
    if (distance <= distanciaMin) {
      for (int i = 0; i < 3; i++) {
        controlCarrito(4, 500, 255, 255); // Girar a la izquierda
        delay(300);
        
        distance = medirDistancia();
        if (distance > distanciaMin) {
          break;
        }
      }
    }

    // Si sigue habiendo obstáculo, retroceder
    if (distance <= distanciaMin) {
      controlCarrito(3, 500, 255, 255); // Girar a la derecha
      controlCarrito(2, 1000, 255, 255); // Retroceder
      delay(500);
    }
  }
}
