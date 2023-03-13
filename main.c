#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <time.h>

int ennemy_queue = 1;
int pitch_angle = 0;

const int MAX_MISSILES = 5; // Maximum de missiles pouvant être tirés
const int MAX_BOMBES = 5; // Maximum de missiles pouvant être tirés
const int MAX_FLOAT_ENNEMY = 5; // Maximum zepplin a l'ecran

// Console de debugging
void debug(SDL_Renderer * renderer, int a, int b, int c, int d, int e, int f) {
  char chaine[100];
  sprintf(chaine, " sprite_x : %d sprite_y : %d timer : %d ennemy_queue : %d // ennemy_active 1 : %d ennemy_active 2 : %d", a, b, c, d, e, f);
  SDL_Color couleurTexte = {
    255,
    255,
    255
  };


  // Couleur blanche pour le texte
  TTF_Font * police = TTF_OpenFont("police/arial.ttf", 25); // Charger une police de caractères
  SDL_Surface * texteSurface = TTF_RenderText_Blended(police, chaine, couleurTexte);
  SDL_Texture * texteTexture = SDL_CreateTextureFromSurface(renderer, texteSurface);
  SDL_Rect texteRect = {
    250,
    100,
    0,
    0
  };

  // Position du texte dans la fenêtre
  SDL_QueryTexture(texteTexture, NULL, NULL, & texteRect.w, & texteRect.h);
  SDL_RenderCopy(renderer, texteTexture, NULL, & texteRect);
  SDL_FreeSurface(texteSurface);
  SDL_DestroyTexture(texteTexture);
}




//Fonction de gestion de timer
int timer(int g) {

  time_t seconds;
  seconds = time(NULL);
  int second_modulo = seconds % g;
  return second_modulo;
}

