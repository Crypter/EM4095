#include <ESP8266WiFi.h>

static inline int32_t asm_ccount(void) {
    int32_t r; asm volatile ("rsr %0, ccount" : "=r"(r)); return r; }

static inline void asm_nop() {
    asm volatile ("nop"); }

static uint64_t data=0;
static uint8_t  state=0, ostate=0, state_bit=0;


static uint32_t tick=0, tock=0, dtick=0;
static uint32_t first_data=0, second_data=0;
static uint8_t counter=0;

uint8_t getVendor(uint64_t RAW){
  return ((RAW >> 47) & 0xF0) | ((RAW >> 46) & 0xF);
}

uint32_t getID(uint64_t RAW){
  uint32_t result=0;
  RAW = RAW >> 1; //compensate for stop bit
  for (int i=0; i< 10; i++)
  {
    RAW = RAW >> 5;
    result |= (RAW & 0xF) << (i*4);
  }
  return result;
}

void setup() {
  pinMode(D1, INPUT);
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);
//  pinMode(D2, INPUT);
  WiFi.forceSleepBegin();
//  ESP.wdtDisable();
  Serial.begin(9600);
  delay(10);
  Serial.println();
}

void loop() {
  for (;;) {
    if (!counter) yield();
    counter++;
    state = GPI&0b00100000;
    
   if (state!=ostate){
      digitalWrite(BUILTIN_LED, HIGH);

      tick=asm_ccount();
      dtick=tick-tock;
//      Serial.print((uint16_t)state>>5);
//      Serial.print(", ");
//      Serial.print(dtick);
//      Serial.print(", ");
      dtick = (dtick - 20000) / 40000;
//      Serial.println(dtick);

      
      if (dtick) { //+60000
        state_bit = (ostate >> 5);
        data = data<<1;
        data |= state_bit;
      } else if ( state&0b00100000 ) { //-60000
        data = data<<1;
        data |= state_bit;
      }

      first_data = data>>32;
      second_data = data&0xFFFFFFFF;

      if ( ((first_data&0xFF800000) == 0xFF800000) && !(second_data&1) ){
        if (
          !(__builtin_popcountll(data&0x7C000000000000) % 2) &&
          !(__builtin_popcountll(data&0x3E00000000000) % 2) &&
          !(__builtin_popcountll(data&0x1F0000000000) % 2) &&
          !(__builtin_popcountll(data&0xF800000000) % 2) &&
          !(__builtin_popcountll(data&0x7C0000000) % 2) &&
          !(__builtin_popcountll(data&0x3E000000) % 2) &&
          !(__builtin_popcountll(data&0x1F00000) % 2) &&
          !(__builtin_popcountll(data&0xF8000) % 2) &&
          !(__builtin_popcountll(data&0x7C00) % 2) &&
          !(__builtin_popcountll(data&0x3E0) % 2) &&
          
          !(__builtin_popcountll(data&0x42108421084210) % 2) &&
          !(__builtin_popcountll(data&0x21084210842108) % 2) &&
          !(__builtin_popcountll(data&0x10842108421084) % 2) &&
          !(__builtin_popcountll(data&0x8421084210842) % 2)
          ){
            digitalWrite(BUILTIN_LED, LOW);
            Serial.printf("RAW: %08X%08X Ven: %03u UID: ", first_data, second_data, getVendor(data));
            uint32_t uid = getID(data);
            char buffer[100];
            sprintf(buffer, "%04ld", uid/1000000L);
            Serial.print(buffer); 
            sprintf(buffer, "%05ld", uid%1000000L);
            Serial.println(buffer);
          }
      }

      
      ostate=state;
      tock=tick;
    }
  }
//  Serial.printf("%d,%d\r\n", digitalRead(D1), digitalRead(D2));
}
