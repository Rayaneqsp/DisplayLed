# Projeto de Controle de LEDs e Exibição no Display SSD1306 com a BitDogLab

## Descrição do Projeto

Este projeto permite o controle de LEDs e a exibição de caracteres e números usando uma **BitDogLab** com o microcontrolador **RP2040**. Ele interage com a comunicação serial para receber caracteres e números do **Serial Monitor** do VS Code e controla LEDs endereçáveis **WS2812**, LEDs RGB (vermelho, verde e azul) e um display **SSD1306**. 

### Componentes

- **BitDogLab (RP2040)** – Placa de desenvolvimento.
- **Matriz de LEDs WS2812 (5x5)** – Conectada à GPIO 7.
- **LED RGB** (Vermelho, Verde e Azul) – Conectados às GPIOs 11, 12 e 13.
- **Botão A** – Conectado à GPIO 5.
- **Botão B** – Conectado à GPIO 6.
- **Display OLED SSD1306** – Conectado via I2C (GPIO 14 e 15).

## Funcionalidades do Projeto

### 1. Entrada de Caracteres via UART
- O projeto permite a entrada de caracteres (letras e números) via **Serial Monitor** do **VS Code**.
- Ao digitar um número (0-9), o número é exibido na **matriz WS2812 (5x5)** e no display **SSD1306**.
- Se um caractere alfanumérico for digitado, ele será exibido no display **SSD1306** em letras maiúsculas.

### 2. Controle do LED RGB (Verde e Azul) com Botões
- **Botão A (GPIO 5)**: Pressionar o Botão A alterna o estado do **LED RGB Verde**.
- **Botão B (GPIO 6)**: Pressionar o Botão B alterna o estado do **LED RGB Azul**.
- Ambas as ações são registradas no **Serial Monitor** e exibidas no **SSD1306**.

### 3. Exibição de Números no Display e Matriz WS2812
- Quando um número entre **0-9** for digitado no **Serial Monitor**, ele será exibido na matriz **WS2812** e no display **SSD1306**.

## Requisitos do Projeto

1. **Interrupções (IRQ)**: As funcionalidades dos botões A e B são baseadas em interrupções para detectar pressões de maneira eficiente.
2. **Debounce**: O tratamento de **debounce** dos botões é feito via software para evitar múltiplos registros de uma única pressão.
3. **Controle de LEDs**: O projeto usa LEDs comuns e LEDs endereçáveis **WS2812**.
4. **Exibição no Display SSD1306**: O display é usado para exibir tanto texto (com letras maiúsculas) quanto os números da matriz WS2812.
5. **Comunicação Serial via UART**: O programa se comunica com o PC via **UART** para exibir informações no **Serial Monitor**.
6. **Organização e Comentários**: O código está bem estruturado e comentado para facilitar a compreensão.

## Como Usar

### Passo 1: Conectando os Componentes

1. **Display OLED SSD1306**:
   - **SDA**: GPIO 14
   - **SCL**: GPIO 15

2. **Matriz 5x5 de LEDs WS2812**:
   - **DIN**: GPIO 7

3. **LED RGB**:
   - **Vermelho**: GPIO 13
   - **Verde**: GPIO 11
   - **Azul**: GPIO 12

4. **Botões**:
   - **Botão A**: GPIO 5
   - **Botão B**: GPIO 6

### Passo 2: Compilação e Carregamento do Código

1. Compile o código usando **VSCode** com a extensão do **SDK do Raspberry Pi Pico**.
2. Carregue o código na **BitDogLab** (RP2040) usando o método de upload adequado.

### Passo 3: Usando a Comunicação Serial

1. Abra o **Serial Monitor** no **VSCode**.
2. Digite um **caractere** (letra ou número):
   - O número será exibido na **matriz WS2812** e no display **SSD1306**.
   - O caractere será exibido no display **SSD1306** em maiúsculas.

### Passo 4: Interagindo com os Botões

- **Botão A (GPIO 5)**: Pressione o **Botão A** para alternar o estado do **LED verde**.
  - **Display**: O estado do LED verde será exibido como "LED VERDE = On" ou "LED VERDE DESLIGADO".
  - **Serial Monitor**: A mensagem "LED Verde ON" ou "LED Verde OFF" será exibida.

- **Botão B (GPIO 6)**: Pressione o **Botão B** para alternar o estado do **LED azul**.
  - **Display**: O estado do LED azul será exibido como "LED AZUL = On" ou "LED AZUL DESLIGADO".
  - **Serial Monitor**: A mensagem "LED Azul ON" ou "LED Azul OFF" será exibida.

### Exemplo de Uso

1. **Digitando um número**:
   - Digite **"3"** no **Serial Monitor**.
   - O número 3 será exibido na **matriz 5x5 WS2812** e no **display SSD1306**.

2. **Pressionando Botões**:
   - Pressione **Botão A** para acionar o **LED verde**.
     - O display mostrará "LED VERDE = On" e o **Serial Monitor** exibirá "LED Verde ON".
   - Pressione **Botão B** para acionar o **LED azul**.
     - O display mostrará "LED AZUL = On" e o **Serial Monitor** exibirá "LED Azul ON".

### Exemplo de Saída no Serial Monitor

- **Entrada de número 3**:
  ```
  Número 3
  ```

- **Pressionando Botão A (LED verde)**:
  ```
  LED Verde ON
  ```

- **Pressionando Botão B (LED azul)**:
  ```
  LED Azul ON
  ```

## Contribuições

Se você encontrar erros ou quiser melhorar o projeto, sinta-se à vontade para abrir uma **issue** ou enviar um **pull request**.

## Licença

Este projeto está licenciado para estudo e conhecimento.

## Autora:

**RAYANE QUEIROZ DOS SANTOS PASSOS**

## LINK DO VÍDEO:

