#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include<time.h>
#define DEFAULT_SIZE 1024

struct giocata {
    int numeri[10];
    char ruote[11][11];
    float importi[5];
    int quanti_imp;
    int quante_ruote;
    int quanti_num;
};

int fact(int z) {
    int f = 1, i;
    if (z == 0) {
        return (f);
    } else {
        for (i = 1; i <= z; i++) {
            f = f * i;
        }
    }
    return (f);
}

void estrazioni() {
    FILE* file_estrazioni;
    int i;
    int j;



    int ruota = 0;
    int array[5];
    char ruote[11][1024] = {"bari", "cagliari", "firenze", "genova", "napoli", "milano", "palermo", "roma", "torino", "venezia", "nazionale"};
    file_estrazioni = fopen("estrazioni.txt", "a");
    fclose(file_estrazioni);
    while (ruota < 11) {
        i = 0;

        while (i != 5) {
            bool good = true;
            int number = rand() % 90 + 1;
            for (j = 0; j < i; j++) {

                if (array[j] == number) {
                    good = false;
                    break;
                }
            }
            if (good) {

                array[i] = number;

                i = i + 1;
            }
        }

        file_estrazioni = fopen("estrazioni.txt", "a"); //file in cui memorizzo tutte le estrazioni
        fprintf(file_estrazioni, "%s %d %d %d %d %d\n", ruote[ruota], array[0], array[1], array[2], array[3], array[4]);
        fclose(file_estrazioni);
        if (ruota == 0)
            file_estrazioni = fopen("ultimaestrazione.txt", "w"); //file in cui memorizzo solo l'ultima etsrazione per effettuare controllo vincite
        else
            file_estrazioni = fopen("ultimaestrazione.txt", "a");
        fprintf(file_estrazioni, "%s %d %d %d %d %d\n", ruote[ruota], array[0], array[1], array[2], array[3], array[4]);
        fclose(file_estrazioni);
        ruota++;
    }

}

void azzera_attive() { //funzione che dopo salva le giocate attive in passate e azzera attive, per ogni utente accedo alle giocate attive, le copio in passate e svuoto il primo file
    FILE* fd_lista_ut; //fd per accedere alla lista utenti reg
    FILE* fd_g_passate; //fd per accedere alle giocate passate di un utente
    FILE* fd_g_attive; //fd per accedere alle giocate attive di un utente
    char user[DEFAULT_SIZE]; //var per memorizzare utente letto da lista registrati
    char buffer[DEFAULT_SIZE]; //buffer per appoggio e memorizzazione
    fd_lista_ut = fopen("utentiregistrati.txt", "r");
    while (fgets(buffer, DEFAULT_SIZE, fd_lista_ut) != NULL) {
        sscanf(buffer, "%s", user);
        strcpy(buffer, user);
        strcat(buffer, "giocateattive.txt");
        fd_g_attive = fopen(buffer, "r");
        strcpy(buffer, user);
        strcat(buffer, "giocatepassate.txt");
        fd_g_passate = fopen(buffer, "a");
        while (fgets(buffer, DEFAULT_SIZE, fd_g_attive) != NULL) {
            fprintf(fd_g_passate, "%s", buffer);
        }
        fclose(fd_g_passate);
        fclose(fd_g_attive);
        strcpy(buffer, user);
        strcat(buffer, "giocateattive.txt");

        fd_g_attive = fopen(buffer, "w");

        fclose(fd_g_attive);
    }
    fclose(fd_lista_ut);

}

