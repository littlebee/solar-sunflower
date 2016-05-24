

char sprintfBuffer[1024];

class C {
public:
  C(){
    // something clever here
  }
};


class A {
public:
  A(C *pC) : _pC(pC) {
    
    sprintf(sprintfBuffer, "A constructor: pC=%p _pC=%p", pC, _pC);
    Serial.println(sprintfBuffer);
  }

protected:
  C *_pC;
  
};


class B : public A {
public:
  
  B(C *pC) : A(pC) {
    
    sprintf(sprintfBuffer, "B constructor: pC=%p _pC=%p", pC, _pC);
    Serial.println(sprintfBuffer);
  }

};



void setup() {
  Serial.begin(9600);
  Serial.println('ready');
  C *cPointer = new C();
  B *bPointer = new B(cPointer);
  
}


void loop() {
  
}