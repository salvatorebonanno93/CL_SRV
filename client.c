#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h> //per isdigit
#define DEFAULT_SIZE 1024

struct giocata { //strttura per memorizzare la giocata
    int numeri[10];
    char ruote[11][11];
    float importi[5];
    int quanti_imp;
    int quante_ruote;
    int quanti_num;
};

void showCommands() {

    printf("*****************************GIOCO DEL LOTTO ***************************** \n"
            "Sono disponibili i seguenti comandi: \n"
            "1) !help <comando> --> mostra i dettagli di un comando\n"
            "2) !signup <username> <password> --> crea un nuovo utente ?\n"
            "3) !login <username> <password> --> autentica un utente\n"
            "4) !invia_giocata g --> invia una giocata g al server\n"
            "5) !vedi_giocate tipo --> visualizza le giocate precedenti dove tipo = {0,1} \n"
            "	                       e permette di visualizzare le giocate passate '0' \n"
            "	                       oppure le giocate attive '1' (ancora non estratte) \n"
            "6) !vedi_estrazione <n> <ruota> --> mostra i numeri delle ultime n estrazioni sulla ruota specificata\n"
            "7) !esci --> termina il client \n");

}

bool check(char* ruota) { //controllo quando invio !vedi_estrazione sulla ruota
    if (strcmp(ruota, "bari") == 0 || strcmp(ruota, "cagliari") == 0 || strcmp(ruota, "firenze") == 0 || strcmp(ruota, "genova") == 0 ||
            strcmp(ruota, "milano") == 0 || strcmp(ruota, "napoli") == 0 || strcmp(ruota, "palermo") == 0 ||
            strcmp(ruota, "roma") == 0 || strcmp(ruota, "torino") == 0 || strcmp(ruota, "venezia") == 0 ||
            strcmp(ruota, "nazionale") == 0 || strcmp(ruota, "bari") == 0 || ruota[0] == '\0')
        return true;
    return false;






}

bool check_duplicati(int* array, int dim) {
    //controllo sui numeri duplicati
    int i, j;


    for (i = 0; i < dim - 1; i++) {
        j = i + 1;
        while (j < dim) {
            if (array[i] == array[j])
                return false;
            j++;
        }

    }

    return true;
}

void stampa_giocata(struct giocata* gt) {

    int i;
    for (i = 0; i < gt->quante_ruote; i++) {
        printf("%s ", gt->ruote[i]);
        if (strcmp(gt->ruote[i], "tutte") == 0)
            break;
    }
    for (i = 0; i < gt->quanti_num; i++) {
        printf("%d ", gt->numeri[i]);
    }
    for (i = 0; i < gt->quanti_imp; i++) {
        if (i == 0)
            printf("%s %f ", "ESTRATTO", gt->importi[i]);
        if (i == 1)
            printf("%s %f ", "AMBO", gt->importi[i]);
        if (i == 2)
            printf("%s %f ", "TERNO", gt->importi[i]);
        if (i == 3)
            printf("%s %f ", "QUATERNA", gt->importi[i]);
        if (i == 4)
            printf("%s %f ", "CINQUINA", gt->importi[i]);
    }
    printf("\n");

}

