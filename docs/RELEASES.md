# Publicação de uma versão

Cada lançamento deve conter somente artefatos do port, nunca ROMs, saves ou assets extraídos do jogo.

## Arquivos da versão 1.0

- `tmc-3ds-v1.0.cia` — pacote instalável pelo FBI.
- `tmc-3ds-v1.0.3dsx` — versão para Homebrew Launcher, quando gerada.
- `tmc-3ds-v1.0-qr.png` — QR Code que aponta diretamente para a CIA do release.

## Passos

1. Atualize `platform/3ds/version.txt`.
2. Execute `platform/3ds/build.sh` e teste a CIA em um 3DS.
3. Crie a release com a tag correspondente.
4. Envie a CIA e a 3DSX como assets da release.
5. Gere o QR Code apenas depois de saber a URL final da CIA e envie-o como asset da mesma release.

Para uma cópia nova do código, inicialize as dependências antes de compilar:

```sh
git submodule update --init --recursive
```

O QR Code deve apontar ao download direto da CIA, no formato:

```text
https://github.com/BC-Estrategias/zelda-tmc-3ds/releases/download/v1.0/tmc-3ds-v1.0.cia
```

Isso permite instalar pelo FBI usando **Remote Install → Scan QR Code**.
