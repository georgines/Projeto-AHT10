#include <stdio.h>
#include "pico/stdlib.h"
#include "Aht10.h"
#include "PortaSerial.h"
#include "u8g2pico.h"

#define UART_ID uart1
#define UART_BAUD_RATE 115200
#define UART_TX_PIN 8
#define UART_RX_PIN 9

#define AHT10_I2C_PORT i2c0
#define AHT10_SDA_PIN 0
#define AHT10_SCL_PIN 1

#define DISPLAY_I2C_PORT i2c1
#define DISPLAY_SDA_PIN 14
#define DISPLAY_SCL_PIN 15
#define DISPLAY_I2C_ADDRESS 0x3C

void inicializarDisplay(u8g2pico_t *u8g2);
void exibirDados(u8g2pico_t *u8g2, float temperatura, float umidade);

int main()
{
    PortaSerial portaSerial(UART_ID, UART_BAUD_RATE, UART_TX_PIN, UART_RX_PIN);
    if (!portaSerial.iniciar())
        return -1;

    Aht10 sensor(AHT10_I2C_PORT, AHT10_SDA_PIN, AHT10_SCL_PIN);
    u8g2pico_t u8g2;   

    sleep_ms(1000);

    portaSerial.enviarTextoComNovaLinha("Inicializando sensor...");
    sensor.inicializar();
    portaSerial.enviarTextoComNovaLinha("AHT10 inicializado com sucesso!");

    inicializarDisplay(&u8g2);
    portaSerial.enviarTextoComNovaLinha("Display inicializado.");

    while (true)
    {
        if (sensor.disponivel())
        {
            float temperatura = sensor.obterTemperaturaCelsius();
            float umidade = sensor.obterUmidadeRelativa();

            char mensagem[64];
            snprintf(mensagem, sizeof(mensagem), "Temperatura: %.2f°C\tUmidade: %.2f%%", temperatura, umidade);
            portaSerial.enviarTextoComNovaLinha(mensagem);

            exibirDados(&u8g2, temperatura, umidade);
        }
        else
        {
            portaSerial.enviarTextoComNovaLinha("Leitura nao disponivel do AHT10.");
        }

        sleep_ms(1000);
    }
}

void inicializarDisplay(u8g2pico_t *u8g2)
{
    u8g2_Setup_ssd1306_i2c_128x64_noname_f_pico(u8g2, DISPLAY_I2C_PORT, DISPLAY_SDA_PIN, DISPLAY_SCL_PIN, U8G2_R0, DISPLAY_I2C_ADDRESS);
    u8g2_InitDisplay(u8g2);
    u8g2_SetPowerSave(u8g2, 0);
    u8g2_ClearBuffer(u8g2);
    u8g2_SendBuffer(u8g2);
}

void exibirDados(u8g2pico_t *u8g2, float temperatura, float umidade)
{
    u8g2_ClearBuffer(u8g2);
    u8g2_SetFont(u8g2, u8g2_font_ncenB08_tr);

    char buffer[32];
    snprintf(buffer, sizeof(buffer), "Temp: %.2f C", temperatura);
    u8g2_DrawStr(u8g2, 0, 10, buffer);

    snprintf(buffer, sizeof(buffer), "Umid: %.2f %%", umidade);
    u8g2_DrawStr(u8g2, 0, 20, buffer);

    if (umidade > 70.0f)
    {
        u8g2_DrawStr(u8g2, 0, 30, "Alerta: Alta Umid!");
    }
    if (temperatura < 20.0f)
    {
        u8g2_DrawStr(u8g2, 0, 40, "Alerta: Baixa temp!");
    }

    u8g2_SendBuffer(u8g2);
}
