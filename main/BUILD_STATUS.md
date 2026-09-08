# BUILD STATUS - POLICIA BR

- Fonte completa do sistema policial integrada.
- Build aberto adicionado em `main/illicit/Makefile`.
- CI adicionado em `.github/workflows/build.yml` usando PS3DEV/PSL1GHT.
- O CI nao baixa nem inclui Sony SDK/SNC proprietario.
- Artefatos esperados: `PoliciaBR.sprx`, `PoliciaBR.self`, `PoliciaBR.elf`.
- A geracao do SPRX ainda depende da compatibilidade da toolchain PS3DEV com este codigo de hook de jogo e dos enderecos do EBOOT BLES01807.
