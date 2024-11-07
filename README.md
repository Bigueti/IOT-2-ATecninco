Monitoramento de Umidade com ESP32, MQTT, MySQL e Node-RED
Descrição do Projeto

Este projeto foi desenvolvido para monitorar a umidade de um ambiente utilizando a placa ESP32 e um sensor de umidade. A ESP32 coleta a umidade e envia os dados para um broker MQTT, enquanto o Node-RED processa e apresenta esses dados em um dashboard. Os dados também são armazenados em um banco de dados MySQL, em uma base chamada trabalhoiot. O projeto oferece funcionalidades como visualização de dados em tempo real, backups periódicos, e pesquisa histórica.
Tecnologias Utilizadas
ESP32: Placa de desenvolvimento utilizada para ler os dados do sensor de umidade e enviar as informações via MQTT.
MQTT: Protocolo de comunicação utilizado para transmitir os dados do ESP32 para o servidor.
MySQL: Banco de dados utilizado para armazenar as leituras de umidade, com informações de data, hora, mês e ano.
Node-RED: Plataforma de automação para integrar o MQTT com o MySQL e exibir os dados em um dashboard interativo.

Como Rodar o Projeto

1. Configuração do MQTT
O broker MQTT está configurado para aceitar conexões no IP IPv4 da sua rede local e na porta 18219, com autenticação via credenciais:
Username: teste
Password: teste
Você pode configurar seu próprio broker MQTT local ou utilizar um broker público, caso não tenha um. Certifique-se de atualizar o código da ESP32 para enviar os dados para o broker configurado.

2. Configuração do MySQL
Criação da Tabela no MySQL: O banco de dados trabalhoiot deve ser configurado com a seguinte tabela para armazenar as leituras de umidade:
CREATE DATABASE trabalhoiot;
USE trabalhoiot;

CREATE TABLE StatusUmid (
    Id INT AUTO_INCREMENT PRIMARY KEY,
    datahora DATETIME,
    status FLOAT,
    mes INT,
    ano INT
);
Configuração de Conexão: A conexão com o banco de dados MySQL será realizada a partir do Node-RED. Certifique-se de configurar as credenciais de acesso corretamente para que o Node-RED possa inserir os dados na tabela StatusUmid.
3. Configuração do Node-RED
Configuração do IP do Node-RED: O Node-RED está configurado para escutar no IP IPv4 da sua rede local e na porta 18250.

Dashboard do Node-RED: O dashboard para visualização dos dados pode ser acessado através do link:
http://<IP-DO-NODE-RED>:18250/IOTapresentacao

Funcionalidades do Dashboard:
Visualização em Tempo Real: Exibe os dados de umidade recebidos via MQTT.
Backup dos Dados: Permite realizar backup de todos os dados armazenados ou de dados de um período específico (ano, mês, dia).
Pesquisa Histórica: Permite pesquisar e visualizar os valores de umidade para um dia, mês e ano específicos.
Botão de Reset de Wi-Fi: Existe um botão que permite reiniciar a configuração de Wi-Fi da ESP32 diretamente do dashboard.