//-- La classe Telemetry gère la communication du robot avec un navigateur web
//-- Elle reçoit les ordres et retourne les mesures et informations utiles
//-- Elle doit être dérivée (class virtuelle pure) pour définir les callbacks
//-- Une seule instanciation d'une seule classe dérivée est autorisée

//-- Il faut un objet Telemetry le setup Arduino après avoir initialisé la classe Serial
//-- Il faut appeler loop dans le loop Arduino

//-- L'utilisation de la librairie requière l'installation de ArduinoWebsockets
//-- à l'aide du Gestionnaire de Librairies

#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoWebsockets.h>
#include <string>

#define HTTP_PORT 80
#define WS_PORT   81


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
        EMERGENCY_STOP,
		RETURN_HOME,
        CMD_3,
        CMD_4,
        CMD_5,
	};
	//-- Callback de traitement des commandes
	virtual void onCommand (enum ECommand Command) = 0;


	//-- Remonte la distance d'un obstacle par rapport à une position donnée du robot
	//-- Les angles sont positifs vers la droite en vue de dessus
    //-- Le repère est au choix du robot mais doit rester fixe au cours du temps
	//-- @x Coordonnée X en milimètre
	//-- @y Coordonnée Y en milimètre
	//-- @Heading Angle du robot par rapport à l'axe des X en milliradian
	//-- @RelativeDirection Angle relatif de la direction en milliiradian
	//-- @Distance Distance du mur détecté (ou en négatif, distance min sans mur)
	void logPosition(int16_t x, int16_t y, int16_t Heading, int16_t RelativeDirection, int16_t Distance);

    //-- Envoie un texte quelconque pour être logué
    void logInfo(const std::string &Info);

	//-- A appeler dans le loop() "Arduino" afin que Telemetry puisse à son tour appeler les callbacks
	void loop();

private:


	WebServer                    server; 
	websockets::WebsocketsServer webSocket;
	websockets::WebsocketsClient client;

	static Telemetry *theSingleton;

	static void onMessage(websockets::WebsocketsClient &client, websockets::WebsocketsMessage message);

	static void onEvent(websockets::WebsocketsClient &client, websockets::WebsocketsEvent event, String data);

	static void returnWebPage();

	static void return404Page();

};


Telemetry *Telemetry::theSingleton = nullptr;



Telemetry::Telemetry (bool AccessPoint, const char *Ssid, const char *Password)
: server(HTTP_PORT)
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
        webSocket.listen(WS_PORT);
        if (!webSocket.available()) {
            Serial.println("Websocket ne fonctionne pas");
        } else {
			server.on("/", returnWebPage);
			server.onNotFound(return404Page);

			// Démarrage serveur web
			server.begin();

			Serial.println("Serveur web actif");
			theSingleton = this;
        }
	}
}


void Telemetry::onMessage(websockets::WebsocketsClient &client, websockets::WebsocketsMessage message) {
	auto data = message.data();
    const char *text = data.c_str();
	Serial.print("Message recu : ");
	Serial.println(text);
    if (text[0] == 'M') {
        const char *sep = strchr (text, '|');
        if (sep != nullptr) {
            theSingleton->onSetMotorSpeed(atoi(text+1), atoi(sep+1));
        }
    } else if (text[0] == 'C') {
        theSingleton->onCommand((ECommand)(text[1]-'0'));
    }
}


void Telemetry::onEvent(websockets::WebsocketsClient &client, websockets::WebsocketsEvent event, String data) {
	if (event == websockets::WebsocketsEvent::ConnectionClosed) {
		Serial.println("Connection fermee");
        theSingleton->onConnection(false);
	}
}


void Telemetry::loop () {
	server.handleClient();
    if (webSocket.poll()) {
        Serial.println("Nouvelle connexion websocket demandee");
        if (!client.available()) {
            client = webSocket.accept();
            client.onMessage(onMessage);
            client.onEvent(onEvent);
            onConnection(true);
        } else {
            Serial.println("Un seul client autorisé. Connexion rejetée.");
        }
    }
    if (client.available()) {
        client.poll();
    }
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


void Telemetry::logInfo(const std::string &Info) {
    if (client.available()) {
        std::string LogData = "L";
        LogData += Info;
        Serial.print("Sending: ");
        Serial.println(LogData.c_str());
        client.send(LogData.c_str());
    } else {
        Serial.println("Client perdu???");
    }
}


void Telemetry::logPosition(int16_t x, int16_t y, int16_t Heading, int16_t RelativeDirection, int16_t Distance) {
    if (client.available()) {
        Serial.print("Pos ");
        Serial.print(x);
        Serial.print(",");
        Serial.print(y);
        Serial.print(")");
        Serial.print(Heading);
        Serial.print("/");
        Serial.print(RelativeDirection);
        Serial.print("/");
        Serial.println(Distance);
        char Data [10] = { 
            x & 0xFF, x >> 8, 
            y & 0xFF, y >> 8, 
            Heading & 0xFF, Heading >> 8, 
            RelativeDirection & 0xFF, RelativeDirection >> 8, 
            Distance & 0xFF, Distance >> 8, 
        };
        client.sendBinary(Data, sizeof(Data));
    }
}
