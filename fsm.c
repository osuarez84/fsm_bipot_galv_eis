#include <stdint.h>
#include <stdio.h>
#include "fsm.h"

#define NSTATES			60
#define DEBUG			1


/* Variables para el seguimiento de los estados
y eventos que disparan las transiciones */
stateType next_state;
status_I_measure status_I_we1;
status_I_measure status_I_we2;
mode_com communication_mode;
state_experiment experiment;
lut_state lut1A_state;
lut_state lut1B_state;
lut_state lut2A_state;
lut_state lut2B_state;
mode df_mode;
mode mode_working;
state_pretreatment pretreatment;
general_state state_equipment;


/* Testing variables */
uint8_t cont = 0;
stateType lista_estados[200];
char state;
uint32_t n;

// Array que apunta a las funciones que corren en cada estado
void(*state_table[])(void) = {
	conection,
	Idle,
	calibration,
	bipot,
	pot,
	galv,
	eis,
	PrepE,
	Pretreatment,
	Measuring,
	FS_ch,
	Ending,
	Error
};

main() {
	start();


#ifdef DEBUG
	/* Código de testeo para sacar un fichero con el listado de estados */
	FILE *f = fopen("List_states.txt", "w");
	if (f == NULL) {
		printf("Error opening file!\n");
		exit(1);
	}

	/* Para el testeado sustituimos el while(1) por un for de duración
	controlada */
	for (n = 0; n <= NSTATES; n++) {
		state_table[next_state]();

		/* Código para testear y guardar todos los estados por los
		que pasa la FSM según los eventos que hemos programado. */
		//vector1(n);
		vector2(n);
		lista_estados[n] = next_state;
		fprintf(f, "%s\n", printToFile(next_state));

		if (n == NSTATES) {
			fclose(f);
		}

	}

#endif // DEBUG




#ifndef DEBUG
	while (1) {

		state_table[next_state]();
	}
#endif // ~DEBUG 
}





void start() {

	// inicializamos todo el equipo paso a paso
	mode_working = M_NONE;
	df_mode = M_NONE;
	status_I_we1 = I_DEFAULT;
	status_I_we2 = I_DEFAULT;
	communication_mode = C_NONE;
	pretreatment = P_NONE;
	lut1A_state = L_EMPTY;
	lut1B_state = L_EMPTY;
	lut2A_state = L_EMPTY;
	lut2B_state = L_EMPTY;
	experiment = E_NONE;

	next_state = CONECT;

}


void conection() {

	if (communication_mode == C_BT) {
		// Recibimos los datos de conexión
		// Enviamos el ACK al PC
		next_state = IDLE;

	}
	else if (communication_mode == C_USB) {

		// Recibimos los datos de conexión
		// Enviamos el ACK al PC
		next_state = IDLE;
	}

}


void Idle() {

	if (df_mode == M_BIPOT) {
		next_state = BIPOT;
	}
	else if (df_mode == M_POT) {
		next_state = POT;
	}
	else if (df_mode == M_GALV) {
		next_state = GALV;
	}
	else if (df_mode == M_EIS) {
		next_state = EIS;
	}


}


void bipot() {
	mode_working = M_BIPOT;
	next_state = PREP_E;

}

void pot() {
	mode_working = M_POT;
	next_state = PREP_E;
}

void galv() {

	mode_working = M_GALV;
	next_state = PREP_E;
}


void eis() {

	mode_working = M_EIS;
	next_state = PREP_E;
}




void PrepE() {

	/* MODO POT/BIPOT */
	if (mode_working == M_BIPOT) {
		// Configuramos FS
		// Configuramos filtros
		// Generamos primer refresco LUT1 y LUT2
		// lut1A_state = REFRESHED y lut2A_state

		// Habilitamos electródos

		/* Hay que preparar dos LUTs, una hará de buffer y se irá cargando mientras se envía la primera */

		// Aplicamos el pretratamiento que proceda

	}
	else if (mode_working == M_POT) {
		// Configuramos FS auto o no
		// Configuramos filtros
		//Generamos primer refresco de LUT
		// lut1A_state = REFRESHED;

		// Habilitamos electródos

		// Aplicamos el pretratamiento que proceda


	}
	else if (mode_working == M_GALV) {
		// Configuramos FS auto o no
		// Configuramos filtros
		//Generamos primer refresco de LUT
		// lut1A_state = REFRESHED;

		// Habilitamos electródos

		// Aplicamos el pretratamiento que proceda

	}
	else if (mode_working == M_EIS) {
		// TODO

	}

	next_state = PRETREATMENT;
}


void Pretreatment() {

	if (pretreatment == P_FINISHED) {
		// Esperamos a que el pretratamiento termine
		if (mode_working == M_BIPOT) {
			// Enviamos LUT1A a DAC WE1
			// Enviamos LUT2A a DAC WE2

			// Comenzamos siguiente refresco LUT1B
			// Comenzamos siguiente refresco LUT2B

		}
		else if (mode_working == M_POT) {
			// Enviamos LUT1A a DAC WE1

			// Comenzamos siguiente refresco LUT1B

		}
		else if (mode_working == M_GALV) {
			// Enviamos LUT1A a DAC WE1

			// Comenzamos siguiente refresco LUT1B
		}
		else if (mode_working == M_EIS) {
			// TODO

		}

		next_state = MEASURING;
	}

}



