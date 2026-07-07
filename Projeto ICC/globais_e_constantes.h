/**
 * ARQUIVO: globais_e_constantes.h
 * DESCRIÇÃO GERAL:
 *   Cabeçalho central do projeto. Concentra:
 *      1) As constantes de configuração/"balanceamento" do jogo (limites de arrays,
 *         tempos, tamanhos de fonte, etc);
 *      2) Os enums auxiliares que representam estados (da nave, do jogo, da rodada, etc);
 *      3) As structs de dados usadas para representar balas (Arma), naves (Nave) e
 *         popups de pontuação (Score_Popups);
 *      4) A declaração (extern) de todas as variáveis globais do jogo, cuja definição
 *         (alocação de fato) acontece em main.c.
 *
 *   Este arquivo é incluído por main.c e por funcoes_logicas.h, funcionando como o
 *   "contrato" de dados compartilhado entre os dois.
 */
#pragma once
#include "raylib.h"

//===========================   DEFINIÇÃO DE VARIÁVEIS E CONSTANTES DE OPERAÇÃO ====================//
//Constantes de Operação:
#define MAX_OPCOES 3            //Quantidade de opções navegáveis no menu principal (Iniciar/Controles/Sair)

#define MAX_ENEMIES 9            //Número máximo de inimigos comuns simultâneos em tela (grade de até 9 naves)
#define MAX_BULLETS 30           //Tamanho do "pool" de balas de cada nave (player, cada inimigo e o boss têm seu próprio array de balas desse tamanho)
#define MAX_INTERVALS 5          //Constante auxiliar (reservada; não é usada diretamente nas funções abaixo)
#define PLAYER_TIME_BETWEEN_BULLETS 0.25f  //Cooldown (em segundos) entre um tiro e outro do player
#define FALL_TIME 2.5f           //Duração (em segundos) da animação de entrada dramática dos inimigos/boss no início de cada rodada

#define MAX_SCORE_POPUPS 32      //Quantidade máxima de popups de pontuação (+10, -10, etc) exibidos ao mesmo tempo
#define HUD_HEIGHT 60            //Altura em pixels da faixa do HUD no topo da tela (área não jogável)
#define HUD_FONT_SIZE 40         //Tamanho da fonte usada nos textos do HUD (score, rodada, vida)
#define SPACING 5                //Espaçamento entre caracteres usado em todas as chamadas de DrawTextEx
#define TITLE_FONT_SIZE 70       //Tamanho da fonte usada em títulos (menu, tela de morte/vitória)
#define TEXT_FONT_SIZE 60        //Tamanho da fonte usada em textos gerais (menu de controles, diálogos da cutscene)

#define MAX_ENEMIES_TYPE 3       //Quantidade de "tipos" de inimigo comum (0=Padrão, 1=Rápido, 2=Lerdo), cada um com seus próprios atributos
#define MAX_EXPLOSION_SPRITES 11 //Quantidade de frames/sprites da animação de explosão de uma nave
#define CUTSCENE_ELEMENTS 3      //Quantidade de imagens usadas na cutscene final (fosas, musk derrotado, cena de "cinema")

#define NOME_ARQ "score.txt"     //Nome do arquivo que armazena os high scores
#define MAX_HIGH_SCORES 5        //Quantidade de scores mostrados no menu(TOP 5)

//MODO DEBUG:
#define DEBUG 0                  //Se 1, habilita impressões de diagnóstico no console (ver main.c) durante o carregamento de texturas

//Enum auxiliar para verficação do deslocamento do inimigo:
//Indica a direção de movimento lateral atual de uma nave (inimigo comum ou boss)
typedef enum{
    PARADO = 0,     //Nave não está se deslocando lateralmente no momento
    ESQUERDA,       //Nave está se movendo para a esquerda
    DIREITA         //Nave está se movendo para a direita
}Deslocamento;

//Enum auxiliar para verificar a origem da bala
//Usado para diferenciar as regras de colisão/dano e a direção de deslocamento de cada bala
typedef enum{
    JOGADOR = 0,    //Bala disparada pelo player (sobe na tela, colide com inimigos/boss)
    INIMIGO,        //Bala disparada por um inimigo comum (desce na tela, colide com o player)
    BOSS            //Bala disparada pelo boss (desce na tela, colide com o player)
}Origem_Bala;

