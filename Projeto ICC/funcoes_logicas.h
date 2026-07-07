/**
 * ARQUIVO: funcoes_logicas.h
 * DESCRIÇÃO GERAL:
 *   Concentra TODA a lógica de jogo (implementada inline, como funções "static" de header,
 *   já que não há um .c separado para elas): inicialização de entidades, movimentação,
 *   disparo e colisão de balas, desenho de sprites/HUD/menu, e as telas de fim de jogo
 *   (morte, vitória e cutscene).
 *
 *   As funções estão organizadas nos seguintes grupos:
 *     - Inicialização:      configuram os atributos iniciais de player/inimigos/boss
 *     - Round:               controlam a transição entre rodadas e a animação de entrada
 *     - Naves:                movimentação e desenho de player/inimigos/boss e suas explosões
 *     - Balas:                criação, movimentação, colisão e desenho de projéteis
 *     - HUD:                  desenho da barra de status, popups de score e menu principal
 *     - Fim de jogo:          telas de derrota, vitória e a cutscene narrativa final
 *     - Auxiliares:           montagem de retângulos de colisão reutilizados pelas demais funções
 */
#pragma once
#include "raylib.h"
#include "globais_e_constantes.h"

//===========================   DECLARAÇÃO DE FUNÇÕES ====================//
//Funções de inicialização:
void inicia_rodada();                                                          //Prepara a área jogável e chama a inicialização de player/inimigos/boss
void inicia_player(Texture2D *textura_nave, Texture2D *textura_arma);          //Define os atributos iniciais (posição, vida, arma, etc) do player
void inicia_inimigos(Texture2D **texturas_nave, Texture2D *textura_arma);      //Define os atributos iniciais de todos os inimigos comuns (por tipo)
void inicia_boss(Texture2D *textura_nave, Texture2D *textura_arma);            //Define os atributos iniciais do boss
//Funções do round
void update_enemies(Round_State *state_rodada);                               //Posiciona/reativa os inimigos (ou o boss) no início de uma nova rodada
void entrada_dramatica(Round_State *state_rodada, float *tempo_entrada);       //Anima a "queda"/entrada dos inimigos e a "subida" do player no início da rodada
//Funções das naves:
bool podeMover(float x_novo, float y_novo, Nave *ship);                        //Verifica se um deslocamento proposto mantém a nave dentro dos limites da área jogável
void move_inimigos(float tempo_passado);                                      //Sorteia e atualiza o deslocamento lateral aleatório dos inimigos comuns
void move_boss(float tempo_passado);                                          //Atualiza o deslocamento lateral aleatório do boss
void imprime_player();                                                        //Desenha o sprite do player (e sua explosão, se aplicável)
void explode_nave(Nave *ship);                                                //Avança a animação de explosão de uma nave até finalizá-la (estado MORTO)
void imprime_inimigos();                                                      //Desenha todos os inimigos vivos/explodindo
void imprime_boss();                                                          //Desenha o boss (e sua explosão, se aplicável)
void imprime_explosao(Nave *ship);                                            //Desenha o frame atual da animação de explosão sobre uma nave
//Funções da bala:
void adiciona_bala(float xo, float yo, Arma *bullets);                        //Ativa a primeira bala livre do pool na posição informada e toca o som de disparo
void move_bala(Arma *bullets);                                                //Move todas as balas ativas de um pool e as desativa ao sair da tela ou colidir
void imprime_bala(Arma *bullets);                                             //Desenha todas as balas ativas de um pool
void balas_inimigos();                                                        //Controla o disparo aleatório dos inimigos comuns e move suas balas
int colisao_bala_nave(float ynovo, float xnovo, Arma *bullet);                //Verifica/processa colisão de uma bala com player, inimigos ou boss (dano, score, popup)
void balas_boss();                                                            //Controla o disparo aleatório do boss e move suas balas
//Funções Textuais e de Menu:
void imprime_HUD();                                                           //Desenha a barra superior com score, rodada e barra de vida do player
void add_score_popup(int valor, Arma *bullet);                                //Ativa um popup de pontuação flutuante na posição de uma bala
void imprime_score_popup();                                                   //Desenha e atualiza a transparência dos popups de pontuação ativos
void imprime_pausa();                                                         //Imprime o menu de pausa
void imprime_menu(int opcao, bool opcao_2, bool easter_egg);                  //Desenha a tela de menu principal (título, botões e tela de controles/easter egg)
void imprime_scores();                                                        //Imprime os high scores no menu principal
void add_score();                                                             //Função que adiciona o score da última partida em scores.txt
void troca_musica(Music **music);                                            //Para a música atual e inicia a música correta para o tema/estado do jogo
//Funções de fim de jogo:
void kill_everyone();                                                         //Força o estado MORTO em player, boss e todos os inimigos (fim de partida)
void player_death(float *tempo_animacao);                                    //Controla a contagem e o desenho da tela de "GAME OVER"
void game_won(float *tempo_animacao);                                        //Controla a contagem e o desenho da tela de vitória (tema padrão)
void cutscene(float *tempo_cutscene, Music **game_music);                     //Controla a exibição dos diálogos da cutscene final (tema "fosas")
void cinema(int *linha, float *tempo_cutscene, bool *flag_musica, Music **game_music); //Controla a cena final de fade-in após o fim dos diálogos da cutscene
//Funções auxiliares:
void create_rectangle_ship(Rectangle *rec, Nave *ship, float x_novo, float y_novo);   //Monta o Rectangle de colisão/desenho de uma nave, com deslocamento opcional
void create_rectangle_bullet(Rectangle *rec, Arma *bullet, float x_novo, float y_novo); //Monta o Rectangle de colisão/desenho de uma bala, com deslocamento opcional

