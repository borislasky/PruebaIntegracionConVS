/*
 * WiFiSinc.h
 *
 * Librería para la conexión **síncrona** a WiFi.
 * Permite varios SSIDs
 * Intenta conectarse por orden de mejor a peor señal
 * 
 * El array de SSIDs y passwords debe estar en auth_wifi.h
 *
 * En tiempo de inicialización se puede especificar:
 *   - Modo verbose. Default: true. Si queremos que vaya sacando en Serial
 *     lo que va haciendo.
 *   - Número de reintentos antes de pasar al siguiente SSID. Default: 20
 *   - Acción a realizar en el caso de que sea el último SSID y haya consumido el número de reintentos:
 *      - REBOOT = 0. Default. Reboota el dispositivo
 *      - Deep Sleep. Se le pasa el número de microsegundos que debe estar en este estado
 *
*/

#ifndef WIFI_SINC_h
#define WIFI_SINC_h

#include <ESP8266WiFi.h>

class WiFiSinc{
	public:
		WiFiSinc();
		WiFiSinc(bool);
		WiFiSinc(bool, int);
		WiFiSinc(bool, int, unsigned long);
		
		int estado();
		void IpEstatica(char*, char*, char*);
		int ConectaWiFi();
		IPAddress IP();

	private:
		int verbose; 		// Si true saca por Serial información sobre la conexión
		int reintentos; // Número de reintentos antes de pasar a otra conexión
										// o ejecutar la acción de reboot o deep sleep
		unsigned long accion;	// Si 0, reboot. Si >0 deep sleep durante accion microsegundos
		

};

#endif
