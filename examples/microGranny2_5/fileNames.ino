//#include <mozzi_rand.h>

void dt(){
  hw.displayText("SRCH");
}
void listNameUp(){
  name[1]++;
  if(name[1]>=58 && name[1]<65) name[1]=65;
  else if(name[1]>=91){
    name[1]=48; 
    upWithFirstLetter();

  }
  while(!file.open(&root, name, O_READ)){
    //    hw.updateM
    hw.updateDisplay();
//    hw.displayText("SRCH");
    dt();
    name[1]+=1;
    if(name[1]>=58 && name[1]<65) name[1]=65;
    else if(name[1]>=91){
      name[1]=48; 
      upWithFirstLetter();
    }
  }
  file.close();
  indexed(activeSound,false);
  if(!playBegin(name,activeSound)) listNameUp();
  stopSound();

}
void listNameDown(){

  name[1]--;
  if(name[1]<48){
    name[1]=90;
    downWithFirstLetter();

  }
  else if(name[1]<65 && name[1]>58) name[1]=58;

  while(!file.open(&root, name, O_READ)){
    hw.updateDisplay();
    dt();
   // hw.displayText("SRCH");
    name[1]-=1;
    if(name[1]<48){
      name[1]=90;
      downWithFirstLetter();
    }
    else if(name[1]<65 && name[1]>58) name[1]=58;
  }
  file.close();
  indexed(activeSound,false);

  if(!playBegin(name,activeSound)) listNameDown();

  stopSound();
}

void upWithFirstLetter(){
  name[0]++;
  if(name[0]>=58 && name[0]<65) name[0]=65;
  else if(name[0]>=91) name[0]=48,name[1]=48;
}
void downWithFirstLetter(){
  name[0]--;
  if(name[0]<65 && name[0]>58) name[0]=58;
  if(name[0]<48) name[0]=90,name[1]=90;

}