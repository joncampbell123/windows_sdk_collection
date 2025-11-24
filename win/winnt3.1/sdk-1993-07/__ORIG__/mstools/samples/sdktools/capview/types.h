class TIMETYPE
{
    double       t;

public:

    TIMETYPE(int i=0) {t = i; return;};

    TIMETYPE& operator=(TIMETYPE t2) { t = t2.t; return *this;};
 //   TIMETYPE& operator=(int i2) { t = i2; return *this;};
    TIMETYPE& operator=(unsigned long i2) { t = i2; return *this;};
    TIMETYPE& operator+=(TIMETYPE t2) { t += t2.t; return *this;};
    TIMETYPE& operator=(char * pch) { t = atol(pch); return *this;}
    
    TIMETYPE& operator/(int i) { TIMETYPE t2; t2.t = t / i; return t2;}

    BOOL operator>(TIMETYPE t2) { return t > t2.t;}

    float PerCent(TIMETYPE t2) {
        return (float) ((100.0 * t) / t2.t);
    }
    char * format(char * rgch) { sprintf(rgch, "%lu", (unsigned long) t); return rgch; };
};
