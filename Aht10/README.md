# Aht10

Biblioteca C++ para o Raspberry Pi Pico W que encapsula a comunicação com o sensor digital de temperatura e umidade AHT10 via barramento I2C. A classe `Aht10` cuida das etapas de inicialização, calibração, disparo de medições e conversão dos registros brutos em valores físicos prontos para uso em aplicações embarcadas.

## Recursos principais

- Inicialização completa do periférico I2C do RP2040 com configuração opcional da frequência de comunicação.
- Sequência automática de reset e calibração seguindo a documentação do sensor.
- Conversão direta dos registros de 20 bits em temperatura (°C) e umidade relativa (%).
- Verificação de disponibilidade de leitura antes de expor os valores calculados.
- Interface simples com métodos `atualizarMedicoes`, `obterTemperaturaCelsius` e `obterUmidadeRelativa`.

## Requisitos

- Raspberry Pi Pico W (RP2040) com SDK 2.2.0.
- Ferramentas de build configuradas com CMake e Ninja (estrutura padrão do Pico SDK).
- Sensor AHT10 conectado ao barramento I2C0 com resistores de pull-up externos apropriados ou pull-ups internos habilitados.
  - SDA: GP0
  - SCL: GP1
  - Alimentação: 3V3 e GND

## Como adicionar ao seu projeto

1. Garanta que a pasta `Aht10` esteja dentro do diretório raiz do seu projeto (ao lado do `CMakeLists.txt` principal).
2. No `CMakeLists.txt` principal, inclua a subpasta e vincule a biblioteca ao seu executável:

```cmake
add_subdirectory(Aht10)

add_executable(seu_programa
    main.cpp
    # outros fontes
)

target_link_libraries(seu_programa
    PRIVATE
        Aht10
)
```

3. Conecte o sensor AHT10 ao barramento I2C desejado e compartilhe o GND com o Pico.

## Exemplo rápido de uso

```cpp
#include "pico/stdlib.h"
#include "Aht10.h"

#define PINO_SDA_I2C 0u
#define PINO_SCL_I2C 1u

int main() {
    stdio_init_all();

    Aht10 sensor(i2c0, PINO_SDA_I2C, PINO_SCL_I2C);

    if (!sensor.inicializarSensor()) {
        while (true) {
            tight_loop_contents();
        }
    }

    while (true) {
        if (sensor.atualizarMedicoes()) {
            const float temperatura_c = sensor.obterTemperaturaCelsius();
            const float umidade_relativa = sensor.obterUmidadeRelativa();
            printf("Temperatura: %.2f C, Umidade: %.2f %%\r\n",
                   temperatura_c,
                   umidade_relativa);
        }

        sleep_ms(1000);
    }
}
```

### Pontos de atenção no exemplo

- `stdio_init_all` é opcional e só é necessário caso queira logs via USB/UART.
- Após `atualizarMedicoes`, utilize `obterTemperaturaCelsius` e `obterUmidadeRelativa` apenas quando `leituraDisponivel` retornar `true`.
- Ajuste a frequência I2C passando um valor customizado para `inicializarSensor` quando necessário.

## Boas práticas

- Garanta resistores de pull-up adequados no barramento I2C para comunicação estável.
- Evite chamar `atualizarMedicoes` em intervalos inferiores a 100 ms para respeitar o tempo de conversão do sensor.
- Trate condições de erro retornadas pelos métodos booleanos para recuperar o sensor em caso de falhas momentâneas.
