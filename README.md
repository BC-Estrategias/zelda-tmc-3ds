# The Legend of Zelda: The Minish Cap — Definitive 3DS Edition

<p align="center">
  <img width="100%" alt="The Minish Cap 3DS" src="https://github.com/user-attachments/assets/db99e777-12a2-4222-86c3-7c8f14062586" />
</p>

<p align="center">
  <strong>A native Nintendo 3DS port with dual-screen UI, Full View, RetroAchievements, Randomizer, quality-of-life features and built-in updates.</strong>
</p>

<p align="center">
  <a href="https://github.com/BC-Estrategias/zelda-tmc-3ds/releases/latest"><strong>Latest Release</strong></a>
  ·
  <a href="https://github.com/BC-Estrategias/zelda-tmc-3ds/releases/latest/download/tmc-3ds-latest.cia"><strong>Download CIA</strong></a>
  ·
  <a href="https://github.com/BC-Estrategias/zelda-tmc-3ds/releases/latest/download/tmc-3ds-latest.3dsx"><strong>Download 3DSX</strong></a>
  ·
  <a href="https://github.com/BC-Estrategias/zelda-tmc-3ds/issues"><strong>Report a Bug</strong></a>
</p>

<p align="center">
  <a href="#english">🇺🇸 English</a> · <a href="#português-brasil">🇧🇷 Português (Brasil)</a>
</p>

> Unofficial fan project. A legally obtained copy of **The Legend of Zelda: The Minish Cap** for Game Boy Advance is required. No ROM, save data, firmware or extracted Nintendo assets are distributed with the CIA/3DSX.

---

# English

## What is this?

This project is a native Nintendo 3DS adaptation of **The Legend of Zelda: The Minish Cap**.

Gameplay runs on the top screen while the bottom screen provides maps, dungeon information, quest status, items, cheats, settings, Randomizer controls, RetroAchievements and the built-in updater.

This edition is based on the work of **EstebanPdN/zelda-tmc-3ds**, **Project Picori**, **zeldaret/tmc** and the dual-screen work from **tmc-android**, with additional 3DS-specific features and refinements maintained in this repository.

## Highlights

| Feature | Support |
| --- | --- |
| Native CIA / 3DSX | ✅ |
| New 3DS / New 2DS XL | ✅ |
| Old 3DS / Nintendo 2DS | ✅ |
| Pixel Perfect Full View 400×240 | ✅ New 3DS / New 2DS XL |
| Dual-screen interface | ✅ |
| Quick Save / Quick Load | ✅ |
| Link speed 1× / 1.5× / 2× | ✅ |
| Turbo | ✅ |
| Cheats and QoL options | ✅ |
| Randomizer | ✅ |
| RetroAchievements | ✅ |
| Built-in updater | ✅ |
| UI languages | 🇺🇸 English · 🇧🇷 Português · 🇪🇸 Español |
| USA ROM | ✅ |
| Europe ROM | ✅ |
| Compatible PT-BR USA-based ROMs | ✅ |

### Full View

On **New Nintendo 3DS / New Nintendo 2DS XL**, **Wide + Pixel Perfect** can enable a true **400×240 Full View** in compatible rooms.

The renderer only uses the expanded viewport when the room can safely fill it. Small interiors, fixed screens, incompatible overlays and transitions automatically fall back to the safe presentation mode instead of stretching the image.

Normal dialogue can remain in Full View in compatible areas.

> **Old Nintendo 3DS / Nintendo 2DS are supported**, but Full View is not available on those systems.

### Native bottom-screen UI

The lower screen includes:

- World map and dungeon maps
- Quest and progression information
- Inventory and touch item selection
- Cheats for health, rupees, shells, Kinstones, bombs and arrows
- Display, gameplay and quality-of-life settings
- Randomizer controls
- RetroAchievements
- Built-in updater
- Interface language selection

Press **X** during gameplay to cycle through the main bottom-screen tabs.

### Quality of life

- Link movement speed: **1× / 1.5× / 2×**
- Faster text
- Faster sword charge
- Faster Minish portals
- Faster Kinstone fusion flow
- Faster figurine flow
- Missing heart-piece markers on the map
- Optional direct boot to file select
- Configurable turbo

### Quick Save / Quick Load

| Shortcut | Action |
| --- | --- |
| **ZL + X** | Quick Save |
| **ZL + Y** | Quick Load confirmation |
| **ZR** | Hold Turbo on New 3DS |
| **C-Stick** | Turbo |
| **X** | Cycle bottom-screen tabs |

### RetroAchievements

RetroAchievements runs directly on the 3DS with:

- Persistent login
- Achievement list on the bottom screen
- Badges, descriptions and points
- In-game unlock notifications
- USA ROM support
- Compatible PT-BR translations based on the USA ROM

The European game version runs normally, but the achievement set used by this edition targets the USA base and compatible translations.

## Install

### FBI QR — recommended

In FBI:

```text
Remote Install > Scan QR Code
```

