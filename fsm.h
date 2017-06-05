


typedef enum {
	CONECT = 0,
	IDLE,
	CAL,
	BIPOT,
	POT,
	GALV,
	EIS,
	PREP_E,
	PRETREATMENT,
	MEASURING,
	FS_CH,
	ENDING,
	ERR

}stateType;




typedef enum {
	S_WAITING = 0x00,
	S_RUNNING = 0x01,
	S_ERROR = 0x02,

}general_state;

typedef enum {
	M_NONE = 0x00,
	M_BIPOT = 0x01,
	M_POT = 0x02,
	M_GALV = 0x03,
	M_EIS = 0x04
}mode;


typedef enum {
	I_DEFAULT = 0x00,
	I_SATURA = 0x01,
	I_BELOW_THRESHOLD = 0x02
}status_I_measure;

typedef enum {
	C_NONE = 0x00,
	C_BT = 0x01,
	C_USB = 0x02
}mode_com;

typedef enum {
	L_EMPTY = 0x00,
	L_REFRESHED = 0x01,
	L_FINISHED = 0x02
}lut_state;

typedef enum {
	P_NONE = 0x00,
	P_RUNNING = 0x01,
	P_FINISHED = 0x02
}state_pretreatment;

typedef enum {
	E_NONE = 0x00,
	E_RUNNING = 0x01,
	E_CANCELLED = 0x02,
	E_FINISHED = 0x03,
	E_ERROR = 0x04
}state_experiment;



void lista_eventos(uint8_t);
void start();
void conection();
void Idle();
void bipot();
void pot();
void galv();
void eis();
void PrepE();
void Pretreatment();
void Measuring();
void FS_ch();
void Ending();
void Error();
void calibration();

void vector1(uint32_t cont);
void vector2(uint32_t cont);
const char* printToFile(stateType);