//===========================   DEFINIÇÃO DE FUNÇÕES ====================//
//Funções de inicialização:
void inicia_rodada(){
    /**DESCRIÇÃO: Função que inica o jogo e seta variáveis de ambiente
     * Param: nenhum
     * Return: nenhum
     */
    
    //Defino a origem da minha tela, ou seja a parte jogável
    origem_tela.x = 0;
    origem_tela.y = HUD_HEIGHT; //De 60 para cima é o HUD

    fosas ? inicia_player(&texture_fosorio, &laser_azul) : inicia_player(&texture_player, &laser_azul);
    fosas ? inicia_inimigos(texturas_inimigos, &laser_vermelho) : inicia_inimigos(texturas_inimigos, &laser_vermelho);
    fosas ? inicia_boss(&musk_inimigo, &texture_bitcoin) : inicia_boss(&texture_boss, &laser_vermelho);

}
void inicia_player(Texture2D *textura_nave, Texture2D *textura_arma){
    /**Função responsável por inicializar os parâmetros do player
     * Param: textura_nave - ponteiro para a textura da nave a ser usada (varia conforme o tema "fosas")
     *        textura_arma - ponteiro para a textura da bala do player
     * Return: nenhum
     */
    //As velocidades estão em pixels por frame
    const int tamanho_player = 80;
    const float velocidade_player = 600.0f;
    const int tamanho_bala = 15;
    const float velocidade_bala = 3000.0f;
    int dano_bala = 10;
    const float life = 100.0f;

    //O player dará spawn no centro inferior da tela:
    player.posicao = (Vector2) {(GetScreenWidth() - tamanho_player)/2, GetScreenHeight()};
    player.tamanho = tamanho_player;
    player.velocidade = velocidade_player;
    player.intervalo_balas = PLAYER_TIME_BETWEEN_BULLETS;  //RECOLOCAR PLAYER_TIME
    player.tempo_desde_ultimo_tiro = PLAYER_TIME_BETWEEN_BULLETS;
    for(int i = 0; i < MAX_BULLETS; i++){
        player.gun[i] = (Arma){
                            (Vector2) {0 , 0},
                            tamanho_bala,
                            velocidade_bala,
                            dano_bala,
                            false,
                            JOGADOR,
                            textura_arma,
                        };
    }
    player.estado = VIVO;
    player.life = life;
    player.textura = textura_nave;

    player.tempo_frame = 0.1f;
    player.frame_explosao = 0;
}
void inicia_inimigos(Texture2D **texturas_nave, Texture2D *textura_arma){
    /**Função responsável por inicializar os parâmetros de todos os inimigos comuns (MAX_ENEMIES naves)
     * Param: texturas_nave - vetor de ponteiros para as texturas dos 3 tipos de inimigo (e suas variantes do tema "fosas")
     *        textura_arma  - ponteiro para a textura da bala usada por todos os inimigos comuns
     * Return: nenhum
     * Observação: os vetores locais (velocidade/tamanho/dano/etc) são indexados por "tipo" de inimigo
     *             (0 = padrão, 1 = rápido, 2 = lerdo), definido pelo índice i dentro do laço abaixo.
     */

    const int tamanho_inimigo = 100;
    const int tempo_deslocamento = 1.0f;
    float velocidade_inimigo[MAX_ENEMIES_TYPE] = {400.0f, 500.0f, 300.0f};
    int tamanho_bala[MAX_ENEMIES_TYPE] = {13,15,17};
    float velocidade_bala[MAX_ENEMIES_TYPE] = {600.0f ,700.0f, 500.0f};
    float dano_bala[MAX_ENEMIES_TYPE] = {10.0, 5.0, 15.0};
    float tempo_entre_balas[MAX_ENEMIES_TYPE] = {0.6f ,0.4f, 0.8f};
    float life[MAX_ENEMIES_TYPE] = {100.0f, 50.0, 150.0};
    int valor_score[MAX_ENEMIES_TYPE] = {10, 5, 15};
    
    int xo = (GetScreenWidth()/MAX_ENEMIES - tamanho_inimigo)/2;
    int yo = HUD_HEIGHT;
    int index;

    for(int i = 0; i < MAX_ENEMIES; i++){

        //Inimigos: 0 - Inimigo Padrão
        //          1,2,3 - Inimigo Rapido
        //          4,5,6,7,8 - Inimigo Lerdo
        if(i == 0) index = 0;
        if(i == 1 || i == 2 || i == 3) index = 1;
        if(i == 4 || i == 5 || i == 6 || i == 7 || i == 8 || i == 9) index = 2;

        enemies[i].posicao = (Vector2){xo, yo};
        enemies[i].tamanho = tamanho_inimigo;
        enemies[i].velocidade = velocidade_inimigo[index];
        enemies[i].intervalo_balas = tempo_entre_balas[index];
        enemies[i].tempo_desde_ultimo_tiro = enemies[i].intervalo_balas;
        enemies[i].deslocamento = PARADO;
        enemies[i].tempo_deslocamento = tempo_deslocamento;

        for(int k = 0; k < MAX_BULLETS; k++){
            enemies[i].gun[k] = (Arma){
                (Vector2){0, 0},
                tamanho_bala[index],
                velocidade_bala[index],    //A velocidade da bala do inimigo é inversamente proporcional ao tempo entre disparos
                dano_bala[index],
                false,
                INIMIGO,
                textura_arma,
            };
        }

        enemies[i].estado = MORTO;
        enemies[i].life = life[index];
        enemies[i].valor_score = valor_score[index];
        enemies[i].tipo = index + 1;
        enemies[i].textura = texturas_nave[index + 3*(unsigned int)fosas];

        enemies[i].tempo_frame = 0.1f;
        enemies[i].frame_explosao = 0;

        xo += GetScreenWidth()/MAX_ENEMIES;
    }
}

void inicia_boss(Texture2D *textura_nave, Texture2D *textura_arma){
    /**Função responsável por inicializar os parâmetros do boss
     * Param: textura_nave - ponteiro para a textura da nave do boss (varia conforme o tema "fosas")
     *        textura_arma - ponteiro para a textura da bala do boss
     * Return: nenhum
     */
    const int tamanho_boss = 160;
    const int velocidade_boss = 300.0f;
    const int tempo_deslocamento = 2.0f;
    const int tamanho_bala = 30;
    const float velocidade_bala = 1500.0f;
    int dano_bala = 20;
    const float tempo_entre_balas = 0.6f;
    const float life = 200.0f;
    int valor_score = 20;

    //O boss spawna no centro superior da tela(Descontando o HUD):
    boss.posicao = (Vector2){( GetScreenWidth() - tamanho_boss)/2, HUD_HEIGHT};
    boss.tamanho = tamanho_boss;
    boss.velocidade = velocidade_boss;
    boss.intervalo_balas = tempo_entre_balas;
    boss.tempo_desde_ultimo_tiro = boss.intervalo_balas;
    boss.deslocamento = PARADO;
    boss.tempo_deslocamento = tempo_deslocamento;

    for(int k = 0; k < MAX_BULLETS; k++){
        boss.gun[k] = (Arma){
            (Vector2) {0,0},
            tamanho_bala,
            velocidade_bala,
            dano_bala,
            false,
            BOSS,
            textura_arma
        };
    }

    boss.estado = MORTO;
    boss.life = life;
    boss.valor_score = valor_score;

    boss.tempo_frame = 1.0f;
    boss.frame_explosao = 0;

    boss.textura = textura_nave;
}
//Funções do round:
void update_enemies(Round_State *state_rodada){
    /**Função chamada no início de cada rodada (quando *state_rodada == NOVA_RODADA) para
     * reposicionar/reativar os inimigos comuns (rodadas 1 a 5) ou acordar o boss (rodada 6).
     * Param: state_rodada - ponteiro para o estado da rodada; é avançado para ANIMANDO ao final
     * Return: nenhum
     */
    float life[MAX_ENEMIES_TYPE] = {100.0f, 50.0, 150.0};
    const int tamanho_inimigo = 100;
    int xo; 
    int n_inimigos;

    if(*state_rodada == NOVA_RODADA){
        player.life = 100.0f;
        n_inimigos = 2 * rodada - 1;    //Os inimigos aumentam de 2 em 2 em impares(1,3,5,7,9,...)
        if(n_inimigos > MAX_ENEMIES) n_inimigos = MAX_ENEMIES;

        xo = (GetScreenWidth()/n_inimigos - tamanho_inimigo)/2;

        if(rodada < 6){
            //===============   RODADAS DE 1 À 5:
            //Os inimigos 6,7,8 spawnam abaixo da primiera fileira
            //O inimo 9 spawna na ultima
            for(int i = 0; i < n_inimigos; i++){
                if(i >= 5 && i <= 7) {
                    enemies[i].posicao = (Vector2){xo, (tamanho_inimigo + HUD_HEIGHT)};
                }
                else if(i >= 8) {
                    enemies[i].posicao = (Vector2){xo, (2 * tamanho_inimigo + HUD_HEIGHT)};
                }
                else {
                    enemies[i].posicao = (Vector2){xo, 0}; // Começa bem no HUD
                }

                enemies[i].life = life[enemies[i].tipo - 1];
                enemies[i].estado = VIVO;

                enemies[i].tempo_frame = 0.25f;
                enemies[i].frame_explosao = 0;

                xo += GetScreenWidth()/n_inimigos;
            }

            //Garanto que os inimigos excedentes do array fiquem MORTOS
            for(int i = n_inimigos; i < MAX_ENEMIES; i++){
                enemies[i].estado = MORTO; // evita lixo de memória da rodada anterior
            }
            *state_rodada = ANIMANDO;
        }
        else if(rodada >= 6){
            // 1. Garante que todos os inimigos comuns fiquem MORTOS para não atrapalhar
            for(int i = 0; i < MAX_ENEMIES; i++){
                enemies[i].estado = MORTO; 
            }

            // 2. Acorda o Boss
            boss.estado = VIVO;

            // 3. Libera a animação de entrada dramática!
            *state_rodada = ANIMANDO;
        }
    }
    
}
void entrada_dramatica(Round_State *state_rodada, float *tempo_entrada){
    /**Função que anima a entrada dos inimigos/boss no início da rodada (eles "descem" HUD_HEIGHT
     * pixels ao longo de FALL_TIME segundos) e, apenas na rodada 1, também anima o player "subindo"
     * até sua posição de combate. Ao terminar o tempo de animação, avança o estado da rodada.
     * Param: state_rodada - ponteiro para o estado da rodada; só executa se estiver em ANIMANDO
     *        tempo_entrada - ponteiro para o contador regressivo da animação
     * Return: nenhum
     */
    if (*state_rodada != ANIMANDO) return;

    float dt = GetFrameTime();

    *tempo_entrada -= dt;
    if (*tempo_entrada <= 0.0f){
        *tempo_entrada = FALL_TIME;
        *state_rodada = RODADA_ATUAL; // Finaliza a animação
        return; 
    }

    // Distância que o inimigo percorre (a altura do HUD)
    
    // Distância que o jogador vai "subir" para entrar na tela, no nosso caso 150 pixels para cima
    float distancia_subida_player = 150.0f; 
    float velocidade_subida_player = distancia_subida_player / FALL_TIME; 

    float velocidade_queda_enemy = (float)HUD_HEIGHT / FALL_TIME;

    for(int i = 0; i < MAX_ENEMIES; i++){
        if(enemies[i].estado == VIVO){
            enemies[i].posicao.y += velocidade_queda_enemy * dt;
        }
    }

    if(boss.estado == VIVO){
        boss.posicao.y += velocidade_queda_enemy * dt;
    }

    // Move o player para cima apenas na rodada 1 (Y decresce para subir na tela)
    if(rodada == 1){
        player.posicao.y -= velocidade_subida_player * dt;
    }
}
//Funções das naves:
bool podeMover(float x_novo, float y_novo, Nave *ship){
    /**Verifica se um deslocamento (x_novo, y_novo) aplicado à nave "ship" a mantém dentro
     * dos limites da área jogável (respeitando a faixa do HUD no topo).
     * Param: x_novo, y_novo - deslocamento proposto (delta), não a posição final
     *        ship - ponteiro para a nave que se deseja mover
     * Return: true se o movimento é válido (nave permanece dentro da tela); false caso contrário
     *         (também retorna false se a nave não estiver VIVA)
     */
    bool flag = true;

    if(ship->estado != VIVO) return false;
    //Relembrando que o centro das coordenadas da nave está no canto superior esquerdo
    //Movimentos para a esquerda e para cima não precisam levar em consideração o tamanho da nave:
     //Movimentos para a direita e para baixo precisam levar em conta o tamanho da nave:
    if(
        (x_novo + ship->posicao.x) < 0 ||
        (y_novo + ship->posicao.y) < HUD_HEIGHT ||
        (x_novo + ship->posicao.x + ship->tamanho) > comprimento_tela ||
        (y_novo + ship->posicao.y + ship->tamanho) > altura_tela
    ) flag = false;
    return flag;
}

