
#include <18F4550.h>
#include <math.h>
//#device adc=10

//FUSES
#FUSES NOWDT  //Fuse No watchdog timer
//Fuse que hace el "switcheo" entre el cristal externo e interno o detiene al microcontrolador 
//en caso de que nuestro cristal externo este dañado
#FUSES NOBROWNOUT 
#FUSES HS//Fuse para cristal externo (high speed)
#FUSES NOPROTECT

#use delay(clock=8M)

//Direcciones de memoria del ADC
#BYTE ADCON0=0xFC2
#BYTE ADCON1=0XFC1
#BYTE ADCON2=0XFC0

//Direcciones de memoria del A/D High y Low Register
#BYTE ADRESH=0XFC4
#BYTE ADRESL=0XFC3

//Direcciones de memoria de los puertos D y B

#BYTE PORTB=0xF81
#BYTE PORTD=0xF83
#BYTE TRISB=0xF93
#BYTE TRISD=0xF95
//Secuencia para el display de 7 segmentos de catodo comun
int16 const SecuenciaDisplay[]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};
//Declaracion de las variables 
int16 decenas=0, unidades=0;

void Display();

void main()
{
  
   //Puerto B Y D como salida
   TRISB=0x00;
   TRISD=0x00;
   
   //CONFIGURACIONES DE LOS REGISTROS PRINCIPALES
   
   //Bit 7 y 6 se leen como 0
   //Bits 5 a 2 se configuran como 0000 para activar el canal 0 (AN0)
   //Bit 1 se configura como 1 para indicar que la conversion esta en progreso
   //Bit 0 se configura como 1 para habilitar el modulo
   ADCON0=0b00000011;
   
   //Bit 7 y 6 se leen como 0
   //Bit 5 Seleccionar el voltaje de referencia negativo, en este caso se pone en 0 (Voltage ref = 0 (GND))
   //Bit 4 Seleccionar el voltaje de referencia positivo, en este caso se pone en 0 (Voltage ref = 0 (5V))
   //Bit 3 a 0 A/D Port Configuration Control bits: Configuracion del AN0 como pin analogico (1110 para usar el AN0)
   ADCON1=0b00001110;
   
   //Bit 7 para seleccionar el formato del resultado: 1 para cargar 8 bits al ADRESL y 2 al ADRESH
   //Bit 6 se lee como 0
   //Bit 5 a 3 configura al TAD (tiempo de adquisicion) 000 para un TAD DE 0
   //Bit 2 a 0 Tiempo de conversion (Forzosamente le toma al PIC 2 ciclos de reloj realizar una conversion de 10-bits -> 000)
   ADCON2=0b10000000;
   
   int16 ADC1, ADC2;
   int16 LecturaTotal;
   int16 Distancia = 0;
   

   while(TRUE)
   {
      //delay_ms(2);
      ADCON0 = 0x03;
      //0011 Realizar conversion y habilitar modulo
      //A la variable ADC1 se le asignan los 8 bits que se cargan en el ADRESL al estar justificado a la derecha
      ADC1 = ADRESL;
      //A la variable ADC2 se le asignan los 2 bits que se cargan en el ADRESH al estar justificado a la derecha
      ADC2 = ADRESH;
      //Se suman los 8 bits del ADC1 con la variable ADC2, antes se hace un corrimiento de 8 bits a la izq. para obtener el valor correcto
      LecturaTotal = (ADC2<<8)+ADC1;
      ADCON0 = 0x00; //Deshabilita, ya se termino de realizar la conversion y entonces se apaga para retener la informacion
      
      //Conversion de la lectura total del ADC al distancia en cm con la formula obtenida en excel
      Distancia = pow((LecturaTotal/3066.9),(1/(-0.825)));    
      //Calculo de los valores que se mostraran en los displays de 7 segmentos:
      decenas=Distancia/10;
      unidades=Distancia%10;
      //Funcion para mostrar los valores en el display
      Display();
      
   }


}

void Display(){
      //Enviar al puerto B un 0
      PORTB=0x00;
      //Enviar al puerto B un 00000010, con lo cual se enciende el PIN_B1 (display 2)
      PORTB=0x02;
      //Enviar al puerto D el valor de la posicion de las unidades obtenido en la funcion main
      PORTD=SecuenciaDisplay[decenas];
      //Delay de 7 ms para hacer la multiplexacion entre displays
      delay_ms(7);
      //Enviar al puerto B un 0
      PORTB=0x0;
      //Enviar al puerto B un 00000001, con lo cual se enciende el PIN_B0 (display 1)
      PORTB=0x01;
      //Enviar al puerto D el valor de la posicion de las decenas obtenido en la funcion main
      PORTD=SecuenciaDisplay[unidades];
      //Delay de 7 milisegundos
      delay_ms(7);
}
