ORG 0000H

START:
MOV TMOD, #02H
MOV R7, #00H

AGUARDAR:
JNB P2.7, CONFIGURAR
SJMP AGUARDAR

CONFIGURAR:
CLR A
MOV B, #1EH
JB P2.2, SOMA4
RETORNO_SOMA4:
JB P2.1, SOMA2
RETORNO_SOMA2:
JB P2.0, SOMA1
RETORNO_SOMA1:
ADD A, #02H
MUL AB
MOV R5, A
JB P2.7, DESLIGAR
JB P2.6, HORARIO_INICIO
JNB P2.6, ANTIHORARIO_INICIO

SOMA4:
ADD A, #05H
SJMP RETORNO_SOMA4
SOMA2:
ADD A, #03H
SJMP RETORNO_SOMA2
SOMA1:
ADD A, #02H
SJMP RETORNO_SOMA1

HORARIO_INICIO:
MOV R7, #00H
CLR P1.2
SJMP CONTAGEM_TEMPO

HORARIO_PARAR:
SETB P1.2
MOV R5, #03H
SJMP CONTAGEM_TEMPO

HORARIO_DELTA:
CLR P1.3
SJMP PRONTO_HORARIO

ANTIHORARIO_INICIO:
MOV R7, #01H
CLR P1.6
SJMP CONTAGEM_TEMPO

ANTIHORARIO_PARAR:
SETB P1.6
MOV R5, #03H
SJMP CONTAGEM_TEMPO

ANTIHORARIO_DELTA:
CLR P1.5
SJMP PRONTO_ANTIHORARIO

CONTADOR_50MS:
MOV TL0, #0C0H
MOV TH0, #04DH
SETB TCON.4
JNB TCON.5, $
CLR TCON.4
DEC R5
SJMP CONTAGEM_TEMPO

CONTAGEM_TEMPO:
JNB P1.0, IGNORAR_OFF
JB P2.7, DESLIGAR
IGNORAR_OFF:
CJNE R5, #00H, CONTADOR_50MS
CLR TCON.4

FINALIZA_TEMPO:
MOV A, R7
JNB P1.2, HORARIO_PARAR
JNB P1.6, ANTIHORARIO_PARAR
JB ACC.1, ESPERA_3S_OFF
JNB ACC.0, HORARIO_DELTA
JB ACC.0, ANTIHORARIO_DELTA

PRONTO_HORARIO:
JB P2.7, DESLIGAR
JNB P2.6, INVERTER
SJMP PRONTO_HORARIO

PRONTO_ANTIHORARIO:
JB P2.7, DESLIGAR
JB P2.6, INVERTER
SJMP PRONTO_ANTIHORARIO

DESLIGAR:
SETB P1.2
SETB P1.6
SETB P1.3
SETB P1.5
MOV A, R7
SETB ACC.1
MOV R7, A
SJMP ESPERA_3S

INVERTER:
SJMP DESLIGAR

ESPERA_3S:
MOV R5, #4BH
CLR P1.0
SJMP CONTAGEM_TEMPO

ESPERA_3S_OFF:
MOV A, R7
CLR ACC.1
MOV R7, A
SETB P1.0
LJMP AGUARDAR