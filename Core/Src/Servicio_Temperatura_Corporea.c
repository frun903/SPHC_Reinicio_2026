/*
 * Servicio_Temperatura_Corporea.c
 *
 *  Created on: Oct 16, 2024
 *      Author: frun903
 */


#include "Servicio_Temperatura_Corporea.h"
#include "OneWire.h"

#define cantidad_sensores 2.0; //Aqui cambiar la cantidad de sensrores
extern float Temp[MAXDEVICES_ON_THE_BUS];
float temperatura_corporal_promedio;
float sensor_Izquierdo;  //Temp[0]
float sensor_derecho;    //Temp[1]
float sensor_ubi; //Temp[num];

float Get_Temperatura_Sensor_Izquierdo(){
	get_ROMid();
	get_Temperature();

	return sensor_Izquierdo= Temp[0];
}

float Get_Temperatura_Sensor_Derecho(){
	get_ROMid();
	get_Temperature();

	return sensor_derecho= Temp[1];
}



float Get_Temperatura_por_sensor(int num){
	get_ROMid();
	get_Temperature();

		return sensor_ubi= Temp[num];
}

float Get_Temperatura_Promedio_Canido(){
	get_ROMid();
	get_Temperature();

	temperatura_corporal_promedio= (Temp[0]+Temp[1]) /cantidad_sensores;
	return temperatura_corporal_promedio;
}
