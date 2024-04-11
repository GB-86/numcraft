
double abs(double x){
    return (x<0)? -x :x;
}
double sin(double angle);


double cos(double angle){
    return sin(angle +1.57079632679);
}
double sin(double angle){
    if (angle>1.57079632679){
        return -sin(angle - 3.14159265359);
    }
    if (angle<-1.57079632679){
        return -sin(angle + 3.14159265359);
    }
    double x2=angle*angle;
    double x3=x2*angle;
    double x5=x2*x3;
    //double x7=x2*x5;
    double s= angle-x3*0.166666666667+x5*0.00833333333333;
    //-x7*0.000198412698413;
    if (abs(s-1)<0.0045){
        return (s<0)? -1:1;
    }
    return s;
}
double q_rsqrt(double number)
{ 

  long i;
  float x2, y;
  const float threehalfs = 1.5F;

  x2 = (float)number * 0.5F;
  y  = (float)number;
  i  = * ( long * ) &y;                       // evil floating point bit level hacking
  i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
  y  = * ( float * ) &i;
  y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
  // y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed
  return (double)y;
}
    