void move_inimigos(float tempo_passado){
    /**Função responsável pelo movimento lateral automático/aleatório dos inimigos comuns.
     * A cada chamada, sorteia um único inimigo candidato a começar a se mover; em seguida,
     * para todos os inimigos já em movimento, atualiza a posição (ou inverte a direção,
     * caso o movimento levaria a nave para fora dos limites da tela).
     * Param: tempo_passado - tempo decorrido desde o último frame (GetFrameTime()), usado para
     *        calcular o deslocamento proporcional à velocidade da nave
     * Return: nenhum
     */
    float xnovo;
    int i;

    //SORTEIO ALEATORIAMENTE UM INIMIGO PARA MOVER-SE, CASO ESTEJA PARADO E VIVO, SORTEIO A DIREÇÃO DO MOVIMENTO
    i = GetRandomValue(0, MAX_ENEMIES - 1);
    //Há 50% de chance de um ÚNICO inimigo mover POR FRAME e, sobre isso, 50 % de chance para a esquerda ou para a direita:
    //Deve estar parado para alterar seu estado de deslocamento
    if(enemies[i].deslocamento == PARADO && enemies[i].estado == VIVO && GetRandomValue(0,1)){
        enemies[i].deslocamento = GetRandomValue(1,2);
    }
    //CONTO O TEMPO DE MOVIMENTO DE CADA UM E ALTERO SUA POSIÇÃO, SE ESTIVER VIVO:
    for(i = 0; i < MAX_ENEMIES; i++){
        if(enemies[i].estado != VIVO) continue;

        if(enemies[i].tempo_deslocamento <= 0.0f){
            enemies[i].deslocamento = PARADO;
            enemies[i].tempo_deslocamento = 1.5f;
        }
        if(enemies[i].deslocamento != PARADO) {
            enemies[i].tempo_deslocamento -= GetFrameTime();
            xnovo = enemies[i].velocidade * tempo_passado;  //x > 0: deslocamento para a direita
            if(enemies[i].deslocamento == ESQUERDA) xnovo *= -1;
            if(podeMover(xnovo, enemies[i].posicao.y, &enemies[i])){
                enemies[i].posicao.x += xnovo;
            }
            //Caso fiquem travados forço a mover pelo lado oposto
            else{
                if(enemies[i].deslocamento == ESQUERDA) enemies[i].deslocamento = DIREITA;
                else enemies[i].deslocamento = ESQUERDA;
                enemies[i].posicao.x -= xnovo;
            }
        }
    }
}

void move_boss(float tempo_passado){
    /**Função responsável pelo movimento lateral automático/aleatório do boss.
     * Segue a mesma lógica de move_inimigos(), mas aplicada a uma única nave (o boss).
     * Param: tempo_passado - tempo decorrido desde o último frame (GetFrameTime())
     * Return: nenhum
     */
    float xnovo;
    int i;

    if(boss.estado != VIVO) return;
    //Há 50% de chance do boss se mover e sobre isso, a direção é aleatoria:
    if(GetRandomValue(0,1) && boss.deslocamento == PARADO){
        boss.deslocamento = GetRandomValue(1,2);
    }

    if(boss.tempo_deslocamento <= 0.0f){
        boss.deslocamento = PARADO;
        boss.tempo_deslocamento = 2.0f;
    }

    if(boss.deslocamento != PARADO){
        boss.tempo_deslocamento -= GetFrameTime();
        xnovo = boss.velocidade * tempo_passado;

        if(boss.deslocamento == ESQUERDA) xnovo *= -1;

        if(podeMover(xnovo, boss.posicao.y, &boss)){
            boss.posicao.x += xnovo;
        }
        else{
            if(boss.deslocamento == ESQUERDA) boss.deslocamento = DIREITA;
            else boss.deslocamento = ESQUERDA;

            boss.posicao.x -= xnovo;
        }
    }

}
void imprime_player(){
    /**Desenha o sprite do player na sua posição atual e, se ele estiver EXPLODINDO,
     * desenha também a animação de explosão sobre ele.
     * Param/Return: nenhum (opera diretamente sobre a variável global "player")
     */
    Texture2D *textura = player.textura;
    DrawTexturePro(
            *textura,
            // Origem: Pega as dimensões reais de pixels do arquivo 
            (Rectangle){
                0,0, (float) textura->width, textura->height
            },
            // Destino: Encaixa na posição atual do jogador e usa o tamanho físico dele 
            (Rectangle){
                player.posicao.x, player.posicao.y , (float) player.tamanho, (float) player.tamanho
            },
            (Vector2) {0,0 },
            0.0f, 
            // Transparência: Deixa o player mais transaprente a cada dano que toma
            WHITE
    );
    
    if(player.estado == EXPLODINDO) imprime_explosao(&player);
}

void explode_nave(Nave *ship){
    /**Avança a animação de explosão de uma nave: cada frame_explosao fica visível por
     * "tempo_frame" segundos; ao esgotar todos os MAX_EXPLOSION_SPRITES frames, a nave
     * é definitivamente marcada como MORTO.
     * Param: ship - ponteiro para a nave (player, inimigo ou boss) que está explodindo
     * Return: nenhum
     */
    //O tempo_frame determina quanto tempo o frame da explosão fica aparecendo:
    ship->tempo_frame -= GetFrameTime();
    if(ship->tempo_frame <= 0.0f){
        ship->frame_explosao++;
    }
    if(ship->frame_explosao >= MAX_EXPLOSION_SPRITES){
        ship->estado = MORTO;
        return;
    }
}
void imprime_inimigos(){
    /**Percorre todos os inimigos comuns e desenha os que não estão MORTOs (VIVO ou EXPLODINDO),
     * incluindo a animação de explosão quando aplicável.
     * Param/Return: nenhum (opera diretamente sobre o vetor global "enemies")
     */
    //Ponteiro auxiliar para ajudar a manipular um inimigo por vez
    Nave *inimigo;

    for(int i = 0; i < MAX_ENEMIES; i++){
        inimigo = &enemies[i];

        Texture2D *textura = inimigo->textura;
        
        if(inimigo->estado != MORTO){
            DrawTexturePro(
                *textura,
                // Origem: Pega as dimensões reais de pixels do arquivo 
                (Rectangle){
                    0,0,(float)textura->width,(float)textura->height
                },
                // Destino: Encaixa na posição atual do inimigo e usa o tamanho físico dele 
                (Rectangle){
                    inimigo->posicao.x, inimigo->posicao.y, (float) inimigo->tamanho, (float)inimigo->tamanho
                },
                (Vector2) {0, 0},
                0.0f,
                // Transparência: Deixa o inimigo mais transaprente a cada dano que toma
                //Fade(WHITE, inimigo->life/100)
                WHITE
            );
        }

        if(inimigo->estado == EXPLODINDO) imprime_explosao(inimigo);
    }
}
void imprime_boss(){
    /**Desenha o sprite do boss enquanto ele não estiver MORTO, incluindo a animação de
     * explosão quando aplicável.
     * Param/Return: nenhum (opera diretamente sobre a variável global "boss")
     */
    Texture2D *textura = boss.textura;

    if(boss.estado != MORTO){
        DrawTexturePro(
            *textura,
            // Origem: Pega as dimensões reais de pixels do arquivo 
            (Rectangle){
                0, 0, (float) textura->width, (float) textura->height
            },
            // Destino: Encaixa na posição atual do boss e usa o tamanho físico dele 
            (Rectangle){
                boss.posicao.x, boss.posicao.y, (float) boss.tamanho, (float) boss.tamanho
            },
            (Vector2){0, 0},
            0.0f,
            WHITE
        );

        if(boss.estado == EXPLODINDO) imprime_explosao(&boss);
    }
}

