#define DO_DEBUG_SERIAL_

#ifdef DO_DEBUG_SERIAL
	#define TRACELN(t) Serial.println(t);
	#define TRACELNF(t) Serial.println(String(F(t)));
	#define TRACE(t) Serial.print(t);
	#define TRACEF(t) Serial.print(String(F(t)));
#else
	#define TRACELN(t) /**/
	#define TRACELNF(t) /**/
	#define TRACE(t) /**/
	#define TRACEF(t) /**/
#endif

// ---------------- НАСТРОЙКИ ----------------
#define AUTO_GAIN 1       // автонастройка по громкости (экспериментальная функция)
#define VOL_THR 25        // порог тишины (ниже него отображения на матрице не будет)

#define LOW_PASS 30        // нижний порог чувствительности шумов (нет скачков при отсутствии звука)
#define DEF_GAIN 80       // максимальный порог по умолчанию (при GAIN_CONTROL игнорируется)

#define FHT_N 256          // ширина спектра х2
#define SPECTRUMS 32       // 
// вручную забитый массив тонов, сначала плавно, потом круче
//byte posOffset[16] = {2, 3, 4, 6, 8, 10, 12, 14, 16, 20, 25, 30, 35, 60, 80, 100};
//byte posOffset[16] = {1, 2, 3, 4, 5, 6, 7, 10, 13, 16, 20, 25, 30, 40, 50, 75};
//byte posOffset[SPECTRUMS] = {1, 2, 3, 4, 5, 6, 7, 10, 13, 16, 20, 25, 30, 35, 45, 60, 70, 80, 90, 100};
byte posOffset[SPECTRUMS] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 33, 36, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85};
byte bufLevel[64];// = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//byte posOffset[16] = {2, 3, 4, 5, 6, 8, 10, 15, 18, 22, 25, 30, 40, 50, 65, 85};
// ---------------- НАСТРОЙКИ ----------------

// ---------------------- ПИНЫ ----------------------
#define AUDIO_IN_L 1          // пин, куда подключен звук
#define AUDIO_IN_R 2          // пин, куда подключен звук
// ---------------------- ПИНЫ ----------------------

// --------------- БИБЛИОТЕКИ ---------------
#define LOG_OUT 1
#include <FHT.h>          // преобразование Хартли

#include <GyverOLED.h>

// --------------- БИБЛИОТЕКИ ---------------
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

//double prevVolts = 100.0;
byte gain = DEF_GAIN;   // усиление по умолчанию
unsigned long gainTimer;
byte maxValue, maxValue_f;
float k = 0.1;

#define OLED_ADDR   0x3C
GyverOLED<SSD1306_128x32, OLED_BUFFER> oled(OLED_ADDR);        // с буфером
//GyverOLED<SSD1306_128x32, OLED_NO_BUFFER> oled(OLED_ADDR);     // без буфера

void setup() {
	// поднимаем частоту опроса аналогового порта до 38.4 кГц, по теореме
	// Котельникова (Найквиста) частота дискретизации будет 19 кГц
	// http://yaab-arduino.blogspot.ru/2015/02/fast-sampling-from-analog-input.html
	sbi(ADCSRA, ADPS2);
	cbi(ADCSRA, ADPS1);
	sbi(ADCSRA, ADPS0);

	// для увеличения точности уменьшаем опорное напряжение,
	// выставив EXTERNAL и подключив Aref к выходу 3.3V на плате через делитель
	// GND ---[2х10 кОм] --- REF --- [10 кОм] --- 3V3
	analogReference(EXTERNAL);

#ifdef DO_DEBUG_SERIAL
	Serial.begin(9600);
#endif
TRACELN("Start oled...");	
  oled.init();  // инициализация
TRACELN("ok");  

  // --------------------------
  // настройка скорости I2C
  //Wire.setClock(800000L);   // макс. 800'000

  // --------------------------
  oled.clear();   // очистить дисплей (или буфер)
TRACELN("Clear...");  
  
  oled.update();  // обновить. Только для режима с буфером! OLED_BUFFER
	
  /*
	Wire.setClock(400000L);
	oled.begin(&Adafruit128x64, OLED_ADDR);
	oled.clear();*/
	
	/*lcd.init();
	lcd.backlight();
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("AlexGyver");
	lcd.setCursor(0, 1);
	lcd.print("SpektrumAnalyzer");
	lcdChars();   // подхватить коды полосочек*/
  oled.dot(1, 1);
	delay(2000);
}

