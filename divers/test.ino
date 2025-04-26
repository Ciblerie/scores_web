#include <WiFi.h>
    #include <HTTPClient.h>
    #include <time.h> // Pour obtenir l'heure via NTP

    // Configuration WiFi
    const char* ssid = "Livebox-9410"; // Remplacez par votre SSID WiFi
    const char* password = "37322944"; // Remplacez par votre mot de passe WiFi

    // Configuration Firestore API
    const char* firestoreBaseUrl = "https://firestore.googleapis.com/v1/projects/ciblerie-2-0/databases/(default)/documents/scores"; // URL pour accéder à la collection `scores`

    // Configuration du serveur NTP pour l'heure UTC
    const char* ntpServer = "pool.ntp.org";
    const long gmtOffset_sec = 0;       // Décalage UTC (aucun décalage)
    const int daylightOffset_sec = 0;  // Pas de décalage pour UTC

    void setup() {
    Serial.begin(115200);
    connectToWiFi();

    // Configurer le serveur NTP pour UTC
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer); // Synchroniser avec le serveur NTP
    Serial.println("⌛ Synchronisation de l'heure...");

    // Attendre que l'heure soit synchronisée
    while (time(nullptr) < 100000) {
    delay(100);
    Serial.print(".");
    }

    // *** Début de la partie test pour l'envoi du pseudo et score ***
    //
    // Générer des valeurs aléatoires pour les tests
    randomSeed(analogRead(15));
    // Test rapide d'envoi des données
    String testPseudo = "Nina"; // Pseudo
    int testScore = random(5, 1000); // Score de 5 à 1000 en aléatoire pour les tests
    sendScoreToFirestore(testPseudo, testScore);
    //
    // *** FIN de la partie test pour l'envoi du pseudo et score ***
    }

    void loop() {
    // Boucle vide pour les tests
    }

    void connectToWiFi() {
    Serial.println("⌛ Connexion au WiFi...");
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    }

    Serial.println("\n✅ Connecté au WiFi.");
    Serial.print("Adresse IP locale : ");
    Serial.println(WiFi.localIP());
    }

    String getCurrentTimestampUTC() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
    Serial.println("❌ Impossible d'obtenir l'heure UTC.");
    return "";
    }

    // Formatter l'heure en ISO 8601 UTC
    char buffer[30];
    strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", &timeinfo);
    return String(buffer);
    }

    void sendScoreToFirestore(String pseudo, int score) {
    HTTPClient http;
    http.begin(firestoreBaseUrl);
    http.addHeader("Content-Type", "application/json");

    // Obtenir le timestamp UTC actuel
    String timestamp = getCurrentTimestampUTC();

    if (timestamp == "") {
    Serial.println("❌ Impossible d'envoyer les données. Timestamp non valide.");
    return;
    }

    // Préparation des données JSON pour Firestore
    String jsonData = "{"
    "\"fields\": {"
    "\"pseudo\": {\"stringValue\": \"" + pseudo + "\"},"
    "\"score\": {\"integerValue\": " + String(score) + "},"
    "\"timestamp\": {\"stringValue\": \"" + timestamp + "\"}"
    "}"
    "}";

    Serial.println("📡 Envoi de score à Firestore...");
    Serial.println("📄 Données : " + jsonData);

    // Envoi de la requête POST
    int httpResponseCode = http.POST(jsonData);

    if (httpResponseCode > 0) {
    Serial.println("✅ Données envoyées à Firestore avec succès.");
    String response = http.getString();
    Serial.println("📨 Réponse Firestore : \"" + response + "\"");
    } else {
    Serial.println("❌ Erreur lors de l'envoi : " + String(httpResponseCode));
    }

    http.end();
    }
