#include <avr/io.h> 
#include <avr/interrupt.h> 
 
#define MAXOCR4 65535

void Config_Puertos(void);
void Config_Input_Capture(void);
void Mostrar_frec(unsigned long,unsigned int);


volatile unsigned int T_ini=0,T_fin=0,N_de_pulsos,Inp_cap;
unsigned long F_base;


void setup(void) 
{ Serial.begin(9600); 
  
  Config_Puertos();
  Config_Input_Capture();
} 

//***************************************************************
 void loop(void)
 {
   if (Inp_cap==2) {
                    Mostrar_frec(F_base,N_de_pulsos);
                    delay(1000);
                    Inp_cap=0;
                    TIMSK4|=(1<<5);
                   }
 } 
//***************************************************************

void Config_Puertos(void){

  DDRF = 0b00001111;      // PortF0 a PortF3 salida para configurar el sensor.
  PORTF =0b00000101;      // Salida al 20% (Fo=1;F1=0) - al 2% si Fo=0,F1=0 - , Filtro Rojo 
  DDRL &=~(1<<PD0);       // Pin_0 de PORTL es ENTRADA.
                          // (Utilizo PL0 para Input Capture)
  PORTL|=(1<<PL0);        // Activo resist.Pull_Up en PL0
  DDRD |=(1<<PD0);        // Pin_0 de PORTD es SALIDA
}


void Config_Input_Capture(void){
  cli();
  TCCR4A=0;
  TCCR4B=0;
  OCR4A=MAXOCR4;

  TCCR4B|=(1<<CS40); F_base=16000000;    // F_base=16Mhz/1=16MHz
  TCCR4B|=(1<<WGM42);   //Habilita comparación TCNT4 con OCR4A.
  TIMSK4|=(1<<5);       //Habilito interrupción por Input_Capture
  TCCR4B|=(1<<6);       //Input_Capture por flanco positivo.
  TCCR4B|=(1<<7);       //Supresor de ruidos activado.
  sei();                // Activa Globalmente las Interupciones.
 
}



void Mostrar_frec(unsigned long F_base,unsigned int pulsos){
  float aux;
  aux=(F_base/pulsos)/100;
  aux=(float)aux/10;
  Serial.print(aux);
  Serial.print("Khz");
  Serial.println();
  
}



ISR(TIMER4_CAPT_vect){
  if(Inp_cap==0){
        T_ini=ICR4;
        Inp_cap=1;
        }
  else{
        if(Inp_cap==1){
          T_fin=ICR4;
          Inp_cap=2;
          TIMSK4&=~(1<<5);
          if(T_fin>T_ini)
              N_de_pulsos= (T_fin-T_ini);
            else
              if (T_fin<T_ini)
              N_de_pulsos= T_fin+MAXOCR4-T_ini; 
          }
 TIFR4|=(1<<ICF4); 
      }
 }
