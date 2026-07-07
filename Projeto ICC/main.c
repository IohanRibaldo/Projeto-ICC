/********************************************************************************************************************************************************************************************************
                                                                    Universidade de São Paulo-Campus de São Carlos
Nome:Iohan Guilherme Ribaldo
Nº USP: 17108832
Professor:FOsório
Disciplina:Introdução à Ciência de Computação

DESCRIÇÃO: TRABALHO FINAL DE ICC
********************************************************************************************************************************************************************************************************/
#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

//============================ -    INCLUSÃO DE SUBROTINAS  ==================
#include "globais_e_constantes.h"   //Constantes, enums, structs e declaração (extern) das globais
#include "funcoes_logicas.h"        //Toda a lógica de jogo (inicialização, movimento, colisão, desenho, telas)
//===================   INSTANCIANDO GLOBAIS    ===================
//A partir daqui as variáveis "extern" declaradas em globais_e_constantes.h são de fato
//alocadas (definidas). Este é o único arquivo do projeto onde isso acontece.
Nave player;                                   //Instância única da nave do jogador
Nave enemies[MAX_ENEMIES];                     //Vetor com as naves dos inimigos comuns
Nave boss;                                      //Instância única da nave do chefe final
Score_Popups score_popups[MAX_SCORE_POPUPS] = {0};  //Pool de popups de pontuação, inicializado zerado (todos inativos)

Vector2 origem_tela;                            //Canto superior esquerdo da área jogável (abaixo do HUD)

int score = 0;                                  //Pontuação inicial
bool score_animando = false;                    //Flag auxiliar reservada (não usada nas funções documentadas)
int rodada = 1;                                 //Rodada inicial (recalculada para 6 ao iniciar a partida, ver mais abaixo)

Game_State game;                                //Estado atual do jogo (definido para MENU logo no início de main())
bool fosas = false;                             //Tema inicial: false = tema padrão ("Núcleo de Robótica Aeroespacial")

 //Defino o tamanho da janela para 1600 x 900 pixels
const int comprimento_tela = 1600;              //Largura da janela do jogo
const int altura_tela = 900;                    //Altura da janela do jogo

//Texturas:
Texture2D texture_player;      //Sprite do player (tema padrão)
Texture2D laser_azul;          //Sprite da bala do player
Texture2D laser_vermelho;      //Sprite da bala de inimigos/boss (tema padrão)
Texture2D texture_bitcoin;     //Sprite da bala do boss (tema "fosas")
Texture2D texture_fosorio;     //Sprite do player (tema "fosas")
Texture2D texture_boss;        //Sprite do boss (tema padrão)
Texture2D musk_inimigo;        //Sprite do boss (tema "fosas")
Texture2D java_script_mental;  //Imagem do easter egg do menu
Texture2D background;          //Imagem de fundo do menu/partida
//Fontes:
Font title_font;   //Fonte dos títulos
Font text_font;    //Fonte dos textos gerais
Font hud_font;     //Fonte do HUD
//Variáveis de som:
Music laserhawk;        //Trilha da partida (tema padrão)
Music tie_fighter;      //Trilha da partida (tema "fosas")
Music vitoria;          //Trilha da tela de vitória (tema padrão)
Music absolute_cinema;  //Trilha da cutscene final (tema "fosas")
Sound laser_sound;      //Som de disparo
Sound laser_boom;       //Som de impacto/explosão

//Ponteiros Auxiliares:
//Alocados dinamicamente mais abaixo (calloc) para permitir indexação por tipo/índice
//nas funções de inicialização e desenho (ver inicia_inimigos, imprime_explosao, cutscene, cinema).
Texture2D **texturas_inimigos;   //Vetor de ponteiros para as texturas dos tipos de inimigo (ambos os temas)
Texture2D **texturas_explosao;   //Vetor de ponteiros para os frames da animação de explosão
Texture2D **texturas_cutscene;   //Vetor de ponteiros para as imagens da cutscene final

FILE *Arquivo;