//Enum auxiliar para verificar o estado da nave:
//Controla o ciclo de vida de qualquer Nave (player, inimigo comum ou boss)
typedef enum{
    MORTO = 0,      //Nave destruída/inativa: não é desenhada nem participa da lógica de jogo
    EXPLODINDO,     //Nave foi derrotada e está tocando a animação de explosão (ver explode_nave)
    VIVO            //Nave ativa: pode se mover, atirar e colidir normalmente
}Ship_State;

//Enum auxiliar para saber o estado do jogo:
//Controla em qual "tela"/modo o jogo está, usado no loop principal (main.c) para decidir o que atualizar/desenhar
typedef enum{
    MENU,           //Tela de menu principal (navegação por teclado/mouse)
    GAME,           //Partida em andamento (rodadas de inimigos e/ou boss)
    PAUSA,          //Partida Pausada
    MORTE,          //Tela de "Game Over" após a morte do player
    VITORIA,        //Tela/sequência de vitória após a derrota do boss (cutscene ou tela simples, dependendo do modo "fosas")
    CINEMA,         //Estado reservado para a fase final da cutscene (a transição efetiva é tratada internamente por cutscene()/cinema())
    SAIDA           //Sinaliza que o jogo deve encerrar o loop principal e fechar a janela
}Game_State;

//Enum auxiliar para animar a mudança da rodada:
//Controla a transição entre o fim de uma rodada e o início do combate da próxima
typedef enum{
    NOVA_RODADA,    //Rodada acabou de ser configurada (inimigos/boss posicionados), aguardando a animação de entrada
    ANIMANDO,       //Animação de entrada dramática em andamento (naves "caindo"/subindo até a posição de combate)
    RODADA_ATUAL,   //Animação concluída: a rodada está de fato em jogo (movimento, tiros e colisões liberados)
}Round_State;

//Struct dos tiros:
//Representa uma única bala. Cada Nave possui seu próprio array (pool) de MAX_BULLETS balas,
//reaproveitando (Object Pool) as instâncias inativas em vez de alocar/desalocar em tempo real.
typedef struct{
    Vector2 posicao;        //Posição atual da bala na tela (canto superior esquerdo do sprite)
    int tamanho;            //Lado (em pixels) do quadrado usado para desenhar e para colisão da bala
    float velocidade;       //Velocidade de deslocamento vertical da bala, em pixels/segundo
    float dano;             //Quantidade de vida retirada do alvo ao colidir
    bool ativa;             //true enquanto a bala está em voo/visível; false quando disponível para reuso no pool
    Origem_Bala origem;     //De quem partiu o disparo (define direção do movimento e quem pode ser atingido)

    Texture2D *textura;     //Ponteiro para a textura (sprite) usada para desenhar a bala
} Arma;

//Struct das naves
//Representa qualquer nave do jogo: o player, um inimigo comum ou o boss.
//A mesma struct é reaproveitada para os três casos, diferenciando o comportamento via os
//campos "tipo"/"origem" das balas e pela lógica específica de cada conjunto de funções.
typedef struct{
    Vector2 posicao;                  //Posição atual da nave (canto superior esquerdo do sprite)
    int tamanho;                      //Lado (em pixels) do quadrado usado para desenhar e para colisão da nave
    float velocidade;                 //Velocidade de deslocamento da nave, em pixels/segundo
    float intervalo_balas;            //Tempo mínimo (em segundos) entre dois disparos consecutivos
    float tempo_desde_ultimo_tiro;    //Contador regressivo até o próximo disparo estar liberado
    Deslocamento deslocamento;        //Direção de deslocamento lateral atual (PARADO/ESQUERDA/DIREITA)
    float tempo_deslocamento;         //Contador regressivo que determina por quanto tempo a nave mantém a direção atual antes de reavaliar
    Arma gun[MAX_BULLETS];            //Pool de balas pertencente a esta nave
    Ship_State estado;                //Estado de vida da nave (MORTO/EXPLODINDO/VIVO)
    float life;                       //Pontos de vida restantes; quando <= 0 a nave passa a EXPLODINDO

    int valor_score;                  //Pontuação concedida ao jogador por atingir/abater esta nave (usado só em inimigos/boss)
    int tipo;                         //Índice do "tipo" de inimigo comum (1, 2 ou 3), usado para selecionar textura/atributos

    float tempo_frame;                //Contador regressivo que controla a troca de frame da animação de explosão
    int frame_explosao;               //Índice do frame atual da animação de explosão (0 a MAX_EXPLOSION_SPRITES-1)

    Texture2D *textura;                //Ponteiro para a textura (sprite) usada para desenhar a nave
} Nave;

