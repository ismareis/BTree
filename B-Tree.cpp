#include <iostream>
#include <fstream>
#include <string>
using namespace std;

template <typename T>
class Node {
public:
    T* keys;
    Node<T>** children;
    int n;
    bool leaf;

    Node(int m, bool _leaf) {
        leaf = _leaf;
        keys = new T[2 * m - 1];
        children = new Node<T>*[2 * m];
        n = 0;
    }
};

template <typename T>
class BTree {
    private:
        Node<T>* root;
        int m;


        // FUNÇÕES INTERNAS DE BUSCA

        bool search(Node<T>* node, T k) {
            if (!node) return false;

            int i = 0;
            while (i < node->n && k > node->keys[i]) i++;

            if (i < node->n && node->keys[i] == k)
                return true;

            if (node->leaf)
                return false;

            return search(node->children[i], k);
        }
        



        // FUNÇÕES INTERNAS DE INSERÇÃO

        void split(Node<T>* parent, int index) {
            Node<T>* full = parent->children[index];
            Node<T>* newNode = new Node<T>(m, full->leaf);

            // copia chaves da metade direita
            for (int j = 0; j < m - 1; j++)
                newNode->keys[j] = full->keys[j + m];

            // se tem filhos, copia filhos da metade da direita
            if (!full->leaf) {
                for (int j = 0; j < m; j++)
                    newNode->children[j] = full->children[j + m];
            }

            // atualiza n
            newNode->n = m - 1;
            full->n = m - 1;

            // desloca filhos do pai para a direita para abrir espaço para newNode
            for (int j = parent->n; j >= index + 1; j--)
                parent->children[j + 1] = parent->children[j];

            //insere newNode no pai
            parent->children[index + 1] = newNode;

            // desloca as chaves do pai para a direita para abrir espaço para chave
            for (int j = parent->n - 1; j >= index; j--)
                parent->keys[j + 1] = parent->keys[j];

            // sobe o meio
            parent->keys[index] = full->keys[m - 1];
            parent->n++;
        }

        void insertRecursive(Node<T>* node, T k) {
            // começa pelo último índice
            int i = node->n - 1;

            // chegou na folha
            if (node->leaf) {
                // procura lugar certo para a chave da direita para esquerda e vai abrindo espaço
                while (i >= 0 && node->keys[i] > k) {
                    node->keys[i + 1] = node->keys[i];
                    i--;
                }

                // insere chave
                node->keys[i + 1] = k;
                node->n++;
                return;
            }

            // encontra filho que quer inserir da esquerda para direita
            i = 0;
            while (i < node->n && k > node->keys[i])
                i++;

            // node filho cheio
            if (node->children[i]->n == 2 * m - 1) {
                split(node, i);

                // decide se vai para filho esquerdo ou direito, porque split mudou estrutura
                if (k > node->keys[i])
                    i++;
            }

            insertRecursive(node->children[i], k);
        }
        

        // FUNÇÕES INTERNAS DE REMOÇÃO

        // maior valor da subárvore (folha mais à direita)
        T getMaior(Node<T>* node) {
            while (!node->leaf)
                node = node->children[node->n];
            return node->keys[node->n - 1];
        }

        // Caso 3: irmão esquerdo rico > rotação para direita
        void rotacaoDireita(Node<T>* parent, int i) {
            Node<T>* child  = parent->children[i]; // node com underflow
            Node<T>* brother = parent->children[i - 1]; // node rico

            // abre espaço nas chaves (esquerda para direita)
            for (int j = child->n - 1; j >= 0; j--)
                child->keys[j + 1] = child->keys[j];

            // se é folha, abre espaço nos filhos (esquerda para direita)
            if (!child->leaf)
                for (int j = child->n; j >= 0; j--)
                    child->children[j + 1] = child->children[j];

            // desce chave do pai para o filho
            child->keys[0] = parent->keys[i - 1];

            // sobe última chave do irmão para o pai
            parent->keys[i - 1] = brother->keys[brother->n - 1];

            // se não é folha, transfere último filho do irmão
            if (!child->leaf)
                child->children[0] = brother->children[brother->n];

            // atualiza n
            child->n++;
            brother->n--;
        }

