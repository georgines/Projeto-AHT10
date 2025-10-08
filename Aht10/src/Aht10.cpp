#include "Aht10.h"

Aht10::Aht10(i2c_inst_t *controlador_i2c, uint8_t pino_sda, uint8_t pino_scl, uint8_t endereco)
    : controladorI2c(controlador_i2c),
      pinoSda(pino_sda),
      pinoScl(pino_scl),
      enderecoI2c(endereco),
      leituraDisponivel(false),
      amostragemTemperatura(0),
      amostragemUmidade(0)
{
}

void Aht10::inicializar()
{
    i2c_init(controladorI2c, 100000);
    gpio_set_function(pinoSda, GPIO_FUNC_I2C);
    gpio_set_function(pinoScl, GPIO_FUNC_I2C);

    leituraDisponivel = false;
    amostragemTemperatura = 0;
    amostragemUmidade = 0;
    
    resetar();
    sleep_ms(20);
    calibrar();
}

float Aht10::obterTemperaturaCelsius() const
{
    return ((amostragemTemperatura * 200.0f) / (1 << 20)) - 50.0f;
}

float Aht10::obterUmidadeRelativa() const
{
    return (amostragemUmidade * 100.0f) / (1 << 20);
}

void Aht10::enviarComandoMedicao()
{
    uint8_t cmd[3] = {
        AHT10_CMD_MEASURE,
        AHT10_CMD_MEASURE_ARG,
        AHT10_CMD_MEASURE_ARG2
    };
    enviarComandos(cmd, sizeof(cmd));
}

void Aht10::lerDados(uint8_t *buf)
{
    i2c_read_blocking(controladorI2c, enderecoI2c, buf, 6, false);
}

void Aht10::processarDados(uint8_t *buf)
{
    amostragemUmidade = ((uint32_t)buf[1] << 12) | ((uint32_t)buf[2] << 4) | ((buf[3] >> 4) & 0x0F);
    amostragemTemperatura = (((uint32_t)(buf[3] & 0x0F)) << 16) | ((uint32_t)buf[4] << 8) | buf[5];
}

bool Aht10::disponivel()
{
    leituraDisponivel = false;

    enviarComandoMedicao();
    sleep_ms(80);
    
    uint8_t buf[6];
    lerDados(buf);
    processarDados(buf);
    
    leituraDisponivel = true;
    return leituraDisponivel;
}

void Aht10::calibrar()
{
    uint8_t cmd[3] = {
        AHT10_CMD_CALIBRATION,
        AHT10_CMD_CALIBRATION_ARG,
        AHT10_CMD_CALIBRATION_ARG2
    };

    enviarComandos(cmd, sizeof(cmd));
}

void Aht10::resetar()
{
    uint8_t cmd[3] = {
        AHT10_CMD_SOFT_RESET,
        AHT10_CMD_SOFT_RESET_ARG,
        AHT10_CMD_SOFT_RESET_ARG2
    };
    enviarComandos(cmd, sizeof(cmd));   
    sleep_ms(20);
}

void Aht10::enviarComandos(const uint8_t *comandos, size_t tamanho)
{
    i2c_write_blocking(controladorI2c, enderecoI2c, comandos, tamanho, false);
}
