/*
 * SPDX-FileCopyrightText: 2021 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <math.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "serveur.h"
int socketfd;

int visualize_plot()
{
  const char *browser = "firefox";

  char command[256];
  snprintf(command, sizeof(command), "%s %s", browser, svg_file_path);

  int result = system(command);

  if (result == 0)
  {
    printf("SVG file opened in %s.\n", browser);
  }
  else
  {
    printf("Failed to open the SVG file.\n");
  }

  return 0;
}

double degreesToRadians(double degrees)
{
  return degrees * M_PI / 180.0;
}

int plot(char *data)
{
  int i;
  char *saveptr = NULL;
  char *str = data;
  char *token = strtok_r(str, ",", &saveptr);
  const int num_colors = 10;

  double angles[num_colors];
  memset(angles, 0, sizeof(angles));

  FILE *svg_file = fopen(svg_file_path, "w");
  if (svg_file == NULL)
  {
    perror("Error opening file");
    return 1;
  }

  fprintf(svg_file, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n");
  fprintf(svg_file, "<svg width=\"400\" height=\"400\" xmlns=\"http://www.w3.org/2000/svg\">\n");
  fprintf(svg_file, "  <rect width=\"100%%\" height=\"100%%\" fill=\"#ffffff\" />\n");

  double center_x = 200.0;
  double center_y = 200.0;
  double radius = 150.0;

  double start_angle = -90.0;

  str = NULL;
  i = 0;
  while (1)
  {
    token = strtok_r(str, ",", &saveptr);
    if (token == NULL)
    {
      break;
    }
    str = NULL;
    angles[i] = 360.0 / num_colors;

    double end_angle = start_angle + angles[i];

    double start_angle_rad = degreesToRadians(start_angle);
    double end_angle_rad = degreesToRadians(end_angle);

    double x1 = center_x + radius * cos(start_angle_rad);
    double y1 = center_y + radius * sin(start_angle_rad);
    double x2 = center_x + radius * cos(end_angle_rad);
    double y2 = center_y + radius * sin(end_angle_rad);

    fprintf(svg_file, "  <path d=\"M%.2f,%.2f A%.2f,%.2f 0 0,1 %.2f,%.2f L%.2f,%.2f Z\" fill=\"%s\" />\n",
            x1, y1, radius, radius, x2, y2, center_x, center_y, token);

    start_angle = end_angle;
    i++;
  }

  fprintf(svg_file, "</svg>\n");

  fclose(svg_file);

  visualize_plot();
  return 0;
}

/* renvoyer un message (*data) au client (client_socket_fd)
 */
