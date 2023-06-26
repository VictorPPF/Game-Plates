#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
//#include <stdlib.h>
// #include<math.h>
#include <time.h>

#define NUM_PRATOS 6

const float FPS = 100;  

const int SCREEN_W = 960;
const int SCREEN_H = 540;

//largura do jogador
const float JOGADOR_W = 50;
const float JOGADOR_H = 100;

typedef struct Jogador {
	
	float x;
	int equilibrando;
	int mov_esq, mov_dir;
	ALLEGRO_COLOR cor;
	float vel;
	
} Jogador;

typedef struct Prato {
	float x; //Coordenada
	ALLEGRO_COLOR cor;
	/* um valor entre 255 e 0, em que 255 = prato equilibrado e
	   0 = prato com maxima energia, prestes a cair */
	int energia;
	int tempoParaAparecer;
	int aparente;
	int caiu;
	int sendoEquilibrado;
	
} Prato;


void desenha_cenario() {
	
	ALLEGRO_COLOR BKG_COLOR = al_map_rgb(194,0,223);
	//colore a tela de branco (rgb(255,255,255))
	al_clear_to_color(BKG_COLOR); 
	
}

void desenha_jogador(Jogador j) {
	
	al_draw_filled_triangle(j.x, SCREEN_H - JOGADOR_H, 
							j.x - JOGADOR_W/2, SCREEN_H,
							j.x + JOGADOR_W/2, SCREEN_H,
							j.cor);	
	
}

void atualiza_jogador(Jogador *j) {
	if(j->mov_esq) {
		
		if(j->x - j->vel > 0)
			j->x -= j->vel;
	}
	if(j->mov_dir) {
		
		if(j->x + j->vel < SCREEN_W)
			j->x += j->vel;
	}
	if(j->equilibrando){
		j->vel=0;
		j->cor=al_map_rgb(0,255,0);
	}else{
		j->vel=1;
		j->cor = al_map_rgb(34,139,34);
	}
}


void InicializaJogador(Jogador *j) {
	j->x = SCREEN_W / 2;
	j->equilibrando = 0;
	j->cor = al_map_rgb(34,139,34);
	j->mov_esq = 0;
	j->mov_dir = 0;
	j->vel = 1;
}

float geraTempoPrato(int min, int max) {

	return rand()%(max-min+1)+min;
}

void inicializaPratos(Prato pratos[]) {
	
	//CONSERTAR ESTA FUNCAO!
	float coord=(float)((SCREEN_W)/(NUM_PRATOS+1.0));
	int i;
	for(i=0; i<NUM_PRATOS; i++) {
		pratos[i].x = coord*(i+1); //Coord
		pratos[i].tempoParaAparecer = geraTempoPrato(i*i-NUM_PRATOS*i+NUM_PRATOS*2.8, i*i-NUM_PRATOS*i+NUM_PRATOS*2.8+10);
		pratos[i].energia = 255;
		pratos[i].aparente = 0;
		pratos[i].caiu = 0;
		pratos[i].sendoEquilibrado = 0;
	}
	

}

void desenha_pratos(Prato pratos[]){
	ALLEGRO_COLOR LINE_COLOR = al_map_rgb(0,0,0);
	ALLEGRO_COLOR LINE_COLOR2 = al_map_rgb(0,0,255);
	int pe=SCREEN_H-100;
	int cabeca= SCREEN_H-400;

	int i;
	for(i=0; i<NUM_PRATOS; i++){
		if(pratos[i].sendoEquilibrado){
			al_draw_line(pratos[i].x, pe, pratos[i].x, cabeca,  LINE_COLOR2, 2); //Desenha o bastao quando equilibrado
		}else{
			al_draw_line(pratos[i].x, pe, pratos[i].x, cabeca,  LINE_COLOR, 2); //Desenha os bastoes
		}
		if(pratos[i].aparente){
			if(pratos[i].caiu){
				al_draw_filled_ellipse(pratos[i].x, SCREEN_H-10, 20, 10, pratos[i].cor); //Desenha o prato no chao
			}else{
				al_draw_filled_ellipse(pratos[i].x, cabeca, 20, 10, pratos[i].cor); //Desenha os pratos
			}
		}
	}
}

void atualiza_pratos(Prato prato[], int tempoEmSegundos){
	int i;
	for(i=0; i<NUM_PRATOS; i++){
		
		if(prato[i].energia<=0)
			prato[i].caiu=1;
			
		if(prato[i].tempoParaAparecer<=tempoEmSegundos){
			prato[i].aparente=1;
		}
		prato[i].cor=al_map_rgb(255,prato[i].energia,prato[i].energia);
		if(prato[i].aparente){
			if(prato[i].sendoEquilibrado){
				if(prato[i].energia+10>=255){
					prato[i].energia=255;
				}else{
					prato[i].energia+=1;
				}
			}
			if((rand()%100)==1){//Dificulade do jogo
				if(prato[i].energia-10<=0){
					prato[i].energia=0;
				}else{
					prato[i].energia-=10;
				}
			}
		}
	}
}

