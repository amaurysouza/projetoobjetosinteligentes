  #include <SPI.h>
  #include <Ethernet.h>
  #include <PubSubClient.h>
  
  // Set your MAC address and IP address here
  byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
  IPAddress ip(192, 168, 0, 177);
  const char* statusIrrigacao = "";
  const char* server = "hairdresser.cloudmqtt.com";
  const char* user = "oakutdhd";
  const char* pass = "bHXPbMBnM0r2";
  // Ethernet and MQTT related objects
  EthernetClient ethClient;
  PubSubClient mqttClient(ethClient);

  #define pinoAnalog A0 // Define o pino A0 como "pinoAnalog" - De onde vem os dados do sensor
  #define pinoRele 8 // Define o pino 8 como "pinoRele" - Envia comandos para ativar e desativar o motor
  #define pino5V 7 // Define o pino 7 como "pino5V" - pino que alimenta o sensor
  
  int ValAnalogIn; // Variável do valor analógico de umidade
  
  void setup() {
    Serial.begin(9600); 
    Serial.println("Inicializando"); // Imprime a frase no monitor serial
    
      // Start the ethernet connection
    Ethernet.begin(mac, ip);           
    
    delay(5000);      
      // Set the MQTT server to the server stated above and the broker's port
    mqttClient.setServer(server, 15427);//                    
   
    while (!mqttClient.connected()){ 
  
   
    // Attempt to connect to the server with the ID "myClientID"
    if (mqttClient.connect("myClientID", user, pass)) 
    {
      Serial.println("Connection has been established, well done");
   
      // Establish the subscribe event
      mqttClient.setCallback(subscribeReceive);
    } 
    else 
    {
      Serial.println("Looks like the server connection failed...");
    }
    }
    mqttClient.subscribe("projetorega");
    
    pinMode(pinoRele, OUTPUT); // Declara o pinoRele como Saída
    pinMode(pino5V, OUTPUT); // Declara o pino5V como Saída
    digitalWrite(pino5V, HIGH); // Põem o pino5V em estado Alto = 5V
    
  }
  
  void loop() {
    // This is needed at the top of the loop!
    mqttClient.loop();
    
    ValAnalogIn = analogRead(pinoAnalog); // Guarda o valor de pinoAnalog (o sensor) na variável
    int Porcentagem = map(ValAnalogIn, 1023, 0, 0, 100); // Converte o valor em porcentagem
  
    Serial.print(Porcentagem); // Imprime o valor em porcentagem no monitor Serial
    Serial.println("%"); // Imprime o símbolo junto ao valor encontrado
  
    if (Porcentagem <= 30) { // Se a porcentagem for menor ou igual à
      statusIrrigacao = "Irrigando";
      Serial.println("Irrigando."); // Imprime a frase no monitor serial
      digitalWrite(pinoRele, HIGH); // Altera o estado do pinoRele para nível Alto
    }
  
    else { //
      statusIrrigacao = "Umidade adequada";
      Serial.println("Nível de água adequado"); // Imprime a frase no monitor serial
      digitalWrite(pinoRele, LOW); // Altera o estado do pinoRele para nível Baixo
    }
     if(mqttClient.publish("projetorega", statusIrrigacao))
    {
      Serial.println("Publish message success");
    }
    else
    {
      Serial.println("Could not send message :(");
    }
    delay (1000); // Estabelece o tempo de 1s para reinicializar a leitura
  }
  
  
  void subscribeReceive(char* topic, byte* payload, unsigned int length)
  {
    // Print the topic
    Serial.print("Topic: ");
    Serial.println(topic);
   
    // Print the message
    Serial.print("Message: ");
    for(int i = 0; i < length; i ++)
    {
      Serial.print(char(payload[i]));
    }
   
    // Print a newline
    Serial.println("");
  }
