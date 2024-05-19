# Sensor

Objetivo principal é medir a distância utilizando um sensor ultrassônico e enviar essa informação para um servidor MQTT. Aqui está uma visão simplificada do que ele faz:

1 Configuração Inicial: Conecta-se à rede WiFi e ao servidor MQTT.
2 Medição da Distância: Utiliza um sensor ultrassônico para medir a distância.
3 Publicação da Distância: Se a distância medida for inferior a 20 cm, publica essa informação em um tópico MQTT.
4 Atualização do LCD: Se a distância for superior a 20 cm, atualiza um display LCD com a distância medida.
5 Reconexão MQTT: Reconecta-se automaticamente ao servidor MQTT se a conexão for perdida.


Diagrama:  

![image](https://github.com/silviaribeiro/Projeto_Arduino/assets/86807769/a5fe5868-fc29-4933-ade7-b02bafe16458)
