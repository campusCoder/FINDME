#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <time.h>

// structure to hold the fields used by each client
struct info{
	char name[20];
	char email[30];
	char coordenates[40];
	char postal[50];
};

// create contents of locations in a list to use as random locations
int create_list(struct info *list, int elements, char *nombre, char *correo){
	for(int x=0; x < elements; x++){
		memset(&list[x], 0, sizeof(struct info));
	}

	for(int x=0; x < elements; x++){
		strcpy(list[x].name, nombre);
		strcpy(list[x].email, correo);
	}

	strcpy(list[0].coordenates, "48d 51m 12s North, 2d 20m 55s East");
	strcpy(list[0].postal, "Rue de la baguette 23, Paris 43029, France");

	strcpy(list[1].coordenates, "60d 10m 15s North, 24d 56m 15s East");
	strcpy(list[1].postal, "helsinginkatu 40, Helsinki 00745, Finland");

	strcpy(list[2].coordenates, "18d 58m 39s North, 72d 49m 33s East");
	strcpy(list[2].postal, "Bollywood street 102, Mumbai 32902, India");

	strcpy(list[3].coordenates, "58d 18m 1s North, 134d 25m 2s West");
	strcpy(list[3].postal, "Polarbear street 34, Juneau 00010, Alaska");

	strcpy(list[4].coordenates, "35d 41m 5s North, 139d 41m 4s East");
	strcpy(list[4].postal, "Anime street 290, Tokio 54032, Japan");

	strcpy(list[5].coordenates, "23d 33m 3s South, 46d 38m 7s West");
	strcpy(list[5].postal, "Bikini street 128, Rio de Janeiro 45930, Brasil");

	strcpy(list[6].coordenates, "39d 55m 9s North, 116d 23m 9s East");
	strcpy(list[6].postal, "Overpopulated street 9999999, Beijing 98272, China");	

	strcpy(list[7].coordenates, "18d 56m 19s South, 47d 31m 17s East");
	strcpy(list[7].postal, "Cool pinguins street 209, Antananarivo 78920, Madagascar");

	strcpy(list[8].coordenates, "33d 52m 10s South, 151d 12m 30s East");
	strcpy(list[8].postal, "Kangaroo street 98, Sidney 23092, Australia");

	strcpy(list[9].coordenates, "30d 3m 1s North, 31d 14m 1s East");
	strcpy(list[9].postal, "Pyramid street 77, Cairo 92039, Egypt");

	return 0;
}

int separate_function(char *to_sepa, char **save, int how_many){
	save[0] = strtok(to_sepa, ":");

	for(int x=1; x < how_many ; x++){
		save[x] = strtok(NULL, ":");
	}

	for(int y=0; y < how_many; y++)
		printf("This is the word #%d: %s\n", y, save[y]);

	return 0;
}

// function to build the string to be sent to the server
int build_location(char *locstr, struct info extract, int num_cl, int indic, int option, char *query, char *query2){
	memset(locstr, 0, (sizeof(char)*strlen(locstr)));
	printf("num_cl is: %d\nindic is: %d\noption is: %d\nquery is: %s\nquery 2 is: %s\n", num_cl, indic, option, query, query2);

	// build the string with a zero as client ID
	if(!num_cl){
		strcpy(locstr, "0$QUERY\n");
		return 0;
	}
	else{

		// option value of zero means the function was called as the initial location advertisement, before user inputs anything
		// here we just update our own data
		if(!option){
			char temp[10] = "";
			sprintf(locstr, "%d$", num_cl);
			strcat(locstr, "UPDATE$NAME$");
			strcat(locstr, extract.name);
			strcat(locstr, "$EMAIL$");
			strcat(locstr, extract.email);
			strcat(locstr, "$ADDRESS$");
			strcat(locstr, extract.postal);
			strcat(locstr, "$LOCATION$");
			strcat(locstr, extract.coordenates);
			strcat(locstr, "$TIMESTAMP$");
			sprintf(temp, "%d\n", indic);
			strcat(locstr, temp);
			return 0;
		}

		// this part is when user requests either a query or an update
		else{

			// build the query with a name to be resolved
			if(option == 1){
				sprintf(locstr, "%d$", num_cl);
				strcat(locstr, "QUERY$NAME$");
				strcat(locstr, query);
				strcat(locstr, "$LOCATION$");
				strcat(locstr, extract.coordenates);
				strcat(locstr, "\n");
				return 0;
			}

			// build the query with an email address to be resolved
			else if(option == 2){
				sprintf(locstr, "%d$", num_cl);
				strcat(locstr, "QUERY$EMAIL$");
				strcat(locstr, query);
				strcat(locstr, "$LOCATION$");
				strcat(locstr, extract.coordenates);
				strcat(locstr, "\n");
				return 0;
			}
			else if(option == 3){
				char temp[10] = "";
				sprintf(locstr, "%d$", num_cl);
				strcat(locstr, "UPDATE$NAME$");
				strcat(locstr, query);
				strcat(locstr, "$EMAIL$");
				strcat(locstr, query2);
				strcat(locstr, "$ADDRESS$");
				strcat(locstr, extract.postal);
				strcat(locstr, "$LOCATION$");
				strcat(locstr, extract.coordenates);
				strcat(locstr, "$TIMESTAMP$");
				sprintf(temp, "%d\n", indic);
				strcat(locstr, temp);
				return 0;				
			}
		}
	}
	return -1;
}