void check_vincite(char* s) {//per ogni utente, prelevo ogni giocata e controllo se c'è vincita, se sì salvo l'importo
    FILE* fd_lista_ut; //fd per accedere alla lista utenti reg
    FILE* fd_ult_estr; //fd per accedere a ultime estrazioni
    FILE* fd_g_attive; //fd per accedere alle giocate attive
    FILE* fd_vincite; //fd per scrivere la vincita
    FILE* fd_consuntivo; //fd per scrivere il consuntivo
    struct giocata giocata_lett; //strutt per memorizzare giocata utente letta
    char ruota[10]; //var per memorizzare la ruota di ult estr su cui sto facendo il controllo
    int num_estratti[5]; //var per memprizzare i numeri estratti di ruota[10]
    char user[DEFAULT_SIZE]; //var per memorizzare utente letto da lista registrati
    char buffer[DEFAULT_SIZE]; //buffer per appoggio e memorizzazione

    fd_lista_ut = fopen("utentiregistrati.txt", "r");
    while (fgets(buffer, DEFAULT_SIZE, fd_lista_ut) != NULL) {
        float estratto_tot = 0; //variabili calcolo vincita totale per salvare nel consuntivo
        float ambo_tot = 0;
        float terno_tot = 0;
        float quat_tot = 0;
        float cinquina_tot = 0;
        int res;
        int i = 0;
        bool timestamp = false; //flag per memorizzare il timestamp della vincita
        sscanf(buffer, "%s", user);
        memset(buffer, 0, DEFAULT_SIZE);
        strcpy(buffer, user);
        strcat(buffer, "giocateattive.txt");
        fd_g_attive = fopen(buffer, "r");
        while (!feof(fd_g_attive)) {

            i = 0;
            memset(buffer, 0, DEFAULT_SIZE);

            char nuovalinea; //var che mi consente di spostare il puntatore del descr sulla nuova linea
            memset(giocata_lett.numeri, 0, (sizeof (int)*11));
            memset(giocata_lett.ruote, 0, (sizeof (char)*11 * 11));
            memset(giocata_lett.importi, 0, (sizeof (float)*5));
            giocata_lett.quante_ruote = 0;
            giocata_lett.quanti_imp = 0;
            giocata_lett.quanti_num = 0;

            res = fscanf(fd_g_attive, "%d", &giocata_lett.quante_ruote);

            if (res == EOF) {
                //printf("Non riesco a leggere un intero: il file e' finito\n");
                break;
            }

            while (i < giocata_lett.quante_ruote) {
                fscanf(fd_g_attive, "%s", giocata_lett.ruote[i]);
                if (strcmp(giocata_lett.ruote[i], "tutte") == 0)
                    break;
                i++;
            }

            i = 0;
            fscanf(fd_g_attive, "%d", &giocata_lett.quanti_num);

            while (i < giocata_lett.quanti_num) {
                fscanf(fd_g_attive, "%d", &giocata_lett.numeri[i]);
                i++;
            }

            i = 0;
            fscanf(fd_g_attive, "%d", &giocata_lett.quanti_imp);
            while (i < giocata_lett.quanti_imp) {
                fscanf(fd_g_attive, "%f", &giocata_lett.importi[i]);

                i++;

            }

            fscanf(fd_g_attive, "%c", &nuovalinea);
            float vincita_estratto = 0; //valori per calcolare la vincita sulla singola giocata
            float vincita_ambo = 0;
            float vincita_terno = 0;
            float vincita_quat = 0;
            float vincita_cinquina = 0;
            int k = 0;


            vincita_estratto = (11.23 * giocata_lett.importi[0]) / (giocata_lett.quanti_num * giocata_lett.quante_ruote);


            k = (fact(giocata_lett.quanti_num) / (fact(2) * fact(giocata_lett.quanti_num - 2))); //numerodiambi

            if (k != 0)
                vincita_ambo = (250.0 / (k * giocata_lett.quante_ruote))*(giocata_lett.importi[1] / k);

            k = (fact(giocata_lett.quanti_num) / (fact(3) * fact(giocata_lett.quanti_num - 3))); //numeroditerni
            if (k != 0)
                vincita_terno = (4500.0 / (k * giocata_lett.quante_ruote))*(giocata_lett.importi[2] / k);

            k = (fact(giocata_lett.quanti_num) / (fact(4) * fact(giocata_lett.quanti_num - 4))); //numerodiquaterne
            if (k != 0)
                vincita_quat = (120000.0 / (k * giocata_lett.quante_ruote))*(giocata_lett.importi[3] / k);

            k = (fact(giocata_lett.quanti_num) / (fact(5) * fact(giocata_lett.quanti_num - 5))); //numerodicinquine
            if (k != 0)
                vincita_cinquina = (6000000.0 / (k * giocata_lett.quante_ruote))*(giocata_lett.importi[4] / k);

            fd_ult_estr = fopen("ultimaestrazione.txt", "r");

            while (fgets(buffer, DEFAULT_SIZE, fd_ult_estr) != NULL) {
                sscanf(buffer, "%s %d %d %d %d %d", ruota, &num_estratti[0], &num_estratti[1], &num_estratti[2], &num_estratti[3], &num_estratti[4]);
				//per ogni giocata, controllo nel file ultime estrazioni se sono stati estratti i numeri giocati
                int j = 0;
                int k = 0;
                int r = 0;

                
                for (r = 0; r < giocata_lett.quante_ruote; r++) {


                    int estratto = 0; //variabili per contare le combinazioni sulla ruota estratta
                    int ambo = 0;
                    int terno = 0;
                    int quaterna = 0;
                    int cinquina = 0;
                    if (strcmp(ruota, giocata_lett.ruote[r]) == 0 || strcmp(giocata_lett.ruote[0], "tutte") == 0) { //se la ruota 
                        //giocata dall'utente è uguale a quella di ult estr o se l'utente ha giocato tutte le ruote allora controllo
                        for (j = 0; j < giocata_lett.quanti_num; j++) {
                            for (k = 0; k < 5; k++) {
                                if (giocata_lett.numeri[j] == num_estratti[k]) {
                                    if (estratto == 0) {
                                        estratto++;
                                        continue;
                                    }
                                    if (estratto != 0 && ambo == 0) {
                                        estratto++;
                                        ambo++;
                                        continue;
                                    }
                                    if (estratto != 0 && ambo != 0 && terno == 0) {
                                        estratto++;
                                        ambo++;
                                        terno++;
                                        continue;
                                    }
                                    if (estratto != 0 && ambo != 0 && terno != 0 && quaterna == 0) {
                                        estratto++;
                                        ambo++;
                                        terno++;
                                        quaterna++;
                                        continue;
                                    }
                                    if (estratto != 0 && ambo != 0 && terno != 0 && quaterna != 0 && cinquina == 0) {
                                        estratto++;
                                        ambo++;
                                        terno++;
                                        quaterna++;
                                        cinquina++;
                                        continue;
                                    }
                                }

                            }
                        }

                        memset(buffer, 0, DEFAULT_SIZE);
                        strcpy(buffer, user);
                        strcat(buffer, "vincite.txt");
                        fd_vincite = fopen(buffer, "a");
                        
                        if ((strcmp(ruota, giocata_lett.ruote[r]) == 0 || strcmp(giocata_lett.ruote[0], "tutte") == 0)
                                && (estratto != 0 || ambo != 0 || terno != 0 || quaterna != 0 || cinquina != 0)) {
							if (timestamp == false) {
								printf("stampo time stamp\n");
                           		 fprintf(fd_vincite, "ESTRAZIONE DEL %s", s);
                            	timestamp = true;
                        }
                            fprintf(fd_vincite, "%s ", ruota);
                            int z = 0;
                            for (z = 0; z < giocata_lett.quanti_num; z++)
                                fprintf(fd_vincite, "%d ", giocata_lett.numeri[z]);
                            if (estratto != 0)
                                fprintf(fd_vincite, "%s %f ", "ESTRATTO", estratto * vincita_estratto); //inserisco nelle vincite numerodiestratti*lavincitasull'estratto
                            if (ambo != 0)
                                fprintf(fd_vincite, "%s %f ", "AMBO ", ambo * vincita_ambo);
                            if (terno != 0)
                                fprintf(fd_vincite, "%s %f ", "TERNO", terno * vincita_terno);
                            if (quaterna != 0)
                                fprintf(fd_vincite, "%s %f ", "QUATERNA", quaterna * vincita_quat);
                            if (cinquina != 0)
                                fprintf(fd_vincite, "%s %f ", "CINQUINA", cinquina * vincita_cinquina);
                            fprintf(fd_vincite, "%c", '\n');
                            

                            estratto_tot += (estratto * vincita_estratto);
                            ambo_tot += (ambo * vincita_ambo);
                            terno_tot += (terno * vincita_terno);
                            quat_tot += (quaterna * vincita_quat);
                            cinquina_tot += (cinquina * vincita_cinquina);
                        }
						fclose(fd_vincite);
                    }
                    if (giocata_lett.quante_ruote == 11)
                        break;
                }

            }
            fclose(fd_ult_estr);
            if (feof(fd_g_attive))
                break;

        }
        fclose(fd_g_attive);
        strcpy(buffer, user);
        strcat(buffer, "consuntivo.txt");

        fd_consuntivo = fopen(buffer, "r");
        float a, b, c, d, e; //variabili per memorizzare vecchio consuntivo

        fscanf(fd_consuntivo, "%f %f %f %f %f", &a, &b, &c, &d, &e); //prelevo
        fclose(fd_consuntivo);
        a += estratto_tot; //aggiorno
        b += ambo_tot;
        c += terno_tot;
        d += quat_tot;
        e += cinquina_tot;
        fd_consuntivo = fopen(buffer, "w");
        fprintf(fd_consuntivo, "%f %f %f %f %f", a, b, c, d, e);

        fclose(fd_consuntivo);
    }
    fclose(fd_lista_ut);
    azzera_attive();
}

