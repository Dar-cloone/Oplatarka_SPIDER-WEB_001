#include "odrive_controller.h"

/* ===== inicjalizacja ===== */
void OdriveController::begin() {
    Serial1.begin(ODRIVE_UART_BAUD);
    delay(20);
    clearErrors();                       // skasuj poprzednie błędy
}

/* ===== sterowanie ===== */
void OdriveController::setTorque(float Nm)             { odrive_.setTorque(Nm);        }
void OdriveController::setPosition(float turns)        { odrive_.setPosition(turns);   }
void OdriveController::setVelocity(float turns_s)      { odrive_.setVelocity(turns_s); }
void OdriveController::setState(ODriveAxisState s)     { odrive_.setState(s);          }
void OdriveController::clearErrors()                   { odrive_.clearErrors();        }

/* ===== odczyty ===== */
float OdriveController::getPosition() { return odrive_.getPosition(); }
float OdriveController::getVelocity() { return odrive_.getVelocity(); }

/* ===== parametry ogólne ===== */
long  OdriveController::getParameterAsInt (const String& p) { return odrive_.getParameterAsInt(p);  }
float OdriveController::getParameterAsFloat(const String& p){ return odrive_.getParameterAsFloat(p);}
