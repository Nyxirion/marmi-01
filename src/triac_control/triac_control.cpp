#include "triac_control.h"

// ===== Parámetros =====
const unsigned int PULSO_TRIAC_US = 100;
const unsigned int RETARDO_MAX_US = 7550; // 0% potencia
const unsigned int RETARDO_MIN_US = 200;  // 100% potencia

// ===== Estado compartido ISR ↔ loop =====
volatile unsigned int intensidad = 0; // 0..1000 (0.1% resolución)
volatile unsigned long ultimoCruce = 0;
volatile bool pulsoActivo = false;

volatile unsigned int intensidad_vista_por_isr = 0;

// ===== ISR del cruce por cero =====
void zeroCrossISR()
{
    intensidad_vista_por_isr = intensidad;
  unsigned long ahora = micros();
  // Antirrebote: ignora pulsos a menos de 4 ms del anterior
  if ((ahora - ultimoCruce) < 4000UL)
    return;
  ultimoCruce = ahora;

  // 0% → no dispara nada
  if (intensidad == 0)
  {
    digitalWrite(TRIAC_PIN, LOW);
    TIMSK1 &= ~(1 << OCIE1A); // apaga interrupción del timer
    pulsoActivo = false;
    return;
  }

  // Intensidad → retardo (inverso: más intensidad, menos retardo)
  unsigned long retardo = map(intensidad, 0, 1000, RETARDO_MAX_US, RETARDO_MIN_US);

  pulsoActivo = false;
  TCNT1 = 0;                             // reinicia contador
  OCR1A = (unsigned int)(retardo * 2UL); // µs → ticks (0.5 µs/tick)
  TIFR1 |= (1 << OCF1A);                 // limpia flag pendiente
  TIMSK1 |= (1 << OCIE1A);               // habilita interrupción del timer
}

// ===== ISR del Timer1: enciende y apaga la compuerta =====
ISR(TIMER1_COMPA_vect)
{
  if (!pulsoActivo)
  {
    // Primer disparo: encender compuerta
    digitalWrite(TRIAC_PIN, HIGH);
    pulsoActivo = true;
    OCR1A = TCNT1 + (PULSO_TRIAC_US * 2); // reprograma para apagar en 100 µs
  }
  else
  {
    // Segundo disparo: apagar compuerta
    digitalWrite(TRIAC_PIN, LOW);
    pulsoActivo = false;
    TIMSK1 &= ~(1 << OCIE1A); // deshabilita hasta el próximo ZC
  }
}

// ===== Configuración del Timer1 =====
void configurarTimer1()
{
  noInterrupts();
  TCCR1A = 0; // modo Normal
  TCCR1B = 0;
  TCNT1 = 0;
  TCCR1B |= (1 << CS11);    // prescaler 8 → 0.5 µs/tick
  TIMSK1 &= ~(1 << OCIE1A); // arranca con interrupción apagada
  interrupts();
}

void triacConfigSetup()
{
  pinMode(ZERO_CROSS_PIN, INPUT_PULLUP); // pull-up por si el opto es open-collector
  pinMode(TRIAC_PIN, OUTPUT);
  digitalWrite(TRIAC_PIN, LOW);

  configurarTimer1();
  attachInterrupt(digitalPinToInterrupt(ZERO_CROSS_PIN), zeroCrossISR, FALLING);
}