        // Caso 3: irmão direito rico > rotação para esquerda
        void rotacaoEsquerda(Node<T>* parent, int i) {
            Node<T>* child   = parent->children[i]; // node com underflow
            Node<T>* brother = parent->children[i + 1]; // node rico

            // desce chave do pai para o fim do filho
            child->keys[child->n] = parent->keys[i];

            // sobe primeira chave do irmão para o pai
            parent->keys[i] = brother->keys[0];

            // se não folha, transfere primeiro filho do irmão
            if (!child->leaf)
                child->children[child->n + 1] = brother->children[0];

            // desloca chaves irmão para esquerda
            for (int j = 1; j < brother->n; j++)
                brother->keys[j - 1] = brother->keys[j];
            
            // se não folha, desloca filhos do irmão para esquerda
            if (!brother->leaf)
                for (int j = 1; j <= brother->n; j++)
                    brother->children[j - 1] = brother->children[j];

            // atualiza n
            child->n++;
            brother->n--;
        }

        // Caso 4: dois nodes pobres
        void merge(Node<T>* parent, int i) {
            Node<T>* left  = parent->children[i]; // node esquerdo pobre
            Node<T>* right = parent->children[i + 1]; // node direito pobre

            // puxa chave separadora do pai para o nó esquerdo
            left->keys[left->n] = parent->keys[i];

            // copia chaves do irmão direito
            for (int j = 0; j < right->n; j++)
                left->keys[left->n + 1 + j] = right->keys[j];

            // se não folha, copia filhos do irmão direito
            if (!left->leaf)
                for (int j = 0; j <= right->n; j++)
                    left->children[left->n + 1 + j] = right->children[j];
            
            // atualiza n
            left->n += right->n + 1;

            // remove chave separadora do pai
            for (int j = i + 1; j < parent->n; j++)
                parent->keys[j - 1] = parent->keys[j];

            // remove ponteiro para filho direito do pai
            for (int j = i + 2; j <= parent->n; j++)
                parent->children[j - 1] = parent->children[j];

            // atualiza n
            parent->n--;

            // remove node direito
            delete right;
        }

        // garante que o filho[i] tem ao menos m chaves antes de descer
        void fix(Node<T>* node, int i) {
            Node<T>* child = node->children[i];

            if (child->n >= m)
                return; // filho sem underflow

            bool hasLeft  = (i > 0) && (node->children[i - 1]->n >= m);
            bool hasRight = (i < node->n) && (node->children[i + 1]->n >= m);

            if (hasLeft) {
                rotacaoDireita(node, i);   // Caso 3: irmão esquerdo rico
            } else if (hasRight) {
                rotacaoEsquerda(node, i);  // Caso 3: irmão direito rico
            } else {
                // Caso 4: todos irmãos pobres > merge
                if (i < node->n)
                    merge(node, i); // merge com irmão direito
                else
                    merge(node, i - 1); // merge com irmão esquerdo
            }
        }

        void removeRecursive(Node<T>* node, T k) {
            int i = 0;
            while (i < node->n && k > node->keys[i]) i++;

            // chave encontrada neste nó
            if (i < node->n && node->keys[i] == k) {

                if (node->leaf) {
                    // Caso 1: remove da folha direto
                    for (int j = i + 1; j < node->n; j++)
                        node->keys[j - 1] = node->keys[j];
                    node->n--;

                } else {
                    // Caso 2: nó interno → substitui pelo predecessor
                    T maior = getMaior(node->children[i]);
                    node->keys[i] = maior;

                    // garante que o filho esquerdo tem chaves suficientes
                    fix(node, i);

                    // remove o predecessor na subárvore esquerda
                    removeRecursive(node->children[i], maior);
                }

            } else {
                // chave não está neste nó, desce para o filho correto
                if (node->leaf) {
                    cout << "Chave nao encontrada\n";
                    return;
                }

                // garante que filho tem chaves suficientes antes de descer
                fix(node, i);

                // após fix, o índice pode ter mudado (merge com esquerdo)
                if (i > node->n) i--;

                removeRecursive(node->children[i], k);
            }
        }


