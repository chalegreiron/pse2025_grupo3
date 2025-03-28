// Configurações do LCD
sbit LCD_RS at RB4_bit;
sbit LCD_EN at RB5_bit;
sbit LCD_D4 at RB0_bit;
sbit LCD_D5 at RB1_bit;
sbit LCD_D6 at RB2_bit;
sbit LCD_D7 at RB3_bit;

sbit LCD_RS_Direction at TRISB4_bit;
sbit LCD_EN_Direction at TRISB5_bit;
sbit LCD_D4_Direction at TRISB0_bit;
sbit LCD_D5_Direction at TRISB1_bit;
sbit LCD_D6_Direction at TRISB2_bit;
sbit LCD_D7_Direction at TRISB3_bit;

// Botão único em RA1
#define BTN    PORTA.F1

// LEDs para cada bebida (Active HIGH)
#define LED_HEINEKEN  PORTA.F2  // RA2 (D1)
#define LED_SKOL      PORTA.F3  // RA3 (D2)
#define LED_BRAHMA    PORTA.F4  // RA4 (D3)
#define LED_ANTARTICA PORTA.F5  // RA5 (D4)

// Variáveis globais
char *chops[4] = {"Heineken", "Skol", "Brahma", "Antartica"};
char *tamanhos[3] = {"Pequeno", "Medio", "Grande"};
int selected_chop = 0;
int selected_tamanho = 0;
enum {INICIO, ESCOLHA_CHOP, ESCOLHA_TAMANHO, AGUARDE} estado = INICIO;

// Protótipos das funções
void mostrarMensagemInicial();
void mostrarMenuChop();
void mostrarMenuTamanho();
void acenderLedBebida();
void desligarLEDs();
void mostrarAguarde();

// Função para mensagem inicial (5s)
void mostrarMensagemInicial() {
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_FIRST_ROW);
    Lcd_Out_CP("Bem-vindo a");
    Lcd_Cmd(_LCD_SECOND_ROW);
    Lcd_Out_CP("maquina de chop");
}

// Função para mostrar menu de chops
void mostrarMenuChop() {
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_FIRST_ROW);
    Lcd_Out_CP("Escolha o Chop:");
    Lcd_Cmd(_LCD_SECOND_ROW);
    Lcd_Out_CP(chops[selected_chop]);
}

// Função para mostrar menu de tamanhos
void mostrarMenuTamanho() {
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_FIRST_ROW);
    Lcd_Out_CP("Escolha o Tamanho:");
    Lcd_Cmd(_LCD_SECOND_ROW);
    Lcd_Out_CP(tamanhos[selected_tamanho]);
}

// Função para desligar todos os LEDs
void desligarLEDs() {
    PORTA &= 0xC3; // Mantém RA0, RA1 e limpa RA2-RA5 (0b11000011)
}

// Função para acender o LED da bebida selecionada
void acenderLedBebida() {
    desligarLEDs();
    switch(selected_chop) {
        case 0: LED_HEINEKEN = 1; break;   // Heineken
        case 1: LED_SKOL = 1; break;       // Skol
        case 2: LED_BRAHMA = 1; break;     // Brahma
        case 3: LED_ANTARTICA = 1; break;  // Antarctica
    }
}

// Função para mostrar "Aguarde..." com tempo variável
void mostrarAguarde() {
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_FIRST_ROW);
    Lcd_Out_CP("Aguarde...");
    acenderLedBebida();

    // Tempos corrigidos (sem duplicação)
    switch(selected_tamanho) {
        case 0: Delay_ms(3000); break; // Pequeno (3s)
        case 1: Delay_ms(5000); break; // Médio (5s)
        case 2: Delay_ms(7000); break; // Grande (7s)
    }

    desligarLEDs();
}

// Função principal
void main() {
    // Inicializa o LCD
    Lcd_Init();

    // Configura RA1 como entrada, outros como saída
    TRISA = 0x02; // 0b00000010 (apenas RA1 como entrada)
    CMCON = 0x07; // Desativa comparadores
    OPTION_REG.NOT_RBPU = 0; // Habilita pull-ups

    // Garante LEDs desligados no início
    desligarLEDs();

    // Mensagem de boas-vindas (5 segundos)
    estado = INICIO;
    mostrarMensagemInicial();
    Delay_ms(5000);

    // Menu principal
    estado = ESCOLHA_CHOP;
    mostrarMenuChop();

    // Loop infinito
    while(1) {
        if (BTN == 0) {
            Delay_ms(50); // Debounce

            if (BTN == 0) {
                unsigned int tempo_pressionado = 0;

                // Mede tempo de pressionamento
                while(BTN == 0 && tempo_pressionado < 2000) {
                    Delay_ms(10);
                    tempo_pressionado += 10;
                }

                if (tempo_pressionado >= 1000) { // Pressionamento longo (seleciona)
                    if (estado == ESCOLHA_CHOP) {
                        estado = ESCOLHA_TAMANHO;
                        selected_tamanho = 0;
                        mostrarMenuTamanho();
                    }
                    else if (estado == ESCOLHA_TAMANHO) {
                        estado = AGUARDE;
                        mostrarAguarde();
                        estado = ESCOLHA_CHOP;
                        mostrarMenuChop();
                    }
                }
                else { // Pressionamento curto (rola opções)
                    if (estado == ESCOLHA_CHOP) {
                        selected_chop = (selected_chop + 1) % 4;
                        mostrarMenuChop();
                    }
                    else if (estado == ESCOLHA_TAMANHO) {
                        selected_tamanho = (selected_tamanho + 1) % 3;
                        mostrarMenuTamanho();
                    }
                }
                while(BTN == 0); // Espera soltar o botão
            }
        }
        Delay_ms(10);
    }
}