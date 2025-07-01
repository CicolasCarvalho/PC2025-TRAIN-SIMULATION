# Simulação de Entidades e Cidades

Este trabalho foi desenvolvido por Nícolas dos Santos Carvalho, UEM 2025

Este projeto é uma simulação gráfica desenvolvida em C com a biblioteca Raylib. Ele simula um mundo com cidades e entidades (agentes) que se movem entre elas. O projeto suporta multithreading para gerenciar a lógica das entidades, permitindo a distribuição da carga de trabalho entre múltiplos núcleos de processador.

## Funcionalidades

-   Geração procedural de um mundo com cidades e conexões (arestas).
-   Simulação de múltiplas entidades movendo-se entre as cidades.
-   Renderização gráfica utilizando a biblioteca Raylib.
-   Suporte a multithreading para atualização do estado das entidades.
-   Controles interativos para visualizar diferentes aspectos da simulação.
-   Configuração flexível através de macros em um arquivo de cabeçalho (`defs.h`).

## Dependências

Para compilar e executar este projeto, você precisará dos seguintes componentes:

-   **Compilador C**: `gcc` ou `clang`.
-   **Make**: Para automatizar o processo de compilação.
-   **Biblioteca Raylib**: Para a renderização gráfica.
-   **Biblioteca Pthreads**: Para o suporte a multithreading (geralmente incluída em sistemas baseados em Linux).

### Instalação das Dependências (Exemplo para Debian/Ubuntu)

1.  **Instale as ferramentas de compilação:**
    ```bash
    sudo apt update
    sudo apt install build-essential
    ```

2.  **Instale a biblioteca Raylib:**
    ```bash
    sudo apt install libraylib-dev
    ```

## Como Compilar e Executar

O projeto utiliza um `Makefile` para simplificar o processo de compilação.

1.  **Clone o repositório (se ainda não o fez):**
    ```bash
    # git clone <URL_DO_REPOSITORIO>
    # cd <NOME_DO_DIRETORIO>
    ```

2.  **Compile o projeto:**
    Execute o comando `make` no diretório raiz do projeto.
    ```bash
    make
    ```
    Isso compilará os fontes e criará o executável em `build/out`.

3.  **Execute a aplicação:**
    Você pode executar o programa diretamente ou usar o comando do Makefile.
    ```bash
    ./build/out
    ```
    Ou, de forma mais simples:
    ```bash
    make run
    ```

4.  **Limpar os arquivos de compilação:**
    Para remover os arquivos de objeto e o executável, use:
    ```bash
    make clean
    ```

## Controles da Simulação

Durante a execução, você pode usar as seguintes teclas:

-   `J`: Ativa/desativa a visualização das cidades de destino para cada entidade.
-   `H`: Mostra/esconde as arestas (conexões) entre as cidades.
-   `1` ao `9`: Filtra a visualização para mostrar apenas as entidades gerenciadas por uma thread específica (se `MULTITHREADING` estiver ativado).
-   `0`: Remove o filtro e mostra as entidades de todas as threads.

## Configuração (`src/defs.h`)

É possível personalizar vários parâmetros da simulação editando o arquivo `/home/nicolas/uem/PC2025-TRAIN-SIMULATION/src/defs.h`.

Aqui estão as principais macros que você pode alterar:

-   **`DEBUG_INFO`**:
    -   `1`: Ativa logs detalhados no console, incluindo nome do arquivo e linha de cada mensagem. Útil para depuração.
    -   `0`: Desativa os logs detalhados para uma saída mais limpa.

-   **`WINDOW_WIDTH`, `WINDOW_HEIGHT`**:
    -   Define a largura e a altura da janela da aplicação em pixels.
    -   Exemplo: `define WINDOW_WIDTH 1280`, `define WINDOW_HEIGHT 720`.

-   **`CITY_COUNT`**:
    -   Define o número de cidades que serão geradas no mundo.
    -   Exemplo: `define CITY_COUNT 20`.

-   **`ENTITY_COUNT`**:
    -   Define o número total de entidades (agentes) na simulação.
    -   Exemplo: `define ENTITY_COUNT 500`.

-   **`MULTITHREADING`**:
    -   `1`: Ativa o modo de multithreading. A lógica das entidades será dividida entre várias threads.
    -   `0`: Desativa o multithreading. Todas as entidades serão processadas na thread principal.

-   **`THREAD_COUNT`**:
    -   Define o número de threads que serão criadas se `MULTITHREADING` estiver ativado. O número de entidades (`ENTITY_COUNT`) será distribuído entre essas threads.
    -   Exemplo: `define THREAD_COUNT 4`.

**Importante**: Após modificar o arquivo `defs.h`, você precisa **recompilar o projeto** com `make` para que as alterações tenham efeito.