bool checkgiocata(char* buffer_par, struct giocata* gt) {  //funzione per il corretto inserimento della giocata e per  inizializ la strutt da inviare
    int i, j, x, extra, controllo_numero;
    char buffer[DEFAULT_SIZE];
    char importi[DEFAULT_SIZE];
    char numeri[2];
    char* pt; //puntatore per scorrere la stringa inserita
    char* appo; //puntatore d'appoggio per i controlli
    int quantigiocati, numerimporti, numeroruoteinserite, numeroruotecontate;
    bool tutte_pres = false; //flag nel caso siano presenti tutte le ruote

    strcpy(buffer, buffer_par);

    memset(importi, 0, DEFAULT_SIZE); //inizializzo
    memset(numeri, 0, 2);
    memset(&pt, 0, sizeof (pt));
    memset(buffer_par, 0, DEFAULT_SIZE);

    
    i = 0;
    j = 0; //indice per fare il controllo sui numeri
    x = 0; //indice per memorizzare i numeri inseriti nell'array giocata.numeri[10]
    extra = 0; //per evitare numeri a 3 cifre
    controllo_numero = 0; //per evitare numeri >90

    //controllo sull'ordine dei parametri -r -n -i

    pt = strstr(buffer, "-i");

    if (pt == NULL) {
        printf("%s", "ERRORE SINTASSI manca il parametro -i, inserire lo spazio dopo la i\n");
        return 0;
    }

    pt--;

    if (*(pt) != ' ') {
        printf("%s", "Manca lo spazio prima la i \n");
        return false;
    }

    pt = strstr(buffer, "-n");

    if (pt == NULL) {

        printf("%s", "ERRORE SINTASSI manca il parametro -n, inserire lo spazio dopo la n\n");
        return 0;
    }

    pt--;

    if (*(pt) != ' ') {
        printf("%s", "Manca lo spazio prima la n \n");
        return false;
    }


    pt = strstr(buffer, "-r");
    if (pt == NULL) {

        printf("%s", "ERRORE SINTASSI manca il parametro -r, inserire lo spazio dopo la n\n");
        return 0;
    }

    pt--;
    if ((*pt) != ' ') {
        printf("%s", "Manca lo spazio prima del  -r \n");
        return false;
    }

    pt = strstr(buffer, "-r");
    appo = strstr(buffer, "-n");
    if (appo < pt) {
        printf("Devi inserire il parametro -r prima di -n\n");
        return false;
    }
    pt = strstr(buffer, "-r");
    appo = strstr(buffer, "-i");
    if (appo < pt) {
        printf("Devi inserire il parametro -r prima di -i\n");
        return false;
    }
    pt = strstr(buffer, "-n");
    appo = strstr(buffer, "-i");
    if (appo < pt) {
        printf("Devi inserire il parametro -n prima di -i\n");
        return false;
    }


    quantigiocati = 0;
    pt = strstr(buffer, "-n");
    pt++;
    pt++;
    if (*(pt) != ' ') {
        printf("%s", "Manca lo spazio dopo la n \n");
        return false;
    }
    pt++;
    while ((*pt) != '-') { //prelevo i numeri

        appo = pt;
        appo++;
        if (*(pt) == ' ' && i == 0) {
            printf("%s", "Hai inserito uno spazio di troppo dopo la n\n");
            return 0;
        }
        if (*(pt) == ' ' && *(appo) != ' ') { //quando sono sullo spazio vuol dire che ho un nuovo numero
            controllo_numero = atoi(numeri);
            if (controllo_numero == 0 || controllo_numero > 90) {
                printf("Inserito numero non nell'intervallo 0-90\n");

                return 0;
            }

            quantigiocati++;
            extra = 0;
            j = 0;
            gt->numeri[x] = controllo_numero;
            x++;
            controllo_numero = 0;
            strcat(buffer_par, numeri);
            strcat(buffer_par, " ");
            memset(numeri, 0, 2);
        }
        if (*(pt) == ' ' && *(appo) == ' ') {
            printf("Inserito spazio di troppo tra i numeri\n");

            return 0;
        }
        if (*(pt) != ' ') {
            numeri[j] = *pt;

        }

        
        if (*(pt) != ' ' && !isdigit(numeri[j])) {
            printf("%s", "Non hai inserito numero valido \n");
            return 0;
        }
        if (*(pt) != ' ') {
            extra++;
        }


        if (*(pt) != ' ' && extra > 2) {
            printf("%s", "Non hai inserito numero valido \n");
            return 0;
        }
        if (*(pt) != ' ')
            j++;
        pt++;
        i++;

    }

    j = 0;

    gt->quanti_num = x;
    if (check_duplicati(gt->numeri, x) == false) {
        printf("Presenti duplicati\n");
        return false;

    }


    if (quantigiocati > 10) {
        printf("Numeri giocati superiori a 10\n");
        return 0;

    }


    pt = strstr(buffer, "-i");

    pt++;
    pt++;

    if (*(pt) != ' ') {
        printf("%s", "Manca lo spazio dopo la i \n");
        return false;
    }

    pt++;

    i = 0;
    numerimporti = 0;

    while ((*pt) != '\0') { //prelevo gli importi
        appo = pt;

        if (*(pt) == ' ' && i == 0) {
            printf("%s", "Hai inserito uno spazio di troppo dopo la i\n");
            return 0;
        }
        importi[i] = *pt;
        if ((*appo) == ' ' && *(++appo) == ' ') {
            printf("%s", "Hai inserito uno spazio di troppo dopo l'ultimo importo o tra gli importi 1'\n");
            printf("iterazione %d\n", i);
            printf("appo vale %c\n", *appo);
            return 0;
        }
        appo--;
        if ((*appo) == ' ' && *(++appo) == '\0') {
            printf("%s", "Hai inserito uno spazio di troppo dopo l'ultimo importo o tra gli importi\n");

            return 0;
        }
        appo--;
        if ((*appo) == ' ' && *(++appo) == '\n') {
            printf("%s", "Hai inserito uno spazio di troppo dopo l'ultimo importo o tra gli importi\n");

            return 0;
        }
        if (importi[i] == ' ') { //se trovo uno spazio incremento numero importi

            numerimporti++;
        }
        if (!isdigit(importi[j]) && importi[i] != '.') {
            printf("IMPORTO NON VALIDO INSERITO");
            return 0;
        }
        pt++;
        i++;
    }

    numerimporti++;
    if (numerimporti > 5) {

        printf("%s", "Il numero di importi deve essere ninore uguale a 5,"
                " controlla di non aver inserito uno spazio dopo l'ultimo numero'\n");
        return 0;


    }
 if (numerimporti > quantigiocati) {

        printf("%s", "Il numero di importi deve essere minore o uguale ai numeri giocati,"
                " controlla di non aver inserito uno spazio dopo l'ultimo numero'\n");
        return 0;


    }
    //salvo gli importi in giocata
    gt->quanti_imp = numerimporti;

    if (numerimporti == 1)
        sscanf(importi, "%f", &gt->importi[0]);
    if (numerimporti == 2)
        sscanf(importi, "%f %f", &gt->importi[0], &gt->importi[1]);
    if (numerimporti == 3)
        sscanf(importi, "%f %f %f", &gt->importi[0], &gt->importi[1], &gt->importi[2]);

    if (numerimporti == 4)
        sscanf(importi, "%f %f %f %f", &gt->importi[0], &gt->importi[1], &gt->importi[2], &gt->importi[3]);
    if (numerimporti == 5)
        sscanf(importi, "%f %f %f %f %f", &gt->importi[0], &gt->importi[1], &gt->importi[2], &gt->importi[3], &gt->importi[4]);
    //printf("numero importi: %d \n", numerimporti);
    //printf("numero giocati: %d \n", quantigiocati);
    //


    pt = strstr(buffer, "-r");
    pt++;
    pt++;
    if (*(pt) != ' ') {
        printf("%s", "Manca lo spazio dopo il -r \n");
        return false;
    }
    pt++;
    numeroruoteinserite = 0;
    numeroruotecontate = 0;
    while ((*pt) != '-') { //scorro la stringa per contare le ruote inserite
        if ((*pt) == ' ')
            numeroruoteinserite++;
        pt++;

    }
    pt = strstr(buffer, "roma"); //analizzo le ruote presenti nella stringa per vedere se il numero combacia e se ci sono errori 
    //di battitura
    if (pt != NULL) {

        strcat(buffer_par, "roma ");
        strcpy(gt->ruote[numeroruotecontate], "roma");
        numeroruotecontate++;

    }

    pt = strstr(buffer, "napoli");
    if (pt != NULL) {

        strcat(buffer_par, "napoli ");
        strcpy(gt->ruote[numeroruotecontate], "napoli");
        numeroruotecontate++;
    }



    pt = strstr(buffer, "bari");
    if (pt != NULL) {

        strcat(buffer_par, "bari ");
        strcpy(gt->ruote[numeroruotecontate], "bari");
        numeroruotecontate++;
    }
    pt = strstr(buffer, "palermo");
    if (pt != NULL) {

        strcat(buffer_par, "palermo ");
        strcpy(gt->ruote[numeroruotecontate], "palermo");
        numeroruotecontate++;
    }
    pt = strstr(buffer, "cagliari");
    if (pt != NULL) {

        strcat(buffer_par, "cagliari ");
        strcpy(gt->ruote[numeroruotecontate], "cagliari");
        numeroruotecontate++;
    }
    pt = strstr(buffer, "firenze");
    if (pt != NULL) {

        strcat(buffer_par, "firenze ");
        strcpy(gt->ruote[numeroruotecontate], "firenze");
        numeroruotecontate++;
    }
    pt = strstr(buffer, "genova");
    if (pt != NULL) {

        strcat(buffer_par, "genova ");
        strcpy(gt->ruote[numeroruotecontate], "genova");
        numeroruotecontate++;
    }
    pt = strstr(buffer, "tutte");
    if (pt != NULL) {

        strcat(buffer_par, "tutte ");
        strcpy(gt->ruote[numeroruotecontate], "tutte");
        numeroruotecontate++;
        tutte_pres = true;
    }
    pt = strstr(buffer, "milano");
    if (pt != NULL) {

        strcat(buffer_par, "milano ");
        strcpy(gt->ruote[numeroruotecontate], "milano");

        numeroruotecontate++;
    }
    pt = strstr(buffer, "torino");
    if (pt != NULL) {

        strcat(buffer_par, "torino ");
        strcpy(gt->ruote[numeroruotecontate], "torino");
        numeroruotecontate++;
    }
    pt = strstr(buffer, "venezia");
    if (pt != NULL) {

        strcat(buffer_par, "venezia ");
        strcpy(gt->ruote[numeroruotecontate], "venezia");
        numeroruotecontate++;
    }
    pt = strstr(buffer, "nazionale");
    if (pt != NULL) {

        strcat(buffer_par, "nazionale ");
        strcpy(gt->ruote[numeroruotecontate], "nazionale");
        numeroruotecontate++;
    }



    if (numeroruotecontate != numeroruoteinserite) {
        printf("%s", "Controlla di aver digitato bene il nome delle ruote, di averne inserito una inesistente o di uno spazio di troppo\n");
        return 0;
    }


    if (numeroruoteinserite > 1 && tutte_pres == true) {
        printf("%s", "Non puoi inserire altre ruote se hai inserito tutte\n");
        return 0;
    }
    if (tutte_pres == true)
        gt->quante_ruote = 11;
    else
        gt->quante_ruote = numeroruoteinserite;
    strcat(buffer_par, importi);


    printf("%s", "Giocata Corretta\n");
    stampa_giocata(gt);

    return 1;


}