int main(int argc, char* argv[]) {
    if(argc <= 1){
		printf( "Errore: Porta non specificata\n");
		exit(-1);
    }
    int tempo_estrz;
    if(argv[2]==NULL)
        tempo_estrz=5;
    else
        tempo_estrz=atoi(argv[2]);
    
    pid_t estrazioni_child, connection_child;
    estrazioni_child = fork();

    if (estrazioni_child == 0) { //figlio che effettua estrazioni
        srand(time(NULL));
        int numero_di_estrazioni;
        FILE* fextr;
        if (fopen("estrazioni.txt", "r") == NULL) {
            printf("creo file estrazioni.txt \n");
            fextr = fopen("estrazioni.txt", "a");
            fclose(fextr); //creo il file se non c'è
        }
        if (fopen("ultimaestrazione.txt", "r") == NULL) {
            printf("creo file ultimaestrazione.txt \n");
            fextr = fopen("ultimaestrazione.txt", "a");
            fclose(fextr); //creo il file se non c'è
        }
        if (fopen("numerodiestrazioni.txt", "r") == NULL) {
            printf("creo file numerodiestrazioni.txt \n");
            fextr = fopen("numerodiestrazioni.txt", "a");
            numero_di_estrazioni = 0;
            fclose(fextr); //creo il file se non c'è
        } else {

            fextr = fopen("numerodiestrazioni.txt", "r");
            fscanf(fextr, "%d", &numero_di_estrazioni);
            fclose(fextr);

        }

        while (1) {
            sleep(tempo_estrz*60);
            printf("ESTRAZIONE IN CORSO\n");
            estrazioni();
            numero_di_estrazioni++;
            fextr = fopen("numerodiestrazioni.txt", "w");
            fprintf(fextr, "%d", numero_di_estrazioni);
            fclose(fextr);
            time_t timestamp_estrzn;
            timestamp_estrzn = time(NULL);

            check_vincite(asctime(localtime(&timestamp_estrzn)));

        }

    } else {
        
        int port;
        port = atoi(argv[1]);
         if(port < 1 || port > 65535){
            printf( "Errore: Porta non valida\n");
            exit(-1);
    }
    

        int ret, sd, new_sd, len_cl_addr, lenOK;
        uint16_t lmsgOK; //lunghezza del messaggio per confermare la connessione o per comunicare se è bloccata
        uint64_t time1, time2, timeBlock; //variabili per gestire il tempo di blocco
        char tempoBlocco[DEFAULT_SIZE];//variabile per prelevare il tempoblocco dal file utenti bloccati.txt
        struct sockaddr_in my_addr, cl_addr;
        char client_addr_string[INET_ADDRSTRLEN];
        char client_addr_check[INET_ADDRSTRLEN];
        char bufferCheckConn[DEFAULT_SIZE]; //buffer in cui memorizzo gli utenti bloccati letti da utentibloccati.txt
        static const char alphanum[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"; //stringa per creare session_ID
        bool blocked_conn;
        FILE* fd;
        FILE* fd2;
        /* Creazione socket */
        sd = socket(AF_INET, SOCK_STREAM, 0);
        /* Creazione indirizzo di bind */
        memset(&my_addr, 0, sizeof (my_addr)); // Pulizia 
        my_addr.sin_family = AF_INET;
        my_addr.sin_port = htons(port);
        my_addr.sin_addr.s_addr = INADDR_ANY;



        if (fopen("utentiregistrati.txt", "r") == NULL) {
            printf("creo file utentiregistrati.txt \n");
            fd = fopen("utentiregistrati.txt", "a");
            fclose(fd); //creo il file se non c'è
        }
        if (fopen("utentibloccati.txt", "r") == NULL) {
            printf("creo file utentibloccati.txt \n");
            fd = fopen("utentibloccati.txt", "a");
            fclose(fd); //creo il file se non c'è
        }
        if (fopen("loggati.txt", "r") == NULL) {
            printf("creo file loggati.txt \n");
            fd = fopen("loggati.txt", "a");
            fclose(fd); //creo il file se non c'è
        }

        ret = bind(sd, (struct sockaddr*) &my_addr, sizeof (my_addr));
        if (ret < 0) {
            perror("Errore in fase di bind: \n");
			close(sd);
            exit(-1);
        }
        ret = listen(sd, 50);
        while (1) {



            len_cl_addr = sizeof (cl_addr);

            // Accetto nuove connessioni
            new_sd = accept(sd, (struct sockaddr*) &cl_addr, &len_cl_addr);
            inet_ntop(AF_INET, &cl_addr, client_addr_string, INET_ADDRSTRLEN); //memorizzo indirizzo client in notazione puntata decimale


            fd = fopen("utentibloccati.txt", "r");


            if (!fgets(bufferCheckConn, DEFAULT_SIZE, fd)) { //controllo se l'utente è bloccato inc aso di file vuoto
                memset(bufferCheckConn, 0, DEFAULT_SIZE);
                strcpy(bufferCheckConn, "CONNESSIONE_OK");

                blocked_conn = false;
                lenOK = strlen(bufferCheckConn) + 1;
                lmsgOK = htons(lenOK);
                ret = send(new_sd, (void*) &lmsgOK, sizeof (uint16_t), 0);

                ret = send(new_sd, (void*) bufferCheckConn, lenOK, 0);
                if (ret < 0) {
                    perror("Errore in fase di send: \n");
                    exit(-1);
                }

            } else {

                fclose(fd);
                fd = fopen("utentibloccati.txt", "r");
                blocked_conn = false;
                memset(bufferCheckConn, 0, DEFAULT_SIZE);
                while (fgets(bufferCheckConn, DEFAULT_SIZE, fd)) {

                    sscanf(bufferCheckConn, "%s %s", client_addr_check, tempoBlocco); //memorizzo utente e tempo blocco

                    time1 = strtoull(tempoBlocco, NULL, 10); //converto
                    time2 = (uint64_t) time(NULL); //ora corrente
                    timeBlock = time2 - time1;

                    if (strcmp(client_addr_string, client_addr_check) == 0 && (int) timeBlock <= 1800) {

                        int quanti = 1800 - (int) timeBlock;
                        blocked_conn = true;
                        strcpy(bufferCheckConn, "CONNESSIONE_BLOCCATA");

                        lenOK = strlen(bufferCheckConn) + 1; //avviso l'utente che connessione è bloccata
                        lmsgOK = htons(lenOK);
                        ret = send(new_sd, (void*) &lmsgOK, sizeof (uint16_t), 0);
                        ret = send(new_sd, (void*) bufferCheckConn, lenOK, 0);
                        if (ret < 0) {
                            perror("Errore in fase di send: \n");
                            exit(-1);
                        }
                        lenOK = sizeof (int); //invio quanto tempo rimane prima dello sblocco
                        lmsgOK = htons(lenOK);
                        ret = send(new_sd, (void*) &lmsgOK, sizeof (uint16_t), 0);
                        ret = send(new_sd, (void*) &quanti, lenOK, 0);
                        if (ret < 0) {
                            perror("Errore in fase di send: \n");
                            exit(-1);
                        }
                        fclose(fd);
                        close(new_sd);

                        break;
                    }

                }

            }
            if (blocked_conn == true)

                continue;
            else {
                fclose(fd);
                fd = fopen("utentibloccati.txt", "r");
                if (fgets(bufferCheckConn, DEFAULT_SIZE, fd) != NULL) { //controllo se il file non è vuoto, perché se è vuoto invio due volte conn ok e non va bene

                    strcpy(bufferCheckConn, "CONNESSIONE_OK"); //invio che connessione è ok

                    lenOK = strlen(bufferCheckConn) + 1;
                    lmsgOK = htons(lenOK);
                    ret = send(new_sd, (void*) &lmsgOK, sizeof (uint16_t), 0);
                    ret = send(new_sd, (void*) bufferCheckConn, lenOK, 0);
                    if (ret < 0) {
                        perror("Errore in fase di send: \n");
                        exit(-1);
                    }
                }
                fclose(fd);
            }

            connection_child = fork();

            if (connection_child == 0) {
                // Sono nel processo figlio che gestisce le connessioni e alloco le variabili per la connessione

                close(sd);
                uint16_t lmsg;
                // srand(time(NULL));
                char buffer[DEFAULT_SIZE];
                char comando[DEFAULT_SIZE];
                char username[DEFAULT_SIZE];
                char password[DEFAULT_SIZE];
                char usernameCheck[DEFAULT_SIZE]; //array per controllo duplicato username e per controllo login, alla fine in questo array sarà memorizzato l'utente loggato
                char passwordCheck[DEFAULT_SIZE]; //array per controllo duplicato password e per controllo login
                char session_ID[10];
                bool accountEsistente;
                int len;
                int tentativi = 0; //variabile per controllare blocco
                time_t my_time; //var per il timestamp
                memset(&session_ID, 0, 10);
                while (1) {

                    memset(buffer, 0, DEFAULT_SIZE); //ricevo comandi dall'utente
                    ret = recv(new_sd, (void*) &lmsg, sizeof (uint16_t), 0);
                    len = ntohs(lmsg);
                    ret = recv(new_sd, (void*) buffer, len, 0);

                    if (ret < 0) {
                        perror("Errore in fase di ricezione: \n");
                        continue;
                    }

                    sscanf(buffer, "%s", comando);


                    if (strcmp("!signup", comando) == 0) {
                        //printf("sono in string compare \n");
                        memset(username, 0, DEFAULT_SIZE);
                        memset(password, 0, DEFAULT_SIZE);
                        memset(comando, 0, DEFAULT_SIZE);

                        sscanf(buffer, "%s %s %s", comando, username, password);
                        fd = fopen("utentiregistrati.txt", "r");

                        if (!fgets(buffer, DEFAULT_SIZE, fd)) {

                            fd = fopen("utentiregistrati.txt", "a");

                            fprintf(fd, "%s %s\n", username, password);

                            fclose(fd);

                            strcpy(buffer, "Registrazione effettuata");
                            printf("invio %s\n", buffer);

                            len = strlen(buffer) + 1; // Voglio inviare anche il carattere di fine stringa
                            lmsg = htons(len);

                            ret = send(new_sd, (void*) &lmsg, sizeof (uint16_t), 0);

                            ret = send(new_sd, (void*) buffer, len, 0);
                            if (ret < 0) {
                                perror("Errore in fase di send: \n");
                                exit(-1);
                            }
                            memset(buffer, 0, DEFAULT_SIZE); //creo tutti file registro
                            strcpy(buffer, username);
                            strcat(buffer, "giocateattive.txt");
                            if (fopen(buffer, "r") == NULL) {
                                printf("creo file %s\n", buffer);
                                fd = fopen(buffer, "a");
                                fclose(fd);
                            }
                            memset(buffer, 0, DEFAULT_SIZE);
                            strcpy(buffer, username);
                            strcat(buffer, "giocatepassate.txt");
                            if (fopen(buffer, "r") == NULL) {
                                printf("creo file %s\n", buffer);
                                fd = fopen(buffer, "a");
                                fclose(fd); 
                            }
                            memset(buffer, 0, DEFAULT_SIZE);
                            strcpy(buffer, username);
                            strcat(buffer, "vincite.txt");
                            if (fopen(buffer, "r") == NULL) {
                                printf("creo file %s\n", buffer);
                                fd = fopen(buffer, "a");
                                fclose(fd); 
                            }
                            memset(buffer, 0, DEFAULT_SIZE);
                            strcpy(buffer, username);
                            strcat(buffer, "consuntivo.txt");
                            if (fopen(buffer, "r") == NULL) {
                                printf("creo file %s\n", buffer);
                                fd = fopen(buffer, "a");
                                float a, b, c, d, e;
                                a = 0;
                                b = 0;
                                c = 0;
                                d = 0;
                                e = 0;
                                fprintf(fd, "%f %f %f %f %f", a, b, c, d, e);
                                fclose(fd);
                            }
                            continue;
                        }
                        fclose(fd);

                        while (1) { //effettuo controllo se l'account esiste
                            accountEsistente = false;
                            fd = fopen("utentiregistrati.txt", "r");
                            while (fgets(buffer, DEFAULT_SIZE, fd) != NULL) {

                                memset(usernameCheck, 0, DEFAULT_SIZE);
                                sscanf(buffer, "%s", usernameCheck);

                                if (strcmp(username, usernameCheck) == 0) {
                                    accountEsistente = true;
                                    break;
                                }

                            }
                            if (accountEsistente == true) {
                                strcpy(buffer, "Username esistente");
                                len = strlen(buffer) + 1; // Voglio inviare anche il carattere di fine stringa
                                lmsg = htons(len);

                                ret = send(new_sd, (void*) &lmsg, sizeof (uint16_t), 0); //notifico al client che c'è un utente già registrato ocn quel username

                                ret = send(new_sd, (void*) buffer, len, 0);
                                if (ret < 0) {
                                    perror("Errore in fase di send: \n");
                                    exit(-1);
                                }

                                ret = recv(new_sd, (void*) &lmsg, sizeof (uint16_t), 0); //ricevo nuovi dati e ricontrollo

                                len = ntohs(lmsg); // Rinconverto in formato host

                                ret = recv(new_sd, (void*) buffer, len, 0);
                                if (ret < 0) {
                                    perror("Errore in fase di recv: \n");
                                    exit(-1);
                                }

                                sscanf(buffer, "%s %s", username, password);
                                fclose(fd);
                            }

                            if (accountEsistente == false) {
                                fclose(fd);
                                break;
                            }


                        }

                        if (accountEsistente == false) {
                            fd = fopen("utentiregistrati.txt", "a");
                            fprintf(fd, "%s %s\n", username, password);
                            fclose(fd);
                            memset(&buffer, 0, DEFAULT_SIZE);
                            strcpy(buffer, "Registrazione effettuata");
                            len = strlen(buffer) + 1; // Voglio inviare anche il carattere di fine stringa
                            lmsg = htons(len);

                            ret = send(new_sd, (void*) &lmsg, sizeof (uint16_t), 0);

                            ret = send(new_sd, (void*) buffer, len, 0);
                            if (ret < 0) {
                                perror("Errore in fase di send: \n");
                                exit(-1);
                            }
                            memset(buffer, 0, DEFAULT_SIZE); //creo tutti file registro
                            strcpy(buffer, username);
                            strcat(buffer, "giocateattive.txt");
                            if (fopen(buffer, "r") == NULL) {
                                printf("creo file %s\n", buffer);
                                fd = fopen(buffer, "a");
                                fclose(fd);
                            }
                            memset(buffer, 0, DEFAULT_SIZE);
                            strcpy(buffer, username);
                            strcat(buffer, "giocatepassate.txt");
                            if (fopen(buffer, "r") == NULL) {
                                printf("creo file %s\n", buffer);
                                fd = fopen(buffer, "a");
                                fclose(fd); //creo il file se non c'è
                            }
                            memset(buffer, 0, DEFAULT_SIZE);
                            strcpy(buffer, username);
                            strcat(buffer, "vincite.txt");
                            if (fopen(buffer, "r") == NULL) {
                                printf("creo file %s\n", buffer);
                                fd = fopen(buffer, "a");
                                fclose(fd); //creo il file se non c'è
                            }
                            memset(buffer, 0, DEFAULT_SIZE);
                            strcpy(buffer, username);
                            strcat(buffer, "consuntivo.txt");
                            if (fopen(buffer, "r") == NULL) {
                                printf("creo file %s\n", buffer);
                                fd = fopen(buffer, "a");
                                float a, b, c, d, e;
                                a = 0;
                                b = 0;
                                c = 0;
                                d = 0;
                                e = 0;
                                fprintf(fd, "%f %f %f %f %f", a, b, c, d, e);
                                fclose(fd);
                            }


                        }

                    }

                    if (strcmp("!login", comando) == 0) {
                       
                        char userLogged[DEFAULT_SIZE];
                        bool already_logged = false; //flag per controllare se l'utente ha già fatto il login da altro terminale

                        memset(username, 0, DEFAULT_SIZE);
                        memset(password, 0, DEFAULT_SIZE);
                        memset(comando, 0, DEFAULT_SIZE);
                        memset(userLogged, 0, DEFAULT_SIZE); //salvo qui il valore contenuto letto dal file loggati

                        sscanf(buffer, "%s %s %s", comando, username, password);

                        fd = fopen("loggati.txt", "r");
                        while (fgets(buffer, DEFAULT_SIZE, fd) != NULL) { //controllo che l'utente non abbia fatto login da altro terminale

                            sscanf(buffer, "%s", userLogged);

                            if (strcmp(username, userLogged) == 0) {
                                already_logged = true;
                                break;
                            }
                        }

                        fclose(fd);
                        if (already_logged == true) {

                            strcpy(buffer, "USER_LOGGED");
                            len = strlen(buffer) + 1; // Voglio inviare anche il carattere di fine stringa
                            lmsg = htons(len);

                            ret = send(new_sd, (void*) &lmsg, sizeof (uint16_t), 0);

                            ret = send(new_sd, (void*) buffer, len, 0);
                            if (ret < 0) {
                                perror("Errore in fase di send: \n");
                                exit(-1);
                            }

                            continue;
                        }
                        fd = fopen("utentiregistrati.txt", "r");

                        bool USER_NOT_FOUND = true; //variabile per controllare che l'utente che ha fatto il login si è rgistrato o no
                        while (fgets(buffer, DEFAULT_SIZE, fd) != NULL) {

                            memset(&usernameCheck, 0, DEFAULT_SIZE);
                            memset(&passwordCheck, 0, DEFAULT_SIZE);
                            sscanf(buffer, "%s %s", usernameCheck, passwordCheck);

                            if (strcmp(username, usernameCheck) == 0 && strcmp(password, passwordCheck) == 0) { //utente autenticato correttamente
                                USER_NOT_FOUND = false;
                                memset(&buffer, 0, DEFAULT_SIZE);

                                srand(time(NULL));
                                int index;
                                for (index = 0; index < 10; index++) {
                                    session_ID[index] = alphanum[rand() % strlen(alphanum)]; //creo sessionID
                                }
                                strcpy(buffer, session_ID);
                                len = strlen(buffer) + 1; // invio session ID
                                lmsg = htons(len);

                                ret = send(new_sd, (void*) &lmsg, sizeof (uint16_t), 0);

                                ret = send(new_sd, (void*) buffer, len, 0);
                                if (ret < 0) {
                                    perror("Errore in fase di send: \n");
                                    exit(-1);
                                }
                                printf("Utente %s loggato\n", usernameCheck);
                                printf("Il session ID di %s è: %s\n", usernameCheck, buffer);
                                fclose(fd);

                                fd = fopen("loggati.txt", "a");
                                fprintf(fd, "%s\n", username);
                                break;
                            }

                            if ((strcmp(username, usernameCheck) == 0 && strcmp(password, passwordCheck) != 0)) {
                                memset(&buffer, 0, DEFAULT_SIZE);
                                USER_NOT_FOUND = false;
                                tentativi++;
                                if (tentativi == 3) {
                                    printf("tre tentativi\n");   //memorizzo ip, tempo e timestamp dell'utente bloccato
                                    fd2 = fopen("utentibloccati.txt", "a");
                                    my_time = time(NULL);
                                    printf("%s %d %s\n", client_addr_string, (int) my_time, asctime(localtime(&my_time)));
                                    fprintf(fd2, "%s %d %s", client_addr_string, (int) my_time, asctime(localtime(&my_time)));
                                    strcpy(buffer, "LOGIN_BLOCCATO");
                                    printf("%s\n", buffer);
                                    fclose(fd2);
                                    len = strlen(buffer) + 1; 
                                    lmsg = htons(len);

                                    ret = send(new_sd, (void*) &lmsg, sizeof (uint16_t), 0); //notifico il client che ha subito un blocco

                                    ret = send(new_sd, (void*) buffer, len, 0);
                                    if (ret < 0) {
                                        perror("Errore in fase di recv: \n");
                                        exit(-1);
                                    }

                                    break;
                                }
                                strcpy(buffer, "LOGIN_NO"); //l'utente ha sbagliato pass, glielo comunico
                                printf("%s\n", buffer);
                                len = strlen(buffer) + 1; 
                                lmsg = htons(len);

                                ret = send(new_sd, (void*) &lmsg, sizeof (uint16_t), 0);

                                ret = send(new_sd, (void*) buffer, len, 0);
                                if (ret < 0) {
                                    perror("Errore in fase di send: \n");
                                    exit(-1);
                                }

                                break;

                            }


                        }

                        fclose(fd);
                        if (USER_NOT_FOUND == true) { //se non vi è alcun utente con quell'username allora non è registrato
                            memset(buffer, 0, DEFAULT_SIZE);
                            strcpy(buffer, "UTENTE_NON_REG");
                            len = strlen(buffer) + 1; 
                            lmsg = htons(len);

                            ret = send(new_sd, (void*) &lmsg, sizeof (uint16_t), 0);

                            ret = send(new_sd, (void*) buffer, len, 0);
                            if (ret < 0) {
                                perror("Errore in fase di send: \n");
                                exit(-1);
                            }
                        }

                        if (tentativi == 3) {
                            printf("Chiusura connessione causa 3 tentativi\n"); //chiudo il socket di connessione con utente bloccato
                            close(new_sd);
                            exit(-1);
                        }

                    }

                    if (strcmp("!invia_giocata", comando) == 0) {
                        struct giocata giocata_srv;
                        memset(giocata_srv.numeri, 0, (sizeof (int)*10));
                        memset(giocata_srv.ruote, 0, (sizeof (char)*11 * 11));
                        memset(giocata_srv.importi, 0, (sizeof (float)*5));
                        giocata_srv.quante_ruote = 0;
                        giocata_srv.quanti_imp = 0;
                        giocata_srv.quanti_num = 0;
                        memset(buffer, 0, DEFAULT_SIZE);

                        ret = recv(new_sd, (void*) &lmsg, sizeof (uint16_t), 0);
                        len = ntohs(lmsg); // Rinconverto in formato host
                        ret = recv(new_sd, (void*) buffer, len, 0); //ricevo session ID
                        if (ret < 0) {
                            perror("Errore in fase di recv: \n");
                            exit(-1);
                        }

                        if (strcmp(buffer, session_ID) != 0) {
                            strcpy(buffer, "SESSION_ID non valido");
                            len = strlen(buffer) + 1;
                            lmsg = htons(len);
                            ret = send(new_sd, (void*) &lmsg, sizeof (uint16_t), 0); //mando riscontro negativo

                            ret = send(new_sd, (void*) buffer, len, 0);
                            if (ret < 0) {
                                perror("Errore in fase di send: \n");
                                exit(-1);
                            }

                        } else {
                            strcpy(buffer, "SESSION_ID valido");
                            len = strlen(buffer) + 1;
                            lmsg = htons(len);
                            ret = send(new_sd, (void*) &lmsg, sizeof (uint16_t), 0); //mando riscontro positivo 
                            ret = send(new_sd, (void*) buffer, len, 0);
                            if (ret < 0) {
                                perror("Errore in fase di recv: \n");
                                exit(-1);
                            }

                            printf("Session_ID ricevuto okay\n");

                            ret = recv(new_sd, (void*) &lmsg, sizeof (uint16_t), 0);
                            len = ntohs(lmsg);
                            ret = recv(new_sd, (void*) &giocata_srv, len, 0); // Ricevo giocata
                            if (ret < 0) {
                                perror("Errore in fase di recv: \n");
                                exit(-1);
                            }

                            memset(buffer, 0, DEFAULT_SIZE);
                            strcpy(buffer, usernameCheck);
                            strcat(buffer, "giocateattive.txt");
                            fd = fopen(buffer, "a");
                            int i;
                            fprintf(fd, "%d ", giocata_srv.quante_ruote);
                            for (i = 0; i < giocata_srv.quante_ruote; i++) {
                                fprintf(fd, "%s ", giocata_srv.ruote[i]);
                                if (strcmp(giocata_srv.ruote[i], "tutte") == 0)
                                    break;

                            }

                            fprintf(fd, "%d ", giocata_srv.quanti_num);
                            for (i = 0; i < giocata_srv.quanti_num; i++) {
                                fprintf(fd, "%d ", giocata_srv.numeri[i]);

                            }
                            fprintf(fd, "%d ", giocata_srv.quanti_imp);
                            for (i = 0; i < giocata_srv.quanti_imp; i++) {
                                if (i != giocata_srv.quanti_imp - 1)
                                    fprintf(fd, "%f ", giocata_srv.importi[i]);
                                else
                                    fprintf(fd, "%f\n", giocata_srv.importi[i]);
                            }
                            strcpy(buffer, "GIOCATA MEMORIZZATA CORRETTAMENTE");
                            len = strlen(buffer) + 1;
                            lmsg = htons(len);
                            ret = send(new_sd, (void*) &lmsg, sizeof (uint16_t), 0); //mando riscontro su memorizzaz. giocata
                            ret = send(new_sd, (void*) buffer, len, 0);

                            fclose(fd);
                            printf("Giocata dell'utente %s memorizzata\n", usernameCheck);
                        }
                    }

                    if (strcmp("!vedi_giocate", comando) == 0) {

                        int tipo;
                        int i = 0;

                        char ruote[10];
                        int num; //var in cui memorizzo i num letti
                        float imp; //var in cui mem gli importi letti
                        int res; ///var per vedere se sono alla fine del file
                        struct giocata giocata_lett; //struct in cui mem la giocata letta

                        sscanf(buffer, "%s %d", comando, &tipo);

                        ret = recv(new_sd, (void*) &lmsg, sizeof (uint16_t), 0);
                        len = ntohs(lmsg); // Rinconverto in formato host
                        ret = recv(new_sd, (void*) buffer, len, 0); //ricevo session ID
                        if (ret < 0) {
                            perror("Errore in fase di recv: \n");
                            exit(-1);
                        }
                        if (strcmp(buffer, session_ID) != 0) {
                            strcpy(buffer, "SESSION_ID non valido");
                            len = strlen(buffer) + 1;
                            lmsg = htons(len);
                            ret = send(new_sd, (void*) &lmsg, sizeof (uint16_t), 0); //mando riscontro negativo

                            ret = send(new_sd, (void*) buffer, len, 0);
                            continue;
                        } else {
                            strcpy(buffer, "SESSION_ID valido");
                            len = strlen(buffer) + 1;
                            lmsg = htons(len);
                            ret = send(new_sd, (void*) &lmsg, sizeof (uint16_t), 0); //mando riscontro positivo
                            ret = send(new_sd, (void*) buffer, len, 0);
                            if (tipo == 0) {
                                memset(&buffer, 0, DEFAULT_SIZE);
                                strcpy(buffer, usernameCheck);
                                strcat(buffer, "giocatepassate.txt");
                            }
                            if (tipo == 1) {
                                memset(&buffer, 0, DEFAULT_SIZE);
                                strcpy(buffer, usernameCheck);
                                strcat(buffer, "giocateattive.txt");

                            }

                            
                            fd = fopen(buffer, "r"); //finchè non finisce il file prelevo giocate e le invio
                            while (!feof(fd)) {
                                memset(buffer, 0, DEFAULT_SIZE);

                                char nuovalinea;
                                memset(giocata_lett.numeri, 0, (sizeof (int)*11));
                                memset(giocata_lett.ruote, 0, (sizeof (char)*11 * 11));
                                memset(giocata_lett.importi, 0, (sizeof (float)*5));
                                giocata_lett.quante_ruote = 0;
                                giocata_lett.quanti_imp = 0;
                                giocata_lett.quanti_num = 0;

                                res = fscanf(fd, "%d", &giocata_lett.quante_ruote);
                                if (res == EOF) {
                                   
                                    break;
                                }
                               
                                while (i < giocata_lett.quante_ruote) {
                                    fscanf(fd, "%s", ruote);
                                    strcpy(giocata_lett.ruote[i], ruote);
                                    if (strcmp(giocata_lett.ruote[i], "tutte") == 0)
                                        break;
                                    i++;
                                }

                                i = 0;
                                fscanf(fd, "%d", &giocata_lett.quanti_num);
                               
                                while (i < giocata_lett.quanti_num) {
                                    fscanf(fd, "%d", &num);
                                    giocata_lett.numeri[i] = num;
                                    ;
                                    i++;
                                }
                                i = 0;
                                fscanf(fd, "%d", &giocata_lett.quanti_imp);
                                while (i < giocata_lett.quanti_imp) {
                                    fscanf(fd, "%f", &imp);
                                    giocata_lett.importi[i] = imp;
                                    i++;

                                }
                                fscanf(fd, "%c", &nuovalinea);

                                i = 0;
                                strcpy(buffer, "ANCORA"); //comunico all'utente che sta per ricevere una ulteriore giocaat
                                len = strlen(buffer) + 1;
                                lmsg = htons(len);
                                ret = send(new_sd, (void*) &lmsg, sizeof (uint16_t), 0);
                                ret = send(new_sd, (void*) buffer, len, 0);
                                if (ret < 0) {
                                    perror("Errore in fase di send: \n");
                                    exit(-1);
                                }

                                len = (sizeof (int)*13)+(sizeof (char)*11 * 11)+(sizeof (float)*5);
                                lmsg = htons(len);
                                ret = send(new_sd, (void*) &lmsg, sizeof (uint16_t), 0);
                                ret = send(new_sd, (void*) &giocata_lett, len, 0);
                                if (ret < 0) {
                                    perror("Errore in fase di send: \n");
                                    exit(-1);
                                }
                                if (feof(fd))
                                    break;

                            }
                            strcpy(buffer, "FINE_GIOCATE_LETTE"); //comunico all'utente che sono state inviate tutte le giocate
                            len = strlen(buffer) + 1; 
                            lmsg = htons(len);
                            ret = send(new_sd, (void*) &lmsg, sizeof (uint16_t), 0);
                            ret = send(new_sd, (void*) buffer, len, 0);
                            if (ret < 0) {
                                perror("Errore in fase di send: \n");
                                exit(-1);
                            }
                            fclose(fd);
                        }
                        printf("Giocate correttamente inviate all'utente %s\n", usernameCheck);
                    }



                    if (strcmp("!vedi_vincite", comando) == 0) {
                        ret = recv(new_sd, (void*) &lmsg, sizeof (uint16_t), 0);
                        len = ntohs(lmsg); // Rinconverto in formato host
                        ret = recv(new_sd, (void*) buffer, len, 0); //ricevo session ID
                        if (ret < 0) {
                            perror("Errore in fase di recv: \n");
                            exit(-1);
                        }


                        if (strcmp(buffer, session_ID) != 0) {
                            strcpy(buffer, "SESSION_ID non valido");
                            len = strlen(buffer) + 1;
                            lmsg = htons(len);
                            ret = send(new_sd, (void*) &lmsg, sizeof (uint16_t), 0); //mando riscontro negativo

                            ret = send(new_sd, (void*) buffer, len, 0);
                            if (ret < 0) {
                                perror("Errore in fase di send \n");
                                exit(-1);
                            }
                            continue;
                        } else {
                            strcpy(buffer, "SESSION_ID valido");
                            len = strlen(buffer) + 1;
                            lmsg = htons(len);
                            ret = send(new_sd, (void*) &lmsg, sizeof (uint16_t), 0); //mando riscontro positivo e accetto la giocata
                            ret = send(new_sd, (void*) buffer, len, 0);
                            if (ret < 0) {
                                perror("Errore in fase di send: \n");
                                exit(-1);
                            }
                            memset(buffer, 0, DEFAULT_SIZE);
                            strcpy(buffer, usernameCheck);
                            strcat(buffer, "vincite.txt");
                            fd = fopen(buffer, "r");
                            while (fgets(buffer, DEFAULT_SIZE, fd) != NULL) {

                                char flag[DEFAULT_SIZE];
                                strcpy(flag, "ANCORA"); //comunico all'utente che sta per ricevere ulteriori dati
                                len = strlen(buffer) + 1; 
                                lmsg = htons(len);
                                ret = send(new_sd, (void*) &lmsg, sizeof (uint16_t), 0);
                                ret = send(new_sd, (void*) flag, len, 0);
                                if (ret < 0) {
                                    perror("Errore in fase di send: \n");
                                    exit(-1);
                                }

                                len = strlen(buffer) + 1;
                                lmsg = htons(len);
                                ret = send(new_sd, (void*) &lmsg, sizeof (uint16_t), 0); //mando dati
                                ret = send(new_sd, (void*) buffer, len, 0);
                                if (ret < 0) {
                                    perror("Errore in fase di send: \n");
                                    exit(-1);
                                }


                            }

                            memset(&buffer, 0, DEFAULT_SIZE);

                            strcpy(buffer, "FINE");
                            len = strlen(buffer) + 1;
                            lmsg = htons(len);
                            ret = send(new_sd, (void*) &lmsg, sizeof (uint16_t), 0); //mando fine lettura
                            ret = send(new_sd, (void*) buffer, len, 0);
                            if (ret < 0) {
                                perror("Errore in fase di send: \n");
                                exit(-1);
                            }

                            strcpy(buffer, usernameCheck);
                            strcat(buffer, "consuntivo.txt");
                            fd = fopen(buffer, "r");
                            fgets(buffer, DEFAULT_SIZE, fd);
                            fclose(fd);

                            len = strlen(buffer) + 1;
                            lmsg = htons(len);
                            ret = send(new_sd, (void*) &lmsg, sizeof (uint16_t), 0); //mando consuntivo
                            ret = send(new_sd, (void*) buffer, len, 0);
                            if (ret < 0) {
                                perror("Errore in fase di send: \n");
                                exit(-1);
                            }

                        }
                        printf("Vincite inviate correttamente all'utente %s\n", usernameCheck);
                    }

                    if (strcmp("!vedi_estrazione", comando) == 0) {
                        char ruota_req[10];
                        char ruota_lett[10];
                        int num_estr_req;
                        int num_estr;
                        int x = 0;
                        memset(ruota_req, 0, 10);
                        sscanf(buffer, "%s %d %s", comando, &num_estr_req, ruota_req);
                        // printf("BUFFER VALE: %s %d %s\n", buffer, num_estr_req, ruota_req);

                        ret = recv(new_sd, (void*) &lmsg, sizeof (uint16_t), 0);
                        len = ntohs(lmsg); // Rinconverto in formato host
                        ret = recv(new_sd, (void*) buffer, len, 0); //ricevo session ID
                        if (ret < 0) {
                            perror("Errore in fase di recv: \n");
                            exit(-1);
                        }



                        if (strcmp(buffer, session_ID) != 0) {
                            strcpy(buffer, "SESSION_ID non valido");
                            len = strlen(buffer) + 1;
                            lmsg = htons(len);
                            ret = send(new_sd, (void*) &lmsg, sizeof (uint16_t), 0); //mando riscontro negativo

                            ret = send(new_sd, (void*) buffer, len, 0);
                            if (ret < 0) {
                                perror("Errore in fase di send: \n");
                                exit(-1);
                            }
                            continue;
                        } else {
                            strcpy(buffer, "SESSION_ID valido");
                            len = strlen(buffer) + 1;
                            lmsg = htons(len);
                            ret = send(new_sd, (void*) &lmsg, sizeof (uint16_t), 0); //mando riscontro positivo e procedo
                            ret = send(new_sd, (void*) buffer, len, 0);
                            if (ret < 0) {
                                perror("Errore in fase di send: \n");
                                exit(-1);
                            }

                            fd = fopen("numerodiestrazioni.txt", "r");
                            fscanf(fd, "%d", &num_estr);
                            fclose(fd);
                            if (num_estr < num_estr_req) {
                                strcpy(buffer, "NUM_GRANDE");

                                len = strlen(buffer) + 1; // Voglio inviare anche il carattere di fine stringa
                                lmsg = htons(len);

                                ret = send(new_sd, (void*) &lmsg, sizeof (uint16_t), 0); //informo il client che ha chiesto un numero di estrazioni troppo elavato

                                ret = send(new_sd, (void*) buffer, len, 0);
                                if (ret < 0) {
                                    perror("Errore in fase di send: \n");
                                    exit(-1);
                                }
                                continue;
                            }

                            fd = fopen("estrazioni.txt", "r");

                            while (x < (num_estr - num_estr_req)*11) {
                                fgets(buffer, DEFAULT_SIZE, fd); //mi posiziono sulla estrazione corrretta
                                x++;
                            }

                            while (fgets(buffer, DEFAULT_SIZE, fd) != NULL) {
                                sscanf(buffer, "%s", ruota_lett);
                                if (ruota_req[0] != '\0' && strcmp(ruota_lett, ruota_req) == 0) { //se l'utente ha specificato la ruota invio la ruota specifica

                                    len = strlen(buffer) + 1; // Voglio inviare anche il carattere di fine stringa
                                    lmsg = htons(len);

                                    ret = send(new_sd, (void*) &lmsg, sizeof (uint16_t), 0);

                                    ret = send(new_sd, (void*) buffer, len, 0);
                                    if (ret < 0) {
                                        perror("Errore in fase di recv: \n");
                                        exit(-1);
                                    }

                                }
                                if (ruota_req[0] == '\0') { //se l'utente non ha specifiacato la ruota le mando tutte
                                    //printf("BUFFER ELSE INVIATO: %s",buffer);
                                    len = strlen(buffer) + 1; // Voglio inviare anche il carattere di fine stringa
                                    lmsg = htons(len);

                                    ret = send(new_sd, (void*) &lmsg, sizeof (uint16_t), 0);

                                    ret = send(new_sd, (void*) buffer, len, 0);
                                    if (ret < 0) {
                                        perror("Errore in fase di recv: \n");
                                        exit(-1);
                                    }


                                }

                            }
                            strcpy(buffer, "FINE");

                            len = strlen(buffer) + 1; // informo il client che l'operazione è finita
                            lmsg = htons(len);

                            ret = send(new_sd, (void*) &lmsg, sizeof (uint16_t), 0);

                            ret = send(new_sd, (void*) buffer, len, 0);
                            printf("Estrazioni inviate correttamente all'utente %s\n", usernameCheck);
                            fclose(fd);

                        }
                    }
                    if (strcmp("!esci", comando) == 0) {
                        char appoggio[DEFAULT_SIZE * 2]; //buffer appoggio in cui memorizzo tutti gli utenti attualmente loggati
                        int i = 0; //mi serve per fare o strcat o strcpy
                        char usr_lett[DEFAULT_SIZE]; //buffer dii appoggio in cui memorizzo la stringa letta da loggati.txt
                        ret = recv(new_sd, (void*) &lmsg, sizeof (uint16_t), 0);
                        len = ntohs(lmsg); // Rinconverto in formato host
                        ret = recv(new_sd, (void*) buffer, len, 0); //ricevo session ID
                        //printf("SESSION ID_RICEVUTO: %s %s\n", buffer, session_ID);

                        if (strcmp(buffer, session_ID) != 0) {
                            strcpy(buffer, "SESSION_ID non valido");
                            len = strlen(buffer) + 1;
                            lmsg = htons(len);
                            ret = send(new_sd, (void*) &lmsg, sizeof (uint16_t), 0); //mando riscontro negativo

                            ret = send(new_sd, (void*) buffer, len, 0);
                            continue;
                        } else {
                            memset(session_ID, 0, 10);
                            memset(&buffer, 0, DEFAULT_SIZE);
                            strcpy(buffer, "Addio");
                            len = strlen(buffer) + 1; // Voglio inviare anche il carattere di fine stringa
                            lmsg = htons(len);
                            ret = send(new_sd, (void*) &lmsg, sizeof (uint16_t), 0);

                            ret = send(new_sd, (void*) buffer, len, 0);
                            fd = fopen("loggati.txt", "r");

                            while (fgets(buffer, DEFAULT_SIZE, fd) != NULL) { //elimino dai loggati.txt l'utente che ha effettuato il login
                                sscanf(buffer, "%s", usr_lett);
                                if (strcmp(usr_lett, usernameCheck) != 0) { //memroizzo tutti gli utenti tranne quello che ha fatto il logout e poi li ricopio in loggati.txt
                                    if (i == 0)
                                        strcpy(appoggio, buffer);
                                    else
                                        strcat(appoggio, buffer);
                                    i++;
                                }
                            }

                            fclose(fd);
                            fd = fopen("loggati.txt", "w");
                            fprintf(fd, "%s", appoggio);
                            fclose(fd);
                            printf("User %s disconnesso\n", usernameCheck);


                            close(new_sd);

                            exit(1);

                        }
                    }

                }
            } else {
                // Processo padre
                printf("processo padre chiudo socket connessione\n");
                close(new_sd);
            }
        }
    }
}