void Measuring() {
	// TODO: puede utilizarse el estado de experiment para inicializar
	// todo solamente en la primera entrada al estado Measuring().
	// P.Ej:
	// if(experiment == E_NONE){
	//		+ Inicializame toda la temporización para recoger muestras en el ADC
	//		+ Inicializame toda la temporización para lanzar las muestras al DAC
	//		
	//		experiment = E_RUNNING; => de esta manera no vuelve a entrar en esta parte
	// }
	// else if(experiment == E_RUNNING){
	// ....
	// }
	// else if(experiment == E_FINISHED | experiment == E_CANCELLED){
	// ...
	// }
	// Esto significaría hacer una FSM dentro del propio estado, pasando a trabajar tipo HSM.

	if ((experiment != E_FINISHED) & (experiment != E_CANCELLED)) {
		if (mode_working == M_POT) {


			/* Comprobamos si debemos de refrescar alguna LUT */
			if (lut1A_state == L_FINISHED) {
				// Refrescamos la LUT
				lut1A_state = L_REFRESHED;
			}
			else if (lut1B_state == L_FINISHED) {
				// Refrescamos la LUT
				lut1A_state = L_REFRESHED;
			}


			/* Comprobamos si debemos cambiar el FS */
			if (status_I_we1 != I_DEFAULT) {
				next_state = FS_CH;
			}

		}
		else if (mode_working == M_BIPOT) {
			if (lut1A_state == L_FINISHED) {
				// Refrescamos la LUT
				lut1A_state = L_REFRESHED;
			}
			else if (lut1B_state == L_FINISHED) {
				// Refrescamos la LUT
				lut2A_state = L_REFRESHED;
			}
			if (lut1B_state == L_FINISHED) {
				// Refrescamos la LUT
				lut1B_state = L_REFRESHED;
			}
			else if (lut2B_state == L_FINISHED) {
				// Refrescamos la LUT
				lut2B_state = L_REFRESHED;
			}

			if ((status_I_we1 != I_DEFAULT) | (status_I_we2 != I_DEFAULT)) {
				next_state = FS_CH;
			}
		}
		else if (mode_working == M_GALV) {
			/* Comprobamos si debemos de refrescar alguna LUT */
			if (lut1A_state == L_FINISHED) {
				// Refrescamos la LUT
				lut1A_state = L_REFRESHED;
			}
			else if (lut1B_state == L_FINISHED) {
				// Refrescamos la LUT
				lut2A_state = L_REFRESHED;
			}
			if (lut1B_state == L_FINISHED) {
				// Refrescamos la LUT
				lut1B_state = L_REFRESHED;
			}

			/* Comprobamos si debemos cambiar el FS */
			if (status_I_we1 != I_DEFAULT) {
				next_state = FS_CH;
			}
		}
		else if (mode_working == M_EIS) {
			// TODO
		}
	}
	else {
		next_state = ENDING;
	}




}


// TODO: comprobamos si se ha habilitado MAS de un FS. En caso de que haya solamente uno, no debemos entrar en este estado.
void FS_ch() {

	if (mode_working == M_POT) {
		if (status_I_we1 == I_SATURA) {
			// Cambiamos FS WE1 a un FS mayor
			// Reseteamos FLAG_SATURA_WE1 => flag_satura_we1 = DEFAULT;
		}
		else if (status_I_we1 == I_BELOW_THRESHOLD) {
			// Cambiamos FS WE1 a una FS menor
			// Reseteamos status_I_we1 => status_I_we1 = DEFAULT;

		}

		status_I_we1 = I_DEFAULT;		// Reseteamos el flag

	}
	else if (mode_working == M_BIPOT) {
		if (status_I_we1 == I_SATURA) {
			// Cambiamos FS WE1 a un FS mayor
			// Reseteamos la status_I_we1 => status_I_we1 = DEFAULT;
		}
		else if (status_I_we1 == I_BELOW_THRESHOLD) {
			// Cambiamos FS WE1 a un FS menor
			// Reseteamos la status_I_we1 => status_I_we1 = DEFAULT;
		}
		if (status_I_we2 == I_SATURA) {
			// Cambiamos FS WE2 a un FS mayor
			// Reseteamos la status_I_we2 => status_I_we2 = DEFAULT;
		}
		else if (status_I_we2 == I_BELOW_THRESHOLD) {
			// Cambiamos FS WE2 a un FS menor
			// Reseteamos la status_I_we2 => status_I_we2 = DEFAULT;
		}
		status_I_we1 = I_DEFAULT;		// Reseteamos los flags
		status_I_we2 = I_DEFAULT;

	}

	else if (mode_working == M_GALV) {
		if (status_I_we1 == I_SATURA) {
			// Cambiamos FS WE1 a un FS mayor
			// Reseteamos FLAG_SATURA_WE1 => flag_satura_we1 = DEFAULT;
		}
		else if (status_I_we1 == I_BELOW_THRESHOLD) {
			// Cambiamos FS WE1 a una FS menor
			// Reseteamos status_I_we1 => status_I_we1 = DEFAULT;

		}

		status_I_we1 = I_DEFAULT;		// Reseteamos el flag
	}

	else if (mode_working == M_EIS) {
		// TODO
	}

	next_state = MEASURING;



}


