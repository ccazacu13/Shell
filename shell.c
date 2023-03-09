// COMPILE using:  gcc shell.c -o shell -lreadline
// RUN using: ./shell on linux

//// Cazacu Cristian-Gabriel, Pasare Roxana-Francisca, Cucos Maria-Marianita
// grupa 243
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <readline/readline.h>
#define NSize 1024

char ** words;
char *command_line, *output, *p, *nr_comanda, *istoric;
char current_path[NSize];
int nr_comenzi, nr_words, got_error;

// --- Comenzi ---
// touch fisier | makedir
// echo sir de caractere | echo
// echo folder | makedir

// lss || ls
// cd nfm || ls || history
// cd nfm || ls && history

// cd nfm && ls
// help && ls
// ls && lss && help
// ls && cd nfm || ls

void hist(){
    printf("%s", istoric);
}

void clear(){
	write(1, "\33[H\33[2J", 7); // escape sequence pentru eliberarea terminalului
}

void cd(char* folder){
    if (chdir(folder)){
        got_error = 1;
    }
}

void pwd(){
    if (getcwd(current_path, sizeof(current_path))) { // se obtine current path-ul cu ajutorul unei functii
        strcat(output, current_path);
        printf("%s\n", output);
    } 
    else{
        got_error = 2;
    }
}

void ls(){
    pid_t pid = fork (); // se creeaza un proces nou pentru executarea functiei ls din bin
    if (pid == 0){
        char *arguments[] = {"ls", NULL}; 
        execve ("/bin/ls", arguments , NULL);
        exit(0);
    }
    else{
        int status;
        wait(NULL);
    }
}

void touch(char* file){
    FILE *aux;
    aux = fopen(file, "w"); // se creeaza un nou fisier cu permisiune de scriere si se deschide cu functia fopen
    if (aux){
        strcat(output, "Fisierul a fost creat cu succes!"); 
        // printf("Fisierul a fost creat cu succes!\n");
        puts(output + 5);
    }
    else{
        got_error = 7;
    }
    fclose(aux);
}

void rm (char* filename){
    char file_path[1024];
    if (getcwd(file_path, sizeof(file_path)) == NULL) { // se obtine calea curenta 
        got_error = 2;
    }

    // adaugam numele fisierul ce trebuie sters la current path pentru a il sterge
    strcat(file_path, "/");
    strcat(file_path, filename);

    if(!remove(file_path)){
        strcat(output, "Fisierul a fost sters cu succes!");
        printf("%s\n", output + 5);
	}
	else{
		got_error = 8;
	}
}

void cp(char* file1, char* file2){
	char curr_car;
	FILE *f1, *f2;
	
	if ((f1 = fopen(file1, "r")) == 0){ // deschidem fisierul sursa pentru citire
		got_error = 3;
        return;
	}

    if ((f2 = fopen(file2, "w")) == 0){ // deschidem fisierul de destinatie pentru scriere
		got_error = 4;
        return;
	}

	curr_car = fgetc(f1); // citim continutul primului fisier caracter cu caracter

	while(curr_car != EOF){
		fputc(curr_car, f2); //scrie in al doilea fisier caracter cu caracter
		curr_car = fgetc(f1);
	}

    strcat(output, "Fisierul a fost copiat cu succes!");
    printf("%s\n", output + 5);

	fclose(f1);
	fclose(f2);
}

void makedir(char* folder){
    if (getcwd(current_path, sizeof(current_path)) == NULL) { // se obtine current path-ul
        got_error = 2;
        return;
    }

    strcat(current_path, "/");
    strcat(current_path, folder);

    if (mkdir(current_path, 0777) == -1){ // se creeaza un director cu permisiuni de scriere, citire si executie (0777)
        got_error = 5;
    }
    else{
        strcat(output, "Directorul a fost creat cu succes!");
        printf("%s\n", output + 5);
    }
}

