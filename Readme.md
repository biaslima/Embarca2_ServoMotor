# Controle de Servo Motor com Raspberry Pi Pico W

Este projeto demonstra o controle de um servo motor SG90 utilizando a placa Raspberry Pi Pico W. O sistema oferece três modos de operação distintos, selecionáveis através de um botão, e utiliza um joystick para controle de posição e velocidade. O status de cada modo é indicado por um LED RGB.

## Hardware Utilizado

- **Microcontrolador:** Raspberry Pi Pico W
- **Placa de Desenvolvimento:** BitDogLab (BDL)
- **Atuador:** Servo Motor SG90
- **Entrada:** Joystick Analógico
- **Sinalização:** LED RGB
- **Alimentação:** Bateria (conforme imagem)

## Funcionalidades

O firmware, implementado no arquivo `ServoSG90.c`, possui três modos de operação que podem ser alternados através do `BOTAO_MODO`.

1.  **Modo Manual:**
    - **Indicação:** LED Azul aceso.
    - **Funcionamento:** A posição do servo motor é controlada diretamente pela posição do joystick. O valor lido pelo ADC (0-4095) é mapeado para a faixa de pulso do servo (500-2500 µs).

2.  **Modo Sweep (Varredura):**
    - **Indicação:** LED Verde aceso.
    - **Funcionamento:** O servo motor se move continuamente entre suas posições mínima e máxima. A velocidade desse movimento é controlada pelo joystick: quanto maior a deflexão do joystick, mais rápido o movimento.

3.  **Modo Gravação e Reprodução (Record & Playback):**
    - **Indicação:** LED Vermelho aceso (piscando durante a gravação, fixo durante a reprodução).
    - **Funcionamento:** Ao entrar neste modo, o sistema começa a gravar até 200 posições do servo controladas pelo joystick. Quando o limite de gravação é atingido, o LED para de piscar e o sistema entra em um loop, reproduzindo continuamente a sequência de movimentos gravada.

## Detalhes Técnicos

### Mapeamento de Pinos (Pinout)

O hardware é conectado aos seguintes pinos da Raspberry Pi Pico W, conforme definido em `ServoSG90.c` e ilustrado no diagrama da placa.

| Componente      | Pino GPIO | Descrição                                    |
| :-------------- | :-------- | :------------------------------------------- |
| Servo Motor     | GPIO8     | Saída PWM para controle do servo.            |
| Joystick        | GPIO27    | Entrada analógica (ADC1) para leitura.       |
| LED Vermelho    | GPIO13    | Indica o modo Gravação/Reprodução.           |
| LED Azul        | GPIO12    | Indica o modo Manual.                        |
| LED Verde       | GPIO11    | Indica o modo Sweep.                         |
| Botão Reset     | GPIO6     | Coloca a placa em modo bootloader USB.       |
| Botão Modo      | GPIO5     | Alterna entre os modos de operação.          |

### Configuração do PWM

O controle do servo é realizado via PWM (Pulse Width Modulation) com uma frequência de **50 Hz**, padrão para servos SG90. Esta configuração é obtida da seguinte forma:

- **Clock do Sistema:** 125 MHz
- **Divisor de Clock (clkdiv):** 125.0f
- **Valor de Wrap (ciclo):** 19999

**Cálculo da Frequência:**
`Frequência = Clock / (clkdiv * (wrap + 1)) = 125.000.000 / (125 * (19999 + 1)) = 50 Hz`

A posição do servo é definida pela largura do pulso, que varia entre **500 µs** (posição mínima) e **2500 µs** (posição máxima).

## Estrutura do Projeto

O repositório está organizado para ser utilizado com o Visual Studio Code e a extensão oficial para Raspberry Pi Pico.

-   `ServoSG90.c`: Arquivo principal contendo toda a lógica de controle do servo, leitura do joystick e gerenciamento dos modos de operação.
-   `CMakeLists.txt`: Script de compilação que define o projeto, suas dependências (`pico_stdlib`, `hardware_pwm`, `hardware_adc`) e o arquivo fonte.
-   `pico_sdk_import.cmake`: Script padrão do SDK para localizar e importar as bibliotecas necessárias para a compilação.
-   `.vscode/`: Pasta contendo as configurações do ambiente de desenvolvimento no VS Code, como tarefas de compilação, configurações de depuração e definições do C/C++.
-   `imagens/`: Contém imagens da placa e diagramas de pinos para referência.
