/* covid_registry.c
Sistema simples para cadastrar pacientes diagnosticados com COVID-19
Alunas: Caroline Moraes Monteiro - UP25116559
        Rayssa Luanna Gomes da Serra Vieira - UP25119984
        Rosicléia Silva de Oliveira -  UP25106074
Compilar: gcc -std=c99 -o covid_registry covid_registry.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#define MAX_PACIENTES 1000

#define TAM_NOME 100

#define TAM_CONTATO 50

#define TAM_DATA 12 // Corrigido para 12 para acomodar DD/MM/AAAA\0

#define TAM_GRAV 20

#define TAM_OBS 200

#define ARQ "pacientes.csv"

typedef struct {
    int id;
    char nome[TAM_NOME];
    int idade;
    char sexo; /* 'M' 'F' 'O' */
    char contato[TAM_CONTATO];
    char data_diag[TAM_DATA]; /* DD/MM/AAAA */
    char gravidade[TAM_GRAV]; /* leve, medio, grave */
    char obs[TAM_OBS];
    int ativo; /* 1 = presente, 0 = removido */
} Paciente;

Paciente lista[MAX_PACIENTES];
int total = 0;
int next_id = 1;

/* utilitários */
void limpa_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

void ler_string(const char *prompt, char *dest, int tamanho) {
    printf("%s", prompt);
    if (fgets(dest, tamanho, stdin)) {
        size_t ln = strlen(dest);
        if (ln > 0 && dest[ln-1] == '\n') {
            dest[ln-1] = '\0';
        }
    } else {
        dest[0] = '\0';
        limpa_buffer();
    }
}

/* carregar CSV */
void carregar() {
    FILE *f = fopen(ARQ, "r");
    if (!f) return;

    char linha[1024];
    if (!fgets(linha, sizeof(linha), f)) {
        fclose(f);
        return;
    } /* pular header */

    while (fgets(linha, sizeof(linha), f)) {
        Paciente p;
        memset(&p, 0, sizeof(Paciente)); // Inicializa a struct
        char ativo_c;
        char *tok;

        tok = strtok(linha, ";\n");
        if (!tok) continue;
        p.id = atoi(tok);

        tok = strtok(NULL, ";\n");
        if (!tok) continue;
        strncpy(p.nome, tok, TAM_NOME-1);
        p.nome[TAM_NOME-1] = '\0';

        tok = strtok(NULL, ";\n");
        if (!tok) continue;
        p.idade = atoi(tok);

        tok = strtok(NULL, ";\n");
        if (!tok) continue;
        p.sexo = tok[0];

        tok = strtok(NULL, ";\n");
        if (!tok) continue;
        strncpy(p.contato, tok, TAM_CONTATO-1);
        p.contato[TAM_CONTATO-1] = '\0';

        tok = strtok(NULL, ";\n");
        if (!tok) continue;
        strncpy(p.data_diag, tok, TAM_DATA-1);
        p.data_diag[TAM_DATA-1] = '\0';

        tok = strtok(NULL, ";\n");
        if (!tok) continue;
        strncpy(p.gravidade, tok, TAM_GRAV-1);
        p.gravidade[TAM_GRAV-1] = '\0';

        tok = strtok(NULL, ";\n");
        if (tok) {
            strncpy(p.obs, tok, TAM_OBS-1);
            p.obs[TAM_OBS-1] = '\0';
        }

        tok = strtok(NULL, ";\n");
        if (!tok) {
            ativo_c = '1';
        } else {
            ativo_c = tok[0];
        }
        p.ativo = (ativo_c == '1');

        if (total < MAX_PACIENTES) lista[total++] = p;
        if (p.id >= next_id) next_id = p.id + 1;
    }
    fclose(f);
}

