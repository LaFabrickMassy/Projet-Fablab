#include <D:\user\Documents\01-Sources\RobotC2\Projet-Fablab\Robotique\telemetry\Telemetry.h>


class RobotTelemetry : public Telemetry {
public:
    void onConnection (bool Connected) override {
        Serial.println(Connected ? "Le client est connecte" : "Le client est deconnecte");
    }

    void onSetMotorSpeed (int8_t MotorLeft, int8_t MotorRight) override {
        // Affiche la consigne reçue
        Serial.print("Vitesse: ");
        Serial.print(MotorLeft);
        Serial.print(", ");
        Serial.println(MotorRight);

        //-- Simule un mouvement et une détection et retourne ces infos
        logPosition(MotorLeft, MotorRight, MotorRight-MotorLeft, 300, 1000);
    }
    
    //-- Callback de traitement des commandes
    void onCommand (enum ECommand Command) override {
        Serial.print("Commande: ");
        Serial.println(Command);
        std::string text = "Bien reçu ";
        text += Command;
        logInfo(text);
    }

} *theTelemetry;



void setup() {
    // put your setup code here, to run once:

    Serial.begin(115200);

    theTelemetry = new RobotTelemetry();
}

void loop() {
    // put your main code here, to run repeatedly:
    theTelemetry->loop();
}