        // FUNÇÕES INTERNAS DE CAMINHAMENTO

        void inOrder(Node<T>* node) {
            if (!node) return;

            int i;
            for (i = 0; i < node->n; i++) {
                if (!node->leaf)
                    inOrder(node->children[i]);

                cout << node->keys[i] << " ";
            }

            if (!node->leaf)
                inOrder(node->children[i]);
        }

        void preOrder(Node<T>* node) {
            if (!node) return;

            for (int i = 0; i < node->n; i++)
                cout << node->keys[i] << " ";

            if (!node->leaf) {
                for (int i = 0; i <= node->n; i++)
                    preOrder(node->children[i]);
            }
        }

        void postOrder(Node<T>* node) {
            if (!node) return;

            if (!node->leaf) {
                for (int i = 0; i <= node->n; i++)
                    postOrder(node->children[i]);
            }

            for (int i = 0; i < node->n; i++)
                cout << node->keys[i] << " ";
        }



        // RECURSIVA DE CONVERSÃO DOT

        void dotRecursive(Node<T>* node, int& id, ofstream& out) {
            if (!node) return;

            int myId = id++;

            // desenha o nó como uma tabela HTML (record shape)
            out << "  node" << myId << " [label=\"";
            for (int i = 0; i < node->n; i++) {
                if (i > 0) out << " | ";
                out << node->keys[i];
            }
            out << "\"];\n";

            // desenha filhos
            if (!node->leaf) {
                for (int i = 0; i <= node->n; i++) {
                    int childId = id;
                    dotRecursive(node->children[i], id, out);

                    // linha do pai para o filho
                    out << "  node" << myId << " -> node" << childId << ";\n";
                }
            }
        }
    public:
        BTree(int _m) {
            root = nullptr;
            m = _m;
        }
        
        // Inserção Principal
        void Insert(T k) {
            // verificar se já existe
            if (search(root, k)) {
                cout << "Chave ja existe\n";
                return;
            }

            // árvore vazia
            if (root == nullptr) {
                root = new Node<T>(m, true);
                root->keys[0] = k;
                root->n = 1;
                return;
            }

            // raiz cheia
            if (root->n == 2 * m - 1) {
                Node<T>* newRoot = new Node<T>(m, false);
                newRoot->children[0] = root;

                split(newRoot, 0);

                root = newRoot;
            }

            insertRecursive(root, k);
        }

        // Remoção Principal
        void Remove(T k) {
            // Arvoré vazia
            if (root == nullptr){ 
                cout << "Arvore vazia\n";
                return; 
            }

            // Chave não existe
            if (!search(root, k)) {
                cout << "Chave nao encontrada\n";
                return;
            }

            removeRecursive(root, k);

            // Se a raiz ficou vazia após um merge, desce um nível
            if (root->n == 0) {
                Node<T>* old = root;
                root = root->leaf ? nullptr : root->children[0];
                delete old;
            }
        }

        // Busca Principal
        bool Search(T k){
            return search(root, k);
        }

        // Caminhamentos
        void InOrder() {
            inOrder(root);
            cout << endl;
        }

        void PreOrder() {
            preOrder(root);
            cout << endl;
        }

        void PostOrder() {
            postOrder(root);
            cout << endl;
        }


        // Conversão para .dot
        void ToDot(const string& filename) {
            ofstream out(filename);
            out << "digraph BTree {\n";
            out << "  node [shape=record];\n";

            int id = 0;
            dotRecursive(root, id, out);

            out << "}\n";
            out.close();
            cout << "Arquivo gerado: " << filename << endl;
        }
};

int main(){
    int m = 2;
    BTree<int> tree(m);

    int adicionar[] = {};

    int remover[] = {};

    for (int v : adicionar) {
        tree.Insert(v);
    }

    for (int v : remover) {
        tree.Remove(v);
    }

    tree.ToDot("final.dot");
}