void testa_equilibrio(Jogador j, Prato prato[]){
	int i;
	for(i=0; i<NUM_PRATOS; i++){
		if(j.x >= prato[i].x-2 && j.x <= prato[i].x+2 && j.equilibrando){
			prato[i].sendoEquilibrado=1;
			//printf("Ta sendo equilibrado aqui moco\n\n\n\n");
		}else{
			prato[i].sendoEquilibrado=0;
		}	
	}
		

}

void perdeu(ALLEGRO_FONT **font){

	ALLEGRO_COLOR BKG_COLOR = al_map_rgb(255,255,255);
	//colore a tela de branco (rgb(255,255,255))
	al_clear_to_color(BKG_COLOR);
	char textoGameOver[50];
	sprintf(textoGameOver, "PERDEU PLAYBOY");
    //snprintf(textoGameOver, sizeof(textoGameOver), "PERDEU PLAYBOY");
	al_draw_text(*font, al_map_rgb(0, 0, 0), 10, 10, ALLEGRO_ALIGN_CENTER, textoGameOver);


}

void desenha_pontuacao(int tempoEmSegundos, ALLEGRO_FONT **font){
	char tempoEmTexto[25];
    snprintf(tempoEmTexto, sizeof(tempoEmTexto), "Tempo em segundos: %d", tempoEmSegundos);
	al_draw_text(*font, al_map_rgb(255, 255, 255), 10, 10, ALLEGRO_ALIGN_LEFT, tempoEmTexto);
}
 
