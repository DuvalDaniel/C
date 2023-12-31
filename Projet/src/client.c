/*
 * SPDX-FileCopyrightText: 2021 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "client.h"
#include "bmp.h"

/*
 * Fonction d'envoi et de réception de messages
 * Il faut un argument : l'identifiant de la socket
 */

int envoie_recois_message(int socketfd, char contenu=0)
{

  char data[1024];
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));
  // Demandez à l'utilisateur d'entrer un message
  char message[1024];
  printf("Votre message (max 1000 caracteres): ");
  fgets(message, sizeof(message), stdin);
  strcpy(data, "message: ");
  strcat(data, message);

  int write_status = write(socketfd, data, strlen(data));
  if (write_status < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // lire les données de la socket
  int read_status = read(socketfd, data, sizeof(data));
  if (read_status < 0)
  {
    perror("erreur lecture");
    return -1;
  }

  printf("Message recu: %s\n", data);

  return 0;
}

int envoie_recois_name(int socketfd)
{

  char data[1024];
  strcpy(data,"nom");

  char hostname[256];
  gethostname(hostname, sizeof(hostname));

  int write_status = write(socketfd, data, 1024);
  if (write_status < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  int write_status2 = write(socketfd, hostname, 256);
  if (write_status2 < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));
  memset(hostname, 0, sizeof(hostname));

  // lire les données de la socket
  int read_status = read(socketfd, data, sizeof(data));
  if (read_status < 0)
  {
    perror("erreur lecture");
    return -1;
  }

  printf("Message recu: %s\n", data);

  return 0;
}

