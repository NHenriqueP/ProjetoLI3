# Documentação do Projeto

Este projeto implementa um sistema de carregamento de dados (CSV) e execução de queries.

## Estrutura
- `src/entidades/`: tipos do domínio (ex.: aeroportos, voos, reservas, passageiros)
- `src/gestores/`: estruturas de dados e operações de acesso (inserção, lookup)
- `src/parsers/`: parsing de CSV e criação/validação de entidades
- `src/queries/`: implementação das queries
- `src/app_runner.c`: orquestração (carregar dados + executar comandos)

## Como gerar esta documentação

- Via Make:
  - `make docs`

- Diretamente com Doxygen:
  - `doxygen docs/doxygen/Doxyfile`

O HTML é gerado em `docs/doxygen/html/index.html`.

## Documentos

<!-- (removido) funcoes_10.md era um documento extra, não obrigatório -->