int main(){
	struct sockaddr_in serv;
	memset(&serv, 0, sizeof(serv));
	int option = 0, sokk = 0, ran_loc = 0, id = 0, indic = 0;
	char location[101] = "", query[50] = "", answer[250] = "", update[70] = "", **save = NULL, name[20] = "", email[25] = "";
	struct info arr[6];
	FILE *idfile = NULL;
	socklen_t temp_size;

	// open a socket, declare what kind it is and save IP and port into sockaddr_in structure
	sokk = socket(AF_INET, SOCK_DGRAM, 0);
	serv.sin_family = AF_INET;
	serv.sin_port = htons(7000);
	inet_pton(AF_INET, "127.0.0.1", &serv.sin_addr);

	// check if this client has already an ID assigned
	idfile = fopen("ID_file", "r");

	// no ID assigned, fopens fails because the file doesn't exist
	if(idfile == NULL){
		perror("This client has not yet assigned an ID:");

		// build a string with zero as ID
		if(build_location(location, arr[ran_loc], id, indic, 0, NULL, NULL) != 0){
			printf("Error while building the string to be sent to the server, idfile = NULL\n");
			return -1;
		}

		// send the string to the server containing zero as ID
		if(strlen(location) != sendto(sokk, location, strlen(location), 0, (struct sockaddr *) &serv, sizeof(serv))){
			perror("some error happened when sending zero as ID, the location information could not be sent properly (id is 0):");
			return -1;
		}

		// read from the server what ID it assigns to this client and save it into a file
		temp_size = sizeof(serv);
		if(recvfrom(sokk, &id, sizeof(int), 0, (struct sockaddr*)&serv, &temp_size) == -1){	// modify this in order to decode properly the answer from server
			perror("some error happened at reading the ID sent by the server:");
			return -1;
		}
		idfile = fopen("ID_file", "w");
		if(idfile == NULL){
			perror("An error happened when creating the ID_file in order to write the newly assigned ID:");
			return -1;
		}
		if(fwrite(&id, sizeof(id), 1, idfile) != 1){
			printf("The newly assigned ID could not be written into the file\n");
			return -1;
		}

		// When running the client first time, the identity of the user needs to be specified
		printf("This is the first time you use this program, specify who you are:\n");
		printf("What is your name? write it and then press enter:\n");
		scanf("%s", name);
		printf("What is your email? write it and then press enter");
		scanf("%s", email);

	}

	// this client has already an ID, read it from the file and store it in variable id
	else{
		if(fread(&id, sizeof(int), 1, idfile) != 1){
			printf("some error happened at reading the ID stored in the file\n");
			return -1;
		}
		printf("The id read from the file is: %d\n", id);
	}

	// used for timestamp
	indic++;

	// create array of different locations
	if(create_list(arr, 10, name, email) != 0){
		printf("An error happened while creating the locations\n");
		return -1;
	}

	// pick a location randomly to use as current location
	srand(time(NULL));
	ran_loc = rand() % 10;

	// build a string containing the location information (first mandatory update)
	if(build_location(location, arr[ran_loc], id, indic, 0, NULL, NULL) != 0){
		printf("Error while building the string to be sent to the server, ID_file exists\n");
		return -1;
	}
	printf("The string to be sent to the server as initial location is (with exiting idfile): %s\n", location);

	// send location information to the server (first mandatory update)
	ssize_t first_sent = sendto(sokk, location, strlen(location), 0, (struct sockaddr *) &serv, sizeof(serv));
	if(strlen(location) != first_sent){
		perror("some error happened while sending location info to the server, the location information could not be sent properly:");
		return -1;
	}

	// greet the user and ask what operation s/he wants
	printf("Please select the number from the following options that best suits your request\n");
	printf("1) I have a name and want to get the rest of the information\n");
	printf("2) I have an email address and want to get the rest of the information\n");
	printf("3) I want to update my information\n");
	printf("4) I'm done, Exit\n");

	for( ; ; ){
		scanf("%d", &option);
		indic++;
		if(option == 1){

			// ask the user to input the information s/he has
			printf("Please write the name from which you want to get information and press enter\n");
			scanf("%s", query);

			// build the query string with name as parameter
			if(build_location(location, arr[ran_loc], id, indic, option, query, NULL) != 0){
				printf("Error while creating string for query by name\n");
				return -1;
			}
			printf("The string just built is (for, option 1): %s", location);

			// send the information to the server
			if(strlen(location) != sendto(sokk, location, strlen(location), 0, (struct sockaddr *) &serv, sizeof(serv))){
				printf("some error happened, the query could not be sent to the server\n");
				return -1;
			}
			printf("All good, waiting for the server query answer\n");

			// read the answer from the server
			temp_size = sizeof(serv);
			if(recvfrom(sokk, answer, sizeof(answer), 0, (struct sockaddr *) &serv, &temp_size) == -1){
				perror("some error happened at reading the answer of the query\n");
				return -1;
			}

			// display the info of the query to the user
			printf("%s\n", answer);
			continue;
		}
		else if(option == 2){

			// ask the user to input the information s/he has
			printf("Please write the email address from which you want to get information and press enter\n");
			scanf("%s", query);

			// build the query string with email as parameter
			if(build_location(location, arr[ran_loc], id, indic, option, query, NULL) != 0){
				printf("Error while creating string for query by email\n");
				return -1;
			}
			printf("The string just built is (for, option 2): %s", location);

			// send the information to the server
			if(strlen(location) != sendto(sokk, location, strlen(location), 0, (struct sockaddr *) &serv, sizeof(serv))){
				printf("some error happened, the query could not be sent to the server\n");
				return -1;
			}
			printf("All good, waiting for the server query answer\n");

			// read the answer from the server
			temp_size = sizeof(serv);
			if(recvfrom(sokk, answer, sizeof(answer), 0, (struct sockaddr *) &serv, &temp_size) == -1){
				perror("some error happened at reading the answer of the query\n");
				return -1;
			}

			// display the info of the query to the user
			printf("%s\n", answer);
			continue;
		}
		else if(option == 3){

			// Tell the user to input the information in the format <name:email>
			printf("Please write the updated information in the format <name:email>");
			scanf("%s", update);

			// Separate the words in the given string
			separate_function(update, save, 3);

			// build the update string with the new name or email
			if(build_location(location, arr[ran_loc], id, indic, option, save[0], save[1]) != 0){
				printf("Error when creating a string for an update\n");
				return -1;
			}
			printf("The string just built is (for, option 3): %s", location);

			// send the information to the server
			if(strlen(location) != sendto(sokk, location, strlen(location), 0, (struct sockaddr *) &serv, sizeof(serv))){
				printf("some error happened, the query could not be sent to the server\n");
				return -1;
			}
			printf("All good, waiting for the server query answer\n");

			// read the answer from the server
			temp_size = sizeof(serv);
			if(recvfrom(sokk, answer, sizeof(answer), 0, (struct sockaddr *) &serv, &temp_size) == -1){
				perror("some error happened at reading the answer of the query\n");
				return -1;
			}

			// display the info of the query to the user
			printf("%s\n", answer);
			continue;
		}
		else if(option == 4)
			break;
		else
			printf("The option you provided is invalid, please indicate the desired option by its number (1, 2, 3 or 4)");
	}

	// close everything and exit
	close(sokk);
	return 0;
}

/*	following steps to do

/	create a function to separate the fields from the server answer to then be printed to screen
/	take care of filtering out the $ signs in the input from the user
/	when integrating with the server, change IP address and port

*/
