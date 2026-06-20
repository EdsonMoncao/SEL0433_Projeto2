// Edson Rodrigues Monção Neto - 14566780

// Configurações dos pinos do LCD
sbit LCD_RS at LATD4_bit;
sbit LCD_EN at LATD5_bit;
sbit LCD_D4 at LATD0_bit;
sbit LCD_D5 at LATD1_bit;
sbit LCD_D6 at LATD2_bit;
sbit LCD_D7 at LATD3_bit;

sbit LCD_RS_Direction at TRISD4_bit;
sbit LCD_EN_Direction at TRISD5_bit;
sbit LCD_D4_Direction at TRISD0_bit;
sbit LCD_D5_Direction at TRISD1_bit;
sbit LCD_D6_Direction at TRISD2_bit;
sbit LCD_D7_Direction at TRISD3_bit;

// Variáveis
int tempo_restante = 0;
short contando = 0;
short modo_selecionado = 0;
int contador_auxiliar = 0;
short limpar_tela = 0;

// Rotina de interrupções
void interrupt() {
    // Botão de tempo curto
    if (INTCON.INT0IF) {
        if (!contando) { modo_selecionado = 1; tempo_restante = 10; }
        INTCON.INT0IF = 0; // Limpa a flag
    }

    // Botão de tempo longo
    if (INTCON3.INT1IF) {
        if (!contando) { modo_selecionado = 2; tempo_restante = 60; }
        INTCON3.INT1IF = 0;
    }

    // Botão iniciar
    if (INTCON3.INT2IF) {
        if (modo_selecionado > 0 && !contando) {
            contando = 1;
            limpar_tela = 1; // Avisa o main() para limpar o display

            // Inicia timers
            if (modo_selecionado == 1) {
                T1CON = 0x31;
                PIE1.TMR1IE = 1;
            } else {
                T0CON = 0x84;
                INTCON.TMR0IE = 1;
            }
        }
        INTCON3.INT2IF = 0;
    }

    // Timer0 - Estouro a cada 1 segundo
    if (INTCON.TMR0IF) {
        TMR0H = 0x0B; TMR0L = 0xDC;
        if (tempo_restante > 0) tempo_restante--;
        INTCON.TMR0IF = 0;
    }

    // Timer1 - Estouro a cada 250ms
    if (PIR1.TMR1IF) {
        TMR1H = 0x0B; TMR1L = 0xDC;
        contador_auxiliar++;
        if (contador_auxiliar >= 4) { // 4x 250ms = 1 segundo
            contador_auxiliar = 0;
            if (tempo_restante > 0) tempo_restante--;
        }
        PIR1.TMR1IF = 0;
    }
}

void main() {
    unsigned int leitura_adc;
    unsigned long temp_calculada;
    char txt_temp[10];
    char txt_tempo[10];

    // Configuração
    TRISA = 0xFF;  // Entradas Analógicas
    TRISB = 0xFF;  // Botões
    TRISC.RC0 = 0; // LED da resistência
    PORTC.RC0 = 0;

    // Configuração de interrupções externas
    INTCON2.INTEDG0 = 1; // Borda de subida
    INTCON2.INTEDG1 = 1;
    INTCON2.INTEDG2 = 1;
    INTCON.INT0IE = 1;
    INTCON3.INT1IE = 1;
    INTCON3.INT2IE = 1;
    INTCON.GIE = 1; // Ativa interrupções globais
    INTCON.PEIE = 1;

    // Inicialização do LCD
    Lcd_Init();
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_CURSOR_OFF);

    // Inicialização do ADC
    ADC_Init();
    ADCON1 = 0x3B; // Seta pinos AN2 (Vref-) e AN3 (Vref+) para alimentação externa

    while(1) {
        // Limpa a tela fora das interrupções de forma segura
        if (limpar_tela == 1) {
            Lcd_Cmd(_LCD_CLEAR);
            limpar_tela = 0;
        }

        if (contando) {
            // Usa ADC_Get_Sample para Vref externo
            leitura_adc = ADC_Get_Sample(0);

            // Cálculo da temperatura
            temp_calculada = ((unsigned long)leitura_adc * 1000) / 1023;

            // Formatação do Display para "XX.X °C" (ou "100.0 °C")
            if (temp_calculada >= 1000) {
                // Caso atinja 100 graus ou sature
                txt_temp[0] = '1'; txt_temp[1] = '0'; txt_temp[2] = '0';
                txt_temp[3] = '.'; txt_temp[4] = '0';
            } else {
                txt_temp[0] = ((temp_calculada / 100) % 10) + '0'; // Dezena
                txt_temp[1] = ((temp_calculada / 10) % 10) + '0';  // Unidade
                txt_temp[2] = '.';
                txt_temp[3] = (temp_calculada % 10) + '0';         // Decimal
                txt_temp[4] = '\0';
            }

            // Adicionando os símbolos
            txt_temp[5] = 223; // Símbolo para a temperatura
            txt_temp[6] = 'C';
            txt_temp[7] = '\0';

            // Escrevendo a temperatura formatada no LCD
            Lcd_Out(1, 1, "Temp: ");
            Lcd_Out(1, 7, txt_temp);

            // Controle do LED
            // temp_calculada armazena as casas decimais. 60°C é 600, 80°C é 800.
            if (temp_calculada < 600) {
                PORTC.RC0 = 1; // Abaixo de 60: Liga
            } else if (temp_calculada > 800) {
                PORTC.RC0 = 0; // Acima de 80: Desliga
            }

            // Exibição do Tempo
            txt_tempo[0] = (tempo_restante / 10) + '0';
            txt_tempo[1] = (tempo_restante % 10) + '0';
            txt_tempo[2] = 's';
            txt_tempo[3] = '\0';
            Lcd_Out(2, 1, "Tempo: ");
            Lcd_Out(2, 8, txt_tempo);
            Lcd_Out(2, 11, "      ");

            // Fim do tempo
            if (tempo_restante <= 0) {
                contando = 0;
                modo_selecionado = 0;
                PORTC.RC0 = 0;     // Desliga resistência por segurança
                INTCON.TMR0IE = 0; // Desliga Timer0
                PIE1.TMR1IE = 0;   // Desliga Timer1
                limpar_tela = 1;   // Limpa a tela para a mensagem inicial
            }
        } else {
            Lcd_Out(1, 1, "Selecione tempo:");
            if (modo_selecionado == 1) Lcd_Out(2, 1, "Curto: 10s      ");
            else if (modo_selecionado == 2) Lcd_Out(2, 1, "Longo: 60s      ");
            else Lcd_Out(2, 1, "                ");
        }
        Delay_ms(200);
    }
}