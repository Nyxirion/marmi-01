#include "pid.h"

void pid_config_init(pid_controller_t* pid){
    pid->integral = 0.0f;
    pid->prevError = 0.0f;
    pid->derivative = 0.0f;
    pid->output = 0.0f;
    pid->useAntiWindup = true; // this is set true by default for security reasons 
}

float computePID(pid_controller_t *pid, float measurement){
float error = pid->setpoint - measurement;
    float P = pid->kp * error;

    // 1. Calcular el término derivativo (sin cambios)
    pid->derivative = 2.0f * pid->kd * (error - pid->prevError) / (pid->sampleTime + 2.0f * pid->filterTau)
                    + (2.0f * pid->filterTau - pid->sampleTime) * pid->derivative 
                    / (2.0f * pid->filterTau + pid->sampleTime);

    // 2. Pre-calcular cuánto sumaríamos a la integral en este ciclo (Regla Trapezoidal)
    float integral_update = pid->ki * pid->sampleTime * 0.5f * (error + pid->prevError);

    // 3. Calcular una salida tentativa para evaluar si entraremos en saturación
    float tentativeOutput = P + pid->integral + integral_update + pid->derivative;

    // 4. Lógica de Clamping (Anti-Windup)
    if (pid->useAntiWindup) {
        // Si la salida excede el límite superior Y el error es positivo (quiere seguir subiendo)
        if (tentativeOutput > pid->maxOutputLim && error > 0.0f) {
            integral_update = 0.0f; // Cortamos (clamp) la integración
        }
        // Si la salida excede el límite inferior Y el error es negativo (quiere seguir bajando)
        else if (tentativeOutput < pid->minOutputLim && error < 0.0f) {
            integral_update = 0.0f; // Cortamos (clamp) la integración
        }
    }

    // 5. Aplicar la actualización a la integral real
    // Si hubo clamping, integral_update será 0. Si no, se suma normal.
    pid->integral += integral_update;

    // 6. Calcular la salida final y forzar límites duros
    float finalOutput = P + pid->integral + pid->derivative;

    if (finalOutput > pid->maxOutputLim) {
        pid->output = pid->maxOutputLim;
    } else if (finalOutput < pid->minOutputLim) {
        pid->output = pid->minOutputLim;
    } else {
        pid->output = finalOutput;
    }

    // Actualizar estados para el siguiente ciclo
    pid->prevError = error;

    return pid->output;
}