void imprime_explosao(Nave *ship){
    /**Desenha, sobre a posição de uma nave, o frame atual (ship->frame_explosao) da
     * animação de explosão, ampliado para o dobro do tamanho da nave (centralizado via offset).
     * Param: ship - ponteiro para a nave que está explodindo
     * Return: nenhum
     */
    int i;
    //VOu deixar a explosão 2 vezes maior e para isso preciso de um offset já que a imagem vai cair para baixo e para a direita
    const float offset = (ship->tamanho*2.0f - ship->tamanho)/2.0f;

    Texture2D *explosao = texturas_explosao[ship->frame_explosao];

    DrawTexturePro(
        *explosao,
        //Origem: Pega as dimensões reais de pixels do arquivo
        (Rectangle){
            0, 0, (float)explosao->width, (float)explosao->height
        },
        //Destino: Coloco o sprite de explosão na nave:
        (Rectangle){
            ship->posicao.x - offset, ship->posicao.y - offset, (float)2.0f*ship->tamanho, (float)2.0f*ship->tamanho
        },
        (Vector2) { 0 , 0 },
        0.0f,
        WHITE
    );
}

//FUNÇÕES DAS BALAS:
void adiciona_bala(float xo, float yo, Arma *bullets){
    /**Função que adiciona uma bala à tela
     * param:   xo = posição de saída em x da bala
     *          yo = posição de saída em y da bala
     *          *bullet = ponteiro para verificar se há balas disponíveis e atualizar suas posições
     */
    Arma *bullet;
    //Procuro pela primeira bala inativa:
    for(int i = 0; i < MAX_BULLETS; i++){
        bullet = &bullets[i];
        if(!bullet->ativa){
            bullet->posicao.x = xo;
            bullet->posicao.y = yo;
            bullet->ativa = true;
            break;
        }
    }
    PlaySound(laser_sound);
}

void move_bala(Arma *bullets){
    /**Função que move todas as balas ATIVAS de um pool na tela e as apaga (ativa = false)
     * quando saem dos limites da área jogável ou colidem com uma nave.
     * Param: bullets - ponteiro para o início do vetor (pool) de MAX_BULLETS balas a atualizar
     * Return: nenhum
     */
    float tempo_passado = GetFrameTime();
    float y_novo;
    //Ponteiro auxiliar para verificar uma bala por vez
    Arma *bullet;
    //A bala deve estar ativa para mudar de posiçõa:
    for(int i = 0; i < MAX_BULLETS; i++){
        bullet = &bullets[i];
        y_novo = bullet->velocidade * tempo_passado;
        //CONDIÇÕES PARA DESATIVAR A BALA:
        //Se sair do limite do mapa, nesse caso, devemos esperar ela sair completamente, logo, 
        // para a esquerda e para a direita devemos levar em conta o tamanho da bala:
        if(bullet->ativa){
            if(
                (bullet->posicao.x + bullet->tamanho) < 0 ||
                bullet->posicao.x > comprimento_tela ||
                (bullet->posicao.y - bullet->tamanho) < HUD_HEIGHT ||
                bullet->posicao.y > altura_tela 
            ) bullet->ativa = false;
            //Colisão com uma nave:
            if(colisao_bala_nave(y_novo, 0.0f, bullet)){
                bullet->ativa = false;
            }
        }
        if(bullet->ativa){
            if(bullet->origem == JOGADOR)bullet->posicao.y -= y_novo;
            if(bullet->origem == INIMIGO || bullet->origem == BOSS)bullet->posicao.y += y_novo;
        }
    }
}
int colisao_bala_nave(float ynovo, float xnovo, Arma *bullet){
    /**Verifica e processa a colisão de UMA bala com as naves elegíveis, de acordo com sua origem:
     *   - Bala de INIMIGO/BOSS: verifica colisão apenas com o player
     *   - Bala do JOGADOR: verifica colisão com cada inimigo vivo e com o boss (se vivo)
     * Em caso de colisão: aplica dano, atualiza/zera score, cria um popup de pontuação, toca o
     * som de impacto e, se a vida do alvo chegar a zero, muda seu estado para EXPLODINDO e
     * desativa todas as balas dele.
     * Param: ynovo, xnovo - deslocamento (delta) que a bala teria neste frame, usado para montar
     *        o retângulo de colisão na posição futura (evita "atravessar" o alvo em frames rápidos)
     *        bullet - ponteiro para a bala a ser testada (função só deve ser chamada para balas ativas)
     * Return: 1 (true) se houve colisão (a bala deve ser desativada por quem chamou); 0 (false) caso contrário
     */
    //A Função só é chamada para balas ativas
    //Retângulos Auxiliares:
    Rectangle rec_origin;
    Rectangle rec_other;

    create_rectangle_bullet(&rec_origin, bullet, xnovo, ynovo);
    //COLISÃO COM O PLAYER(A bala não pode ser dele):
    if(bullet->origem != JOGADOR){
        create_rectangle_ship(&rec_other, &player, 0.0f, 0.0f);
        if(CheckCollisionRecs(rec_origin, rec_other)){
            player.life -= bullet->dano;
            if(player.life <= 0.0f){
                //player.estado = MORTO;
                player.estado = EXPLODINDO;
                for(int k = 0; k < MAX_BULLETS; k++) player.gun[k].ativa = false;
            }
            score -= 10;

            add_score_popup((-10), bullet);

            PlaySound(laser_boom);
            return true;
        }
    }
    //COLISÃO COM OS INIMIGOS E RETIRADA DE VIDA:
    if(bullet->origem == JOGADOR){
        for(int i = 0; i < MAX_ENEMIES; i++){

            if(enemies[i].estado == VIVO){
                create_rectangle_ship(&rec_other, &enemies[i], 0.0f, 0.0f);

                if(CheckCollisionRecs(rec_origin,rec_other)) {
                    enemies[i].life -= bullet->dano;

                    if(enemies[i].life <= 0.0f){

                        //enemies[i].estado = MORTO;
                        enemies[i].estado = EXPLODINDO;
                        score += 10 * enemies[i].valor_score;

                        add_score_popup(10 * enemies[i].valor_score, bullet);

                        for(int k = 0; k < MAX_BULLETS; k++) enemies[i].gun[k].ativa = false;
                    }
                    score += enemies[i].valor_score;
                    add_score_popup(enemies[i].valor_score, bullet);

                    PlaySound(laser_boom);
                    return true;
                }
            }
        }
    }
    //COLISAO COM O BOSS E RETIRADA DE VIDA:
    if(bullet->origem == JOGADOR){
        if(boss.estado == VIVO){
            create_rectangle_ship(&rec_other, &boss, 0.0f, 0.0f);

            if(CheckCollisionRecs(rec_origin,rec_other)){
                boss.life -= bullet->dano;

                if(boss.life <= 0){

                    boss.estado = EXPLODINDO;
                    score += 10 * boss.valor_score;

                    add_score_popup(10 * boss.valor_score, bullet);

                    for(int k = 0; k < MAX_BULLETS; k++) boss.gun[k].ativa = false;
                }

                score += boss.valor_score;
                add_score_popup(boss.valor_score, bullet);
                PlaySound(laser_boom);
                return true;

            }
        }
    }
    if(score <  0) score = 0;
    return false;
}

