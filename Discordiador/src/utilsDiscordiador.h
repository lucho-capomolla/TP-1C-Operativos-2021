#ifndef UTILSDISCORDIADOR_H_
#define UTILSDISCORDIADOR_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include "commons/log.h"
#include "commons/string.h"
#include "commons/config.h"
#include "Discordiador.h"
#include"utils/estructuras.h"

codigo_operacion mapeo_valor_consola(char* comando_ingresado);

uint32_t cantidad_argumentos_ingresados(char** parser_consola);

//t_tcb* crear_tripulante(t_datos_hilo* datos_hilo);
void mostrar_tripulante(t_tcb* tripulante);
void terminar_tripulante(tripulante_plani* tripu);
void procesar_mensajes(codigo_operacion operacion, int32_t conexion);

#endif /* UTILSDISCORDIADOR_H_ */