void removedir(char* folder){
    if (getcwd(current_path, sizeof(current_path)) == NULL) { // se obtine current path-ul
        got_error = 2;
        return;
    }

    strcat(current_path, "/");
    strcat(current_path, folder);

    if(rmdir(current_path) == -1){
        got_error = 6;
    }
    else{
        strcat(output, "Directorul a fost sters cu succes!");
        printf("%s\n", output + 5);
    }
}

void echo(){
    for (int i = 1; i < nr_words; i ++){ // parcurgem toate cuvintele scrise dupa comanda 'echo' si le afisam
        printf("%s ", words[i]);
        strcat(output, words[i]);
        strcat(output, " ");
    }
    printf("\n");
}

void help(){	
	printf("\nComenzi:\n");
	printf("--------------------------------------------------------\n\n");
    printf("history: Afiseaza comenzile apelate in aceasta sesiune \n");
	printf("clear: Curata terminalul \n");
	printf("cd: Schimba path-ul catre un anumit director \n");
	printf("pwd: Afiseaza path-ul catre directorul curent \n");
	printf("ls: Afiseaza toate fisiere si directoarele din directorul curent \n");
    printf("touch: Creeaza un nou fisier fara continut \n");
    printf("rm: Sterge un anumit fisier \n");
	printf("cp: Copiaza continutul unui fisier in alt fisier \n");
	printf("makedir: Creeaza un nou director \n");
	printf("removedir: Sterge un director deja existent \n");
    printf("echo: Afiseaza un sir de caractere corespunzator argumentului transmis \n");
    printf("quit: Inchide shell-ul \n");

	printf("\n-----------------------------------------------------\n\n");
}

void execute(char ** words, int nr_args){ // functia principala care apeleaza functiile de mai sus specifice fiecarei comenzi
    // inainte de executarea oricarei comenzi se testeaza daca numarul de argumente este corect, altfel se va afisa o eroare

    if (!strcmp(words[0], "help")){
        if (nr_args != 1){
            got_error = 9;
            return;
        }
        help();
    }
    else if (!strcmp(words[0], "history")){
        if (nr_args != 1){
            got_error = 9;
            return;
        }
        hist();
    }
    else if (!strcmp(words[0], "clear")){
        if (nr_args != 1){
            got_error = 9;
            return;
        }
        clear();
    }
    else if (!strcmp(words[0], "cd")){
        if (nr_args > 2){
            got_error = 9;
            return;
        }
        if(nr_args == 2)cd(words[1]);
        else cd("..");
    }
    else if (!strcmp(words[0], "pwd")){
        if (nr_args != 1){
            got_error = 9;
            return;
        }
        pwd();
    }
    else if(!strcmp(words[0], "ls")){
        if (nr_args != 1){
            got_error = 9;
            return;
        }
        ls();
    }
    else if (!strcmp(words[0], "touch")){
        if(nr_args != 2){
            got_error = 9;
            return;
        }
        touch(words[1]);
    }
    else if (!strcmp(words[0], "cp")){
        if (nr_args != 3){
            got_error = 9;
            return;
        }
        cp(words[1], words[2]);
    }
    else if (!strcmp(words[0], "makedir")){
        if(nr_args != 2){
            got_error = 9;
            return;
        }
        makedir(words[1]);
    }
    else if (!strcmp(words[0], "removedir")){
        if(nr_args != 2){
            got_error = 9;
            return;
        }
       removedir(words[1]);
    }
    else if (!strcmp(words[0], "rm")){
        if(nr_args != 2){
            got_error = 9;
            return;
        }
        rm(words[1]);
    }

    else if (!strcmp(words[0], "echo")){
        echo();
    }

    else if (!strcmp(words[0], "quit")){
        if (nr_args != 1){
            got_error = 9;
            return;
        }
        exit(0);
    }
    else{
        got_error = 10;
        return;
    }
}

