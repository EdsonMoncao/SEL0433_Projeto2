**Autor:** Edson Rodrigues Monção Neto (Nº USP: 14566780)  

## 1. Introdução
Este projeto propõe a criação de um dispositivo embarcado para aferição de temperatura e tempo de um forno industrial. O sistema permite ao usuário selecionar uma contagem regressiva (curta ou longa duração) e monitora continuamente a temperatura interna, exibindo os dados em um display LCD.

## 2. Descrição do Projeto
O projeto foi implementado utilizando o microcontrolador **PIC18F4550** programado em linguagem C (MikroC PRO for PIC) e simulado através do software **SimulIDE**. Os principais recursos de hardware e software utilizados foram:
* Display LCD (16x2)
* Interrupções Externas (INT0, INT1, INT2)
* Timers (TMR0 e TMR1)
* Conversor A/D (ADC)

## 3. Resultados e Discussão
O sistema atendeu a todos os requisitos solicitados. A contagem regressiva funciona perfeitamente baseada nas interrupções dos Timers, sem travar o processamento principal. 
Um destaque do código é o processamento da temperatura: para evitar o alto consumo de memória gerado pelo uso de variáveis do tipo `float`, a matemática foi feita com números inteiros (multiplicando a leitura por 1000) e a formatação da string do LCD foi feita isolando os dígitos matematicamente, resultando no formato "XX.X °C".
Além disso, a lógica da resistência do forno foi implementada com sucesso em um LED conectado ao pino RC0, acendendo para temperaturas abaixo de 60°C e apagando quando a temperatura ultrapassa 80°C.

## 4. Imagens do Projeto

**Compilação no MikroC (Sucesso e Uso de Memória):**
<img width="1470" height="282" alt="Build - MikroC" src="https://github.com/user-attachments/assets/05ca2372-1c2c-48dd-b037-d2fb5e6f9ac5" />

**Simulações no SimulIDE:**
<img width="1082" height="767" alt="Circuito PF - LED desligado" src="https://github.com/user-attachments/assets/7d536442-2846-4680-80f3-6e7471740871" />
<img width="1077" height="740" alt="Circuito PF - LED ligado" src="https://github.com/user-attachments/assets/027edc8c-32d2-43c6-9eee-ce77b72607c0" />
