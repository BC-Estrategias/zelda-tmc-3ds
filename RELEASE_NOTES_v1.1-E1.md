# The Minish Cap 3DS v1.1-E1 — Auto Update Preview

> Pre-release de teste. A versão estável v1.0.1 continua sendo a recomendada para uso normal.

## Objetivo desta build

Esta é a primeira build do fork BC com o sistema de atualização pelo próprio Nintendo 3DS reativado.

### O que testar

- Nova opção **Ajustes > Atualizacao**.
- Canais **Stable** e **Pre-release**.
- Consulta das releases diretamente no repositório BC-Estrategias/zelda-tmc-3ds.
- Changelog exibido na tela superior.
- Download da CIA ou 3DSX correta para o tipo de instalação atual.
- Verificação do tamanho, URL, Title ID e SHA-256 antes da instalação.
- CIA instalada por cima do mesmo Title ID sem apagar ROM ou dados do projeto.
- 3DSX substituído com proteção de rollback.
- Fechamento automático do aplicativo depois de uma atualização concluída.
- Compatibilidade do updater com uma sessão ativa de RetroAchievements.

## Importante

A v1.0.1 estável não possui updater ativo. Portanto, a primeira instalação desta E1 deve ser feita manualmente.

Depois de instalar a E1, o teste real de atualização in-app será feito publicando uma E2. O caminho esperado será:

~~~text
v1.1-E1 -> Ajustes > Atualizacao -> Pre-release -> v1.1-E2 -> Baixar e instalar
~~~

Salve o jogo antes de instalar uma atualização.

## Assets esperados

~~~text
tmc-3ds-v1.1-E1.cia
tmc-3ds-v1.1-E1.3dsx
~~~

A ROM original não é distribuída com esta release.