/* salvar CSV (sobrescreve) */
void salvar() {
    FILE *f = fopen(ARQ, "w");
    if (!f) {
        printf("Erro ao abrir arquivo para salvar.\n");
        return;
    }
    fprintf(f, "id;nome;idade;sexo;contato;data_diag;gravidade;obs;ativo\n");
    for (int i = 0; i < total; ++i) {
        Paciente *p = &lista[i];

        char nome_clean[TAM_NOME];
        char contato_clean[TAM_CONTATO];
        char obs_clean[TAM_OBS];

        strncpy(nome_clean, p->nome, TAM_NOME-1);
        nome_clean[TAM_NOME-1] = '\0';
        for (int j=0; nome_clean[j]; ++j) if (nome_clean[j]==';') nome_clean[j]=',';

        strncpy(contato_clean, p->contato, TAM_CONTATO-1);
        contato_clean[TAM_CONTATO-1] = '\0';
        for (int j=0; contato_clean[j]; ++j) if (contato_clean[j]==';') contato_clean[j]=',';

        strncpy(obs_clean, p->obs, TAM_OBS-1);
        obs_clean[TAM_OBS-1] = '\0';
        for (int j=0; obs_clean[j]; ++j) if (obs_clean[j]==';') obs_clean[j]=',';

        fprintf(f, "%d;%s;%d;%c;%s;%s;%s;%s;%d\n",
            p->id, nome_clean, p->idade, p->sexo, contato_clean, p->data_diag, p->gravidade, obs_clean, p->ativo);
    }
    fclose(f);
}

/* funções CRUD */
void listar() {
    printf("\n--- Lista de Pacientes ---\n");
    if (total==0) {
        printf("Nenhum registro.\n");
        return;
    }

    for (int i=0; i<total; ++i) {
        Paciente *p = &lista[i];
        if (!p->ativo) continue;

        printf("ID: %d | Nome: %s | Idade: %d | Sexo: %c | Data: %s | Gravidade: %s\n",
            p->id, p->nome, p->idade, p->sexo, p->data_diag, p->gravidade);
    }
}

Paciente* buscar_por_id(int id) {
    for (int i=0; i<total; ++i) {
        if (lista[i].id == id) {
            return &lista[i];
        }
    }
    return NULL;
}

void cadastrar() {
    if (total >= MAX_PACIENTES) {
        printf("Capacidade maxima atingida.\n");
        return;
    }

    Paciente p;
    memset(&p, 0, sizeof(Paciente)); // Inicializa a struct
    p.id = next_id++;

    printf("\n--- Cadastro de Paciente (ID %d) ---\n", p.id);
    ler_string("Nome: ", p.nome, TAM_NOME);

    printf("Idade: ");
    if (scanf("%d", &p.idade)!=1) {
        limpa_buffer();
        p.idade=0;
    }
    limpa_buffer();

    printf("Sexo (M/F/O): ");
    p.sexo = getchar();
    limpa_buffer();

    ler_string("Contato (telefone/email): ", p.contato, TAM_CONTATO);
    ler_string("Data do diagnostico (DD/MM/AAAA): ", p.data_diag, TAM_DATA);
    ler_string("Gravidade (leve/medio/grave): ", p.gravidade, TAM_GRAV);
    ler_string("Observacoes (opcional): ", p.obs, TAM_OBS);

    p.ativo = 1;
    lista[total++] = p;
    salvar();
    printf("Paciente cadastrado com sucesso.\n");
}

void mostrar_detalhes(Paciente *p) {
    if (!p) {
        printf("Paciente nao encontrado.\n");
        return;
    }
    printf("\n--- Detalhes do Paciente ID %d ---\n", p->id);
    printf("Nome: %s\n", p->nome);
    printf("Idade: %d\n", p->idade);
    printf("Sexo: %c\n", p->sexo);
    printf("Contato: %s\n", p->contato);
    printf("Data diag: %s\n", p->data_diag);
    printf("Gravidade: %s\n", p->gravidade);
    printf("Observacoes: %s\n", p->obs);
}

/* buscar por nome (substring, case-insensitive) */
void busca_nome() {
    char termo[TAM_NOME];
    ler_string("Digite parte do nome para buscar: ", termo, TAM_NOME);

    /* lowercase both */
    for (int i=0; termo[i]; ++i) termo[i] = (char)tolower((unsigned char)termo[i]);
    int achou = 0;

    for (int i=0; i<total; ++i) {
        if (!lista[i].ativo) continue;
        char nome_low[TAM_NOME];
        strncpy(nome_low, lista[i].nome, TAM_NOME-1);
        nome_low[TAM_NOME-1] = '\0';

        for (int j=0; nome_low[j]; ++j) nome_low[j] = (char)tolower((unsigned char)nome_low[j]);

        if (strstr(nome_low, termo)) {
            printf("ID: %d | Nome: %s | Data: %s | Gravidade: %s\n",
                lista[i].id, lista[i].nome, lista[i].data_diag, lista[i].gravidade);
            achou = 1;
        }
    }
    if (!achou) printf("Nenhum resultado.\n");
}