//Struct para popup de score para naves inimigas abatidas
//Representa um texto flutuante (ex: "+10", "-10") que aparece brevemente no local de uma colisão de bala
typedef struct{
    Vector2 posicao;         //Posição onde o popup é exibido (local da colisão)
    int valor;                //Valor de pontuação exibido (negativo quando o player é atingido)
    float tempo_de_vida;      //Tempo restante (em segundos) até o popup desaparecer; também usado como base do alpha
    float alpha;               //Transparência atual do popup (some gradualmente conforme tempo_de_vida diminui)
    bool ativo;                //true enquanto o popup está visível; false quando disponível para reuso no pool
}Score_Popups;

//VARIÁVEIS GLOBAIS:
//Todas as variáveis abaixo são apenas DECLARADAS aqui (extern); a definição/alocação
//real de cada uma acontece em main.c, permitindo que qualquer arquivo que inclua este
//header enxergue e manipule o mesmo estado global do jogo.

//---- Entidades do jogo ----
extern Nave player;                                //Nave controlada pelo jogador
extern Nave enemies[MAX_ENEMIES];                  //Vetor com todos os inimigos comuns da rodada atual
extern Nave boss;                                  //Nave do chefe final (rodada 6)
extern Score_Popups score_popups[MAX_SCORE_POPUPS]; //Pool de popups de pontuação exibidos na tela

//---- Estado geral da partida ----
extern Vector2 origem_tela;    //Coordenada (x,y) que marca o início da área jogável, logo abaixo do HUD

extern int score;              //Pontuação atual do jogador
extern bool score_animando;    //Flag auxiliar (reservada para eventual animação do score no HUD)
extern int rodada;             //Número da rodada atual (1 a 5 = ondas de inimigos comuns; 6 = boss)

extern Game_State game;        //Estado atual da máquina de estados do jogo (MENU/GAME/MORTE/VITORIA/CINEMA/SAIDA)
extern bool fosas;             //Flag que alterna entre os dois "temas"/skins do jogo (false = tema padrão, true = tema "fosas")

 //Defino o tamanho da janela para 1600 x 900 pixels
extern const int comprimento_tela;  //Largura da janela/tela de jogo, em pixels
extern const int altura_tela;       //Altura da janela/tela de jogo, em pixels
extern Font title_font;             //Fonte usada nos títulos (menu, telas de morte/vitória)
extern Font text_font;              //Fonte usada em textos gerais (controles, diálogos da cutscene, popups)
extern Font hud_font;               //Fonte usada nos textos do HUD (score, rodada, vida)

//---- Texturas (sprites) ----
extern Texture2D texture_player;       //Sprite do player no tema padrão
extern Texture2D laser_azul;           //Sprite da bala do player
extern Texture2D laser_vermelho;       //Sprite da bala de inimigos/boss no tema padrão
extern Texture2D texture_bitcoin;      //Sprite da bala do boss no tema "fosas"
extern Texture2D texture_fosorio;      //Sprite do player no tema "fosas"
extern Texture2D texture_boss;         //Sprite do boss no tema padrão
extern Texture2D musk_inimigo;         //Sprite do boss no tema "fosas"
extern Texture2D java_script_mental;   //Imagem do easter egg exibido no menu
extern Texture2D background;           //Imagem de fundo usada no menu e durante a partida

//Ponteiros para arrays de texturas (permitem indexar por tipo/índice nas funções de inicialização e desenho)
extern Texture2D **texturas_inimigos;  //Vetor de ponteiros para as texturas dos diferentes tipos de inimigo (ambos os temas)
extern Texture2D **texturas_explosao;  //Vetor de ponteiros para os frames da animação de explosão
extern Texture2D **texturas_cutscene;  //Vetor de ponteiros para as imagens usadas na cutscene final

//---- Áudio ----
extern Music laserhawk;        //Trilha sonora da partida no tema padrão
extern Music tie_fighter;      //Trilha sonora da partida no tema "fosas"
extern Music vitoria;          //Trilha sonora da tela de vitória (tema padrão)
extern Music absolute_cinema;  //Trilha sonora da cutscene final (tema "fosas")
extern Sound laser_sound;      //Efeito sonoro de disparo
extern Sound laser_boom;       //Efeito sonoro de impacto/explosão de bala

extern FILE *Arquivo;           //Ponteiro de arquivo