int x=0, y=0, z=0;

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  //  x = random(0, 9000);
  //  y = random(0, 6000);
  //  z = random(0, 360);

  //  x = y = z = 0;

  if (x > 9000)
    x = 0;      
  if (y > 6000)
    y = 0;      
  if (z > 360)
    z = 0;  

  String posXYZ =  String(x)+ ","+  String(y) + ","+ String(z)+"E";

  //  if (!isWhitespace(x) && !isWhitespace(y) && !isWhitespace(z) && 
  //!isControl(x) && !isControl(y) && !isControl(z) )
  Serial.print(posXYZ); 

  x+=3; 
  y+=2; 
  z+=1;

  delay(50);
}