//===========================   ROTINA PRINCIPAL    ====================//
int main(){
    /**Ponto de entrada do jogo. Responsável por:
     *   1) Inicializar janela, áudio e carregar todas as texturas/fontes/sons/músicas;
     *   2) Rodar o loop principal (game loop) até a janela ser fechada ou game == SAIDA,
     *      despachando a lógica e o desenho de acordo com o estado atual (game);
     *   3) Ao final, descarregar (Unload) todos os recursos e fechar a janela.
     */
    //VARIÁVEIS LOCAIS:
    int ynovo, xnovo;              //Deslocamento do player calculado a partir do input do teclado, a cada frame
    float tempo_passado;           //Tempo decorrido desde o último frame (GetFrameTime()), usado nos cálculos de movimento
    float tempo_entrada = FALL_TIME;  //Contador da animação de entrada dramática de inimigos/boss no início da rodada
    float contagem = 3.0f;         //Contador regressivo usado para liberar o easter egg do menu após alguns segundos
    float tempo_animacao;          //Contador regressivo genérico usado nas telas de morte/vitória
    float tempo_cutscene;          //Contador regressivo usado na cena final de fade-in (cutscene do tema "fosas")
    unsigned char opcao = 0;       //Opção atualmente selecionada no menu (1=Iniciar, 2=Controles, 3=Sair)
    bool opcao_2 = false;          //true quando a tela de controles do menu está sendo exibida
    bool easter_egg = false;       //true quando o easter egg do menu já pode ser exibido/clicado
    int n_inimigos_mortos;         //Contador de inimigos comuns MORTOs na rodada atual, usado para detectar fim de rodada

    Round_State state_rodada;      //Estado da transição de rodada (NOVA_RODADA/ANIMANDO/RODADA_ATUAL)
    game = MENU;                   //O jogo sempre inicia exibindo o menu principal

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

    //Inicio a janela e seto o FPS dela
    InitWindow(comprimento_tela, altura_tela, "Raylib Projeto ICC");
    SetTargetFPS(60);

    InitAudioDevice();

    //CARREGANDO ELEMENTOS AUXILIARES:
    //Texturas locais que serão referenciadas pelos ponteiros globais (texturas_inimigos,
    //texturas_explosao, texturas_cutscene) logo abaixo; precisam permanecer vivas durante
    //toda a execução do programa (por isso são carregadas aqui, no escopo de main).
    Texture2D texture_inimigo_1;
    Texture2D texture_inimigo_2;
    Texture2D texture_inimigo_3;
    Texture2D trump;
    Texture2D chatgpt;
    Texture2D texture_java;
    
    Texture2D explosao1;
    Texture2D explosao2;
    Texture2D explosao3;
    Texture2D explosao4;
    Texture2D explosao5;
    Texture2D explosao6;
    Texture2D explosao7;
    Texture2D explosao8;
    Texture2D explosao9;
    Texture2D explosao10;
    Texture2D explosao11;
    
    Texture2D musk_derrotado;
    Texture2D fosorio_won;
    Texture2D cinema;
    //Texturas:
    texture_player = LoadTexture("./assets/spaceship_player_sem_fundo.png");

    texture_inimigo_1 = LoadTexture("./assets/sprites_enemies/PNG_Parts&Spriter_Animation/Ship1/Ship1.png");
    texture_inimigo_2 = LoadTexture("./assets/sprites_enemies/PNG_Parts&Spriter_Animation/Ship2/Ship2.png");
    texture_inimigo_3 = LoadTexture("./assets/sprites_enemies/PNG_Parts&Spriter_Animation/Ship6/Ship6.png");
    texture_boss = LoadTexture("./assets/boss.png");

    laser_azul = LoadTexture("./assets/laser_azul.png");
    laser_vermelho = LoadTexture("./assets/laser_vermelho.png");

    texture_bitcoin = LoadTexture("./assets/bitcoin.png");
    texture_java = LoadTexture("./assets/javascript-logo.png");

    texture_fosorio = LoadTexture("./assets/fosorio.png");
    musk_inimigo = LoadTexture("./assets/musk_inimigo.png");
    trump = LoadTexture("./assets/trump.png");
    chatgpt = LoadTexture("./assets/chatgpt.png");
    
    java_script_mental = LoadTexture("./assets/java_script_mental.png");
    background = LoadTexture("./assets/espaco_bg.png");

    explosao1 = LoadTexture("./assets/sprites_enemies/PNG_Parts&Spriter_Animation/Explosions/Explosion2/Explosion2_1.png");
    explosao2 = LoadTexture("./assets/sprites_enemies/PNG_Parts&Spriter_Animation/Explosions/Explosion2/Explosion2_2.png");
    explosao3 = LoadTexture("./assets/sprites_enemies/PNG_Parts&Spriter_Animation/Explosions/Explosion2/Explosion2_3.png");
    explosao4 = LoadTexture("./assets/sprites_enemies/PNG_Parts&Spriter_Animation/Explosions/Explosion2/Explosion2_4.png");
    explosao5 = LoadTexture("./assets/sprites_enemies/PNG_Parts&Spriter_Animation/Explosions/Explosion2/Explosion2_5.png");
    explosao6 = LoadTexture("./assets/sprites_enemies/PNG_Parts&Spriter_Animation/Explosions/Explosion2/Explosion2_6.png");
    explosao7 = LoadTexture("./assets/sprites_enemies/PNG_Parts&Spriter_Animation/Explosions/Explosion2/Explosion2_7.png");
    explosao8 = LoadTexture("./assets/sprites_enemies/PNG_Parts&Spriter_Animation/Explosions/Explosion2/Explosion2_8.png");
    explosao9 = LoadTexture("./assets/sprites_enemies/PNG_Parts&Spriter_Animation/Explosions/Explosion2/Explosion2_9.png");
    explosao10 = LoadTexture("./assets/sprites_enemies/PNG_Parts&Spriter_Animation/Explosions/Explosion2/Explosion2_10.png");
    explosao11 = LoadTexture("./assets/sprites_enemies/PNG_Parts&Spriter_Animation/Explosions/Explosion2/Explosion2_11.png");

    fosorio_won = LoadTexture("./assets/fosas_cutscene.png");
    musk_derrotado = LoadTexture("./assets/musk_derrotado.png");
    cinema = LoadTexture("./assets/cinema.png");

    //Fontes:
    title_font = LoadFontEx("./assets/PressStart.ttf", TITLE_FONT_SIZE, NULL, 0);
    text_font = LoadFontEx("./assets/Silkscreen.ttf", TEXT_FONT_SIZE, NULL, 0);
    hud_font = LoadFontEx("./assets/PressStart.ttf", HUD_FONT_SIZE, NULL, 0);
    //Sons e Musicas:
    laserhawk = LoadMusicStream("./assets/musica_fundo_game.mp3");
    tie_fighter = LoadMusicStream("./assets/TIE_FIGHTER.mp3");
    laser_sound = LoadSound("./assets/shoot.mp3");
    laser_boom = LoadSound("./assets/Boom.mp3");

    vitoria = LoadMusicStream("./assets/Final.mp3");
    absolute_cinema = LoadMusicStream("./assets/Cinema.mp3");

    //CRIO PONTEIROS AUXILIARES PARA MANIPULAÇÃO DE DIFERENTES TEXTURAS:
    //Ponteiro das texturas dos diferentes inimigos
    texturas_inimigos = (Texture2D **) calloc(2*MAX_ENEMIES_TYPE, sizeof(Texture2D*));
    texturas_inimigos[0] = &texture_inimigo_1;
    texturas_inimigos[1] = &texture_inimigo_2;
    texturas_inimigos[2] = &texture_inimigo_3;
    texturas_inimigos[3] = &chatgpt;
    texturas_inimigos[4] = &texture_java;
    texturas_inimigos[5] = &trump;
    //Ponteiro das texturas de explosão do player
    texturas_explosao = (Texture2D **) calloc(MAX_EXPLOSION_SPRITES, sizeof(Texture2D*));
    texturas_explosao[0] = &explosao1;
    texturas_explosao[1] = &explosao2;
    texturas_explosao[2] = &explosao3;
    texturas_explosao[3] = &explosao4;
    texturas_explosao[4] = &explosao5;
    texturas_explosao[5] = &explosao6;
    texturas_explosao[6] = &explosao7;
    texturas_explosao[7] = &explosao8;
    texturas_explosao[8] = &explosao9;
    texturas_explosao[9] = &explosao10;
    texturas_explosao[10] = &explosao11;
    //Ponteiros das texturas de cutscene final:
    texturas_cutscene = (Texture2D **) calloc(CUTSCENE_ELEMENTS, sizeof(Texture2D*));
    texturas_cutscene[0] = &fosorio_won;
    texturas_cutscene[1] = &musk_derrotado;
    texturas_cutscene[2] = &cinema;

    //Ponteiro para a trilha sonora atualmente ativa; é reatribuído por troca_musica()
    //conforme o tema (fosas) e o estado do jogo mudam.
    Music *game_music = &laserhawk;
    PlayMusicStream(*game_music);

    // DIAGNÓSTICO DE CARREGAMENTO DE IMAGENS(MODO DEBUG 1):
    if(DEBUG){
        printf("--- LOG DE CARREGAMENTO DE TEXTURAS ---\n");
        if(!fosas){
            printf("Player: ID=%u, Largura=%d, Altura=%d\n", texture_player.id, texture_player.width, texture_player.height);
            printf("Inimigo: ID=%u, Largura=%d, Altura=%d\n", texture_inimigo_1.id, texture_inimigo_1.width, texture_inimigo_1.height);
        }
        else{
            printf("Fosorio: ID=%u, Largura=%d, Altura=%d", texture_fosorio.id, texture_fosorio.width, texture_fosorio.height);
        }
        printf("Fundo:  ID=%u, Largura=%d, Altura=%d\n", background.id, background.width, background.height);
        printf("---------------------------------------\n");
    }

    //IMPESSO A RAYLIB DE BORRAR A PIXEL ART QUANDO ELA MUDAR DE TAMANHO
    SetTextureFilter(texture_player, TEXTURE_FILTER_POINT);
    SetTextureFilter(texture_inimigo_1, TEXTURE_FILTER_POINT);
    SetTextureFilter(texture_inimigo_2, TEXTURE_FILTER_POINT);
    SetTextureFilter(texture_inimigo_3, TEXTURE_FILTER_POINT);
    SetTextureFilter(laser_azul, TEXTURE_FILTER_POINT);
    SetTextureFilter(laser_vermelho, TEXTURE_FILTER_POINT);
    SetTextureFilter(texture_bitcoin, TEXTURE_FILTER_POINT);
    SetTextureFilter(texture_java, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(background, TEXTURE_FILTER_POINT);
    SetTextureFilter(texture_fosorio, TEXTURE_FILTER_BILINEAR); 
    SetTextureFilter(java_script_mental, TEXTURE_FILTER_BILINEAR);

    SetRandomSeed(time(NULL));
    
    //===========================   LOOP PRINCIPAL (GAME LOOP)  ====================//
    //A cada iteração (1 por frame): atualiza a música, e então executa a lógica e o
    //desenho referentes ao estado atual do jogo (game). O loop termina quando a janela
    //é fechada pelo usuário ou quando o estado avança para SAIDA (opção "Sair" do menu).
    while(!WindowShouldClose() && game != SAIDA){
        UpdateMusicStream(*game_music);
        if(game == MENU){
            //======================    MENU PRINCIPAL  ======================//
            //Trata navegação (teclado e mouse), o easter egg do menu e a seleção de
            //cada opção (Iniciar, Controles, Sair), finalizando com o desenho da tela
            //de menu por imprime_menu().
            //SEÇÃO LÓGICA:
            //Easter Egg:
            if(contagem > 0.0f) contagem -= GetFrameTime();
            if(contagem <= 0.0f) easter_egg = true;
            //Condição de escape
            if(IsKeyPressed(KEY_P)) break;
            //Navegação pelo teclado
            if(IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)){
                if(opcao <= 1) opcao = 3;
                else opcao--;
            }
            if(IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)){
                if(opcao >= 3) opcao = 1;
                else opcao++;
            }
            //Navegação por mouse/rato:
            if(CheckCollisionPointRec(GetMousePosition(), rec_botao_start)) opcao = 1;
            if(CheckCollisionPointRec(GetMousePosition(), rec_botao_controles)) opcao = 2;
            if(CheckCollisionPointRec(GetMousePosition(), rec_botao_saida)) opcao = 3;
            //Pressionado o botão:
            if(opcao == 1){
                //Opção "Iniciar": confirma clique com o mouse sobre o botão OU tecla ENTER
                if((CheckCollisionPointRec(GetMousePosition(), rec_botao_start) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) || IsKeyPressed(KEY_ENTER)){
                    game = GAME;
                    tempo_animacao = 10.0f;
                    tempo_cutscene = 16.0f;
                    rodada = 1; 
                    score = 0;            
                    inicia_rodada();
                    if(rodada != 1) {
                    // Se não for a primeira ronda, coloca o jogador na posição final de combate
                    player.posicao.y = GetScreenHeight() - 150.0f; 
                    }
                    state_rodada = NOVA_RODADA;
                }
            }
            if(opcao == 2){
                 if((CheckCollisionPointRec(GetMousePosition(), rec_botao_controles) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) || IsKeyPressed(KEY_ENTER)){
                    opcao_2 = true;
                }
            }
            if(opcao == 3){
                if((CheckCollisionPointRec(GetMousePosition(), rec_botao_saida) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) || IsKeyPressed(KEY_ENTER)){
                    game = SAIDA;
                }
            }
            //EScape do menu de controle:
            if(opcao_2 == true) if(IsKeyPressed(KEY_Q)) opcao_2 = false;
            //Easter Egg:
            if(game != MENU && !fosas) {contagem = 3.0f; easter_egg = false;}
            if(easter_egg && CheckCollisionPointRec(GetMousePosition(), rec_java) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                fosas = !fosas;
                troca_musica(&game_music);
            }

            //SEÇÃO DE DESENHO:
            imprime_menu(opcao, opcao_2, easter_egg); 
        }
        if(game == GAME || game == PAUSA){
            //======================    LÓGICA DO JOGO  ======================//
            //Enquanto a rodada não está "de fato" em combate (state_rodada != RODADA_ATUAL),
            //atualiza apenas o posicionamento inicial dos inimigos/boss e a animação de
            //entrada dramática. Assim que a animação termina, a lógica completa de combate
            //(movimento, tiros, colisões, HUD) passa a rodar no bloco "else" abaixo.
            //Caso esteja transicionando a rodada faço os inimigos aparecerem dramaticamente:
            if(state_rodada != RODADA_ATUAL){
                update_enemies(&state_rodada);
                entrada_dramatica(&state_rodada, &tempo_entrada);
            }
            //Se a rodada realmente começõu
            else{
                //Reseto variáveis de controle:
                ynovo = xnovo = 0;
                tempo_passado = GetFrameTime();
                if(player.tempo_desde_ultimo_tiro > 0.0f) player.tempo_desde_ultimo_tiro -= GetFrameTime();
                //1.MOVIMENTO DO PLAYER:
                //Verifico input de teclado a cada frame:
                if(IsKeyDown(KEY_W)) ynovo -= player.velocidade * tempo_passado;
                if(IsKeyDown(KEY_A)) xnovo -= player.velocidade * tempo_passado;
                if(IsKeyDown(KEY_S)) ynovo += player.velocidade * tempo_passado;
                if(IsKeyDown(KEY_D)) xnovo += player.velocidade * tempo_passado;
                //Condição de escape
                if(IsKeyPressed(KEY_P)) {
                    if(game == PAUSA) {
                        game = MENU;
                        add_score();    //Adiciona o score caso ele saia da partida no meio
                    }
                    if(game == GAME) game = PAUSA;
                }
                if(game == PAUSA && IsKeyPressed(KEY_ENTER)) game = GAME;

                if(game == GAME){
                    //1. MOVIMENTO DO PLAYER
                    if(podeMover(xnovo , ynovo, &player)){
                        player.posicao.x += xnovo;
                        player.posicao.y += ynovo;
                    }

                    //2.GATILHO/ARMA DO PLAYER
                    //Só adiciono balas a cada 0.5s
                    if(IsKeyDown(KEY_SPACE) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                        if(player.tempo_desde_ultimo_tiro <= 0.0f && player.estado == VIVO){
                        //As balas saem do centro superior do player
                        adiciona_bala(player.posicao.x + player.tamanho/2, player.posicao.y, player.gun);
                        player.tempo_desde_ultimo_tiro = player.intervalo_balas;
                        }
                    }
                    //Update da coordenada das balas
                    //2.MOVIMENTO DOS INIMIGOS:
                    move_inimigos(tempo_passado);
                    //3.ARMA DO JOGADOR:
                    move_bala(player.gun);
                    //4.ARMA DOS INIMIGOS:
                    balas_inimigos();
                    //5.Atualizo os popups se houver:
                    for(int i = 0; i < MAX_SCORE_POPUPS; i++){
                        if(score_popups[i].ativo){
                            score_popups[i].tempo_de_vida -= GetFrameTime();
                            score_popups[i].alpha = score_popups[i].tempo_de_vida;

                            if(score_popups[i].tempo_de_vida <= 0.0f){
                                score_popups[i].ativo = false;
                            }
                        }
                    }
                    //6. CHECAGEM DE MORTE DO PLAYER:
                    if(player.estado == EXPLODINDO) explode_nave(&player);
                    if(player.estado == MORTO){
                        tempo_animacao = 2.5f;
                        game = MORTE;
                    }
                    //7. VERIFICAÇÃO DA MORTE DOS INIMIGOS E AVANÇO DA RODADA:
                    //Conta quantos inimigos comuns já estão MORTOs (destruídos ou nunca ativos
                    //nesta rodada); quando todos estiverem MORTOs, avança para a próxima rodada.
                    n_inimigos_mortos = 0;
                    if(rodada < 6){
                        for(int i = 0; i < MAX_ENEMIES; i++){
                            if(enemies[i].estado == MORTO) n_inimigos_mortos++;
                            if(enemies[i].estado == EXPLODINDO) explode_nave(&enemies[i]);
                        }
                        if(n_inimigos_mortos == MAX_ENEMIES){
                        rodada++; 
                        state_rodada = NOVA_RODADA;
                        }
                    }
                
                    //8. BOSS
                    //A partir da rodada 6 (chefe final), o boss passa a se mover e atirar;
                    //ao ser destruído, o jogo avança para a tela de vitória.
                    if(rodada == 6){
                        if(boss.estado == MORTO) boss.estado = VIVO;
                        move_boss(tempo_passado);
                        balas_boss();
                    }
                    if(boss.estado == EXPLODINDO) explode_nave(&boss);
                    if(boss.estado == MORTO && rodada == 6){
                        game_music = &vitoria;
                        game = VITORIA;
                        troca_musica(&game_music);
                    }

                    //9. O JOGO TERMINOU:
                    if(game == MORTE || game == VITORIA) kill_everyone();
                }
            }

            //======================    DESENHO NO TERMINAL  ======================//
            /**NOTAS PARA MIM MESMO:
             * Tamanho de Texto Mínimo para ficar visível: 40
             * A posição dos elementos que colocam-se no Draw levam em conta o canto superior esquerdo
             */
            BeginDrawing();
            ClearBackground(BLACK);

            //1. IMPRESÃO DO BACKGROUND:
            DrawTexturePro(
                background,
                (Rectangle){
                    0,0, (float) background.width, (float) background.height
                },
                (Rectangle){
                    origem_tela.x,origem_tela.y, (float) GetScreenWidth(), (float) GetScreenHeight()
                },
                (Vector2) {0,0},
                0.0f,
                WHITE
            );
            //4.IMPRESSÃO DO HUD:
            imprime_HUD();
            //3. IMPRESSÃO DO PLAYER:
            imprime_player();
            //4. IMPRESSÃO DOS INIMIGOS E DE SUAS EXPLOSOES:
            imprime_inimigos();
            //5.IMPRESSÃO DAS BALAS DO PLAYER:
            imprime_bala(player.gun);
            //6.IMPRESSÃO DAS BALAS DOS INIMIGOS:
            for(int i = 0; i < MAX_ENEMIES; i++) imprime_bala(enemies[i].gun);
            //7. IMPRESSÃO DOS POPUPS:
            imprime_score_popup();
            //8. IMPRIME O BOSS
            imprime_boss();
            imprime_bala(boss.gun);
            
            if(game == PAUSA){
                imprime_pausa();
            }

            EndDrawing();
        }

        if(game == MORTE){
            //Tela de Game Over: player_death cuida da contagem e do desenho; ao expirar
            //o tempo, ela mesma retorna game para MENU, então aqui só garantimos a troca
            //de música quando essa transição ocorre.
            player_death(&tempo_animacao);
            if(game == MENU){   //Garanto que a musica mude e toque
                troca_musica(&game_music);
                add_score();    //Salvo o score atual
                score = 0;
            }
        }
        if(game == VITORIA){
            //Tela de vitória: no tema "fosas" é exibida a cutscene narrativa completa;
            //no tema padrão, apenas a tela simples de "VOCE VENCEU!!!" (game_won).
            fosas ? cutscene(&tempo_cutscene, &game_music) : game_won(&tempo_animacao);
            if(game == MENU){   //Garanto que a musica mude e toque
                troca_musica(&game_music);
                add_score();    //Salvo o score atual
                score = 0;
            }
        }
    }
    //UNLOAD DE FUNÇÕES QUANDO FINALIZAR O PROGRAMA:
    // 1. Parar e descarregar Streams de Áudio
    StopMusicStream(*game_music);     

    // 2. Descarregar todas as Texturas (VRAM)
    UnloadTexture(texture_player);
    UnloadTexture(texture_inimigo_1);
    UnloadTexture(texture_inimigo_2);
    UnloadTexture(texture_inimigo_3);
    UnloadTexture(texture_bitcoin);
    UnloadTexture(texture_java);
    UnloadTexture(laser_azul);
    UnloadTexture(laser_vermelho);
    UnloadTexture(background);
    UnloadTexture(texture_fosorio);
    UnloadTexture(java_script_mental);
    UnloadTexture(texture_boss);
    UnloadTexture(musk_inimigo);
    UnloadTexture(trump);
    UnloadTexture(chatgpt);

    // Quadros de animação
    UnloadTexture(explosao1);
    UnloadTexture(explosao2);
    UnloadTexture(explosao3);
    UnloadTexture(explosao4);
    UnloadTexture(explosao5);
    UnloadTexture(explosao6);
    UnloadTexture(explosao7);
    UnloadTexture(explosao8);
    UnloadTexture(explosao9);
    UnloadTexture(explosao10);
    UnloadTexture(explosao11);

    // Telas de fim de jogo / cutscenes
    UnloadTexture(musk_derrotado);
    UnloadTexture(fosorio_won);
    UnloadTexture(cinema);

    // 3. Descarregar Fontes de Texto
    UnloadFont(title_font);
    UnloadFont(text_font);
    UnloadFont(hud_font);

    //4.Descarregando Sons
    UnloadMusicStream(laserhawk);
    UnloadMusicStream(tie_fighter);
    UnloadMusicStream(absolute_cinema);
    UnloadMusicStream(vitoria);
    UnloadSound(laser_boom);
    UnloadSound(laser_sound);

    // 5. Liberando memórias alocadas via calloc (Essencial para não ter memory leak)
    free(texturas_inimigos);
    free(texturas_explosao);
    free(texturas_cutscene);

    //6. Encerrar o contexto da janela
    CloseAudioDevice();  
    CloseWindow();
    return 0;
}