void parse_error(int error_code){ // functie pentru afisarea mesajului specific unei erori
    if(error_code == 1) printf("Nu exista un fisier sau director\n");
    if(error_code == 2) printf("Nu se poate localiza path-ul curent\n");
    if(error_code == 3) printf("Nu poate fi deschis fisierul sursa\n");
    if(error_code == 4) printf("Nu poate fi deschis fisierul destinatie\n");
    if(error_code == 5) printf("Nu poate fi creat directorul\n");
    if(error_code == 6) printf("Nu se poate sterge directorul\n");
    if(error_code == 7) printf("Nu poate fi creat fisierul\n");
    if(error_code == 8) printf("Nu se poate sterge fisierul\n");
    if(error_code == 9) printf("Numar invalid de argumente\n");
    if(error_code == 10) printf("Comanda '%s' nu a fost gasita\n", words[0]);
}

int main(){
    istoric  = malloc(NSize * 100 * sizeof(char));
    output = malloc(NSize * sizeof(char));

    while(1){
        if (getcwd(current_path, sizeof(current_path)) != NULL){
            printf("SHELL: %s$ ", current_path);
        } 
        else{
            got_error = 2;
        }

        command_line = readline("");

        if(!strcmp(command_line, "")){ // putem apasa enter in consola fara sa se intample nimic
            printf("\n");
            continue;
        }

        nr_comenzi++;
        nr_comanda = malloc(5 * sizeof(char));

        sprintf(nr_comanda, "%d", nr_comenzi);

        strcat(istoric, nr_comanda); 
        strcat(istoric, ": ");
        strcat(istoric, command_line); // adaugam comanda curenta in istoric 
        strcat(istoric, "\n");

        words = malloc(NSize * sizeof(char*)); // despartim comanda curenta in cuvinte
        nr_words = 0;

        p = strtok(command_line, " ");

        while(p != NULL){
            char* c = malloc(NSize * sizeof(char)); // trecem prin fiecare cuvant al comenzii
            strcpy(c, p);

            if (!strcmp(c, "|")){ // executa comanda dinainte de pipe
                if (nr_words > 0){
                    free(output);
                    output = malloc(NSize * sizeof(char));
                    execute(words, nr_words);
                }

                p = strtok(NULL, " "); // trece la comanda de dupa pipe

                if (p == NULL){ // cazul in care nu mai exista nicio comanda dupa pipe
                    got_error = 9;
                    continue;
                }
                strcpy(c, p);

                // in vectorul de cuvinte vor fi salvate comanda de dupa pipe si rezultatul primei comenzi
                words[0] = c;
                strcpy(words[1], output + 5);

                nr_words = 2;

                free(output);
                output = malloc(NSize * sizeof(char));
                execute(words, nr_words); // comanda a doua va fi rulata avand ca argument rezultatul primei comenzi

                got_error = -1;

                nr_words = 0;
            }
            else if (!strcmp(c, "||")){
                free(output);
                output = malloc(NSize * sizeof(char));
                execute(words, nr_words);

                if(got_error){ // daca a intampinat o eroare o va ignora deoarece doar prima comanda corecta va rula
                    got_error = 0;
                    nr_words = 0;
                    p = strtok(NULL, " ");
                    continue;
                }
                else{ // cand gaseste prima comanda care nu da eroare le va ignora pe restul
                   nr_words = 0;
                   while(p != NULL){
                        strcpy(c, p);
                        if(!strcmp(c, "&&")){
                            break;
                        }
                        p = strtok(NULL, " ");
                    }
                    if (p == NULL) got_error = -1;
                }
            }
            else if (!strcmp(c, "&&")){
                free(output);
                output = malloc(NSize * sizeof(char));
                execute(words, nr_words);
                
                if (got_error != 0){ // la prima eroare intalnita va opri executia
                    break;
                }

                nr_words = 0;
            }
            else words[nr_words ++] = c;

            p = strtok(NULL, " ");
        }

        if (got_error == 0){ // daca nu am avut o eroare va executa comanda curenta
            free(output);
            output = malloc(NSize * sizeof(char));
            execute(words, nr_words);
        }

        if (got_error != 0){ // daca am avut o eroare se va afisa mesajul corespunzator acesteia
            parse_error(got_error);
            got_error = 0;
        }
    }
    return 0;
}