void imprime_bala(Arma *bullets){
    /**Desenha todas as balas ATIVAS de um pool na sua posição atual.
     * Param: bullets - ponteiro para o início do vetor (pool) de MAX_BULLETS balas a desenhar
     * Return: nenhum
     */
    Arma *bullet;
    Texture2D *textura;

    for(int i = 0; i < MAX_BULLETS; i++){
        bullet = &bullets[i];

        if(bullet->ativa){
            textura = bullet->textura;
           
            DrawTexturePro(
                *textura,
                // Origem: Pega o tamanho do arquivo
                (Rectangle){
                    0,0,(float) textura->width,(float) textura->height
                },
                //Destino: Coloca na bala
                (Rectangle){
                    bullet->posicao.x, bullet->posicao.y,(float) bullet->tamanho,(float) bullet->tamanho
                },
                (Vector2) {0,0},
                0.0f,
                WHITE
            );
        }
    }
}

void balas_inimigos(){
    /**Controla o disparo automático de todos os inimigos comuns: quando o cooldown de cada
     * inimigo vivo expira, há 50% de chance de disparar uma bala a partir do centro-inferior
     * de sua nave. Ao final, move as balas de todos os inimigos (vivos ou não, para permitir
     * que balas já disparadas continuem em voo mesmo após a nave ser destruída).
     * Param/Return: nenhum (opera sobre o vetor global "enemies")
     */
    //Ponteiro auxiliar para manipulação de inimigos 
    Nave *inimigo;

    for(int i = 0; i < MAX_ENEMIES; i++){
        inimigo = &enemies[i];
        inimigo->tempo_desde_ultimo_tiro -= GetFrameTime();

        //50% de chance do inimigo atirar
        if(inimigo->tempo_desde_ultimo_tiro <= 0.0f){
            if(GetRandomValue(0,1) && inimigo->estado == VIVO)adiciona_bala(inimigo->posicao.x + inimigo->tamanho/2, inimigo->posicao.y + inimigo->tamanho - 1, inimigo->gun);
            inimigo->tempo_desde_ultimo_tiro = inimigo->intervalo_balas;
        }
    }
    
    for(int i = 0; i < MAX_ENEMIES; i++){
        inimigo = &enemies[i];
        move_bala(inimigo->gun);
    }
}

void balas_boss(){
    /**Controla o disparo automático do boss: segue a mesma lógica de balas_inimigos(),
     * porém aplicada a uma única nave (o boss).
     * Param/Return: nenhum (opera sobre a variável global "boss")
     */
    boss.tempo_desde_ultimo_tiro -= GetFrameTime();

    //50% de chance de atirar:
    if(boss.tempo_desde_ultimo_tiro <= 0.0f){
        if(GetRandomValue(0,1) && boss.estado == VIVO){
            adiciona_bala(boss.posicao.x + boss.tamanho/2, boss.posicao.y + boss.tamanho - 1, boss.gun);
        }
        boss.tempo_desde_ultimo_tiro = boss.intervalo_balas;
    }

    move_bala(boss.gun);
}
//Funções do HUD:
void imprime_HUD(){
    /**Desenha a barra de status fixa no topo da tela: fundo semitransparente, linha
     * divisória, pontuação atual, número da rodada e uma barra de vida do player
     * (largura proporcional a player.life).
     * Param/Return: nenhum
     */
    int posy = (HUD_HEIGHT - HUD_FONT_SIZE)/2;
    //1. Impressão do fundo do HUD:
    DrawRectangle(0,0, GetScreenWidth(), origem_tela.y, Fade(BLACK, 0.6f));
    //2. Linha divisória entre jogo e HUD:
    DrawLine(origem_tela.x, origem_tela.y, GetScreenWidth(), origem_tela.y, RAYWHITE);
    //3.Imprimo o score
    DrawTextEx(hud_font, TextFormat("SCORE: %d", score), (Vector2){10, posy}, HUD_FONT_SIZE, SPACING, YELLOW);
    //4. Imprimo a rodada:
    DrawTextEx(hud_font, TextFormat("ROUND %d", rodada), (Vector2){640, posy}, HUD_FONT_SIZE, SPACING, RED);
    //5. Imprimo a vida:
    DrawTextEx(hud_font, TextFormat("Life: "), (Vector2){1100, posy}, HUD_FONT_SIZE, SPACING, RAYWHITE);
    DrawRectangle(1350, posy, player.life * 2, HUD_FONT_SIZE, GREEN);
}

void add_score_popup(int valor, Arma *bullet){
    /**Ativa o primeiro popup de pontuação livre do pool, posicionado no centro da bala
     * que causou a colisão, exibindo o valor informado (pode ser negativo).
     * Param: valor - quantidade de pontos a exibir (positivo ou negativo)
     *        bullet - ponteiro para a bala que colidiu, usada para posicionar o popup
     * Return: nenhum
     */
    for(int i = 0; i < MAX_SCORE_POPUPS; i++){
        if(!score_popups[i].ativo){
            //O popup sai do centro da bala que colidiu
            score_popups[i].posicao.x = bullet->posicao.x + bullet->tamanho/2;
            score_popups[i].posicao.y = bullet->posicao.y + bullet->tamanho/2;
            //Defino o valor que ele irá imprimir:
            score_popups[i].valor = valor;
            //Defino seu tempo de vida e sua intensidade:
            score_popups[i].tempo_de_vida = score_popups[i].alpha = 1.0f;
            //Defino como ativo
            score_popups[i].ativo = true;
            break;
        }
    }
}

void imprime_score_popup(){
    /**Desenha todos os popups de pontuação ativos: vermelho para valores negativos
     * (dano ao player) e amarelo com prefixo "+" para valores positivos, aplicando
     * a transparência (alpha) atual de cada popup.
     * Param/Return: nenhum (a atualização de tempo/alpha e desativação ocorre no loop
     *               principal em main.c; esta função apenas desenha o estado atual)
     */
    Color c;
    char str[10];

    for(int i = 0; i < MAX_SCORE_POPUPS; i++){
        if(score_popups[i].ativo){
            if(score_popups[i].valor < 0) {
                c = RED;
                sprintf(str, "%d", score_popups[i].valor);
            }
            else {
                c = YELLOW;
                sprintf(str, "+%d", score_popups[i].valor);
            }
            
            DrawTextEx(
                text_font, 
                TextFormat("%s", str),
                score_popups[i].posicao, 
                HUD_FONT_SIZE - 10, 
                SPACING, 
                Fade(c, score_popups[i].alpha)
            );
        }
    }
}
void imprime_pausa(){
    /**Desenha o menu de pausa: apenas o texto contido nele sobre o jogo
     * Param: nenhum
     * Return: nenhum
     */
    const int tamanho_pausa = TITLE_FONT_SIZE;
    const int tamanho_texto = TEXT_FONT_SIZE;
    const int offset_y = 50;

    const char *pausa = "PAUSA";
    const char *opcao = "Pressione [ENTER] para continuar\nPressione [P] para voltar ao menu";

    Vector2 tamanho_medido_pausa = MeasureTextEx(title_font, pausa, tamanho_pausa, SPACING);
    Vector2 tamanho_medido_texto = MeasureTextEx(text_font, opcao, tamanho_texto, SPACING);

    int posx_pausa = (GetScreenWidth() - tamanho_medido_pausa.x)/2;
    int posy_pausa = (GetScreenHeight() - tamanho_medido_pausa.y)/2;

    int posx_opcao = (GetScreenWidth() - tamanho_medido_texto.x)/2;
    int posy_opcao = (GetScreenHeight() + 2*tamanho_medido_pausa.y - tamanho_medido_texto.y)/2;

    DrawTextEx(
        title_font,
        pausa,
        (Vector2){ posx_pausa, posy_pausa},
        tamanho_pausa,
        SPACING,
        YELLOW
    );

    DrawTextEx(
        text_font,
        opcao,
        (Vector2) { posx_opcao, posy_opcao + offset_y},
        tamanho_texto,
        SPACING,
        WHITE 
    );
}