void editar() {
    int id;
    printf("Digite o ID do paciente para editar: ");
    if (scanf("%d", &id)!=1) {
        limpa_buffer();
        printf("ID invalido.\n");
        return;
    }
    limpa_buffer();

    Paciente *p = buscar_por_id(id);
    if (!p || !p->ativo) {
        printf("Paciente nao encontrado.\n");
        return;
    }
    mostrar_detalhes(p);

    printf("\n-- Novo valor (enter para manter) --\n");
    char buf[TAM_NOME];

    ler_string("Nome: ", buf, TAM_NOME);
    if (buf[0]) {
        strncpy(p->nome, buf, TAM_NOME-1);
        p->nome[TAM_NOME-1] = '\0';
    }

    printf("Idade: ");
    if (fgets(buf, sizeof(buf), stdin)) {
        if (buf[0]!='\n') p->idade = atoi(buf);
    }

    printf("Sexo (M/F/O): ");
    if (fgets(buf, sizeof(buf), stdin)) {
        if (buf[0]!='\n') p->sexo = buf[0];
    }

    ler_string("Contato: ", buf, TAM_CONTATO);
    if (buf[0]) {
        strncpy(p->contato, buf, TAM_CONTATO-1);
        p->contato[TAM_CONTATO-1] = '\0';
    }

    ler_string("Data diag (DD/MM/AAAA): ", buf, TAM_DATA);
    if (buf[0]) {
        strncpy(p->data_diag, buf, TAM_DATA-1);
        p->data_diag[TAM_DATA-1] = '\0';
    }

    ler_string("Gravidade: ", buf, TAM_GRAV);
    if (buf[0]) {
        strncpy(p->gravidade, buf, TAM_GRAV-1);
        p->gravidade[TAM_GRAV-1] = '\0';
    }

    ler_string("Observacoes: ", buf, TAM_OBS);
    if (buf[0]) {
        strncpy(p->obs, buf, TAM_OBS-1);
        p->obs[TAM_OBS-1] = '\0';
    }

    salvar();
    printf("Registro atualizado.\n");
}

void remover() {
    int id;
    printf("Digite o ID do paciente para remover: ");
    if (scanf("%d", &id)!=1) {
        limpa_buffer();
        printf("ID invalido.\n");
        return;
    }
    limpa_buffer();

    Paciente *p = buscar_por_id(id);
    if (!p || !p->ativo) {
        printf("Paciente nao encontrado.\n");
        return;
    }

    printf("Tem certeza que deseja remover %s (ID %d)? (S/N): ", p->nome, p->id);
    char c = getchar();
    limpa_buffer();

    if (c=='S' || c=='s') {
        p->ativo = 0;
        salvar();
        printf("Registro removido (marcado como inativo).\n");
    } else {
        printf("Remocao cancelada.\n");
    }
}

void detalhes_por_id() {
    int id;
    printf("Digite o ID do paciente: ");
    if (scanf("%d", &id)!=1) {
        limpa_buffer();
        printf("ID invalido.\n");
        return;
    }
    limpa_buffer();

    Paciente *p = buscar_por_id(id);
    if (!p || !p->ativo) {
        printf("Paciente nao encontrado.\n");
        return;
    }
    mostrar_detalhes(p);
}

void menu() {
    printf("\n=== Sistema de Cadastro COVID-19 ===\n");
    printf("1. Cadastrar paciente\n");
    printf("2. Listar pacientes\n");
    printf("3. Buscar por nome\n");
    printf("4. Buscar por ID (detalhes)\n");
    printf("5. Editar paciente\n");
    printf("6. Remover paciente\n");
    printf("7. Salvar e Sair\n");
    printf("Escolha: ");
}

int main() {
    carregar();
    int opc = 0;

    while (1) {
        menu();
        if (scanf("%d", &opc)!=1) {
            limpa_buffer();
            printf("Opcao invalida.\n");
            continue;
        }
        limpa_buffer();

        switch (opc) {
            case 1:
                cadastrar();
                break;
            case 2:
                listar();
                break;
            case 3:
                busca_nome();
                break;
            case 4:
                detalhes_por_id();
                break;
            case 5:
                editar();
                break;
            case 6:
                remover();
                break;
            case 7:
                salvar();
                printf("Saindo...\n");
                return 0;
            default:
                printf("Opcao invalida.\n");
        }
    }
    return 0;
}