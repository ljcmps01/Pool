/*Descripcion:
 * Es el programa del Colorimetro con la diferencia de que realiza una unica lectura y avisa si se detecto el color rojo
 */

#define MAXOCR4 65535

volatile unsigned int T_ini=0,T_fin=0, N_de_pulsos,Inp_cap,Contador=0;
unsigned long F_base;

float Mostrar_frec(unsigned long F_base,unsigned int pulsos);

void setup(){
  Serial.begin(9600);
  //Configuracion de puertos
  DDRF=0b00001111;  //pines de salida para configuracion del sensor de colores
  PORTF=0b00000101; //Salida al 20% y sin filtro
  
  DDRL&=~(1<<PL0);  //Se determina el PL0 como entrada
  PORTL|=(1<<PL0);  //Se activa pull up de PL0

  cli();
  TCCR4A=0;
  TCCR4B=0;
  OCR4A=MAXOCR4;

  TCCR4B|=(1<<CS40);
  F_base=16000000;
  TCCR4B|=(1<<WGM42);
  TIMSK4|=(1<<5);
  TCCR4B|=(1<<6);       //Input_Capture por flanco positivo.
  TCCR4B|=(1<<7);       //Supresor de ruidos activado.
  sei();

 
}

void loop(){
  if (Inp_cap==2) {
    Serial.print(Mostrar_frec(F_base,N_de_pulsos));
    Serial.println("Khz\n");
    delay(1000);
    Inp_cap=0;
    TIMSK4|=(1<<5);
    Contador++;
    }
  if(Contador==3){
    TIMSK4&=~(1<<5);
    if(Mostrar_frec(F_base,N_de_pulsos)>10)
      Serial.println("Es rojo");
    else
      Serial.println("NO es rojo");
    Contador++;
  }
  if(Contador==4);
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

float Mostrar_frec(unsigned long F_base,unsigned int pulsos){
  float aux;
  aux=(F_base/pulsos)/100;
  aux=(float)aux/10;
  return aux;
  
}
