# Núcleo de pesquisa - VSAT

Esta missão possui o objetivo principal de Desenvolver um estudo sobre a aplicação da technologia [LoRa](https://en.wikipedia.org/wiki/LoRa) para a realização de telemetria em nanosatelites de baixo custo. O objetivo principal é validar através de simulações, softwares e pesquisa a transmissão a 35 Km. 

O projeto possui dois módulos principais, **estação base (EB)** e **nanosatelite (NS)**.

![Diagrama visão geral](/images/Usecase.png)

## Inicialização
Para iniciar a utilização deste projeto:

1. Instale as ferramentas descritas na sessão *Ferramentas*.

## Ferramentas
* [IDE Arduino](https://www.arduino.cc/en/main/software) - A IDE arduino será utilizada para o desenvolvimento do projeto.

## Links importantes
* [Make clean code and be happy!](https://medium.com/coding-skills/clean-code-101-meaningful-names-and-functions-bf450456d90c) -  Um código limpo facilita o trabalho em equipe.

# MISSÃO 1

## Introdução

## Análise técnica

### Levantamento de requisitos  
Os requisitos foram levantados pela equipe VSAT. Este são requisitos especificamente ligados as funcionalidades da missão.

### Requisitos Funcionais
Respeitando a proposta, o sistema deverá atender os seguintes requisitos:

* **RF1** - Sensoreamento de temperatura.
* **RF2** - Sensoreamento sensor de altitude.
* **RF3** - Sensoreamento sensor de pressão.
* **RF4** - Armazenar dados sensoreados.
* **RF5** - Temporizador para telemetria.
* **RF6** - Localizador.
* **RF7** - Telemetria na estação de solo identificanda com aviso sonoro/luminoso.

## Requisitos Não-Funcionais

* **RNF1** - Utilizar tecnologia *LORA 915T30D* para comunicação.
* **RNF2** - Utilizar *CSV* para armazenamento.
* **RNF3** - Utilizar módulo *RTC - DS3231* para temporização.
* **RNF4** - Utilizar módulo *GPS NEO6M*/*Tracker - GF07* para localização.

## Regras de Negócio

1. **RGN1** - *Tempo de armazenamento*

O armazenamento será realizado a cada 1 minuto em SDCard através do formato CSV.   


2. **RGN2** - *Tempo de telemetria*

A telemetria será realizada a cada x minuto, enviando dados para a **EB**.   

3. **RGN3** - *Formato CSV*

As informações serão armazenadas em formato CSV:

```
RTC, temperatura, pressão, altitude, localização
```

## Casos de Uso

1. **UC1** - *Recepção de informação*

Para verificar as informações recebidas será utilizada a estação base **EB**.

***Visão do sistema***

```
RTC, temperatura, pressão, altitude, localização
```

# Implementação técnica

## Diagrama de casos de Uso
![Diagrama de casos de Uso](/images/Usecase.png)



## Cronograma

### ETAPA 1 - Documentação
Entrega da documentação de projeto.
**DATA:**

### ETAPA 2 - xx
Entrega de xx
**DATA:**

### ETAPA 3 - yy
Entrega de yy
**DATA:**

INSERIR UMA TABELA AQUI

## Sequencia de testes

