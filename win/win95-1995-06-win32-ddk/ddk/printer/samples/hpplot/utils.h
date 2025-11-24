//*************************************************************
//  File name: utils.h
//
//*************************************************************

#define ABS(Value) ((Value) > 0 ? (Value) : (-(Value)))
#define WITHIN(value,lower,upper) ((value) >= (lower) && (value) <= (upper))

short FAR PASCAL itoa(LPSTR, short);
long FAR PASCAL ltoa(LPSTR, long);

/*** EOF: utils.h ***/
