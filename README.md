# BTree

Implementação de uma **B-Tree genérica em C++**, desenvolvida para a disciplina de **Algoritmos e Programação: Estruturas de Dados**.

---

## 📌 Descrição

Este projeto contém uma implementação completa de uma **árvore B (B-Tree)**, incluindo:

- Inserção de elementos  
- Remoção de elementos  
- Caminhamentos (pré-ordem, em ordem e pós-ordem)  
- Geração de representação gráfica no formato `.dot`  

A implementação foi projetada para ser **genérica**, permitindo trabalhar com diferentes tipos de dados.

---

## 🗂️ Estrutura do Repositório

O repositório está organizado da seguinte forma:

### 🔹 Arquivo principal

- `B-Tree.cpp`  
  Contém a implementação da estrutura de dados B-Tree, incluindo todas as operações principais e a funcionalidade de exportação para `.dot`.
  

### 🔹 Pastas de Árvores (Árvore 1 até Árvore 5)

Cada pasta representa um **cenário específico de execução da B-Tree**, contendo:

#### 📁 `dot/`
Arquivos `.dot` gerados após cada operação realizada na árvore.

#### 📁 `png/`
Imagens `.png` geradas a partir dos arquivos `.dot`, mostrando a evolução da árvore passo a passo.

#### 📁 `svg/`
Imagem `.svg` contendo o estado **final** da árvore.

#### 📄 `instructions.txt`
Arquivo com:
- Parâmetros da árvore (ex: `m = 4`)
- Sequência de operações (inserções e remoções) utilizadas para gerar a árvore

---

## ⚙️ Como utilizar

### 1. Compilar o código

```bash
g++ B-Tree.cpp -o btree
```

### 2. Executar
```bash
./btree
```


### 3. Gerar visualizações

Os arquivos .dot podem ser convertidos em imagens usando Graphviz:
```bash
dot -Tpng arquivo.dot -o arquivo.png
dot -Tsvg arquivo.dot -o arquivo.svg
```
---

## 📊 Objetivo

O projeto tem como objetivo demonstrar, na prática:
- O funcionamento interno de uma B-Tree
- O comportamento das operações de inserção e remoção
- A evolução estrutural da árvore ao longo das operações

---

## 📚 Observações
- Cada pasta de árvore representa um caso de teste independente
- A visualização gráfica facilita o entendimento das operações
- Os arquivos .dot permitem reproduzir todas as etapas da construção da árvore

---

Projeto desenvolvido para fins acadêmicos.