void drawGraph(int channel) {
	analyzeAudio(channel);   // функция FHT, забивает массив fht_log_out[] величинами по спектру
	
	// 128x32
	int graphPosX = (channel - 1) * 64;
	
	for (int pos = 0; pos < SPECTRUMS; pos++) {   // для окошек дисплея с 0 по SPECTRUMS
		// найти максимум из пачки тонов
		if (fht_log_out[posOffset[pos]] > maxValue) maxValue = fht_log_out[posOffset[pos]];

		//lcd.setCursor(pos, 0);

		// преобразовать значение величины спектра в диапазон 0..SPECTRUMS с учётом настроек
		int posLevel = map(fht_log_out[posOffset[pos]], LOW_PASS, gain, 0, SPECTRUMS);
		posLevel = constrain(posLevel, 0, SPECTRUMS);
		
		drawBar(graphPosX + pos * 2, posLevel, pos+(channel-1)*SPECTRUMS);

		/*if (posLevel > 7) {               // если значение больше 7 (значит нижний квадратик будет полный)
		  lcd.printByte(posLevel - 8);    // верхний квадратик залить тем что осталось
		  lcd.setCursor(pos, 1);          // перейти на нижний квадратик
		  lcd.printByte(7);               // залить его полностью
		} else {                          // если значение меньше 8
		  lcd.print(" ");                 // верхний квадратик пустой
		  lcd.setCursor(pos, 1);          // нижний квадратик
		  lcd.printByte(posLevel);        // залить полосками
		}*/
	}
	
	TRACELN("");

	if (AUTO_GAIN) {
		maxValue_f = maxValue * k + maxValue_f * (1 - k);
		if (millis() - gainTimer > 1500) {      // каждые 1500 мс
			// если максимальное значение больше порога, взять его как максимум для отображения
			if (maxValue_f > VOL_THR) gain = maxValue_f;
			// если нет, то взять порог побольше, чтобы шумы вообще не проходили
			else gain = 100;
			gainTimer = millis();
		}
	}	
}

void loop() {
	drawGraph(AUDIO_IN_L);
	drawGraph(AUDIO_IN_R);

  oled.update();
  
  //delay(2000);
}

void drawBar(int posX, int level, int bar) {
	/*TRACE(level);
	TRACE(" ");*/
  level *= 1.1;
  if (level > 32) {
    level = 32;
  }

  int posX2 = posX + 2;
	
	//display.fillRectByCoords(posX, 0, posX + 3, 32, false); //32 - level
  oled.rect(posX, 0, posX2, 32, OLED_CLEAR); //32 - level
  if (level > 0) {
	  //oled.rect(posX, 32 - level, posX + 3, 32, OLED_FILL);
    for (int y = 0; y < level; y++) {
      //oled.rect(posX, 32 - level, posX + 3, 32, OLED_FILL);
      oled.fastLineH(32-y, posX, posX2);
      //TRACELN(y);
      y++;
    }
    //TRACELN("---");
  }

  int lastMax = bufLevel[bar];
  if (level > lastMax) {
    bufLevel[bar] = level;
  } else {
    oled.fastLineH(32-lastMax, posX, posX2);
    bufLevel[bar] = max(0, bufLevel[bar]-1);
  }
}

void analyzeAudio(int pin) {
	for (int i = 0 ; i < FHT_N ; i++) {
		fht_input[i] = analogRead(pin); // put real data into bins
	}
	fht_window();  // window the data for better frequency response
	fht_reorder(); // reorder the data before doing the fht
	fht_run();     // process the data in the fht
	fht_mag_log(); // take the output of the fht
}
