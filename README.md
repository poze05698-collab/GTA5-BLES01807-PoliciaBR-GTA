# POLICIA BR - GTA V BLES01807

Projeto de mod menu para GTA V PS3 BLES01807, focado em modo historia/offline.

## Sistemas incluidos

- Entrar/sair de servico
- Viaturas e spawn
- Equipamentos policiais
- Abordagem de NPC
- Perseguicao
- Prisao
- Multas e pontuacao
- Sistema de procurado
- Chamados/central
- Radio
- Backup policial com unidade AI
- Operacoes: patrulhamento, blitz, apoio e cerco
- Fardamentos por forca policial (presets de componentes)
- Delegacias
- Areas operacionais/favelas
- Patentes e progresso
- Menu completo pelo controle

## Compilacao

O projeto possui um caminho de build aberto com **PS3DEV/PSL1GHT**. O workflow `.github/workflows/build.yml` instala a toolchain open source em um runner Ubuntu e tenta gerar `.elf`, `.sprx` e `.self`.

O projeto original `illicit.vcxproj` foi mantido apenas como referencia/legado do template. A compilacao nova usa `Makefile` e nao depende do Sony SNC.

## Importante

A compatibilidade em hardware depende da versao exata do EBOOT/TU e dos enderecos nativos usados pelo template BLES01807. O hook em `police_config.h` e herdado da base publica e precisa ser validado no console.

Os uniformes sao presets simples de componentes; uniformes brasileiros 3D reais exigem assets/modelos adicionais.

Uso destinado a Story Mode/offline.