// TODO: Hay una opción para dejar el electródo conectado a un determinado potencial al finalizar el experimento. TENER EN CUENTA.
void Ending() {

	// Desconectar electródos
	// Resetear los demás periféricos a default
	// Enviar mensajes de finalización al software
	if (state_equipment != S_ERROR) {
		// borramos todos los flags al salir para dejarlos por defecto => default y NO_SATURA
		mode_working = M_NONE;
		df_mode = M_NONE;
		status_I_we1 = I_DEFAULT;
		status_I_we2 = I_DEFAULT;
		//communication_mode = C_NONE;
		pretreatment = P_NONE;
		lut1A_state = L_EMPTY;
		lut1B_state = L_EMPTY;
		lut2A_state = L_EMPTY;
		lut2B_state = L_EMPTY;
		experiment = E_NONE;
		next_state = IDLE;
	}
	else if (state_equipment == S_ERROR) {

		next_state = ERR;
	}

}


void Error() {
	// En esta rutina podríamos gestionar los errores de alguna
	// manera o poner al equipo en estado de seguridad si detecta algún fallo.
	// Toda la gestión de los fallos está por definir.

}


void calibration() {
	// TODO :  queda por definir como va a funcionar el equipo durante la calibración


}


/* ------------------------------------------------------------------------ FSM TESTING FUNCTIONS */

/* VECTOR 1 */
void vector1(uint32_t cont) {

	switch (cont) {
	case 0:
		communication_mode = C_USB;
		break;
	case 1:
		df_mode = M_BIPOT;
		break;
	case 2:
		break;
	case 3:
		pretreatment = P_RUNNING;
		break;
	case 4:
		pretreatment = P_RUNNING;
		break;
	case 5:
		pretreatment = P_RUNNING;
		break;
	case 6:
		pretreatment = P_RUNNING;
		break;
	case 7:
		pretreatment = P_RUNNING;
		break;
	case 8:
		pretreatment = P_RUNNING;
		break;
	case 9:
		pretreatment = P_FINISHED;
		break;
	case 10:
		experiment = E_RUNNING;
		break;
	case 11:
		break;
	case 12:
		status_I_we1 = I_SATURA;
		break;
	case 13:
		break;
	case 14:
		break;
	case 15:
		status_I_we2 = I_SATURA;
		break;
	case 16:
		break;
	case 17:
		status_I_we2 = I_BELOW_THRESHOLD;
		break;
	case 18:
		break;
	case 19:
		break;
	case 20:
		experiment = E_FINISHED;
		break;
	}


}

/* VECTOR 2 */
void vector2(uint32_t cont) {

	switch (cont) {
	case 0:
		communication_mode = C_BT;
		break;
	case 1:
		df_mode = M_POT;
		break;
	case 2:
		break;
	case 3:
		pretreatment = P_RUNNING;
		break;
	case 4:	break;
	case 5:	break;
	case 6:	break;
	case 7:	break;
	case 8:
		pretreatment = P_FINISHED;
		break;
	case 9:
		experiment = E_RUNNING;
		break;
	case 10: break;
	case 11:
		status_I_we1 = I_SATURA;
		break;
	case 12: break;
	case 13: break;
	case 14:
		status_I_we1 = I_BELOW_THRESHOLD;
		break;
	case 15: break;
	case 16: break;
	case 17: break;
	case 18: break;
	case 19:
		experiment = E_FINISHED;
		break;
	case 20: break;
	case 21: break;
	case 22: break;
	case 23:
		df_mode = M_EIS;
		break;
	case 24: break;
	case 25:
		pretreatment = P_RUNNING;
		break;
	case 26: break;
	case 27: break;
	case 28:
		pretreatment = P_FINISHED;
		break;
	case 29:
		experiment = E_RUNNING;
		break;
	case 30: break;
	case 31: break;
	case 32: break;
	case 33: break;
	case 34:
		experiment = E_FINISHED;
		break;
	case 35: break;
	case 36: break;
	case 37: break;
	case 38: break;
	case 39: break;
	case 40: break;
	case 41: break;

	}


}

/* Función para imprimir a un txt el listado de estados por los que pasa el equipo */
const char* printToFile(stateType state) {

	switch (state) {
	case CONECT: return "CONECT";
	case IDLE: return "IDLE";
	case CAL: return "CAL";
	case BIPOT: return "BIPOT";
	case POT: return "POT";
	case GALV: return "GALV";
	case EIS: return "EIS";
	case PREP_E: return "PREP_E";
	case PRETREATMENT: return "PRETREATMENT";
	case MEASURING: return "MEASURING";
	case FS_CH: return "FS_CH";
	case ENDING: return "END";
	case ERR: return "ERR";
	}




}