int main(int argc, char* argv[]) {
    if(argc <= 2){
		printf( "Errore: specificare indirizzo IP e Porta\n");
		exit(-1);
	}
    int port = atoi(argv[2]);
     if(port < 1 || port > 65535){
            printf( "Errore: Porta non valida\n");
            exit(-1);
    }

    int ret, sd, len;
    int tentativi = 3;
    int tipo; //serve per tipo di giocata richiesta
    uint16_t lmsg;
    bool CONN_OK; //serve per sapere se è stato effettuato il login
    struct sockaddr_in srv_addr;
    char buffer[DEFAULT_SIZE]; //stringa per memorizzare i dati da inviare e ricevuti
    char comando[DEFAULT_SIZE]; //serve per memorizzare il comando digitato
    char username[DEFAULT_SIZE];
    char password[DEFAULT_SIZE];
    char extra[DEFAULT_SIZE]; //serve per controllare la validità dei comandi
    char parametro[DEFAULT_SIZE]; //serve per controllare i parametri del comando help 
    char session_ID[10];
    struct giocata giocata_cl;
    /* Creazione socket */
    sd = socket(AF_INET, SOCK_STREAM, 0);
    /* Creazione indirizzo del server */
    memset(&srv_addr, 0, sizeof (srv_addr)); // Pulizia 
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(port);
    inet_pton(AF_INET, argv[1], &srv_addr.sin_addr);
    memset(buffer, 0, DEFAULT_SIZE);

    ret = connect(sd, (struct sockaddr*) &srv_addr, sizeof (srv_addr));
    if (ret < 0) {
        perror("Errore in fase di connessione: \n");
        exit(-1);
    }

    ret = recv(sd, (void*) &lmsg, sizeof (uint16_t), 0); //ricevo dim e se connessione è ok o bloccata
    len = ntohs(lmsg); // Rinconverto in formato host
    ret = recv(sd, (void*) buffer, len, 0);

    printf("RICEVUTO %s\n", buffer);
	
    if (ret < 0) {
        perror("Errore in fase di ricezione: \n");
        exit(-1);
    }

    if (strcmp(buffer, "CONNESSIONE_BLOCCATA") == 0) {
        int quanti;
        ret = recv(sd, (void*) &lmsg, sizeof (uint16_t), 0);
        len = ntohs(lmsg); // Rinconverto in formato host
        ret = recv(sd, (void*) &quanti, len, 0); //ricevo il tempo rimasto per lo sblocco
        if (ret < 0) {
            perror("Errore in fase di ricezione: \n");
            exit(-1);
        }
        quanti = quanti / 60;
        printf("Il login è fallito per tre volte quindi il tuo account è stato sospeso per 30 minuti\n");
        printf("Potrai effettuare il login tra %d minuti\n", quanti);

        sleep(8);
        close(sd);
        return 0;
    }

    CONN_OK = false;

    while (1) {

        memset(buffer, 0, DEFAULT_SIZE);
        memset(comando, 0, DEFAULT_SIZE);
        fgets(buffer, DEFAULT_SIZE, stdin);
        sscanf(buffer, "%s", comando);


        if (strcmp(comando, "!help") == 0) {
            memset(comando, 0, DEFAULT_SIZE);
            memset(parametro, 0, DEFAULT_SIZE);
            memset(extra, 0, DEFAULT_SIZE);
            sscanf(buffer, "%s %s %s", comando, parametro, extra);
            if (extra[0] != '\0') {
                printf("INSERISCI COMANDO VALIDO\n");
                continue;
            }

            if (parametro[0] == '\0') {
                showCommands();
                continue;
            }

            if (strcmp(parametro, "!signup") == 0) {
                printf("Il comando signup consente ad un utente di registrarsi fornendo username e passoword \n"
                        "il comando va a buon fine se non ci sono altri utenti con lo stesso username\n"
                        "il comando deve essere seguito dal seguente format: \n"
                        "!signup username password \n");
                continue;
            } else if (strcmp(parametro, "!login") == 0) {
                printf("Il comando login consente ad un utente di loggarsi fornendo username e passoword \n"
                        "Se l'utente invia	credenziali	non	valide,	riceve un messaggio	di errore\n"
                        "il comando deve essere seguito dal seguente format: \n"
                        "!login username password \n");
                continue;
            } else if (strcmp(parametro, "!invia_giocata") == 0) {
                printf("invia una giocata. La giocata è	descritta dalla	schedina, la quale contiene	le	ruote scelte, i	numeri\n"
                        "giocati e gli importi per ogni tipologia di	giocata\n"
                        "il comando deve essere seguito dal seguente format: \n"
                        "!invia_giocata -r ruote -n numeri -i importi \n"
                        "Ruote ammesse: roma napoli milano cagliari palermo genova torino bari firenze venezia nazionale tutte\n"
                        "Numeri ammessi da 1 a 90, tutti i numeri della giocata devono essere diversi\n"
                        "Importi: quantità di importi non superiore alla quantità di numeri giocati \n");
                continue;
            } else if (strcmp(parametro, "!vedi_giocate") == 0) {
                printf("vedi una giocata in corso\n"
                        "il comando deve essere seguito dal seguente format:\n"
                        "!vedi_giocate tipo\n"
                        "tipo=0 per le giocate passate\n"
                        "tipo=1 per le giocate attive\n");
                continue;
            } else if (strcmp(parametro, "!vedi_estrazione") == 0) {
                printf("richiede i numeri estratti nelle ultime n estrazioni su una determinata ruota\n"
                        "il comando deve essere seguito dal seguente format:\n"
                        "!vedi_estrazione n ruota \n"
                        "se non specificata la ruota allora vengono mostrate tutte le ruote\n");
                continue;
            } else if (strcmp(parametro, "!vedi_vincite") == 0) {
                printf("mostra le vincite dell'utente'\n");
                continue;
            } else if (strcmp(parametro, "!esci") == 0) {
                printf("termina l'esecuzione'\n");
                continue;
            } else {
                printf("INSERISCI COMANDO VALIDO\n");
                continue;
            }

        } else if (strcmp(comando, "!invia_giocata") == 0) {

            if (CONN_OK == false) {
                printf("Devi effettuare il login prima di effettuare una giocata\n");
                continue;
            }

            memset(giocata_cl.numeri, 0, (sizeof (int)*10)); //inizializzo giocata
            memset(giocata_cl.ruote, 0, (sizeof (char)*11 * 11));
            memset(giocata_cl.importi, 0, (sizeof (float)*5));
            giocata_cl.quante_ruote = 0;
            giocata_cl.quanti_imp = 0;
            giocata_cl.quanti_num = 0;



            if (checkgiocata(buffer, &giocata_cl) == false) { //controllo e salvo
                printf("GIOCATA ERRATA, REINSERISCILA\n");
                continue;
            } else {


                len = strlen(comando) + 1;
                lmsg = htons(len);
                ret = send(sd, (void*) &lmsg, sizeof (uint16_t), 0); //invio comando
                ret = send(sd, (void*) comando, len, 0);

                if (ret < 0) {
                    perror("Errore in fase di invio: \n");
                    exit(-1);
                }

                len = strlen(session_ID) + 1; //invio session_ID
                lmsg = htons(len);
                ret = send(sd, (void*) &lmsg, sizeof (uint16_t), 0);
                ret = send(sd, (void*) session_ID, len, 0);

                if (ret < 0) {
                    perror("Errore in fase di ricezione: \n");
                    exit(-1);
                }

                memset(buffer, 0, DEFAULT_SIZE);
                ret = recv(sd, (void*) &lmsg, sizeof (uint16_t), 0);

                len = ntohs(lmsg); // Rinconverto in formato host

                ret = recv(sd, (void*) buffer, len, 0); //ricevo riscontro sul session ID

                if (ret < 0) {
                    perror("Errore in fase di ricezione: \n");
                    exit(-1);
                }

                if (strcmp(buffer, "SESSION_ID valido") == 0) {

                    len = (sizeof (int)*13)+(sizeof (char)*11 * 11)+(sizeof (float)*5);
                    lmsg = htons(len);
                    ret = send(sd, (void*) &lmsg, sizeof (uint16_t), 0);
                    ret = send(sd, (void*) &giocata_cl, len, 0);
                    //devo ricevere riscontro su giocata memorizzata
                    memset(buffer, 0, DEFAULT_SIZE);
                    ret = recv(sd, (void*) &lmsg, sizeof (uint16_t), 0);
                    len = ntohs(lmsg); // Rinconverto in formato host
                    ret = recv(sd, (void*) buffer, len, 0);
                    printf("RICEVUTO SU INVIA_GIOCATA:  %s\n", buffer);
                    continue;
                } else {
                    printf("Session Id inviato non è valido\n");
                    continue;
                }
            }

        } else if (strcmp(comando, "!signup") == 0) {
            memset(comando, 0, DEFAULT_SIZE);
            memset(username, 0, DEFAULT_SIZE);
            memset(password, 0, DEFAULT_SIZE);
            sscanf(buffer, "%s %s %s", comando, username, password);

            if (username[0] == '\0' || password[0] == '\0') {
                printf("Hai inserito un campo non valido\n");
                continue;
            }

            len = strlen(buffer) + 1; // invio user e password
            lmsg = htons(len);

            ret = send(sd, (void*) &lmsg, sizeof (uint16_t), 0);

            ret = send(sd, (void*) buffer, len, 0);
            if (ret < 0) {
                perror("Errore in fase di send: \n");
                exit(-1);
            }
            // Attendo risposta
            ret = recv(sd, (void*) &lmsg, sizeof (uint16_t), 0);

            len = ntohs(lmsg); // Rinconverto in formato host

            ret = recv(sd, (void*) buffer, len, 0);

            if (ret < 0) {
                perror("Errore in fase di ricezione: \n");
                exit(-1);
            }

            if (strcmp(buffer, "Username esistente") == 0) { //se esiste l'utente il client invia continuamente nuovi user e pass finché non sono validi
                while (1) {
                    printf("Username esistente, reinserisci username e password:\n");
                    fgets(buffer, DEFAULT_SIZE, stdin);
                    memset(username, 0, DEFAULT_SIZE);
                    memset(password, 0, DEFAULT_SIZE);
                    sscanf(buffer, "%s %s", username, password);
                    if (username[0] == '\0' || password[0] == '\0') {
                        printf("Hai inserito un campo non valido\n");
                        continue;
                    }

                    len = strlen(buffer) + 1; // Voglio inviare anche il carattere di fine stringa
                    lmsg = htons(len);

                    ret = send(sd, (void*) &lmsg, sizeof (uint16_t), 0);

                    ret = send(sd, (void*) buffer, len, 0);
                    if (ret < 0) {
                        perror("Errore in fase di send: \n");
                        exit(-1);
                    }
                    memset(buffer, 0, DEFAULT_SIZE);
                    // Attendo risposta
                    ret = recv(sd, (void*) &lmsg, sizeof (uint16_t), 0);

                    len = ntohs(lmsg); // Rinconverto in formato host

                    ret = recv(sd, (void*) buffer, len, 0);
                    if (ret < 0) {
                        perror("Errore in fase di ricezione: \n");
                        exit(-1);
                    }
                    if (strcmp(buffer, "Registrazione effettuata") == 0) {
                        break;
                    }

                }

            }


            printf("RICEVUTO SIGNUP %s\n", buffer);
            continue;

        } else if (strcmp(comando, "!login") == 0) {
            if (CONN_OK == true) {
                printf("Hai già effettuato il login\n");
                continue;
            }
            memset(comando, 0, DEFAULT_SIZE);
            memset(username, 0, DEFAULT_SIZE);
            memset(password, 0, DEFAULT_SIZE);
            sscanf(buffer, "%s %s %s", comando, username, password);

            if (username[0] == '\0' || password[0] == '\0') {
                printf("Hai inserito un campo non valido\n");
                continue;
            }
            len = strlen(buffer) + 1; // Voglio dim , password e user
            lmsg = htons(len);

            ret = send(sd, (void*) &lmsg, sizeof (uint16_t), 0);

            ret = send(sd, (void*) buffer, len, 0);
            if (ret < 0) {
                perror("Errore in fase di send: \n");
                exit(-1);
            }
            memset(buffer, 0, DEFAULT_SIZE);
            // Attendo risposta
            ret = recv(sd, (void*) &lmsg, sizeof (uint16_t), 0);

            len = ntohs(lmsg); // Rinconverto in formato host

            ret = recv(sd, (void*) buffer, len, 0);
            if (ret < 0) {
                perror("Errore in fase di ricezione: \n");
                exit(-1);
            }
            if (strcmp(buffer, "USER_LOGGED") == 0) {

                printf("Hai già effettuato il login da altro terminale\n");
                continue;
            }

            if (strcmp(buffer, "LOGIN_NO") == 0) {
                tentativi--;
                printf("Riprova il login, tentativi rimasti %d\n", tentativi);
                continue;
            } else if (strcmp(buffer, "UTENTE_NON_REG") == 0) {
                printf("Utente non registrato\n");
                continue;
            } else if (strcmp(buffer, "LOGIN_BLOCCATO") == 0) {
                printf("Login bloccato per 30 minuti\n");
                sleep(3);
                close(sd);
                return 0;
            } else {

                strcpy(session_ID, buffer);
                printf("Il session id è: %s\n", session_ID);
                CONN_OK = true;

            }

        } else if (strcmp(comando, "!vedi_giocate") == 0) {
            if (CONN_OK == false) {
                printf("Devi effettuare il login prima di vedere una giocata\n");
                continue;
            }


            memset(extra, 0, DEFAULT_SIZE);
            sscanf(buffer, "%s %d %s", comando, &tipo, extra);
            if (extra[0] != '\0' || tipo>1) {
                printf("INSERISCI TIPO VALIDO\n");
                continue;
            }
            len = strlen(buffer) + 1; // Voglio inviare anche il carattere di fine stringa
            lmsg = htons(len);

            ret = send(sd, (void*) &lmsg, sizeof (uint16_t), 0);

            ret = send(sd, (void*) buffer, len, 0); //invio comando e parametro
            if (ret < 0) {
                perror("Errore in fase di send: \n");
                exit(-1);
            }

            len = strlen(session_ID) + 1; //invio session_ID
            lmsg = htons(len);
            ret = send(sd, (void*) &lmsg, sizeof (uint16_t), 0);
            ret = send(sd, (void*) session_ID, len, 0);

            if (ret < 0) {
                perror("Errore in fase di send: \n");
                exit(-1);
            }
            memset(buffer, 0, DEFAULT_SIZE);
            ret = recv(sd, (void*) &lmsg, sizeof (uint16_t), 0);

            len = ntohs(lmsg); // Rinconverto in formato host

            ret = recv(sd, (void*) buffer, len, 0); //ricevo riscontro sul session ID
            if (ret < 0) {
                perror("Errore in fase di ricezione: \n");
                exit(-1);
            }

            if (strcmp(buffer, "SESSION_ID valido") == 0) {
                int count = 0;
                while (1) {
                    memset(buffer, 0, DEFAULT_SIZE);
                    memset(giocata_cl.numeri, 0, (sizeof (int)*10));
                    memset(giocata_cl.ruote, 0, (sizeof (char)*11 * 11));
                    memset(giocata_cl.importi, 0, (sizeof (float)*5));
                    giocata_cl.quante_ruote = 0;
                    giocata_cl.quanti_imp = 0;
                    giocata_cl.quanti_num = 0;

                    ret = recv(sd, (void*) &lmsg, sizeof (uint16_t), 0); //ricevo se sta arrivando una giocata e la giocata

                    len = ntohs(lmsg);
                    ret = recv(sd, (void*) buffer, len, 0);

                    if (ret < 0) {
                        perror("Errore in fase di ricezione: \n");
                        exit(-1);
                    }
                    if (strcmp(buffer, "ANCORA") != 0) {
                        if (count == 0)
                            printf("Nessuna_giocata disponibile\n");
                        break;
                    }

                    ret = recv(sd, (void*) &lmsg, sizeof (uint16_t), 0);
                    len = ntohs(lmsg);
                    ret = recv(sd, (void*) &giocata_cl, len, 0);
                    if (ret < 0) {
                        perror("Errore in fase di ricezione: \n");
                        exit(-1);
                    }
                    stampa_giocata(&giocata_cl);
                    count++;
                }

            } else {
                printf("Session Id inviato non è valido\n");
                continue;
            }
        } else if (strcmp(comando, "!esci") == 0) {

            if (CONN_OK == false) {
                printf("Devi effettuare il login prima di chiudere la connessione\n");
                continue;
            }

            len = strlen(buffer) + 1;
            lmsg = htons(len);
            ret = send(sd, (void*) &lmsg, sizeof (uint16_t), 0);
            ret = send(sd, (void*) buffer, len, 0);
            if (ret < 0) {
                perror("Errore in fase di send: \n");
                exit(-1);
            }

            len = strlen(session_ID) + 1; //invio session_ID
            lmsg = htons(len);
            ret = send(sd, (void*) &lmsg, sizeof (uint16_t), 0);
            ret = send(sd, (void*) session_ID, len, 0);
            if (ret < 0) {
                perror("Errore in fase di send: \n");
                exit(-1);
            }
            // Attendo risposta di uscita
            ret = recv(sd, (void*) &lmsg, sizeof (uint16_t), 0);
            len = ntohs(lmsg);
            ret = recv(sd, (void*) buffer, len, 0);

            if (ret < 0) {
                perror("Errore in fase di ricezione: \n");
                exit(-1);
            }
            if (strcmp(buffer, "Addio") == 0) {
                printf("RICEVUTO %s\n", buffer);
                sleep(3);
                break;
            } else {

                printf("Inviato session ID non valido\n");
                continue;
            }

        } else if (strcmp(comando, "!vedi_vincite") == 0) {
            if (CONN_OK == false) {
                printf("Devi effettuare il login prima di vedere le vincite\n");
                continue;
            }
            len = strlen(buffer) + 1; // Voglio inviare anche il carattere di fine stringa
            lmsg = htons(len);

            ret = send(sd, (void*) &lmsg, sizeof (uint16_t), 0);
            ret = send(sd, (void*) buffer, len, 0); //invio comando e parametro
            if (ret < 0) {
                perror("Errore in fase di send: \n");
                exit(-1);
            }

            len = strlen(session_ID) + 1; //invio session_ID
            lmsg = htons(len);
            ret = send(sd, (void*) &lmsg, sizeof (uint16_t), 0);
            ret = send(sd, (void*) session_ID, len, 0);
            if (ret < 0) {
                perror("Errore in fase di send: \n");
                exit(-1);
            }


            ret = recv(sd, (void*) &lmsg, sizeof (uint16_t), 0);
            len = ntohs(lmsg); // Rinconverto in formato host
            ret = recv(sd, (void*) buffer, len, 0); //ricevo riscontro sul session ID
            if (ret < 0) {
                perror("Errore in fase di ricezione: \n");
                exit(-1);
            }

            if (strcmp(buffer, "SESSION_ID valido") == 0) {
                int count = 0;
                while (1) {

                    ret = recv(sd, (void*) &lmsg, sizeof (uint16_t), 0); //ricevo giocata
                    len = ntohs(lmsg);
                    ret = recv(sd, (void*) buffer, len, 0);
                    if (ret < 0) {
                        perror("Errore in fase di ricezione: \n");
                        exit(-1);
                    }

                    if (strcmp(buffer, "ANCORA") != 0) {
                        if (count == 0)
                            printf("NESSUNA VINCITA\n");
                        break;
                    }

                    ret = recv(sd, (void*) &lmsg, sizeof (uint16_t), 0);
                    len = ntohs(lmsg);
                    ret = recv(sd, (void*) buffer, len, 0);
                    if (ret < 0) {
                        perror("Errore in fase di ricezione: \n");
                        exit(-1);
                    }

                    printf("%s", buffer);
                    count++;
                }
                printf("************************************************\n");

                ret = recv(sd, (void*) &lmsg, sizeof (uint16_t), 0);
                len = ntohs(lmsg);
                ret = recv(sd, (void*) buffer, len, 0); //ricevo consuntivo
                if (ret < 0) {
                    perror("Errore in fase di ricezione: \n");
                    exit(-1);
                }

                float a = 0;
                float b = 0;
                float c = 0;
                float d = 0;
                float e = 0;
                sscanf(buffer, "%f %f %f %f %f", &a, &b, &c, &d, &e);
                printf("Vincite su estratto: %f\n", a);
                printf("Vincite su ambo: %f\n", b);
                printf("Vincite su terno: %f\n", c);
                printf("Vincite su quaterna: %f\n", d);
                printf("Vincite su cinquina: %f\n", e);
                printf("************************************************\n");
            } else {
                printf("session id non valido\n");
            }

        } else if (strcmp(comando, "!vedi_estrazione") == 0) {
            if (CONN_OK == false) {
                printf("Devi effettuare il login prima di effettuare una giocata\n");
                continue;
            }
            memset(comando, 0, DEFAULT_SIZE);
            memset(parametro, 0, DEFAULT_SIZE);
            memset(extra, 0, DEFAULT_SIZE);
			int par;
			sscanf(buffer, "%s %s %s", comando, parametro, extra);
			if(parametro[0]=='\0'){
				printf("DEVI INSERIRE IL NUMERO DELL'ESTRAZIONE\n");
				continue;
			}
			par=atoi(parametro);
			if(par==0)	{		
			
				printf("DEVI INSERIRE un numero dopo !vedi_estrazione\n");
				continue;
			}
            
            if (check(extra) == false) {
                printf("ruota inesistente, inserisci il nome della ruota in minuscolo\n");
                continue;
            }

            len = strlen(buffer) + 1;
            lmsg = htons(len);
            ret = send(sd, (void*) &lmsg, sizeof (uint16_t), 0);
            ret = send(sd, (void*) buffer, len, 0); //invio comando
            if (ret < 0) {
                perror("Errore in fase di send: \n");
                exit(-1);
            }

            len = strlen(session_ID) + 1; //invio session_ID
            lmsg = htons(len);
            ret = send(sd, (void*) &lmsg, sizeof (uint16_t), 0);
            ret = send(sd, (void*) session_ID, len, 0);
            if (ret < 0) {
                perror("Errore in fase di send: \n");
                exit(-1);
            }


            ret = recv(sd, (void*) &lmsg, sizeof (uint16_t), 0);
            len = ntohs(lmsg); // Rinconverto in formato host
            ret = recv(sd, (void*) buffer, len, 0); //ricevo riscontro sul session ID
            if (ret < 0) {
                perror("Errore in fase di ricezione: \n");
                exit(-1);
            }

            if (strcmp(buffer, "SESSION_ID valido") == 0) {
                while (1) {

                    ret = recv(sd, (void*) &lmsg, sizeof (uint16_t), 0); //ricevo estrazione e dimensione
                    len = ntohs(lmsg);
                    ret = recv(sd, (void*) buffer, len, 0);
                    if (ret < 0) {
                        perror("Errore in fase di ricezione: \n");
                        exit(-1);
                    }

                    if (strcmp(buffer, "FINE") == 0 || strcmp(buffer, "NUM_GRANDE") == 0) {
                        if (strcmp(buffer, "NUM_GRANDE") == 0)
                            printf("Hai chiesto un numero di estrazioni troppo elevato\n");
                        break;
                    }
                    printf("%s", buffer);

                }


            }
        } else {
            printf("Inserisci comando valido\n");
            continue;
        }

    }
    close(sd);

}


