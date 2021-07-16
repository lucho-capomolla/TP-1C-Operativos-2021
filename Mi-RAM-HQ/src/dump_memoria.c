#include "dump_memoria.h"

void iniciar_archivo_dump(void) {

	char* path_dump = temporal_get_string_time("Dump_%y%m%d%H%M%S.dmp");
	strcat(path_dump, "\0");

	dump_memoria = malloc(sizeof(archivo_dump));

	dump_memoria->tamanio_path = strlen(path_dump);
	dump_memoria->path_dump = malloc(dump_memoria->tamanio_path+1);
	strcpy(dump_memoria->path_dump, path_dump);

	dump_memoria->sem_dump = malloc(sizeof(sem_t));
	sem_init(dump_memoria->sem_dump, 0, 1);
}



void iniciar_dump_memoria(void) {

	log_info(logger, "Se solicitó dump de memoria.");

	FILE* archivo = fopen(dump_memoria->path_dump, "w");

	fwrite("Dump: ", strlen("Dump: "), 1, archivo);
	char* tiempo = temporal_get_string_time("%d/%m/%y %H:%M:%S");
	char* tiempo_escrito = string_new();
	string_append_with_format(&tiempo_escrito, "%s\n", tiempo);

	fwrite(tiempo_escrito, strlen(tiempo_escrito), 1, archivo);

	free(tiempo);
	free(tiempo_escrito);

	fclose(archivo);

	if(esquema_elegido == 'S') {
		registrar_dump_segmentacion();
	}
	else if(esquema_elegido == 'P') {
		//registrar_dump_paginacion();
	}
	else {
		log_error(logger, "No hay ningún esquema de memoria elegido.\n");
	}
}

// TODO: en vez de hacerlo por los segmentos de la patota, hacerlo con la lista de los segmentos totales
/*
void registrar_dump_segmentacion(void) {

	t_list* patotas_dump = list_create();
	t_list* segmentos_dump = list_create();
	patotas_dump = list_duplicate(tablas_segmentos);
	list_sort(patotas_dump, menor_a_mayor_por_pid);

	for(int i=0; i<list_size(patotas_dump); i++){

		t_tabla_segmentos_patota* patota_a_mostrar = (t_tabla_segmentos_patota*) list_get(patotas_dump, i);
		segmentos_dump = list_duplicate(patota_a_mostrar->segmentos);
		list_sort(segmentos_dump, menor_a_mayor_por_segmento);

		for(int j=0; j<list_size(segmentos_dump); j++) {
			t_segmento* segmento_a_mostrar = (t_segmento*) list_get(segmentos_dump, j);

			char* buffer = string_new();
			string_append_with_format(&buffer, "Proceso: %u     ", patota_a_mostrar->patota->pid);
			string_append_with_format(&buffer, "Segmento: %u     ", segmento_a_mostrar->numero_de_segmento);	// Muesta el numero de segmento
			//string_append_with_format(&buffer, "Segmento: %u     ", j+1);	// Arranca desde 1
			string_append_with_format(&buffer, "Inicio: %06p     ", segmento_a_mostrar->inicio);
			string_append_with_format(&buffer, "Tam: %ub\n", segmento_a_mostrar->tamanio_segmento);

			escribir_en_archivo(buffer);
		}
		list_clean(segmentos_dump);
	}
}*/

void registrar_dump_segmentacion(void) {

	t_list* patotas_dump = list_create();
	t_list* segmentos_dump = list_create();
	patotas_dump = list_duplicate(tablas_segmentos);
	list_sort(patotas_dump, menor_a_mayor_por_pid);


	t_list* segmentos_totales = list_duplicate(segmentos);
	list_sort(segmentos_totales, menor_a_mayor_por_segmento);

	for(int j=0; j<list_size(segmentos_totales); j++) {

		t_segmento* segmento_a_mostrar = (t_segmento*) list_get(segmentos_totales, j);

		bool esta_en_patota(void* segmento) {
			return ((t_segmento*)segmento)->numero_de_segmento == segmento_a_mostrar->numero_de_segmento;
		}

		bool buscando_en_patota(void* patota) {
			t_list* segmentos_patota = ((t_tabla_segmentos_patota*)patota)->segmentos;
			return list_any_satisfy(segmentos_patota, esta_en_patota);
		}

		t_tabla_segmentos_patota* patota_encontrada = list_find(patotas_dump, buscando_en_patota);

		if(patota_encontrada != NULL) {
			char* buffer = string_new();
			string_append_with_format(&buffer, "Proceso: %u     ", patota_encontrada->patota->pid);
			string_append_with_format(&buffer, "Segmento: %u     ", segmento_a_mostrar->numero_de_segmento);
			string_append_with_format(&buffer, "Inicio: %06p     ", segmento_a_mostrar->inicio);
			string_append_with_format(&buffer, "Tam: %ub\n", segmento_a_mostrar->tamanio_segmento);

			escribir_en_archivo(buffer);
		}
		else {
			char* buffer = string_new();
			string_append_with_format(&buffer, "Proceso: -     ");
			string_append_with_format(&buffer, "Segmento: %u     ", segmento_a_mostrar->numero_de_segmento);
			string_append_with_format(&buffer, "Inicio: %06p     ", segmento_a_mostrar->inicio);
			string_append_with_format(&buffer, "Tam: %ub\n", segmento_a_mostrar->tamanio_segmento);

			escribir_en_archivo(buffer);
		}
	}
}



/*
void registrar_dump_paginacion(void) {

	t_list* patotas_dump = list_create();
	t_list* paginas_dump = list_create();

	patotas_dump = list_duplicate(tablas_paginas);
	list_sort(patotas_dump, menor_a_mayor_por_frame);

	for(int i=0; i<list_size(patotas_dump); i++) {

		t_tabla_paginas_patota* patota_a_mostrar = list_get(patotas_dump, i);
		paginas_dump = list_duplicate(patota_a_mostrar->paginas);

		t_pagina* pagina_a_mostrar = list_get(tablas_paginas, i);

		char* buffer = string_new();
		string_append_with_format(&buffer, "Marco: %u     ", pagina_a_mostrar->numero_de_frame);

		if(pagina_a_mostrar->estado_pagina == OCUPADO) {
			string_append_with_format(&buffer, "Estado: Ocupado     ");
			string_append_with_format(&buffer, "Proceso: %u     ", patota_a_mostrar->patota->pid);
			string_append_with_format(&buffer, "Pagina: %u\n", pagina_a_mostrar->numero_de_pagina);
		}
		else if (pagina_a_mostrar->estado_pagina == LIBRE){
			string_append_with_format(&buffer, "Estado: Libre     ");
			string_append_with_format(&buffer, "Proceso: -     ");
			string_append_with_format(&buffer, "Pagina: -\n");
		}
		else {
		}
		escribir_en_archivo(buffer);
	}
}*/


void escribir_en_archivo(char* buffer){
	sem_wait(dump_memoria->sem_dump);
	FILE* archivo = fopen(dump_memoria->path_dump, "r+");

	fseek(archivo, 0, SEEK_END);

	fwrite(buffer,strlen(buffer), 1, archivo);

	fclose(archivo);

	sem_post(dump_memoria->sem_dump);
	free(buffer);
}
