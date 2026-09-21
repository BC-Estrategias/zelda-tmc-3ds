# The Minish Cap 3DS v1.1-E6 — Hardening

> Pré-release de estabilização antes da v1.1. Não adiciona novas funções de gameplay.

## Changelog

- Corrige o cálculo de largura/centralização de textos acentuados no atualizador.
- O atualizador agora ignora prereleases antigas malformadas e continua procurando a versão válida mais recente.
- Corrige a limpeza de buffer/arquivo temporário quando a abertura do download no SD falha.
- Remove sinais invertidos espanhóis das poucas frases onde o renderer não possui glifo dedicado.
- Adiciona testes automatizados do parser/formatter do updater ao GitHub Actions.
- Releases agora dependem de build 3DS + testes do updater.
- O pipeline valida explicitamente a existência da CIA também nas publicações automáticas por commit `release:`.
- Mantém PT-BR / English / Español, UTF-8, updater, Full View e todas as correções já validadas nas E4/E5.

## Teste sugerido

1. Atualize da E5 para a E6 pelo próprio updater.
2. Abra Ajustes → Tela → Idioma e alterne PT-BR / EN / ES.
3. Abra Atualização e confira centralização e acentos do texto.
4. Confirme que o changelog abre normalmente.
5. Faça um Quick Save / Quick Load e teste alguns minutos de gameplay em Full View.
6. Abra o updater novamente e confirme que a E6 aparece como instalada/atualizada.

A ROM original não está incluída.
