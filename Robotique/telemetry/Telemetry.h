#include <WiFi.h>
#include <WebServer.h>

//-- La classe Telemetry gère la communication du robot avec un navigateur web
//-- Elle reçoit les ordres et retourne les mesures et informations utiles
//-- Elle doit être dérivée (class virtuelle pure) pour définir les callbacks
//-- Une seule instanciation d'une seule classe dérivée est autorisée

//-- Il faut un objet Telemetry le setup Arduino après avoir initialisé la classe Serial
//-- Il faut appeler loop dans le loop Arduino

class Telemetry {

public:
	//-- Definit les paramètres Wifi et serveur (le serveur HTTP sera obligatoirement sur le port 80 et sur 81 pour WS)
	Telemetry (bool AccessPoint = true, const char *Ssid = "RobotLaFabrick", const char *Password = "LaF@BRICK r0b0t7 est dans la Pl@c3");

	//-- Callback appelée sur connexion ou déconnexion du browser
	virtual void onConnection (bool Connected) = 0;

	//-- Définit l'échelle de vitesse en m/s quand MotorXxxxx vaut 100 (ou -100)
	#define SPEED_SCALE  1.0
	
	//-- Appelé à chaque changement de consigne de vitesse valeurs -100 < MotorXxxxx < +100
	//-- Echelle définie par SPEEED_SCALE
	virtual void onSetMotorSpeed (int8_t MotorLeft, int8_t MotorRight) = 0;
	
	//-- La liste des commandes
	enum ECommand {
		RETURN_HOME,
	};
	//-- Callback de traitement des commandes
	virtual void onCommand (enum ECommand Command) = 0;


	//-- Remonte la distance d'un obstacle par rapport à une position donnée du robot
	//-- Les angles sont positifs vers la droite en vue de dessus
	//-- @x Coordonnée X en milimètre
	//-- @y Coordonnée Y en milimètre
	//-- @Heading Angle du robot par rapport à l'axe des X en milliradian
	//-- @RelativeDirection Angle relatif de la direction en milliiradian
	//-- @Distance Distance du mur détecté (ou en négatif, distance min sans mur)
	void logPosition(int16_t x, int16_t y, int16_t Heading, int16_t RelativeDirection, int16_t Distance);

	//-- A appeler dans le loop() "Arduino" afin que Telemetry puisse à son tour appeler les callbacks
	void loop();

private:


	WebServer server; 

	static Telemetry *theSingleton;

	static void returnWebPage();

	static void return404Page();

};


Telemetry *Telemetry::theSingleton = nullptr;



Telemetry::Telemetry (bool AccessPoint, const char *Ssid, const char *Password)
: server(80)
{
	if (theSingleton != nullptr)
		Serial.println("Seule une creation de Telemetry est autorisee");
	else {
		if (AccessPoint) {
			WiFi.mode(WIFI_AP);
			bool Success = WiFi.softAP(Ssid, Password);
			if (!Success) {
				Serial.println("Echec d'initialisation du Point Acces WiFi");
			} else {
				Serial.print("IP Robot: ");
				Serial.println(WiFi.softAPIP());
			}
		} else {
			WiFi.begin(Ssid, Password); 
			Serial.print("Attente de connexion...");
			while (WiFi.status() != WL_CONNECTED)
			{
				Serial.print(".");
				delay(200);
			}
			Serial.print(" connecté. IP ");
			Serial.println(WiFi.localIP());
			Serial.println("\n");  // Affichage "Connexion établie" et de l'adresse IP
		}
		server.on("/", returnWebPage);
		server.onNotFound(return404Page);
		Serial.println("Serveur web actif");
		server.begin();
		theSingleton = this;
	}
}


void Telemetry::loop () {
	server.handleClient();
}


void Telemetry::returnWebPage() {
	static const char *MainPage = 
#include "HomePage.h"
;

	theSingleton->server.setContentLength(strlen(MainPage)); 
	theSingleton->server.send(200, "text/html", MainPage);
}


void Telemetry::return404Page(){
	theSingleton->server.send(404, "text/plain", "404: Not found");
}
