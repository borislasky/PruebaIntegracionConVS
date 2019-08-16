#include "WiFiSinc.h"
#include "auth_wifi.h"

int sort_desc(const void *cmp1, const void *cmp2){
  // Por requerimiento de qsort, es necesario pasar de void * to int *
  int a = *((int *)cmp1);
  int b = *((int *)cmp2);
  return b - a;
}

void copia(char *aa, char *a, int longi){
  for(int i=0; i<longi; i++)
    aa[i] = a[i];
}

void OrdenaSSIDs_y_Passs(int *q, int longi){
  // q: array de calidad de señal
  // longi: tamaño del array

  int i, j, qq[MAX_SSIDs];
  char ss[MAX_SSIDs][MAX_L_SSID], pp[MAX_SSIDs][MAX_L_SSID];
  bool usado[MAX_SSIDs];
  
  // Copia ssids en ss, passs en pp y q en qq. Pone en false usado
  for(i=0; i<longi; i++){
    copia(ss[i], ssids[i], MAX_L_SSID);
    copia(pp[i], passs[i], MAX_L_SSID);
    qq[i] = q[i];
    usado[i] = false;
  }
  //  Ordena qq descendente
  qsort(qq, longi, sizeof(qq[0]), sort_desc);

  // Ordena s, p, q
  for(i=0; i<longi; i++)
    for(j=0; j<longi; j++){
      if(!usado[j] && qq[i] == q[j]){
        usado[j] = true;
        copia(ssids[i], ss[j], MAX_L_SSID);
        copia(passs[i], pp[j], MAX_L_SSID);
        break;
      }
    }
  for(i=0; i<longi; i++)
    q[i] = qq[i];
}

int busca(String ssid){
	for(int i=0; i<NUM_SSIDs; i++)
		if(!strcmp(ssid.c_str(), ssids[i]))
			return(i);
	return(-1);
}

int iniWiFi(const char *ssid, const char *pass, bool verbose, int reintentos) {
  int cont = 0;
  delay(100);
	if(verbose){
		Serial.print("Conectando a ");
		Serial.println(ssid);
	}
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if(verbose) Serial.print(".");
    cont++;
    if(cont == reintentos){
			if(verbose){
				Serial.println();
				Serial.print("Imposible conectar con ");
				Serial.println(ssid);
			}
      return(0);
    }
  }
	if(verbose){
		Serial.println("");
		Serial.print("WiFi conectada a ");
		Serial.print(ssid);
		Serial.print(" - dirección IP: ");
		Serial.println(WiFi.localIP());
	}
	return(1);
}


void separa(byte *res, char *s){
	int i = 0;
	char* ss = strtok(s, ".");
	while(ss != 0){
		res[i++] = atoi(ss);
		ss = strtok(0, ".");
	}
}

WiFiSinc::WiFiSinc(){
	this->verbose = true;
	this->reintentos = 20;
	this->accion = 0;
}

WiFiSinc::WiFiSinc(bool _verbose){
	this->verbose = _verbose;
	this->reintentos = 20;
	this->accion = 0;
}	

WiFiSinc::WiFiSinc(bool _verbose, int _reintentos){
	this->verbose = _verbose;
	this->reintentos = _reintentos;
	this->accion = 0;
	// Desconecto por si quedaba alguna conexión activa de anteriores reboots
	WiFi.disconnect();
	}

WiFiSinc::WiFiSinc(bool _verbose, int _reintentos, unsigned long _accion){
	this->verbose = _verbose;
	this->reintentos = _reintentos;
	this->accion = _accion;
}

int WiFiSinc::estado(){
	return WiFi.status();
}
	
int WiFiSinc::ConectaWiFi(){
	int nSSIDsUtiles = 0;
  String cSenal;
  int calidad[NUM_SSIDs] = {-255, -255, -255};
	
	// Primero hace un escaneo de los SSIDs que encuentra
	int num_ssids = WiFi.scanNetworks(); // Número de SSIDs	encontrados
	if(this->verbose)
		Serial.printf("Número de SSID encontrados: %d\n", num_ssids);
	
  for(int i=0; i<num_ssids; i++){
    int intensidad = WiFi.RSSI(i);
		int orden = busca(WiFi.SSID(i));
		if(orden >= 0){
			calidad[orden] = intensidad;
			nSSIDsUtiles++;
		}
    if(intensidad >= -60) cSenal = "Perfecta";
    else if(intensidad >= -67) cSenal = "Muy buena";
    else if(intensidad >= -70) cSenal = "Buena";
    else if(intensidad >= -80) cSenal = "Mala";
    else if(intensidad < -80)  cSenal = "No usable";
		if(this->verbose){
			Serial.print("- ");
			Serial.println(WiFi.SSID(i));
			Serial.print("\tIntensidad de señal: ");
			Serial.print(intensidad);
			Serial.print("dBm - ");
			Serial.println(cSenal);
		}
  }
	
	// Ordena las tablas de ssids y passwords por orden decreciente
	// de calidad de señal
	OrdenaSSIDs_y_Passs(calidad, NUM_SSIDs);

	// Intenta conexión con los SSIDs útiles
	for(int i=0; i<nSSIDsUtiles; i++)
		if(iniWiFi(ssids[i], passs[i], this->verbose, this->reintentos))
			return(1);
	return(0);
}

void WiFiSinc::IpEstatica(char *IP, char* Mascara, char *gateway){
	byte IPn[4], Mascn[4], gatewn[4], dns[4] = {8,8,8,8};
	separa(IPn, IP);
	separa(Mascn, Mascara);
	separa(gatewn, gateway);
	WiFi.config(IPn, dns, gatewn, Mascn);
	/*
	for(int i=0;i<4;i++){
		Serial.print(IPn[i]);
		Serial.print(" - ");
		Serial.print(Mascn[i]);
		Serial.print(" - ");
		Serial.println(gatewn[i]);
	}
	*/
}

IPAddress WiFiSinc::IP(){
	return(WiFi.localIP());
}