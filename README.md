# The Legend of Zelda: The Minish Cap — Definitive 3DS Edition

<p align="center">
  <img width="100%" alt="The Minish Cap 3DS" src="https://github.com/user-attachments/assets/db99e777-12a2-4222-86c3-7c8f14062586" />
</p>

<p align="center">
  <strong>Uma edição nativa para Nintendo 3DS com tela dupla, Full View, RetroAchievements e melhorias de qualidade de vida.</strong>
</p>

> Projeto não oficial para uso com uma cópia legal de The Legend of Zelda: The Minish Cap para Game Boy Advance. Nenhuma ROM, save, firmware ou asset extraído da Nintendo é distribuído neste repositório ou incorporado à CIA/3DSX.

## Sobre esta edição

Este projeto é uma adaptação nativa de The Legend of Zelda: The Minish Cap para Nintendo 3DS. A ação fica na tela superior; a tela inferior reúne mapa, informações de masmorras, status da aventura, itens, cheats, configurações, Randomizer e RetroAchievements.

A base do projeto vem de [EstebanPdN/zelda-tmc-3ds](https://github.com/EstebanPdN/zelda-tmc-3ds), do [Project Picori](https://github.com/999sian/tmc), da decompilação [zeldaret/tmc](https://github.com/zeldaret/tmc) e do trabalho de tela dupla de [samyost1/tmc-android](https://github.com/samyost1/tmc-android).

Esta edição é mantida por **Bruno Cruz** e preserva a base do port de Esteban enquanto adiciona uma camada própria de interface, qualidade de vida, RetroAchievements e ajustes de apresentação para o 3DS.

## Destaques desta edição

### Full View refinado

No **New Nintendo 3DS**, a combinação **Wide + Pixel Perfect** ativa o Full View experimental em salas compatíveis.

Nesta edição, o Full View só usa **400×240 reais** quando a sala e o conteúdo efetivamente renderizado conseguem preencher todo o viewport. Interiores pequenos não são reduzidos para uma câmera 200×120 ampliada em 2×, evitando sprites artificialmente grandes.

Diálogos normais permanecem em Full View nas salas compatíveis. Transições, telas fixas, overlays incompatíveis e áreas pequenas fazem fallback automaticamente para o modo seguro sem esticar a imagem.

### Tela inferior nativa

A tela de baixo foi adaptada para concentrar informações e atalhos sem substituir a experiência original:

- mapa do mundo e mapas de masmorras;
- status da aventura, Kinstones e técnicas de espada;
- inventário e itens por toque;
- aba de cheats para recuperar vida e reabastecer rupias, conchas, Kinstones, bombas e flechas;
- configurações de vídeo, gameplay, qualidade de vida, Randomizer e RetroAchievements;
- fora do gameplay, uma tela temática com a Triforce dá acesso às configurações.

O botão **X** alterna rapidamente entre **Missões → Mapa → Itens → Cheats** durante o jogo.

### RetroAchievements nativo

A integração com **RetroAchievements** roda diretamente no 3DS:

- login com sessão persistente no cartão SD;
- lista de conquistas na tela inferior, com badges, descrição e pontuação;
- avisos visuais durante o jogo quando uma conquista é desbloqueada;
- suporte à ROM USA original;
- suporte a traduções **PT-BR baseadas na ROM USA**, usando o mesmo conjunto de conquistas após a verificação de compatibilidade;
- o jogo continua funcionando normalmente sem conta ou sem conexão.

A versão europeia funciona normalmente no jogo, mas o conjunto de RetroAchievements desta edição é voltado à base americana e às traduções compatíveis com ela.

### Qualidade de vida

Esta edição acrescenta opções próprias de gameplay:

- velocidade do Link em **1×, 1,5× ou 2×**;
- texto mais rápido;
- carga da espada acelerada;
- portais Minish mais rápidos;
- fusões de Kinstones mais rápidas;
- fluxo de miniaturas mais rápido;
- marcadores dos pedaços de coração ainda pendentes no mapa;
- opção de iniciar diretamente na seleção de arquivo.

### Save states e turbo

Atalhos atuais:

- **ZL + X** — salva um quick state no próximo ponto seguro;
- **ZL + Y** — abre confirmação antes de carregar o quick state;
- **ZR** — mantém o turbo ativo no New 3DS;
- **C-Stick** — também pode acionar o turbo.

O multiplicador de turbo pode ser configurado entre **2× e 5×**.

### Randomizer

O Project Picori Randomizer também está integrado ao menu do 3DS, com perfil de save separado do jogo normal e opções próprias de lógica, entradas, Kinstones, itens e cosméticos.

### Desempenho e hardware

A build estável atual é voltada ao **New Nintendo 3DS / New Nintendo 2DS XL**.

Ela usa os recursos extras desse hardware, incluindo modo de 804 MHz, cache L2, núcleo adicional e o caminho acelerado de apresentação disponível para o port.

O **Old Nintendo 3DS / 2DS não é suportado por esta build estável**.

## Modos de imagem

Aspecto da tela superior:

- **Wide**
- **Original**
- **Stretch**

Filtros disponíveis nesta build:

- **Blur**
- **Bilinear**
- **Pixel Perfect**

> O antigo modo experimental “Ultra Sharp” não faz parte da build estável atual.

## Instalação

### CIA — recomendado

Baixe a versão mais recente em [Releases](../../releases/latest) e instale o arquivo .cia com o **FBI**.

Para a release atual, no FBI abra:

~~~text
Remote Install > Scan QR Code
~~~

e escaneie:

<p align="center">
  <img width="320" alt="QR Code para instalação do The Minish Cap 3DS" src="https://github.com/BC-Estrategias/zelda-tmc-3ds/releases/download/v1.1/tmc-3ds-v1.1-qr.png" />
</p>

### 3DSX

O arquivo .3dsx da release pode ser usado pelo **Homebrew Launcher**.

## ROM necessária

Na primeira execução, o port procura a ROM nesta pasta:

~~~text
sdmc:/3ds/The Minish Cap 3DS/
~~~

Coloque ali uma ROM .gba legalmente obtida. O nome do arquivo é livre; basta manter a extensão .gba.

Exemplo:

~~~text
sdmc:/3ds/The Minish Cap 3DS/The Minish Cap.gba
~~~

São reconhecidas as bases **USA (BZME)** e **Europe (BZMP)**.

SHA-1 das ROMs limpas conhecidas:

~~~text
USA:    b4bd50e4131b027c334547b4524e2dbbd4227130
Europe: cff199b36ff173fb6faf152653d1bccf87c26fb7
~~~

### Tradução PT-BR

O jogo também funciona com **ROMs traduzidas para PT-BR baseadas na versão americana**, desde que preservem a base compatível BZME.

Para RetroAchievements, esta edição reconhece uma tradução compatível de base USA e utiliza o conjunto de conquistas da versão americana. O hash real da ROM traduzida continua registrado nos diagnósticos; a compatibilidade não altera nem incorpora a ROM ao aplicativo.

A ROM permanece no cartão SD e nunca é embutida na CIA ou no 3DSX.

## Áudio — Dump DSP

O áudio do homebrew requer o firmware DSP do próprio 3DS.

Se o jogo abrir sem música ou efeitos sonoros, abra o menu **Rosalina do Luma3DS**:

~~~text
L + ↓ + SELECT
~~~

Depois selecione:

~~~text
Miscellaneous options
> Dump DSP firmware
~~~

Esse procedimento normalmente precisa ser feito apenas **uma vez por console**.

Depois do dump, feche o Rosalina e abra o jogo novamente.

## Diagnóstico e bugs

Durante o jogo, pressione:

~~~text
L + R + A
~~~

O port pausa e cria um dump de diagnóstico com capturas das telas, memória, estado de execução, informações de desempenho e outros dados úteis para reprodução do problema.

Os dumps ficam em:

~~~text
sdmc:/3ds/The Minish Cap 3DS/dumps/
~~~

Ao reportar crash, bug gráfico ou problema de desempenho, envie essa pasta sempre que possível.

## Compilação

Requisitos principais:

- devkitPro / devkitARM
- libctru
- Citro2D
- Citro3D
- CMake
- makerom e bannertool para gerar CIA

Build:

~~~sh
chmod +x platform/3ds/build.sh
./platform/3ds/build.sh
~~~

Os pacotes são gerados em:

~~~text
build-3ds/game/
~~~

A compilação **não inclui nem incorpora uma ROM**.

## Créditos

- [EstebanPdN/zelda-tmc-3ds](https://github.com/EstebanPdN/zelda-tmc-3ds) — base do port para Nintendo 3DS.
- [Project Picori](https://github.com/999sian/tmc) — engine e infraestrutura do port.
- [zeldaret/tmc](https://github.com/zeldaret/tmc) — decompilação original.
- [samyost1/tmc-android](https://github.com/samyost1/tmc-android) — base do trabalho de tela dupla.
- [Raekwon1603/tmc-android](https://github.com/Raekwon1603/tmc-android) — trabalho de plataforma Android relacionado ao fork de tela dupla.
- **Bruno Cruz** — edição 3DS, interface, qualidade de vida, RetroAchievements, Full View e manutenção desta versão.

## Licença e aviso legal

O código-fonte é distribuído sob a [GPL-3.0](LICENSE). Componentes de terceiros mantêm suas próprias licenças conforme [THIRD-PARTY-LICENSES.md](THIRD-PARTY-LICENSES.md).

Nintendo é proprietária de The Legend of Zelda, The Minish Cap e de todo o conteúdo associado ao jogo.

Este é um projeto não oficial feito por fãs e não possui afiliação ou endosso da Nintendo.

Nenhuma ROM, asset extraído da Nintendo, save ou firmware é distribuído com este projeto.
