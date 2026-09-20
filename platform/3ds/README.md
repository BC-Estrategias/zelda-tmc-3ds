# The Minish Cap 3DS Platform

Este diretório contém o frontend nativo para Nintendo 3DS.

A tela superior apresenta o jogo; a tela inferior reúne mapa, status, itens, cheats, configurações, Randomizer e RetroAchievements.

## Instalação no console

Instale a CIA mais recente com FBI ou use o 3DSX pelo Homebrew Launcher.

Crie no cartão SD:

~~~text
sdmc:/3ds/The Minish Cap 3DS/
~~~

Coloque ali uma ROM .gba legalmente obtida. O nome do arquivo pode ser qualquer um.

O loader aceita bases com os game codes:

~~~text
USA:    BZME
Europe: BZMP
~~~

SHA-1 das ROMs limpas conhecidas:

~~~text
USA:    b4bd50e4131b027c334547b4524e2dbbd4227130
Europe: cff199b36ff173fb6faf152653d1bccf87c26fb7
~~~

Traduções PT-BR baseadas na ROM USA também funcionam quando preservam a base BZME. Para RetroAchievements, ROMs PT-BR compatíveis com essa base podem usar o conjunto da versão americana.

A ROM permanece no cartão SD e nunca é incorporada à CIA ou ao 3DSX.

## Áudio

O áudio requer o DSP firmware do próprio 3DS.

Se homebrew estiver sem som, abra o Rosalina com:

~~~text
L + ↓ + SELECT
~~~

e execute:

~~~text
Miscellaneous options
> Dump DSP firmware
~~~

Normalmente esse procedimento precisa ser feito apenas uma vez por console.

## Display

- Aspectos disponíveis: Wide, Original e Stretch.
- Filtros disponíveis: Blur, Bilinear e Pixel Perfect.
- Full View experimental: no New 3DS, Wide + Pixel Perfect habilita a política Full View em salas compatíveis.
- O Full View desta edição só publica 400×240 quando a sala e o conteúdo renderizado podem preencher o viewport completo.
- Interiores pequenos não são convertidos para uma câmera 200×120 ampliada em 2×.
- Diálogos normais permanecem em Full View quando a sala continua elegível.
- Transições, telas fixas, overlays incompatíveis e mapas pequenos fazem fallback seguro.
- Tela inferior: 320×240 com mapa, masmorras, status, itens, cheats, configurações, Randomizer e RetroAchievements.
- Renderização: presenter PICA200/Citro2D com caminhos de PPU adaptados ao 3DS.
- O perfil de desempenho é selecionado automaticamente pelo modelo do console.
- New 3DS solicita 804 MHz, cache L2 e recursos extras de CPU/GPU quando disponíveis.
- Old 3DS usa um perfil mais econômico com otimizações de renderização/áudio e apresentação adaptativa.

## Atalhos

~~~text
X       alterna Missões > Mapa > Itens > Cheats
ZL + X  quick save state
ZL + Y  abre confirmação de quick load state
ZR      turbo no New 3DS
C-Stick turbo no New 3DS
L+R+A   dump de diagnóstico
~~~

O turbo pode ser configurado entre 2× e 5×.

## Qualidade de vida

As opções persistentes incluem velocidade do Link, texto rápido, carga de espada rápida, portal Minish rápido, Kinstones rápidos, miniaturas rápidas, marcadores de pedaços de coração e início direto na seleção de arquivo.

A aba Cheats permite recuperar/reabastecer recursos durante testes e gameplay.

## RetroAchievements

A integração usa rcheevos diretamente no port 3DS.

- sessão persistente no cartão SD;
- lista de conquistas, badges e pontuação na tela inferior;
- notificações de desbloqueio;
- suporte à ROM USA limpa;
- compatibilidade explícita com ROMs PT-BR baseadas em BZME para usar o set USA;
- o hash real da ROM carregada continua disponível nos diagnósticos.

A versão europeia continua suportada para jogar, mas o conjunto de conquistas desta edição é destinado à base USA.

## Randomizer

O Project Picori Randomizer está integrado com perfil de save separado do jogo normal e opções próprias de lógica, entradas, Kinstones, item pool e cosméticos.

## Diagnóstico

Pressione:

~~~text
L + R + A
~~~

para pausar e gerar um dump em:

~~~text
sdmc:/3ds/The Minish Cap 3DS/dumps/
~~~

Os dumps podem incluir capturas das telas, memória, estado de execução, métricas de performance, informações de áudio, inputs e estado persistente necessário para investigar problemas.

## Build

Requisitos:

- devkitPro
- devkitARM
- libctru
- Citro2D
- Citro3D
- CMake
- makerom e bannertool para empacotar CIA

Execute:

~~~sh
chmod +x platform/3ds/build.sh
./platform/3ds/build.sh
~~~

Os pacotes são escritos em:

~~~text
build-3ds/game/
~~~

Nenhuma ROM, save ou asset extraído da Nintendo é incluído na build.