int main(int argc, char * argv[]) {
  // Initialisation de la SDL
  SDL_Init(SDL_INIT_VIDEO);

  if (TTF_Init() < 0) { // Initialiser la bibliothèque SDL_ttf
    printf("Erreur lors de l'initialisation de SDL_ttf : %s\n", TTF_GetError());
    return -1;
  }

  // CrÈation de la fenÍtre
  SDL_Window * window = SDL_CreateWindow("Mon jeu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1920, 1080, NULL);

  // CrÈation du renderer
  SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  // Chargement de l'image du sprite
  SDL_Surface * sprite_surface = IMG_Load("gfx/helico.png");
  SDL_Surface * sprite_forward_surface = IMG_Load("gfx/helico_tilted_forward.png");
  SDL_Surface * sprite_backward_surface = IMG_Load("gfx/helico_tilted_backward.png");
  SDL_Surface * map_background_surface = IMG_Load("gfx/map_background.png");
  SDL_Surface * map_layer_one_surface = IMG_Load("gfx/map_layer_one.png");

  SDL_Texture * sprite_texture = SDL_CreateTextureFromSurface(renderer, sprite_surface);
  SDL_FreeSurface(sprite_surface);
  SDL_Texture * sprite_forward_texture = SDL_CreateTextureFromSurface(renderer, sprite_forward_surface);
  SDL_FreeSurface(sprite_forward_surface);
  SDL_Texture * sprite_backward_texture = SDL_CreateTextureFromSurface(renderer, sprite_backward_surface);
  SDL_FreeSurface(sprite_forward_surface);
  SDL_Texture * map_background_texture = SDL_CreateTextureFromSurface(renderer, map_background_surface);
  SDL_FreeSurface(map_background_surface);
  SDL_Texture * map_layer_one_texture = SDL_CreateTextureFromSurface(renderer, map_layer_one_surface);
  SDL_FreeSurface(map_layer_one_surface);

  // Chargement de l'image du missile
  SDL_Surface * missile_surface = IMG_Load("gfx/missile_aa.png");
  SDL_Texture * missile_texture = SDL_CreateTextureFromSurface(renderer, missile_surface);
  SDL_FreeSurface(missile_surface);

  // Chargement de l'image de la bombe
  SDL_Surface * bombe_surface = IMG_Load("gfx/bombe.png");
  SDL_Texture * bombe_texture = SDL_CreateTextureFromSurface(renderer, bombe_surface);
  SDL_FreeSurface(bombe_surface);

  // Chargment image zepplin
  SDL_Surface * zepplin_surface = IMG_Load("gfx/zepplin.png");
  SDL_Texture * zepplin_texture = SDL_CreateTextureFromSurface(renderer, zepplin_surface);
  SDL_FreeSurface(zepplin_surface);

  // Position initiale du sprite
  int sprite_x = 500;
  int sprite_y = 500;

  // Tableaux stockant les positions et les vitesses des missiles
  int missile_x[MAX_MISSILES];
  int missile_y[MAX_MISSILES];
  int missile_speed[MAX_MISSILES];
  int missile_active[MAX_MISSILES];

  // Tableaux stockant les positions et les vitesses des bombes
  int bombe_x[MAX_BOMBES];
  int bombe_y[MAX_BOMBES];
  int bombe_speed[MAX_BOMBES];
  int bombe_active[MAX_BOMBES];

  // Tableaux stockant les positions et les vitesses des zepplin
  int ennemy_x[MAX_FLOAT_ENNEMY];
  int ennemy_y[MAX_FLOAT_ENNEMY];
  int ennemy_speed[MAX_FLOAT_ENNEMY];
  int ennemy_active[MAX_FLOAT_ENNEMY];

  // Initialisation des tableaux de missiles
  for (int i = 0; i < MAX_MISSILES; i++) {
    missile_x[i] = -100; // position initiale en dehors de l'ecran
    missile_y[i] = -100; // position initiale en dehors de l'ecran
    missile_speed[i] = 8; // vitesse des missiles
    missile_active[i] = 0; // les missiles ne sont pas actifs au dÈbut
  }

  // Initialisation des tableaux de bombes
  for (int i = 0; i < MAX_BOMBES; i++) {
    bombe_x[i] = -100; // position initiale en dehors de l'ecran
    bombe_y[i] = -100; // position initiale en dehors de l'ecran
    bombe_speed[i] = 3; // vitesse des bombes
    bombe_active[i] = 0; // les bombes ne sont pas actifs au dÈbut
  }

  // Initialisation des tableaux d'ennemy
  for (int i = 0; i < MAX_FLOAT_ENNEMY; i++) {
    ennemy_x[i] = -100; // position initiale en dehors de l'ecran
    ennemy_y[i] = -100; // position initiale en dehors de l'ecran
    ennemy_speed[i] = 3; // vitesse des ennemy
    ennemy_active[i] = 0; // ennemy non actif initialement
  }

  // Boucle principale
  int quit = 0;
  while (quit == 0) {

    // Gestion des évenements
    SDL_Event event;
    while (SDL_PollEvent( & event)) {
      if (event.type == SDL_QUIT) {
        quit = 1;
      } else if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
        case SDLK_UP:

          pitch_angle = 2;
          break;
        case SDLK_DOWN:

          pitch_angle = 0;
          break;
        case SDLK_LEFT:

          pitch_angle = -1;
          break;
        case SDLK_RIGHT:

          pitch_angle = 1;

          break;
        case SDLK_SPACE:

          // Trouver un missile inactif dans le tableau
          for (int i = 0; i < MAX_MISSILES; i++) {
            if (!missile_active[i]) {
              // Activer le missile
              missile_active[i] = 1;
              missile_x[i] = sprite_x + 100; // position horizontale initiale
              missile_y[i] = sprite_y + 100; // position verticale initiale
              break;
            }
          }
          break;
        case SDLK_c:

          // Trouver une bombe inactive dans le tableau
          for (int i = 0; i < MAX_BOMBES; i++) {
            if (!bombe_active[i]) {
              // Activer la bombe
              bombe_active[i] = 1;
              bombe_x[i] = sprite_x + 100; // position horizontale initiale
              bombe_y[i] = sprite_y + 100; // position verticale initiale
              break;
            }
          }
          break;
        }
      }
    }
    if (timer(6) == 5) {
      for (int i = 0; i < MAX_FLOAT_ENNEMY; i++) {
        if (ennemy_queue == 1) {
          if (!ennemy_active[i]) {
            //Activer l'ennemy
            ennemy_active[i] = 1;
            ennemy_x[i] = 1920;
            ennemy_y[i] = 200;
            if (ennemy_active[i] == 1) {
              ennemy_queue = 0;
            }
          }
        }
      }
    }

    if (timer(6) == 4) { // Reset de la variable d'autorisation d'apparition d'ennemy (evite qu'ils soient tous envoyé d'un coup)
      ennemy_queue = 1;
    }

    // Deplacement des missiles actifs
    for (int i = 0; i < MAX_MISSILES; i++) {
      if (missile_active[i]) {
        missile_x[i] += missile_speed[i]; // Deplacement horizontal
        // Verifier si le missile sort de l'ecran
        if (missile_x[i] > 1920) {
          missile_active[i] = 0; // désactiver le missile
          missile_x[i] = -100; // position initiale en dehors de l'ecran
          missile_y[i] = -100; // position initiale en dehors de l'ecran
        }
      }
    }

    // Deplacement des bombes actives
    for (int i = 0; i < MAX_BOMBES; i++) {
      if (bombe_active[i]) {
        bombe_y[i] += bombe_speed[i]; // DÈplacement hvertical
        // VÈrifier si la bombe sort de l'Ècran
        if (bombe_y[i] >= 1080) {
          bombe_active[i] = 0; // désactiver la bombe
          bombe_x[i] = -100; // position initiale en dehors de l'ecran
          bombe_y[i] = -100; // position initiale en dehors de l'ecran
        }
      }
    }

    // Deplacement des zepplin actifs
    for (int i = 0; i < MAX_FLOAT_ENNEMY; i++) {
      if (ennemy_active[i]) {
        ennemy_x[i] -= ennemy_speed[i]; // déplacvement horizontal
        //Verification si l'ennemly sort de l'écran
        if (ennemy_x[i] <= -200) {
          ennemy_active[i] = 0; //désactiver l'ennemi
          ennemy_x[i] = -100; // position initiale en dehors de l'ecran
          ennemy_y[i] = -100; // position initiale en dehors de l'ecran
        }

      }
    }

    // Effacement de l'Ècran
    SDL_RenderClear(renderer);

    // Affichage de la map_background
    SDL_Rect map_background_rect = {
      0,
      0,
      1920,
      1080
    };
    SDL_RenderCopy(renderer, map_background_texture, NULL, & map_background_rect);
    SDL_Rect map_layer_one_rect = {
      0,
      0,
      1920,
      1080
    };
    SDL_RenderCopy(renderer, map_layer_one_texture, NULL, & map_layer_one_rect);

    //Affichage de la console debug
    debug(renderer, sprite_x, sprite_y, timer(6), ennemy_queue, ennemy_active[1], ennemy_active[2]);

    // Affichage du sprite
    if (pitch_angle == 0) {

      SDL_Rect sprite_rect = {
        sprite_x,
        sprite_y,
        200,
        200
      };
      SDL_RenderCopy(renderer, sprite_texture, NULL, & sprite_rect);
      sprite_y += 4;

    }

    if (pitch_angle == 2) {

      SDL_Rect sprite_rect = {
        sprite_x,
        sprite_y,
        200,
        200
      };
      SDL_RenderCopy(renderer, sprite_texture, NULL, & sprite_rect);
      sprite_y -= 3;

    }

    if (pitch_angle == -1) {

      SDL_Rect sprite_rect = {
        sprite_x,
        sprite_y,
        200,
        200
      };
      SDL_RenderCopy(renderer, sprite_backward_texture, NULL, & sprite_rect);
      sprite_x -= 5;

    }
    if (pitch_angle == 1) {

      SDL_Rect sprite_rect = {
        sprite_x,
        sprite_y,
        200,
        200
      };
      SDL_RenderCopy(renderer, sprite_forward_texture, NULL, & sprite_rect);
      sprite_x += 5;
    }

    // Affichage des missiles actifs
    for (int i = 0; i < MAX_MISSILES; i++) {
      if (missile_active[i]) {
        SDL_Rect missile_rect = {
          missile_x[i],
          missile_y[i],
          100,
          50
        };
        SDL_RenderCopy(renderer, missile_texture, NULL, & missile_rect);
      }
    }

    // Affichage des bombes actives
    for (int i = 0; i < MAX_BOMBES; i++) {
      if (bombe_active[i]) {
        SDL_Rect bombe_rect = {
          bombe_x[i],
          bombe_y[i],
          50,
          50
        };
        SDL_RenderCopy(renderer, bombe_texture, NULL, & bombe_rect);
      }
    }

    // Affichage des zepplin actifs
    for (int i = 0; i < MAX_FLOAT_ENNEMY; i++) {
      if (ennemy_active[i]) {
        SDL_Rect zepplin_rect = {
          ennemy_x[i],
          ennemy_y[i],
          150,
          75
        };
        SDL_RenderCopy(renderer, zepplin_texture, NULL, & zepplin_rect);
      }
    }

    //Fonction de limite de déplacement
    if (sprite_x >= 1820) {
      sprite_x = 1820;
    }
    if (sprite_x <= 0) {
      sprite_x = 0;
    }
    if (sprite_y >= 980) {
      sprite_y = 980;
    }
    if (sprite_y <= 0) {
      sprite_y = 0;
    }

    // Actualisation de l'Ècran
    SDL_RenderPresent(renderer);

    // Attente de 16 ms pour avoir environ 60 images par seconde
    SDL_Delay(16);
  }

}
