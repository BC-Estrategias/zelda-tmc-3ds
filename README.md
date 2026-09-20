# The Legend of Zelda: The Minish Cap — Nintendo 3DS Edition

<p align="center">
  <strong>Uma edição para Nintendo 3DS com tela dupla, controles modernos e recursos opcionais de qualidade de vida.</strong>
</p>

> Projeto não oficial, criado para uso com uma cópia legal da ROM de Game Boy Advance. A ROM, seus dados e seus recursos nunca são distribuídos neste repositório nem fazem parte da CIA.

## O que é

Esta é uma adaptação nativa de *The Legend of Zelda: The Minish Cap* para Nintendo 3DS. Ela usa a tela superior para a ação e a inferior para mapa, missões, itens, cheats, configurações e RetroAchievements — sem substituir a experiência original do jogo.

O projeto parte de trabalhos abertos do Project Picori, da decompilação `zeldaret/tmc` e do port Android de tela dupla. Esta edição e suas melhorias para 3DS são mantidas por **Bruno Cruz**.

## Recursos

### Apresentação e controles

- Modos de imagem **Wide**, Original e Stretch, com filtros Blur, Bilinear, Ultra Sharp e Pixel Perfect.
- **Full View experimental** no New Nintendo 3DS: amplia a área visível em cenários compatíveis sem esticar os pixels.
- Interface inferior em português: mapa, masmorras, missões, itens e opções por toque.
- O botão **X** alterna rapidamente entre Missões → Mapa → Itens → Cheats.
- **ZR** ativa avanço rápido. **Y** executa o atalho de rolar e atacar quando esse movimento estiver disponível na aventura.
- Save states rápidos: **ZL + A** salva e **ZL + Y** carrega o ponto mais recente, com confirmação para evitar acionamentos acidentais.

### Qualidade de vida

- Velocidade do Link configurável em **1×, 1,5× ou 2×**, também aplicada à natação e à forma Minish.
- Opções para textos mais rápidos, carga de espada acelerada e portais Minish rápidos.
- Fusão de Kinstones e coleção de miniaturas com fluxos encurtados.
- Acesso rápido a recursos no menu Cheats: vida, rupias, conchas, Kinstones, bombas e flechas.
- O reabastecimento de Kinstones preserva corretamente as peças douradas de história: peças já usadas são removidas; peças ainda necessárias permanecem em quantidade segura.
- Marcadores de pedaços de coração ainda pendentes no mapa-múndi, lidos a partir dos dados permanentes do save.
- Opção para iniciar diretamente na seleção de arquivo.

### RetroAchievements

- Integração nativa com **RetroAchievements** para a versão americana do jogo e ROMs PT-BR baseadas nela.
- Login persistente no cartão SD; a senha não precisa ser digitada a cada inicialização.
- Lista de conquistas na tela inferior, com concluídas primeiro, e aviso visual no canto superior direito quando uma conquista é desbloqueada.
- O jogo continua funcionando normalmente sem conexão de rede ou sem conta RetroAchievements.

### Diagnóstico e estabilidade

- Perfis específicos para New 3DS e Old 3DS.
- Correções de renderização, camadas especiais e quedas pontuais de desempenho encontradas durante a campanha.
- Pressione **L + R + A** para criar um dump de diagnóstico: capturas das telas, estado de desempenho e checkpoint reproduzível.
- Duplicação de estados de sala e carregamentos estáticos evitada quando possível para reduzir pequenas pausas ao mudar de área.

## New Nintendo 3DS

No New 3DS, a edição usa os recursos extras do aparelho: CPU em 804 MHz, cache L2, núcleo adicional para trabalho de renderização e avanço rápido pelo C-Stick. Isso permite a apresentação Full View e reduz a chance de queda de quadros em áreas mais pesadas.

No Old 3DS, o jogo também é suportado com um perfil mais econômico. Alguns recursos visuais experimentais podem ser mais limitados para preservar a fluidez.

## Instalação

1. Baixe a CIA na página de [Releases](../../releases/latest) ou leia o QR Code disponibilizado junto da versão.
2. Instale a CIA com o FBI.
3. No cartão SD, crie a pasta:

   ```text
   sdmc:/3ds/The Minish Cap 3DS/
   ```

4. Coloque nessa pasta sua ROM `.gba` legalmente obtida. O nome do arquivo é livre.
5. Abra o jogo e, se desejar, configure o RetroAchievements pela aba da tela inferior.

### ROMs aceitas

A edição reconhece ROMs USA e Europeias limpas, além de traduções PT-BR compatíveis com a base americana. Para RetroAchievements, a ROM PT-BR é identificada como **PT-BR base USA**, sem alterar a lógica original do jogo.

SHA-1 das ROMs limpas esperadas:

```text
USA:    b4bd50e4131b027c334547b4524e2dbbd4227130
Europe: cff199b36ff173fb6faf152653d1bccf87c26fb7
```

## Atualizações e suporte

- A CIA não contém ROM, assets extraídos, saves nem firmware.
- Ao relatar um problema, envie a pasta criada em `sdmc:/3ds/The Minish Cap 3DS/dumps/`.
- A documentação técnica do alvo 3DS fica em [platform/3ds/README.md](platform/3ds/README.md).

## Compilação

São necessários devkitPro, devkitARM, libctru, Citro2D, Citro3D, CMake, `makerom` e `bannertool`.

```sh
platform/3ds/build.sh
```

Os pacotes são gerados em `build-3ds/game/`. A compilação não incorpora uma ROM.

## Créditos

- [Project Picori](https://github.com/999sian/tmc) — engine e infraestrutura do port.
- [zeldaret/tmc](https://github.com/zeldaret/tmc) — decompilação original.
- [samyost1/tmc-android](https://github.com/samyost1/tmc-android) — base do port Android de tela dupla.
- [EstebanPdN/zelda-tmc-3ds](https://github.com/EstebanPdN/zelda-tmc-3ds) — ponto de partida do port para Nintendo 3DS.
- Bruno Cruz — edição, recursos de qualidade de vida, interface e manutenção desta versão.

## Licença e aviso legal

O código-fonte é distribuído sob a [GPL-3.0](LICENSE). Componentes de terceiros mantêm suas próprias licenças.

*The Legend of Zelda* e *The Minish Cap* pertencem à Nintendo. Este projeto é uma iniciativa de fãs, sem afiliação ou endosso da Nintendo.