int renvoie_message(int client_socket_fd, char *data)
{
  int data_size = write(client_socket_fd, (void *)data, strlen(data));

  if (data_size < 0)
  {
    perror("erreur ecriture");
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}

int renvoie_name(int client_socket_fd, char *data)
{
    char hostname[256];

    // lecture de données envoyées par un client
    int data_size = read(client_socket_fd, hostname, sizeof(hostname));

    if (data_size < 0)
    {
      perror("erreur lecture");
      return (EXIT_FAILURE);
    }else{
      int data_size2 = write(client_socket_fd, (void *)hostname, sizeof(hostname));
      if (data_size2 < 0)
    {
      perror("erreur lecture");
      return (EXIT_FAILURE);
    }
    }
  memset(hostname, 0, sizeof(hostname));
  return (EXIT_SUCCESS);
}

int recois_numeros_calcule(int client_socket_fd, char *data){
  char operande[256];

  char numero1[256];
  char numero2[256];

  char resultat[256];

  float resulatFloat;

  // lecture de données envoyées par un client
    int data_size = read(client_socket_fd, operande, sizeof(operande));
    if (data_size < 0)
    {
      perror("erreur lecture");
      return (EXIT_FAILURE);
    }else{
      int data_size2 = read(client_socket_fd, numero1, sizeof(numero1));
      if (data_size2 < 0)
      {
        perror("erreur lecture");
        return (EXIT_FAILURE);
      }else{
        int data_size3 = read(client_socket_fd, numero2, sizeof(numero2));
        if (data_size3 < 0)
        {
          perror("erreur lecture");
          return (EXIT_FAILURE);
        }else{
          if(strcmp(operande, "+") == 0){
            
            resulatFloat = atof(numero1) + atof(numero2);
            int datat_float = snprintf(resultat, sizeof(resultat), "%.2f", resulatFloat); 
            if (datat_float < 0)
            {
              perror("erreur lecture");
              return (EXIT_FAILURE);
            }
            int data_size11 = write(client_socket_fd, resultat, sizeof(resultat));
            if (data_size11 < 0)
            {
              perror("erreur lecture");
              return (EXIT_FAILURE);
            }
          }else if(strcmp(operande, "-") == 0){
            resulatFloat = atof(numero1) - atof(numero2);
            int datat_float = snprintf(resultat, sizeof(resultat), "%.2f", resulatFloat); 
            if (datat_float < 0)
            {
              perror("erreur lecture");
              return (EXIT_FAILURE);
            }

            int data_size12 = write(client_socket_fd, resultat, sizeof(resultat));
            if (data_size12 < 0)
            {
              perror("erreur lecture");
              return (EXIT_FAILURE);
            }
          }else if(strcmp(operande, "*") == 0){
            //ATTENTION si on veut faire une multiplication on doit mettre * entre guillemets ("*") exemple : ./client calcul : "*" 3 3
            resulatFloat = atof(numero1) * atof(numero2);
            int datat_float = snprintf(resultat, sizeof(resultat), "%.2f", resulatFloat); 
            if (datat_float < 0)
            {
              perror("erreur lecture");
              return (EXIT_FAILURE);
            }

            int data_size13 = write(client_socket_fd, resultat, sizeof(resultat));
            if (data_size13 < 0)
            {
              perror("erreur lecture");
              return (EXIT_FAILURE);
            }
          }else if(strcmp(operande, "/") == 0){
            if(atof(numero2)==0){
              strcpy(resultat, "Impossible de diviser un chiffre par zero");
              int data_size14 = write(client_socket_fd, resultat, sizeof(resultat));
              if (data_size14 < 0)
              {
                perror("erreur lecture");
                return (EXIT_FAILURE);
              }
            }
            resulatFloat = atof(numero1) / atof(numero2);
            int datat_float = snprintf(resultat, sizeof(resultat), "%.2f", resulatFloat); 
            if (datat_float < 0)
            {
              perror("erreur lecture");
              return (EXIT_FAILURE);
            }

            int data_size14 = write(client_socket_fd, resultat, sizeof(resultat));
            if (data_size14 < 0)
            {
              perror("erreur lecture");
              return (EXIT_FAILURE);
            }
          }else{

          }



        }
      }
    }

    memset(operande, 0, sizeof(operande));
    memset(numero1, 0, sizeof(numero1));
    memset(numero2, 0, sizeof(numero2));
    return (EXIT_SUCCESS);

}

int recois_couleurs(int client_socket_fd, char data[1024]){

  int nb_couleurs;

  int data_size = read(client_socket_fd, &nb_couleurs, sizeof(int));
    if (data_size < 0)
    {
      perror("erreur lecture");
      return (EXIT_FAILURE);
    }
  char tableau_couleurs[nb_couleurs][10];

  int data_size2 = read(client_socket_fd, tableau_couleurs, sizeof(tableau_couleurs));
    if (data_size2 < 0)
    {
      perror("erreur lecture");
      return (EXIT_FAILURE);
    }
  FILE *fichierw = fopen("couleurs.txt", "w");

    if (fichierw == NULL) {
        fprintf(stderr, "Impossible d'ouvrir le fichier.\n");
        return 1;
    }

    // Ferme le fichier après l'avoir ouvert, ce qui efface son contenu
  fclose(fichierw);

  FILE *fichiera = fopen("couleurs.txt", "a");

    if (fichiera == NULL) {
        fprintf(stderr, "Impossible d'ouvrir le fichier.\n");
        return 1;
    }

    // Ajoute du contenu à la fin du fichier sans écraser
  for (int i = 0; i < nb_couleurs; ++i) {
    fprintf(fichiera,"%s\n", tableau_couleurs[i]);
  }

  fclose(fichiera);

  char nom_fichier[256] = "couleurs.txt";
  int data_size1 = write(client_socket_fd, nom_fichier, sizeof(nom_fichier));
    if (data_size1 < 0)
    {
      perror("erreur lecture");
      return (EXIT_FAILURE);
    }  

  return 0;
}

int recois_balises(int client_socket_fd, char data[1024]){

  int nb_balises;


  int data_size = read(client_socket_fd, &nb_balises, sizeof(int));
    if (data_size < 0)
    {
      perror("erreur lecture");
      return (EXIT_FAILURE);
    }
  
  char tableau_couleurs[nb_balises][64];

  int data_size2 = read(client_socket_fd, tableau_couleurs, sizeof(tableau_couleurs));
    if (data_size2 < 0)
    {
      perror("erreur lecture");
      return (EXIT_FAILURE);
    }

  FILE *fichierw = fopen("balises.txt", "w");

    if (fichierw == NULL) {
        fprintf(stderr, "Impossible d'ouvrir le fichier.\n");
        return 1;
    }

    // Ferme le fichier après l'avoir ouvert, ce qui efface son contenu
  fclose(fichierw);

  FILE *fichiera = fopen("balises.txt", "a");

    if (fichiera == NULL) {
        fprintf(stderr, "Impossible d'ouvrir le fichier.\n");
        return 1;
    }

    // Ajoute du contenu à la fin du fichier sans écraser
  for (int i = 0; i < nb_balises; ++i) {
    fprintf(fichiera,"%s\n", tableau_couleurs[i]);
  }

  fclose(fichiera);

  char nom_fichier[256] = "balises.txt";
  int data_size1 = write(client_socket_fd, nom_fichier, sizeof(nom_fichier));
    if (data_size1 < 0)
    {
      perror("erreur lecture");
      return (EXIT_FAILURE);
    }  

  return 0;
}


/* accepter la nouvelle connection d'un client et lire les données
 * envoyées par le client. En suite, le serveur envoie un message
 * en retour
 */
int recois_envoie_message(int client_socket_fd, char data[1024])
{
  /*
   * extraire le code des données envoyées par le client.
   * Les données envoyées par le client peuvent commencer par le mot "message :" ou un autre mot.
   */
  printf("Message recu: %s\n", data);
  char code[10];
  sscanf(data, "%s", code);

  // Si le message commence par le mot: 'message:'
  if (strcmp(code, "message:") == 0)
  {
    renvoie_message(client_socket_fd, data);
  }
  else
  {
    plot(data);
  }

  return (EXIT_SUCCESS);
}

// Fonction de gestion du signal Ctrl+C
void gestionnaire_ctrl_c(int signal)
{
  printf("\nSignal Ctrl+C capturé. Sortie du programme.\n");
  // fermer le socket
  close(socketfd);
  exit(0); // Quitter proprement le programme.
}

int main()
{
  int bind_status;

  struct sockaddr_in server_addr;

  /*
   * Creation d'une socket
   */
  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketfd < 0)
  {
    perror("Unable to open a socket");
    return -1;
  }

  int option = 1;
  setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

  // détails du serveur (adresse et port)
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  // Relier l'adresse à la socket
  bind_status = bind(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (bind_status < 0)
  {
    perror("bind");
    return (EXIT_FAILURE);
  }

  // Enregistrez la fonction de gestion du signal Ctrl+C
  signal(SIGINT, gestionnaire_ctrl_c);

  // Écouter les messages envoyés par le client en boucle infinie
  while (1)
  {
    // Écouter les messages envoyés par le client
    listen(socketfd, 10);

    // Lire et répondre au client
    struct sockaddr_in client_addr;
    char data[1024];

    unsigned int client_addr_len = sizeof(client_addr);

    // nouvelle connection de client
    int client_socket_fd = accept(socketfd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_socket_fd < 0)
    {
      perror("accept");
      return (EXIT_FAILURE);
    }

    // la réinitialisation de l'ensemble des données
    memset(data, 0, sizeof(data));

    // lecture de données envoyées par un client
    int data_size = read(client_socket_fd, (void *)data, sizeof(data));

    if (data_size < 0)
    {
      perror("erreur lecture");
      return (EXIT_FAILURE);
    }
    if(strcmp(data,"nom")==0){
    renvoie_name(client_socket_fd, data);
    }else if(strcmp(data,"calcul")==0){
    recois_numeros_calcule(client_socket_fd, data);
    }else if(strcmp(data,"couleurs")==0){
    recois_couleurs(client_socket_fd, data);
    }else if(strcmp(data,"balises")==0){
    recois_balises(client_socket_fd, data);
    }else{
    recois_envoie_message(client_socket_fd, data);
    }

  }

  return 0;
}
