# The Minish Cap 3DS — First Stable Release

<p align="center">
  <img width="320" alt="QR Code para instalação do The Minish Cap 3DS" src="https://github.com/BC-Estrategias/zelda-tmc-3ds/releases/download/v1.0.1/tmc-3ds-v1.0.1-qr.png" />
</p>

## Português

Primeira versão pública estável desta edição de **The Legend of Zelda: The Minish Cap para Nintendo 3DS**.

Baseada no port de EstebanPdN, no Project Picori e na decompilação original do jogo, esta edição acrescenta recursos próprios de interface, qualidade de vida, RetroAchievements e ajustes de Full View para o hardware do 3DS.

### Destaques

- Port nativo com suporte às duas telas.
- Tela inferior com mapa, masmorras, status da aventura, itens, cheats, configurações, Randomizer e RetroAchievements.
- Modos **Wide, Original e Stretch**.
- Filtros **Blur, Bilinear e Pixel Perfect**.
- **Full View experimental refinado** no New 3DS com Wide + Pixel Perfect.
- Full View em 400×240 apenas quando a sala possui conteúdo real suficiente para preencher todo o viewport.
- Diálogos normais continuam em Full View nas áreas compatíveis.
- RetroAchievements nativo, com login persistente, lista de conquistas, badges, pontuação e avisos de desbloqueio.
- Compatibilidade de RetroAchievements com a ROM USA e traduções PT-BR compatíveis baseadas em BZME.
- Velocidade do Link em 1×, 1,5× e 2×.
- Texto, carga de espada, portais Minish, Kinstones e miniaturas com opções de aceleração.
- Marcadores de pedaços de coração ainda pendentes no mapa.
- Quick save com **ZL + X** e quick load com confirmação em **ZL + Y**.
- Turbo configurável entre 2× e 5× no New 3DS, usando ZR ou C-Stick.
- Project Picori Randomizer integrado com save separado do jogo normal.
- Ferramentas de diagnóstico para crashes, glitches e problemas de desempenho.

## Instalação

Instale o arquivo CIA com **FBI** ou use o 3DSX pelo **Homebrew Launcher**.

No FBI:

~~~text
Remote Install > Scan QR Code
~~~

e escaneie o QR acima.

### ROM

Crie:

~~~text
sdmc:/3ds/The Minish Cap 3DS/
~~~

e coloque ali sua ROM .gba legalmente obtida.

O nome do arquivo pode ser qualquer um. São reconhecidas as bases:

~~~text
USA:    BZME
Europe: BZMP
~~~

SHA-1 das ROMs limpas conhecidas:

~~~text
USA:    b4bd50e4131b027c334547b4524e2dbbd4227130
Europe: cff199b36ff173fb6faf152653d1bccf87c26fb7
~~~

**ROMs traduzidas para PT-BR baseadas na versão americana também funcionam**, desde que preservem uma base BZME compatível.

A ROM permanece no cartão SD e nunca é incluída na CIA ou no 3DSX.

## Áudio

Se o jogo abrir sem música ou efeitos sonoros, faça o dump do DSP pelo Luma3DS Rosalina.

Abra:

~~~text
L + ↓ + SELECT
~~~

e selecione:

~~~text
Miscellaneous options
> Dump DSP firmware
~~~

Normalmente isso precisa ser feito apenas uma vez por console.

## Diagnóstico

Durante o jogo, pressione:

~~~text
L + R + A
~~~

para criar um dump em:

~~~text
sdmc:/3ds/The Minish Cap 3DS/dumps/
~~~

Envie essa pasta ao reportar crashes ou bugs sempre que possível.

---

## English

First public stable release of this **The Legend of Zelda: The Minish Cap Nintendo 3DS edition**.

Based on EstebanPdN's 3DS port, Project Picori and the original decompilation, this edition adds its own quality-of-life layer, native RetroAchievements integration and refined Full View behavior.

### Highlights

- Native dual-screen Nintendo 3DS port.
- Bottom-screen map, dungeon information, quest status, items, cheats, settings, Randomizer and RetroAchievements.
- Wide, Original and Stretch aspect modes.
- Blur, Bilinear and Pixel Perfect display styles.
- Refined experimental Full View on New 3DS with Wide + Pixel Perfect.
- 400×240 Full View only when the room has enough real rendered content to fill the viewport.
- Compatible dialogue scenes remain in Full View.
- Native RetroAchievements with persistent login, achievement list, badges, points and unlock notifications.
- RetroAchievements support for the clean USA ROM and compatible Brazilian Portuguese translations based on BZME.
- Configurable Link speed and multiple gameplay quality-of-life options.
- Quick save with **ZL + X** and confirmed quick load with **ZL + Y**.
- Configurable 2×–5× turbo on New 3DS using ZR or C-Stick.
- Integrated Project Picori Randomizer with isolated normal/randomized saves.
- Diagnostic dumps for crashes, graphical issues and performance problems.

## Installation

Install the CIA with **FBI** or use the 3DSX with the **Homebrew Launcher**.

Create:

~~~text
sdmc:/3ds/The Minish Cap 3DS/
~~~

and place your legally obtained .gba ROM in that folder.

Clean ROM SHA-1 values:

~~~text
USA:    b4bd50e4131b027c334547b4524e2dbbd4227130
Europe: cff199b36ff173fb6faf152653d1bccf87c26fb7
~~~

Compatible Brazilian Portuguese translations based on the USA ROM are also supported.

The ROM is never included in the CIA or 3DSX.

## Audio

If homebrew audio is unavailable, open Luma3DS Rosalina:

~~~text
L + ↓ + SELECT
~~~

then run:

~~~text
Miscellaneous options
> Dump DSP firmware
~~~

## Legal

The original game ROM, extracted Nintendo assets, save data and firmware are not included.

This is an unofficial fan-made project and is not affiliated with or endorsed by Nintendo.
