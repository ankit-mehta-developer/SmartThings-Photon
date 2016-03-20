int LEDPORT = D7;
int SERVOPORT = A4;
int BUTTONPORT = D2;
Servo servo;
String MIN_ANGLE = "180";
String MAX_ANGLE = "20";
int currentButtonState = LOW;
int buttonState = 0;
int loopCounter = 0;

void setup() {
    initLed();
    initServo();
    
    Particle.function("execute", execute);
    Particle.function("rotate", rotate);
    Particle.variable("buttonState", &buttonState, INT);
    
    switchLed("ON");
    rotate(MIN_ANGLE);
    delay(500);
    switchLed("OFF");
    delay(500);
    switchLed("ON");
    delay(500);
    switchLed("OFF");
    delay(500);
    switchLed("ON");
    delay(500);
    switchLed("OFF");
    
    initButton();
    
    initRgb();
    setButtonState(0);
}

void initRgb() {
    RGB.control(true);
    RGB.color(0, 0, 0);
    RGB.brightness(10);
}

void setRgbRed() {
    RGB.color(255, 0, 0);
    RGB.brightness(10);
}

void setRgbYellow() {
    RGB.color(255, 255, 0);
    RGB.brightness(10);
}

void setRgbGreen() {
    RGB.color(0, 255, 0);
    RGB.brightness(10);
}

int execute(String command) {
    if(command == "ON" || command == "on") {
        if(buttonState == 1) {
            return 1;
        }
        return execute("TOGGLE");
    }
    else if(command == "OFF" || command == "off") {
        if(buttonState == 0) {
            return 0;
        }
        return execute("TOGGLE");
    }
    else if(command == "TOGGLE" || command == "toggle") {
        switchLed("ON");
        rotate(MAX_ANGLE);
        delay(500);
        rotate(MIN_ANGLE);
        toggleButtonState();
        switchLed("OFF");
        return buttonState;
    }
    return -1;
}

void initServo() {
    servo.attach(SERVOPORT);
}

int rotate(String angle) {
    servo.write(angle.toInt());
    Particle.publish("rotation", "Rotating to angle " + angle + " degrees", PRIVATE);
    return angle.toInt();
}

void initLed() {
    pinMode(LEDPORT, OUTPUT);
}

int switchLed(String command) {
    if(command == "ON" || command == "on") {
        digitalWrite(LEDPORT, HIGH);
        return 1;
    }
    else if(command == "OFF" || command == "off") {
        digitalWrite(LEDPORT, LOW);
        return 0;
    }
    return -1;
}

void initButton() {
    pinMode(BUTTONPORT, INPUT_PULLUP);
    currentButtonState = digitalRead(BUTTONPORT);
}

String toString(int value) {
    if(value == LOW) {
        return "LOW";
    }
    else if (value == HIGH) {
        return "HIGH";
    }
    else {
        return "Other";
    }
}

void toggleButtonState() {
    if(buttonState == 0) {
        setButtonState(1);
    }
    else {
        setButtonState(0);
    }
}

void buttonLoop() {
   int realButtonState = digitalRead(BUTTONPORT);
   if(currentButtonState == realButtonState) {
       if(loopCounter % 10 == 0) {
           RGB.brightness(1);
       }
       else {
           RGB.brightness(0);
       }
       return;
   }
   currentButtonState = realButtonState;
   if(realButtonState == LOW) {
       switchLed("ON");
       Particle.publish("button", "button is pressed");
       execute("TOGGLE");
   }
   else {
       switchLed("OFF");
   }
}

void setButtonState(int value) {
    buttonState = value;
    if(buttonState == 0) {
        setRgbYellow();
    }
    else if(buttonState == 1) {
        setRgbGreen();
    }
    else {
        initRgb();
    }
}

void loop() {
   buttonLoop();
   delay(250);
   loopCounter++;
   if(loopCounter == 10000) {
       loopCounter = 0;
   }
}
