#define CS 8
#define CLK 9
#define datain 10

//timer interrupts
//by Amanda Ghassaei
//June 2012
//https://www.instructables.com/id/Arduino-Timer-Interrupts/

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
*/

//timer setup for timer0, timer1, and timer2.
//For arduino uno or any board with ATMEL 328/168.. diecimila, duemilanove, lilypad, nano, mini...

//this code will enable arduino timer 2 interrupt
//timer2 will interrupt at 8kHz

// Variable de estado 
int estado = 0;
// Contador posición de bit
int cuenta = 0;
// Variable de lectura
int data = 0;
// Lectura completa
unsigned int lectura = 0;
// Lectura anterior, en ángulo
float lastread = 0;
// Factor de conversión a ángulo
float factor = 0.00549324788;

void setup(){
  // Serial para mostar el valor
  Serial.begin(1200);


  //set pins as outputs
  pinMode(CS, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(datain, INPUT);
    
cli();//stop interrupts

//set timer2 interrupt at 8kHz
  TCCR2A = 0;// set entire TCCR2A register to 0
  TCCR2B = 0;// same for TCCR2B
  TCNT2  = 0;//initialize counter value to 0
  // set compare match register for 8khz increments
  OCR2A = 249;// = (16*10^6) / (8000*8) - 1 (must be <256)
  // turn on CTC mode
  TCCR2A |= (1 << WGM21);
  // Set CS21 bit for 8 prescaler
  TCCR2B |= (1 << CS21);   
  // enable timer compare interrupt
  TIMSK2 |= (1 << OCIE2A);


sei();//allow interrupts

}//end setup

  
ISR(TIMER2_COMPA_vect){//timer1 interrupt 8kHz toggles pin 9
//generates pulse wave of frequency 8kHz/2 = 4kHz (takes two cycles for full wave- toggle high then toggle low)
//  if (toggle2){
//    digitalWrite(9,HIGH);
//    toggle2 = 0;
//  }
//  else{
//    digitalWrite(9,LOW);
//    toggle2 = 1;
//  }
  switch (estado) {
  // 0 - Estado natural: CS ON, CLK ON
  // 1 - Inicio comunicación, primer medio ciclo: Bajar CS
  // 2 - Inicio, segundo medio ciclo: -
  // 3 - Inicio, tercer medio ciclo: Bajar CLK
  // 4 - Transferencia, primer ciclo: Subir CLK
  // 5 - Transferencia, segundo ciclo: Bajar CLK, leer datapin, sumar a la variable de destino, shiftear, aumentar cuenta
  // Repetir 4-5 desde el bit 14 al 1
  // 6 - Transferencia, ciclo 14-1: igual que 4
  // 7 - Transferencia, ciclo 14-2: Mantener CLK arriba, leer datapin,sumar a la variable de destino, resetear cuenta
  // 8 - Fin: Mantener CLK arriba, subir CS
  // 9 - Finfin: - volver estado a 0
    case 0:
      digitalWrite(CS,HIGH);
      digitalWrite(CLK,HIGH);
      estado = 1;
      break;
    case 1:
      digitalWrite(CS, LOW);
      estado = 2;
      break;
    case 2: 
      estado = 3;   
      break;
    case 3:
      digitalWrite(CLK, LOW);
      estado = 4;
      break;
    case 4:
      digitalWrite(CLK, HIGH);
      estado = 5;
      break;
    case 5:
      digitalWrite(CLK, LOW);
      data = digitalRead(datain);
      lectura = lectura + data;
      lectura <<= 1;
      ++cuenta;
      if ( cuenta == 15 )
      {
        estado = 6;
      }
      else
      {
        estado = 4;
      }
      break;
    case 6:
      digitalWrite(CLK, HIGH);
      estado = 7;
      break;
    case 7:
      data = digitalRead(datain);
      lectura = lectura + data;
      cuenta = 0;
      estado = 8;
      break;
    case 8:
      digitalWrite(CS, HIGH);
      estado = 9;
      break;
    case 9:
      //lectura <<= 1;
      //lectura >>= 1;
      lastread = lectura*factor;
      estado = 0;
      break;
    default:
    // statements
      break;
  } 
}


void loop(){
  //do other things here
  Serial.println(lastread);
}