void imprime_menu(int opcao, bool opcao_2, bool easter_egg){
    /**Desenha a tela de menu principal: fundo, título (varia conforme o tema "fosas"),
     * e, dependendo de opcao_2, ou os botões do menu (Iniciar/Controles/Sair, destacando
     * a opção atualmente selecionada e opcionalmente o easter egg) ou a tela de controles.
     * Param: opcao - índice da opção atualmente destacada (1=Iniciar, 2=Controles, 3=Sair)
     *        opcao_2 - true exibe a tela de controles em vez dos botões do menu
     *        easter_egg - true habilita o desenho do easter egg clicável no menu
     * Return: nenhum
     */
    //CONFIGURAÇÕES DO MENU:
    const float menu_x         = 550.0f; // Posição X global de todos os botões
    const float menu_start_y   = 270.0f; // Posição Y onde o menu começa
    const float texto_offset_x = 130.0f; // Empurra o texto para a direita (centralizar)
    const float texto_offset_y = 5.0f;  // Empurra o texto para baixo dentro do botão
    const int w = 500;  //Largura do botão
    const int h = TEXT_FONT_SIZE + 20;  //Altura do botão
    //BOTÕES DO MENU:
    Rectangle rec_botao_start = (Rectangle){ menu_x, menu_start_y + (1 * h), w, h };
    Rectangle rec_botao_controles = (Rectangle){ menu_x, menu_start_y + (3 * h), w, h };
    Rectangle rec_botao_saida = (Rectangle){ menu_x, menu_start_y + (5 * h), w, h };
    //EASTER EGG:
    Rectangle rec_java = (Rectangle){ menu_x + w + 20.0, menu_start_y, 500.0, 500.0};
     BeginDrawing();
            ClearBackground(BLACK);
            //1. IMPRESÃO DO BACKGROUND:
            DrawTexturePro(
                background,
                (Rectangle){
                    0,0, (float) background.width, (float) background.height
                },
                (Rectangle){
                    0,0, (float) GetScreenWidth(), (float) GetScreenHeight()
                },
                (Vector2) {0,0},
                0.0f,
                WHITE
            );
            //2. IMPRESSÃO DO TÍTULO:
            if(!fosas){
                DrawTextEx(
                    title_font,
                    TextFormat("NUCLEO DE ROBOTICA"),
                    (Vector2){150, 100},
                    TITLE_FONT_SIZE, 
                    SPACING,
                    YELLOW
                );
                DrawTextEx(
                    title_font,
                    TextFormat("AEROESPACIAL"),
                    (Vector2){350, 100 + TITLE_FONT_SIZE},
                    TITLE_FONT_SIZE, 
                    SPACING,
                    YELLOW
                );
            }
            if(fosas){
                DrawTextEx(
                    title_font,
                    TextFormat("STAR FOSORIO"),
                    (Vector2){400, 100},
                    TITLE_FONT_SIZE, 
                    SPACING,
                    YELLOW
                );
                DrawTextEx(
                    title_font,
                    TextFormat("O IMPERIO STARLINK CONTRA ATACA"),
                    (Vector2){25, 100 + TITLE_FONT_SIZE},
                    TITLE_FONT_SIZE-25, 
                    SPACING,
                    YELLOW
                );
            }
        

            //3. IMPRESSÃO DAS OPÇÕES DO MENU
            if(opcao_2 == false){
                // ----------------------------------------------------------------------------
                // BOTÃO: INICIAR (Multiplicador: 1 * h)
                // ----------------------------------------------------------------------------
                
                DrawRectangleRoundedLinesEx(
                    rec_botao_start,
                    0.5f,
                    4,
                    10.0,
                    LIME
                );
                DrawTextEx(
                    text_font,
                    "Iniciar",
                    (Vector2){ menu_x + texto_offset_x + 10.0f, menu_start_y + texto_offset_y + (1 * h) },
                    TEXT_FONT_SIZE,
                    SPACING,
                    (opcao == 1) ? LIME : WHITE
                );

                // ----------------------------------------------------------------------------
                // BOTÃO: CONTROLES (Multiplicador: 3 * h)
                // ----------------------------------------------------------------------------
                DrawRectangleRoundedLinesEx(
                    rec_botao_controles,
                    0.5f,
                    4,
                    10.0,
                    SKYBLUE
                );
                DrawTextEx(
                    text_font,
                    "Controles",
                    (Vector2){ menu_x + (texto_offset_x - 40.0f), menu_start_y + texto_offset_y + (3 * h) }, 
                    TEXT_FONT_SIZE,
                    SPACING,
                    (opcao == 2) ? SKYBLUE : WHITE
                );

                // ----------------------------------------------------------------------------
                // BOTÃO: SAIR (Multiplicador: 5 * h)
                // ----------------------------------------------------------------------------
                DrawRectangleRoundedLinesEx(
                    rec_botao_saida, 
                    0.5f,
                    4,
                    10.0,
                    RED
                );
                DrawTextEx(
                    text_font,
                    "Sair",
                    (Vector2){ menu_x + (texto_offset_x + 50.0f), menu_start_y + texto_offset_y + (5 * h) },
                    TEXT_FONT_SIZE,
                    SPACING,
                    (opcao == 3) ? RED : WHITE
                );
                //EASTER EGG:
                if(easter_egg){
                    DrawTexturePro(
                        java_script_mental,
                        (Rectangle){0,0, java_script_mental.width, java_script_mental.height},
                        rec_java,
                        (Vector2) {0, 0},
                        0.0f,
                        WHITE
                    );
                    DrawTextEx(
                        text_font,
                        "Clique na Imagem",
                        //(Vector2){550,800},
                        (Vector2){menu_x + rec_java.height - 30, menu_start_y + rec_java.height + 30},
                        TEXT_FONT_SIZE,
                        SPACING,
                        BLUE
                    );
                }
                //------------------------------------------------------------------------------------------
                // IMPRESSÃO DOS HIGH SCORES
                //------------------------------------------------------------------------------------------
                imprime_scores();
            }
            if(opcao_2 == true){
                DrawTextEx(
                    text_font,
                    "W - Mover para cima\nA - Mover para a esquerda\nS - Mover para baixo\nD - Mover para a esquerda\nEspaco/Botao_Esquerdo - Atirar\nP - Tecla de pausa do jogo",
                    (Vector2){menu_x - texto_offset_x - 50.0, menu_start_y},
                    TEXT_FONT_SIZE, 
                    SPACING,
                    WHITE
                );
                DrawTextEx(
                    text_font,
                    "Pressione Q para voltar ao menu ",
                    (Vector2){menu_x - 3*texto_offset_x, menu_start_y + 7*TEXT_FONT_SIZE},
                    TEXT_FONT_SIZE, 
                    SPACING,
                    GREEN
                );
            }
            EndDrawing();
}