int envoie_operateur_numeros(int socketfd, char** argv){
  char data[1024];
  strcpy(data,argv[1]);

  char operande[256];
  strcpy(operande,argv[3]);

  char numero1[256];
  strcpy(numero1,argv[4]);

  char numero2[256];
  strcpy(numero2,argv[5]);

  int write_status = write(socketfd, data, 1024);
  if (write_status < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  int write_status2 = write(socketfd, operande, 256);
  if (write_status2 < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }
  int write_status3 = write(socketfd, numero1, 256);
  if (write_status3 < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  int write_status4 = write(socketfd, numero2, 256);
  if (write_status4 < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));
  memset(operande, 0, sizeof(operande));
  memset(numero1, 0, sizeof(numero1));
  memset(numero2, 0, sizeof(numero2));

  char reponse[256];

  // lire les données de la socket
  int read_status = read(socketfd, reponse, sizeof(reponse));
  if (read_status < 0)
  {
    perror("erreur lecture");
    return -1;
  }

  printf("Resultat: %s\n", reponse);

  return 0;
}

void analyse(char *pathname, char data[][10], char nb_couleurs)
{
  // compte de couleurs
  couleur_compteur *cc = analyse_bmp_image(pathname);

  int count;
  //strcpy(data, "couleurs: ");
  char temp_string[10];
  // if (cc->size < 10)
  // {
  //   sprintf(temp_string, "%d,", cc->size);
  // }
  // strcat(data, temp_string);

  // choisir 10 couleurs
  for (count = 0; count <= nb_couleurs && cc->size - count > 0; count++)
  {
    if (cc->compte_bit == BITS32)
    {
      sprintf(temp_string, "#%02x%02x%02x", cc->cc.cc24[cc->size - count].c.rouge, cc->cc.cc32[cc->size - count].c.vert, cc->cc.cc32[cc->size - count].c.bleu);
    }
    if (cc->compte_bit == BITS24)
    {
      sprintf(temp_string, "#%02x%02x%02x", cc->cc.cc32[cc->size - count].c.rouge, cc->cc.cc32[cc->size - count].c.vert, cc->cc.cc32[cc->size - count].c.bleu);
    }
    strcpy(data[count], temp_string);
  }

  // enlever le dernier virgule
  //data[strlen(data) - 1] = '\0';
}

int envoie_couleurs1(int socketfd, char *argv[]){
  int nb_couleurs;
  nb_couleurs = atoi(argv[3]);
  if(nb_couleurs > 30){
    perror("trop de balises");
    exit(EXIT_FAILURE);
  }
  char tableau_couleurs[nb_couleurs][10];

  for (int i = 4; i< nb_couleurs+4; ++i){
    if(i == nb_couleurs+3){
      strcpy(tableau_couleurs[i-4], argv[i]);
    }else{
    size_t longueur = strlen(argv[i]);
    strcpy(tableau_couleurs[i-4], argv[i]);
    tableau_couleurs[i-4][longueur - 1] = '\0';
    }
  }

  //envoie des sockets

  char data[1024];
  strcpy(data,argv[1]);

  int write_status2 = write(socketfd, data, 1024);
  if (write_status2 < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }
  
  int write_status = write(socketfd, &nb_couleurs, sizeof(int));
  if (write_status < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }


  int write_status3 = write(socketfd, tableau_couleurs, sizeof(tableau_couleurs));
  if (write_status3 < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  //reponse du serveur

  char reponse[256];

  // lire les données de la socket
  int read_status = read(socketfd, reponse, sizeof(reponse));
  if (read_status < 0)
  {
    perror("erreur lecture");
    return -1;
  }

  printf("Couleurs enregistrés dans le fichier : %s\n", reponse);

  return 0;

}

int envoie_balises(int socketfd, char *argv[]){
  int nb_balises;
  nb_balises = atoi(argv[3]);
  if(nb_balises > 30){
    perror("trop de couleurs");
    exit(EXIT_FAILURE);
  }
  char tableau_couleurs[nb_balises][64];

  for (int i = 4; i< nb_balises+4; ++i){
    if(i == nb_balises+3){
      strcpy(tableau_couleurs[i-4], argv[i]);
    }else{
    size_t longueur = strlen(argv[i]);
    strcpy(tableau_couleurs[i-4], argv[i]);
    tableau_couleurs[i-4][longueur - 1] = '\0';
    }
  }

  //envoie des sockets

  char data[1024];
  strcpy(data,argv[1]);

  int write_status2 = write(socketfd, data, 1024);
  if (write_status2 < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }
  
  int write_status = write(socketfd, &nb_balises, sizeof(int));
  if (write_status < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }


  int write_status3 = write(socketfd, tableau_couleurs, sizeof(tableau_couleurs));
  if (write_status3 < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  //reponse du serveur

  char reponse[256];

  // lire les données de la socket
  int read_status = read(socketfd, reponse, sizeof(reponse));
  if (read_status < 0)
  {
    perror("erreur lecture");
    return -1;
  }

  printf("Balises enregistrés dans le fichier : %s\n", reponse);

  return 0;

}

int json(int socketfd){
       // Déclaration des variables
    char input[500];
    char codeValue[50];
    char valeursValue[50];

    // Lecture de l'entrée depuis la console avec fgets
    printf("Veuillez entrer un JSON : ");
    if (fgets(input, sizeof(input), stdin) == NULL) {
        fprintf(stderr, "Erreur lors de la lecture de l'entrée.\n");
        return 1;
    }

    // Utilisation de sscanf pour extraire les valeurs de "code" et "valeurs"
    if (sscanf(input, "{\"code\":\"%49[^\"]\",\"valeurs\":[\"%49[^\"]]}", codeValue, valeursValue) != 2) {
        fprintf(stderr, "Erreur lors de l'extraction des valeurs de 'code' et 'valeurs'.\n");
        return 1;
    }

    // Affichage des valeurs extraites
    printf("Valeur de 'code' : %s\n", codeValue);
    printf("Valeur de 'valeurs' : %s\n", valeursValue);

    // char command[1024]

    // sprintf()

    if(strcmp(codeValue, "message")==0){
      envoie_recois_message(socketfd);
      //system(valeursValue);
    }

    return 0;
}

int envoie_couleurs(int socketfd, char *argv[])
{
  int taille = atoi(argv[2]);
  //on verifie si on nous demande pas une taille superieur a 30
  if(taille > 30){
    perror("taille trop grande");
    exit(EXIT_FAILURE);
  }

  char data[taille][10];
  memset(data, 0, sizeof(data));
  analyse(argv[1], data, taille);

  char data1[1024];
  strcpy(data1,"couleurs");

  int write_status = write(socketfd, data1, 1024);
  if (write_status < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  int write_status1 = write(socketfd, &taille, sizeof(int));
  if (write_status1 < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  int write_status2 = write(socketfd, data, sizeof(data));
  if (write_status2 < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  return 0;
}

int main(int argc, char **argv)
{
  int socketfd;

  struct sockaddr_in server_addr;

  /*if (argc < 2)
  {
    printf("usage: ./client chemin_bmp_image\n");
    return (EXIT_FAILURE);
  }*/

  /*
   * Creation d'une socket
   */
  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketfd < 0)
  {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  // détails du serveur (adresse et port)
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  // demande de connection au serveur
  int connect_status = connect(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (connect_status < 0)
  {
    perror("connection serveur");
    exit(EXIT_FAILURE);
  }
  if (strcmp(argv[1],"message")==0)
  {
    // envoyer et recevoir un message
    envoie_recois_message(socketfd);
  }else if(strcmp(argv[1],"nom")==0){
    envoie_recois_name(socketfd);
  }else if(strcmp(argv[1],"calcul")==0){
    envoie_operateur_numeros(socketfd, argv);
  }else if(strcmp(argv[1],"couleurs")==0){
    envoie_couleurs1(socketfd, argv);
  }else if(strcmp(argv[1],"balises")==0){
    envoie_balises(socketfd, argv);
  }else if(strcmp(argv[1],"JSON")==0){
    json(socketfd);
  }else{
    // envoyer et recevoir les couleurs prédominantes
    // d'une image au format BMP (argv[1])
    envoie_couleurs(socketfd, argv);
  }

  close(socketfd);
}