Then scan:

<p align="center">
  <img width="320" alt="FBI QR Code for latest stable The Minish Cap 3DS release" src="https://github.com/BC-Estrategias/zelda-tmc-3ds/releases/latest/download/tmc-3ds-latest-qr.png" />
</p>

The QR always points to the **latest stable CIA**.

### Direct downloads

- [Latest CIA](https://github.com/BC-Estrategias/zelda-tmc-3ds/releases/latest/download/tmc-3ds-latest.cia)
- [Latest 3DSX](https://github.com/BC-Estrategias/zelda-tmc-3ds/releases/latest/download/tmc-3ds-latest.3dsx)
- [Latest release notes](https://github.com/BC-Estrategias/zelda-tmc-3ds/releases/latest)

After the first installation, future releases can also be installed directly from the **built-in updater**.

## Required ROM

Place a legally obtained `.gba` ROM in:

```text
sdmc:/3ds/The Minish Cap 3DS/
```

The filename can be anything as long as it keeps the `.gba` extension.

Known clean ROM bases:

```text
USA (BZME):    b4bd50e4131b027c334547b4524e2dbbd4227130
Europe (BZMP): cff199b36ff173fb6faf152653d1bccf87c26fb7
```

Compatible PT-BR translations based on the USA ROM are also supported.

The ROM remains on the SD card and is never embedded into the CIA or 3DSX.

## Audio / DSP dump

If the game launches without music or sound effects, open the **Luma3DS Rosalina Menu**:

```text
L + ↓ + SELECT
```

Then:

```text
Miscellaneous options
> Dump DSP firmware
```

This normally only needs to be done once per console.

## Diagnostics and bug reports

During gameplay press:

```text
L + R + A
```

The port creates a local diagnostic dump with runtime information useful for reproducing crashes, graphical bugs and performance problems.

Dumps are stored in:

```text
sdmc:/3ds/The Minish Cap 3DS/dumps/
```

When reporting a bug, attach the relevant dump folder whenever possible.

[Open a GitHub Issue](https://github.com/BC-Estrategias/zelda-tmc-3ds/issues)

---

# Português (Brasil)

## O que é este projeto?

Este projeto é uma adaptação nativa de **The Legend of Zelda: The Minish Cap** para Nintendo 3DS.

O jogo roda na tela superior enquanto a tela inferior reúne mapa, informações de masmorras, progresso, itens, cheats, configurações, Randomizer, RetroAchievements e o atualizador integrado.

Esta edição parte do trabalho de **EstebanPdN/zelda-tmc-3ds**, **Project Picori**, **zeldaret/tmc** e do trabalho de tela dupla do **tmc-android**, acrescentando recursos e refinamentos específicos para o Nintendo 3DS.

## Principais recursos

| Recurso | Suporte |
| --- | --- |
| CIA / 3DSX nativos | ✅ |
| New 3DS / New 2DS XL | ✅ |
| Old 3DS / Nintendo 2DS | ✅ |
| Pixel Perfect Full View 400×240 | ✅ New 3DS / New 2DS XL |
| Interface em duas telas | ✅ |
| Quick Save / Quick Load | ✅ |
| Velocidade do Link 1× / 1,5× / 2× | ✅ |
| Turbo | ✅ |
| Cheats e melhorias de qualidade de vida | ✅ |
| Randomizer | ✅ |
| RetroAchievements | ✅ |
| Atualizador interno | ✅ |
| Idiomas da interface | 🇧🇷 Português · 🇺🇸 English · 🇪🇸 Español |
| ROM USA | ✅ |
| ROM Europe | ✅ |
| Traduções PT-BR compatíveis baseadas na USA | ✅ |

### Full View

No **New Nintendo 3DS / New Nintendo 2DS XL**, a combinação **Wide + Pixel Perfect** pode ativar o **Full View 400×240** em salas compatíveis.

O viewport expandido só é usado quando a sala consegue preencher a área com segurança. Interiores pequenos, telas fixas, overlays incompatíveis e transições fazem fallback automático para o modo seguro, sem esticar a imagem.

Diálogos normais podem permanecer em Full View nas áreas compatíveis.

> **Old Nintendo 3DS / Nintendo 2DS são suportados**, mas não possuem Full View.

### Tela inferior nativa

A tela inferior reúne:

- Mapa do mundo e mapas de masmorras
- Informações de progresso
- Inventário e seleção de itens por toque
- Cheats para vida, rupias, conchas, Kinstones, bombas e flechas
- Ajustes de vídeo, gameplay e qualidade de vida
- Randomizer
- RetroAchievements
- Atualizador interno
- Seleção de idioma da interface

Durante o jogo, pressione **X** para alternar entre as principais abas da tela inferior.

### Qualidade de vida

- Velocidade do Link em **1× / 1,5× / 2×**
- Texto mais rápido
- Carga da espada acelerada
- Portais Minish mais rápidos
- Fusões de Kinstones mais rápidas
- Fluxo de miniaturas mais rápido
- Marcadores de pedaços de coração pendentes no mapa
- Inicialização opcional direto na seleção de arquivo
- Turbo configurável

### Quick Save / Quick Load

| Atalho | Ação |
| --- | --- |
| **ZL + X** | Quick Save |
| **ZL + Y** | Confirmação de Quick Load |
| **ZR** | Segurar Turbo no New 3DS |
| **C-Stick** | Turbo |
| **X** | Alternar abas da tela inferior |

### RetroAchievements

A integração roda diretamente no 3DS com:

- Login persistente
- Lista de conquistas na tela inferior
- Badges, descrição e pontuação
- Avisos durante o jogo ao desbloquear conquistas
- Suporte à ROM USA
- Suporte a traduções PT-BR compatíveis baseadas na ROM USA

A versão europeia funciona normalmente no jogo, mas o conjunto de conquistas desta edição é voltado à base americana e às traduções compatíveis.

## Instalação

### QR pelo FBI — recomendado

No FBI:

```text
Remote Install > Scan QR Code
```

Depois escaneie:

<p align="center">
  <img width="320" alt="QR Code FBI da versão estável mais recente do The Minish Cap 3DS" src="https://github.com/BC-Estrategias/zelda-tmc-3ds/releases/latest/download/tmc-3ds-latest-qr.png" />
</p>

Esse QR sempre aponta para a **CIA estável mais recente**.

### Downloads diretos

- [CIA mais recente](https://github.com/BC-Estrategias/zelda-tmc-3ds/releases/latest/download/tmc-3ds-latest.cia)
- [3DSX mais recente](https://github.com/BC-Estrategias/zelda-tmc-3ds/releases/latest/download/tmc-3ds-latest.3dsx)
- [Notas da versão mais recente](https://github.com/BC-Estrategias/zelda-tmc-3ds/releases/latest)

Depois da primeira instalação, as próximas versões também podem ser instaladas pelo **atualizador dentro do próprio jogo**.

## ROM necessária

Coloque uma ROM `.gba` legalmente obtida em:

```text
sdmc:/3ds/The Minish Cap 3DS/
```

O nome do arquivo é livre, mantendo a extensão `.gba`.

Bases limpas conhecidas:

```text
USA (BZME):    b4bd50e4131b027c334547b4524e2dbbd4227130
Europe (BZMP): cff199b36ff173fb6faf152653d1bccf87c26fb7
```

Traduções PT-BR compatíveis baseadas na ROM USA também são suportadas.

A ROM permanece no cartão SD e nunca é incorporada à CIA ou ao 3DSX.

## Áudio / Dump DSP

Se o jogo abrir sem música ou efeitos sonoros, abra o menu **Rosalina do Luma3DS**:

```text
L + ↓ + SELECT
```

Depois:

```text
Miscellaneous options
> Dump DSP firmware
```

Normalmente isso precisa ser feito apenas uma vez por console.

## Diagnóstico e bugs

Durante o jogo, pressione:

```text
L + R + A
```

O port cria um dump local com informações úteis para reproduzir crashes, bugs gráficos e problemas de desempenho.

Os dumps ficam em:

```text
sdmc:/3ds/The Minish Cap 3DS/dumps/
```

Ao reportar um problema, envie a pasta correspondente sempre que possível.

[Reportar um bug no GitHub](https://github.com/BC-Estrategias/zelda-tmc-3ds/issues)

---

## Build from source / Compilação

Requirements / Requisitos:

- devkitPro / devkitARM
- libctru
- Citro2D
- Citro3D
- CMake
- makerom and bannertool for CIA packaging

```sh
chmod +x platform/3ds/build.sh
./platform/3ds/build.sh
```

Packages are generated in / Os pacotes são gerados em:

```text
build-3ds/game/
```

The build never includes or embeds a ROM.

## Credits / Créditos

- [EstebanPdN/zelda-tmc-3ds](https://github.com/EstebanPdN/zelda-tmc-3ds) — original Nintendo 3DS port base
- [Project Picori](https://github.com/999sian/tmc) — port engine and infrastructure
- [zeldaret/tmc](https://github.com/zeldaret/tmc) — original decompilation
- [samyost1/tmc-android](https://github.com/samyost1/tmc-android) — dual-screen work
- [Raekwon1603/tmc-android](https://github.com/Raekwon1603/tmc-android) — related Android fork work
- **Bruno Cruz** — 3DS edition, UI, QoL, RetroAchievements, Full View and maintenance

## License and legal notice / Licença e aviso legal

Source code is distributed under the [GPL-3.0](LICENSE). Third-party components retain their respective licenses as listed in [THIRD-PARTY-LICENSES.md](THIRD-PARTY-LICENSES.md).

Nintendo owns The Legend of Zelda, The Minish Cap and associated game content.

This is an unofficial fan project and is not affiliated with or endorsed by Nintendo.

No ROM, extracted Nintendo asset, save data or firmware is distributed with this project.