void imprime_scores(){
    const int MAXTXT = 50;                     // Tamanho máximo do buffer de texto para leitura das linhas
    int high_scores[MAX_HIGH_SCORES];          // Vetor que armazenará as pontuações lidas (convertidas para inteiro)
    bool erro = false;                         // Flag para indicar se houve erro ao abrir o arquivo
    int i = 0;                                 // Contador de pontuações lidas e índice do vetor
    char txt[MAXTXT];                          // String auxiliar para receber o texto lido de cada linha

    // ====================================================================================
    // SEÇÃO LÓGICA: LEITURA E PROCESSAMENTO DO ARQUIVO
    // ====================================================================================
    
    // Tenta abrir o arquivo de scores no modo leitura de texto ("rt")
    Arquivo = fopen(NOME_ARQ, "rt");
    
    // Verifica se o arquivo não existe ou não pôde ser aberto
    if(Arquivo == NULL){
        erro = true;
    }
    
    // Se o arquivo foi aberto com sucesso, obtém as pontuações (presumivelmente já ordenadas)
    if(!erro){
        // Lê o arquivo linha por linha até o final (NULL)
        while(fgets(txt, MAXTXT, Arquivo) != NULL){
            // Garante que não ultrapassaremos o limite máximo de pontuações a exibir (ex: Top 5)
            if(i == MAX_HIGH_SCORES) break;
            if(txt[0] == '#') break;

            // Converte a string lida para inteiro (atoi) e armazena no vetor, incrementando o índice
            high_scores[i++] = atoi(txt);
        }
        // Fecha o arquivo para liberar o recurso e evitar vazamentos de memória (memory leaks)
        fclose(Arquivo);
    }

    // ====================================================================================
    // SEÇÃO DE DESENHO: RENDERIZAÇÃO NA TELA (Chamada durante as impressões do menu)
    // ====================================================================================
    
    // Define uma posição Y baseada no centro da tela (um pouco acima do meio) para alinhar a lista
    float pos_y_inicial = GetScreenHeight() / 2.0f - 50.0f; 
    
    // Define o alinhamento horizontal fixado à esquerda da tela (x = 0)
    float centro_x = 0;

    // --- Renderização do Título ---
    const char* titulo = "- HIGH SCORES -";
    // Mede as dimensões do texto com a fonte escolhida para eventuais ajustes
    Vector2 tam_titulo = MeasureTextEx(text_font, titulo, 40, 2);
    // Desenha o título fixado na coordenada X = 0 e um pouco acima da lista de scores (pos_y_inicial - 60)
    DrawTextEx(text_font, titulo, (Vector2){centro_x, pos_y_inicial - 60}, 40, 2, SKYBLUE);

    // --- Renderização das Pontuações ---
    // Verifica se houve erro de leitura ou se o arquivo estava vazio (0 pontuações lidas)
    if(erro || i == 0){
        const char* msg_erro = "Nenhuma pontuacao registrada";
        Vector2 tam_erro = MeasureTextEx(text_font, msg_erro, 25, 2);
        // Exibe a mensagem de erro em cinza
        DrawTextEx(text_font, msg_erro, (Vector2){centro_x, pos_y_inicial}, 25, 2, GRAY);
    } 
    else {
        // Laço para desenhar cada um dos scores lidos e armazenados no vetor
        for(int j = 0; j < i; j++){
            char linha_score[MAXTXT];
            
            // Formata o texto: ex -> "1.  004500" 
            // (%06d formata o inteiro para ter no mínimo 6 dígitos, preenchendo com zeros à esquerda - Estilo Arcade)
            sprintf(linha_score, "%d.  %06d", j + 1, high_scores[j]);
            
            // Mede o tamanho da linha formatada para gerenciar o deslocamento na tela
            Vector2 tam_linha = MeasureTextEx(text_font, linha_score, 30, 2);
            
            // Lógica de Destaque: O 1º lugar (índice 0) fica em Dourado (GOLD), os restantes em Branco
            Color cor_texto = (j == 0) ? GOLD : WHITE; 
            
            // Desenha a linha da pontuação, deslocando ligeiramente o X para a direita 
            // e descendo a posição Y em 40 pixels por linha (j * 40)
            DrawTextEx(text_font, linha_score, (Vector2){centro_x + tam_linha.x/2, pos_y_inicial + (j * 40)}, 30, 2, cor_texto);
        }
    }
}
void add_score(){
    const int MAXTXT = 50;                     // Tamanho máximo do buffer de texto para leitura das linhas
    char txt[MAXTXT];                          // String auxiliar para receber o texto lido de cada linha
    int numero_de_dados = 0;
    int *vetor_scores;
    int posicao_score, index;
    index = 0;

    // =========================================================
    // 1. LER O ARQUIVO ATUAL PARA A MEMÓRIA
    // =========================================================
    Arquivo = fopen(NOME_ARQ, "rt");

    if(Arquivo == NULL){
        return; //A função imprime score vai achar o erro
    }

    while(fgets(txt, MAXTXT, Arquivo) != NULL){
        if(txt[0] == '#') break;
        numero_de_dados++;
    }
    fclose(Arquivo);

    if(numero_de_dados == 0) return;
    //Aloco memória dinamicamente dependendo do meu número de dados:
    vetor_scores = (int *) calloc(numero_de_dados, sizeof(int));

    //Leio novamente e dessa vez, salvo os dados e verifico onde o score atual se encaixa:
    Arquivo = fopen(NOME_ARQ, "rt");

    if(Arquivo == NULL){
        return; //A função imprime score vai achar o erro
    }

    //O arquivo já vem ordenado do maior para o menor
    while(fgets(txt, MAXTXT, Arquivo) != NULL){
        if(txt[0] == '#')break;

        vetor_scores[index++] = atoi(txt);
    }
    //Inicializo posicao score no pior cenario possivel(é o pior score de todos):
    posicao_score = numero_de_dados;

    //Ciclo pelo vetor de scores e acho a posição do score atual:
    for(index = 0; index < numero_de_dados; index++){
        //Só vale se não for o pior score de todos
        if(score >= vetor_scores[index]) {
            posicao_score = index;
            break;
        }
    }
    fclose(Arquivo);
    // =========================================================
    // 2. REESCREVER O ARQUIVO ATUALIZADO
    // =========================================================
    Arquivo = fopen(NOME_ARQ, "wt");

    if(Arquivo == NULL){
        return; //A função imprime score vai achar o erro
    }

    for(index = 0; index < numero_de_dados; index++){
        //Caso não seja o pior score de todos
        if(index == posicao_score){
            fprintf(Arquivo, "%d\n", score);
        }
        fprintf(Arquivo, "%d\n", vetor_scores[index]);
    }

    if(posicao_score == numero_de_dados)fprintf(Arquivo, "%d\n", score);

    fprintf(Arquivo, "#\n");

    fclose(Arquivo);
    free(vetor_scores);

}

void troca_musica(Music **music){
    /**Para a música atualmente tocando e, se o jogo estiver em MENU ou GAME, seleciona
     * e inicia a trilha correta de acordo com o tema ativo (fosas ou padrão).
     * Param: music - ponteiro para o ponteiro de música atualmente ativo (é reatribuído
     *        internamente para apontar para a nova trilha, quando aplicável)
     * Return: nenhum
     */
    StopMusicStream(**music);
    if(game == MENU || game == GAME){
        if(fosas) *music = &tie_fighter;
        else *music = &laserhawk;
    }
    PlayMusicStream(**music);
}
//FUNÇÕES DE FIM DE JOGO:
void kill_everyone(){
    /**Força o estado MORTO em player, boss e em todos os inimigos comuns.
     * Chamada ao final de uma partida (derrota ou vitória) para "limpar" a cena.
     * Param/Return: nenhum
     */
    player.estado = MORTO;

    boss.estado = MORTO;

    for(int i = 0; i < MAX_ENEMIES; i++){
        enemies[i].estado = MORTO;
    }
}
void player_death(float *tempo_animacao) {
    /**Controla a tela de "GAME OVER": decrementa o tempo de animação e, ao zerar,
     * retorna o jogo para o estado MENU. Enquanto isso, desenha o texto centralizado
     * na tela em vermelho.
     * Param: tempo_animacao - ponteiro para o contador regressivo de exibição da tela
     * Return: nenhum
     */
    const int tamanho_texto = TITLE_FONT_SIZE;
    
    //1. Conto o tempo por meio do ponteiro
    *tempo_animacao -= GetFrameTime();

    //Verifico se o tempo acabou
    if(*tempo_animacao <= 0.0f) {
        game = MENU;
    }

    // 2. Meço o texto para alinhar ao centro
    const char* texto = "GAME OVER";
    Vector2 tamanho_medido = MeasureTextEx(title_font, texto, tamanho_texto, SPACING);

    //Alinho o texto ao centro
    float pos_x = (GetScreenWidth() - tamanho_medido.x) / 2.0f;
    float pos_y = (GetScreenHeight() - tamanho_medido.y) / 2.0f;

    BeginDrawing();
    ClearBackground(BLACK);
    
    DrawTextEx(
        title_font,
        texto,
        (Vector2) { pos_x, pos_y },
        tamanho_texto,
        SPACING,
        RED
    );
    
    EndDrawing();
}

void game_won(float *tempo_animacao){
    /**Controla a tela de vitória simples (usada no tema padrão, sem cutscene): decrementa
     * o tempo de animação e, ao zerar, retorna o jogo para o estado MENU. Enquanto isso,
     * desenha o texto centralizado na tela em amarelo.
     * Param: tempo_animacao - ponteiro para o contador regressivo de exibição da tela
     * Return: nenhum
     */
    const float tamanho_texto = TITLE_FONT_SIZE;

    //1. Conto o tempo para a animação
    *tempo_animacao -= GetFrameTime();
    //Verifico se terminou:
    if(*tempo_animacao <= 0.0f){
        game = MENU;
    }

    //2. Meço o texto para imprimir centralizado
    const char *texto = "VOCE VENCEU!!!";
    Vector2 tamanho_medido = MeasureTextEx(title_font, texto, tamanho_texto, SPACING);

    float posx = (GetScreenWidth() - tamanho_medido.x)/2.0f;
    float posy = (GetScreenHeight() - tamanho_medido.y)/2.0f;


    BeginDrawing();
    ClearBackground(BLACK);

    DrawTextEx(
        title_font,
        texto,
        (Vector2) {posx, posy},
        tamanho_texto,
        SPACING,
        YELLOW
    );

    EndDrawing();
}

