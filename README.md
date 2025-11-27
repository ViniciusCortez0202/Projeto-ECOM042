Radar Eletronico (Zephyr)
=========================

Aplicacao  que simula um radar rodoviario usando dois sensores, calcula velocidade, classifica o veiculo e publica o resultado em um barramento Zbus. Uma camera simulada gera a placa (formato Mercosul) e valida se a captura foi bem-sucedida.

Visao rapida
************
- Sensores em `sensor.c`: interrupcao GPIO dispara eventos para a fila interna.
- Processamento em `radar_processing.c`: calcula velocidade com base no tempo entre sensores, classifica o veiculo (leve/pesado) e define status (normal/atencao/infracao).
- Controle em `radar_control.c`: aplica limites configuraveis e limiar de alerta.
- Exibicao em `radar_display.c`: escreve em display real (se existir) e sempre imprime em log com ANSI para cores.
- Barramento em `radar_bus.c`: canais Zbus para publicacao de infracoes e resultado da camera.
- Camera em `camera_service.c` e `plate.c`: simula leitura de placa, com chance de falha configuravel.
- `sensor_sim.c`: gera veiculos sintenticos para demonstracao (eixos, velocidades e status variados).

Requisitos
**********
- Zephyr com suporte a Zbus, GPIO e (opcional) driver de display escolhido em ``zephyr_display``.
- Funciona em QEMU ou em placas reais com dois pinos de entrada para sensores.

Configuracao
************
As opcoes ficam em ``Kconfig``:

- ``RADAR_SENSOR_DISTANCE_MM``: distancia em mm entre os sensores (padrao: 1000).
- ``RADAR_SPEED_LIMIT_LIGHT_KMH``: limite km/h para veiculos leves (padrao: 80).
- ``RADAR_SPEED_LIMIT_HEAVY_KMH``: limite km/h para veiculos pesados (padrao: 60).
- ``RADAR_WARNING_THRESHOLD_PERCENT``: percentual do limite para acionar status de atencao (padrao: 90).
- ``RADAR_CAMERA_FAILURE_RATE_PERCENT``: porcentagem de falha na leitura de placa (padrao: 10).

Como construir e rodar
**********************

```
west build -b mps2/an385 -p auto
west build -t run

```

## Run for Tests
```
west build -b mps2/an385 test -p always
west build -t run

```

Saidas
**************
- Logs trazem linhas como ``[RADAR] Veiculo=LEVE eixos=2 velocidade=95 limite=80 status=INFRACAO``.
- Quando ha infracao, a camera simula captura de placa e publica em ``chan_camera_result``; placas invalidas representam falhas de OCR.
- Se um display estiver presente, a linha resumida tambem e escrita nele; caso contrario apenas o log aparece.