int main(int argc, char **argv){


	unsigned int seed = (unsigned int)time(NULL);

	 srand(seed);
	
	ALLEGRO_DISPLAY *display = NULL;	
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	
	//inicializa o Allegro
	if(!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}
	
	//cria um temporizador que incrementa uma unidade a cada 1.0/FPS segundos
    timer = al_create_timer(1.0 / FPS);
    if(!timer) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}	
	
    //inicializa o módulo de primitivas do Allegro
    if(!al_init_primitives_addon()){
		fprintf(stderr, "failed to initialize primitives!\n");
        return -1;
    }	
	
	//inicializa o modulo que permite carregar imagens no jogo
	if(!al_init_image_addon()){
		fprintf(stderr, "failed to initialize image module!\n");
		return -1;
	}	
	
	//cria uma tela com dimensoes de SCREEN_W, SCREEN_H pixels
	display = al_create_display(SCREEN_W, SCREEN_H);
	if(!display) {
		fprintf(stderr, "failed to create display!\n");
		al_destroy_timer(timer);
		return -1;
	}	
	
	//instala o teclado
	if(!al_install_keyboard()) {
		fprintf(stderr, "failed to install keyboard!\n");
		return -1;
	}
	
	//inicializa o modulo allegro que carrega as fontes
	al_init_font_addon();

	//inicializa o modulo allegro que entende arquivos tff de fontes
	if(!al_init_ttf_addon()) {
		fprintf(stderr, "failed to load tff font module!\n");
		return -1;
	}
	
	//carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
    ALLEGRO_FONT *size_32 = al_load_font("arial.ttf", 32, 1);   
	if(size_32 == NULL) {
		fprintf(stderr, "font file does not exist or cannot be accessed!\n");
	}	
	
	
 	//cria a fila de eventos
	event_queue = al_create_event_queue();
	if(!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
	}	
	
	//registra na fila os eventos de tela (ex: clicar no X na janela)
	al_register_event_source(event_queue, al_get_display_event_source(display));
	//registra na fila os eventos de tempo: quando o tempo altera de t para t+1
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	//registra na fila os eventos de teclado (ex: pressionar uma tecla)
	al_register_event_source(event_queue, al_get_keyboard_event_source()); 	
	
	
	//JOGADOR
	Jogador jogador;
	InicializaJogador(&jogador);
	
	//PRATOS
	Prato pratos[NUM_PRATOS];
	inicializaPratos(pratos);
	int i;
	for(i=0; i<NUM_PRATOS; i++)
		printf("Tempo pra aparecer prato(%d): %d\n", i, pratos[i].tempoParaAparecer);

	int playing=1;
	
	//inicia o temporizador
	al_start_timer(timer);	

	//inicia o tempo em segundos
	int timeInSec=0;
	
	while(playing) {
		
		ALLEGRO_EVENT ev;
		//espera por um evento e o armazena na variavel de evento ev
		al_wait_for_event(event_queue, &ev);
		
		//se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
		if(ev.type == ALLEGRO_EVENT_TIMER) {

			int i;
			for(i=0; i<NUM_PRATOS; i++)
				if(pratos[i].caiu){
					playing = 0;
					al_rest(2);
				}
			
			desenha_cenario();

			atualiza_pratos(pratos, timeInSec);

			desenha_pratos(pratos);
			
			atualiza_jogador(&jogador);
			
			desenha_jogador(jogador);
			
			testa_equilibrio(jogador, pratos);
				

			if(al_get_timer_count(timer)%(int)FPS == 0){
				timeInSec=(int)(al_get_timer_count(timer)/FPS);
				printf("\n%d segundos se passaram...", timeInSec);
			}
		
			desenha_pontuacao(timeInSec, &size_32);
			
			//atualiza a tela (quando houver algo para mostrar)
			al_flip_display();
		}
		//se o tipo de evento for o fechamento da tela (clique no x da janela)
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			playing = 0;
		}		
		//se o tipo de evento for um pressionar de uma tecla
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			//imprime qual tecla foi
			//printf("\ncodigo tecla: %d", ev.keyboard.keycode);
			
			if(ev.keyboard.keycode == ALLEGRO_KEY_A) {
				jogador.mov_esq = 1;
			}
			else if(ev.keyboard.keycode == ALLEGRO_KEY_D) {
				jogador.mov_dir = 1;
			}else if(ev.keyboard.keycode == ALLEGRO_KEY_SPACE){
				jogador.equilibrando = 1;
			}			
		}
		//se o tipo de evento for um pressionar de uma tecla
		else if(ev.type == ALLEGRO_EVENT_KEY_UP) {

			if(ev.keyboard.keycode == ALLEGRO_KEY_A) {
				jogador.mov_esq = 0;
			}
			else if(ev.keyboard.keycode == ALLEGRO_KEY_D) {
				jogador.mov_dir = 0;
			}else if(ev.keyboard.keycode == ALLEGRO_KEY_SPACE){
				jogador.equilibrando = 0;
			}				
		}		
		
		
	}
	
	

	//Texto de Game Over
	al_clear_to_color(al_map_rgb(255,255,255));
	char textoGameOver[100];
	//sprintf(textoGameOver, "GAME OVER");
    snprintf(textoGameOver, sizeof(textoGameOver), "GAME OVER");
	al_draw_text(size_32, al_map_rgb(0, 0, 0), SCREEN_W/3, SCREEN_H/2, ALLEGRO_ALIGN_LEFT, textoGameOver);	
	//reinicializa a tela
	al_flip_display();	
    al_rest(1);		   

	//Salva o recorde
	int maiorRecorde, novoRecorde=0;
	FILE *arq;
	arq=fopen("recorde.txt", "r");
	if(arq == NULL)
		printf("\nErro ao abrir o arquivo\n");
	fscanf(arq, "%d", &maiorRecorde);
	printf("\nTeste\n%d\n\n", maiorRecorde);
	fclose(arq);
	if(timeInSec>maiorRecorde){
		novoRecorde=1;
		maiorRecorde=timeInSec;
		arq=fopen("recorde.txt", "w");
		fprintf(arq,"%d",maiorRecorde);
		if(arq == NULL)
			printf("\nErro ao abrir o arquivo\n");
		fclose(arq);
	}


	char textoPontos[50];
	char textoRecorde[50];
	if(novoRecorde){
		char textoParabens[50];
		snprintf(textoParabens, sizeof(textoParabens), "Novo recorde! Parabéns!");
		al_draw_text(size_32, al_map_rgb(0, 0, 0), SCREEN_W/3, SCREEN_H/2+50, ALLEGRO_ALIGN_LEFT, textoParabens);	
	}
	snprintf(textoPontos, sizeof(textoPontos), "Sua pontuacao foi: %d", timeInSec);
	snprintf(textoRecorde, sizeof(textoRecorde), "Recorde: %d", maiorRecorde);
	al_draw_text(size_32, al_map_rgb(0, 0, 0), SCREEN_W/3, SCREEN_H/2+100, ALLEGRO_ALIGN_LEFT, textoPontos);	
	al_draw_text(size_32, al_map_rgb(0, 0, 0), SCREEN_W/3, SCREEN_H/2+150, ALLEGRO_ALIGN_LEFT, textoRecorde);
	//Atualiza a tela
	al_flip_display();
	al_rest(3);

	//procedimentos de fim de jogo
	al_destroy_font(size_32);
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
	
 
	return 0;
}