void cutscene(float *tempo_cutscene, Music **game_music){
    /**Controla a cutscene narrativa exibida após a vitória no tema "fosas": avança linha a
     * linha de diálogo a cada ENTER/SPACE, alternando o retrato exibido (musk x fosas)
     * conforme a linha atual. Ao esgotar todas as linhas, troca a música e passa o controle
     * para a função cinema(), que exibe a cena final com fade-in.
     * Param: tempo_cutscene - ponteiro para o contador regressivo repassado à função cinema()
     *        game_music - ponteiro para a música atual, usado ao trocar para a trilha "absolute_cinema"
     * Return: nenhum
     */
    //A linha atual só é colocada uma única vez para zero na chamada da função
    static int linha_atual = 0;
    const int total_linhas = 9;

    static bool trocou_musica = false;

    const char *dialogo[] = {
        "Isso nao faz sentido...\nMeus engenheiros usam \nJavaScript!\nComo um codigo arcaico \nem C \nderrubou a minha frota?!",
        "A flexibilidade e a mae \ndo Buffer Overflow, Elon.\nO teu sistema sufocou \nna propria ganancia.",
        "Um ataque de memoria?\nMas como te defendes de\n um fluxo interminavel?!",
        "Com a disciplina que \na tua geracao de \nframeworks esqueceu.\nCom a muralha \nintransponivel: o fgets.",
        "Ele nao confia \nno universo.\nEle olha para o abismo \ne diz:\n'So aceito o que cabe \nna minha memoria'.",
        "Mas e se o fluxo for \nmaior?!\nE se nunca parar?!",
        "Nos dominamos o fluxo.\nEncapsulamos a leitura \ncom um indestrutivel\nwhile(fgets(buffer, \nsizeof(buffer), stdin)\n!= NULL).",
        "Eu nao dependo de \nmaquinas virtuais, Elon.\nEu aponto diretamente \npara a memoria.",
        "EU SOU O \nMASTER OF THE \nUNIVERSE."
    };

    //Avanço o texto
    if(IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)){
        linha_atual++;
    }

    if(linha_atual < total_linhas){
        //===================   DESENHO NO TERMINAL ====================
        //DIÁLOGOS MUSK: 0,2,5
        //DIÁLOGOS FOSAS: 1,3,4,6,7,8
        BeginDrawing();
        ClearBackground(BLACK);
        //IMPRESSÃO DOS PERSONAGENS NO CANTO ESQUERDO
        if(linha_atual == 0 || linha_atual == 2 || linha_atual == 5){
            //Imprimo o musk:
            DrawTexturePro(
                *texturas_cutscene[1],
                //Origem:
                (Rectangle){
                    0, 0, texturas_cutscene[1]->width, texturas_cutscene[1]->height
                },
                //Destino:
                (Rectangle){
                    0, 0, GetScreenWidth()/2, GetScreenHeight()
                },
                (Vector2){0,0},
                0.0f,
                WHITE
            );
        }
        else{
            //Imprimo o fosas
            DrawTexturePro(
                *texturas_cutscene[0],
                //Origem:
                (Rectangle){
                    0, 0, texturas_cutscene[0]->width, texturas_cutscene[0]->height
                },
                //Destino:
                (Rectangle){
                    0, 0, GetScreenWidth()/2, GetScreenHeight()
                },
                (Vector2){0,0},
                0.0f,
                WHITE
            );
        }

        //IMPRESSÃO DOS DIÁLOGOS:
        //Centralização do texto
        int tamanho_texto = TEXT_FONT_SIZE;
        if(linha_atual == 8) tamanho_texto *= 2;
        Vector2 tamanho_medido = MeasureTextEx(text_font, dialogo[linha_atual], tamanho_texto, SPACING);

        //Calcula o centro
        // Adicionamos uma pequena margem (ex: 50 pixels) para não colá-lo no centro exato
        float margem_esquerda_bloco = 100.0f;
        float posx = (GetScreenWidth() / 2.0f) - margem_esquerda_bloco;
        float posy = (GetScreenHeight() - tamanho_medido.y)/2.0f;

        //Defino a cor:
        Color cor = GREEN;
        if(linha_atual == 8) cor = RED;

        //IMPRIMO O TEXTO
        DrawTextEx(
            text_font,
            dialogo[linha_atual],
            (Vector2) { posx, posy },
            tamanho_texto,
            SPACING,
            cor
        );

        //Imprimo o enter
        DrawText("Pressione [ENTER] para avançar...", GetScreenWidth()/2, GetScreenHeight() - 40, 20, LIGHTGRAY);

        EndDrawing();
    }
    else{
        if(!trocou_musica){
            *game_music = &absolute_cinema;
            troca_musica(game_music);
            trocou_musica = true;
        }
        cinema(&linha_atual, tempo_cutscene, &trocou_musica, game_music);
    }
    
}

void cinema(int *linha, float *tempo_cutscene, bool *flag_musica, Music **game_music){
    /**Exibe a cena final da cutscene (imagem "cinema") com um efeito de fade-in progressivo,
     * calculado a partir do tempo restante. Ao esgotar o tempo, retorna o jogo ao MENU e
     * reseta os controles de estado da cutscene (linha atual, flag de música trocada).
     * Param: linha - ponteiro para a linha de diálogo (estática em cutscene()), resetada para 0 ao final
     *        tempo_cutscene - ponteiro para o contador regressivo da cena de fade-in
     *        flag_musica - ponteiro para a flag que indica se a música da cutscene já foi trocada
     *        game_music - ponteiro para a música atual, usado ao retornar a trilha do menu/jogo
     * Return: nenhum
     */
    *tempo_cutscene -= GetFrameTime();

    if(*tempo_cutscene <= 0.0f){
        game = MENU;
        *linha = 0; //Reseto linha aqui por ela ser estática
        *flag_musica = false;
        troca_musica(game_music);
        return;
    }

    //alpha e tempo sao inversamente proporcionais
    //Se tempo 10 --> alpha = 0
    //Se tempo --> 0.1 --> alpha = 1
    float alpha = 1.0f - (*tempo_cutscene/16.0f);

    //Garanto que a proporção fica entre 0 e 1
    if(alpha < 0.0f) alpha = 0.0f;
    if(alpha > 1.0f) alpha = 1.0f;


    BeginDrawing();
    ClearBackground(BLACK);

    DrawTexturePro(
        *texturas_cutscene[2],
        //Origem:
        (Rectangle){
            0,0, texturas_cutscene[2]->width, texturas_cutscene[2]->height
        },
        //Destinho
        (Rectangle){
            (GetScreenWidth() - texturas_cutscene[2]->width)/2, 
            (GetScreenHeight() - texturas_cutscene[2]->height)/2,
            GetScreenWidth()/2,
            GetScreenHeight()
        },
        (Vector2){ 0, 0 },
        0.0f,
        Fade(WHITE, alpha)
    );
    EndDrawing();
    
}
//FUNÇÕES AUXILIARES:
void create_rectangle_ship(Rectangle *rec, Nave *ship, float x_novo, float y_novo){
    /**Monta, em "rec", o retângulo de colisão/desenho de uma nave, aplicando um deslocamento
     * opcional (x_novo, y_novo) à sua posição atual (útil para checar colisão em uma posição
     * futura antes de mover a nave de fato).
     * Param: rec - ponteiro de saída onde o retângulo será escrito
     *        ship - ponteiro para a nave de referência
     *        x_novo, y_novo - deslocamento a somar à posição atual da nave
     * Return: nenhum (resultado escrito em *rec)
     */
    *rec = (Rectangle){
        ship->posicao.x + x_novo,
        ship->posicao.y + y_novo,
        ship->tamanho,
        ship->tamanho
    };
}
void create_rectangle_bullet(Rectangle *rec, Arma *bullet, float x_novo, float y_novo){
    /**Monta, em "rec", o retângulo de colisão/desenho de uma bala, aplicando um deslocamento
     * opcional (x_novo, y_novo) à sua posição atual. Balas do jogador sobem na tela, então o
     * deslocamento vertical é invertido automaticamente para elas.
     * Param: rec - ponteiro de saída onde o retângulo será escrito
     *        bullet - ponteiro para a bala de referência
     *        x_novo, y_novo - deslocamento a somar à posição atual da bala (y_novo é invertido
     *        internamente quando a origem é JOGADOR)
     * Return: nenhum (resultado escrito em *rec)
     */
    if(bullet->origem == JOGADOR) y_novo *= -1; //As balas do jogador devemm subir
    *rec = (Rectangle){
        bullet->posicao.x + x_novo,
        bullet->posicao.y + y_novo,
        bullet->tamanho,
        bullet->tamanho
    };
}