// $Id: bigint.cpp,v 1.76 2016-06-14 16:34:24-07 - - $

#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "bigint.h"
#include "debug.h"
#include "relops.h"

bigint::bigint (long that): uvalue (that), is_negative (that < 0) {
   DEBUGF ('~', this << " -> " << uvalue)
}

bigint::bigint (const ubigint& uvalue, bool is_negative):
                uvalue(uvalue), is_negative(is_negative) {
}

// Negative sign here means positive value
// Positive sign means negative value
bigint::bigint (const string& that) {
   //cout << "\nSign "<< that  << endl; 
   is_negative = that.size() > 0 and that[0] == '_';
   uvalue = ubigint (that.substr (is_negative ? 1 : 0));
   //cout << "\nSign " << is_negative  << endl;
}

bigint bigint::operator+ () const {
   return *this;
}

bigint bigint::operator- () const {
   return {uvalue, not is_negative};
}

bigint bigint::operator+ (const bigint& that) const {
    ubigint result;
    if(that.is_negative && !is_negative)
    {
        if(that.uvalue > uvalue)
        {
            result = that.uvalue - uvalue; 
            return bigint(result, true);
        }
        else
        {
            result = uvalue - that.uvalue; 
            return bigint(result, false);
        }
    }
    else if(is_negative && !that.is_negative)
    {
        if(uvalue > that.uvalue)
        {
            result = uvalue - that.uvalue; 
            return bigint(result, true);
        }
        else
        {
            result = that.uvalue - uvalue; 
            return bigint(result, false);
        }
    }
    else if(is_negative && that.is_negative)
    {
        result = uvalue + that.uvalue;
        return bigint(result, true);
    }
    else
    {
        result = uvalue + that.uvalue;
        return bigint(result, false);
    }
}

bigint bigint::operator- (const bigint& that) const {
    ubigint result;
    bool sign;
    if(that.is_negative && !is_negative)
    {
       // cout << "first  "<< endl; 
        result = uvalue + that.uvalue;
        return bigint(result, false);
        //return bigint(result, true);
    }
    else if(is_negative && !that.is_negative)
    {
        //cout << "Second  "<< endl; 
        result = uvalue + that.uvalue;
        if(result == 0){ return bigint(result,false);}
        else{ return bigint(result, true);}
        
        //return bigint(result,false);
       // }
    }
    else
    {
        sign = is_negative && that.is_negative;
        if(uvalue > that.uvalue)
        {
           // cout << "Third  "<< endl; 
            result = uvalue - that.uvalue;
        }
        else if(uvalue == that.uvalue)
        {
            //cout << "Fourth  "<< endl; 
            result = that.uvalue - uvalue;
            if(result ==  0) sign = false;
            if(result < 0) sign = true;
           
        }
        else
        {
            //if(result == 0) sign = false;
            result = that.uvalue - uvalue;
            //sign = !sign; 
            if(result == 0) sign = false;
            else sign = !sign;
        }
        //if( is_negative = that.size() <= 0) bigint(result, !sign); 
        //if(result  == 0) sign = false; 
        return bigint(result, sign);
    }
}

bigint bigint::operator* (const bigint& that) const {
    ubigint result = uvalue * that.uvalue;
    bool sign = is_negative ^ that.is_negative;
    return bigint(result,sign);
}

bigint bigint::operator/ (const bigint& that) const {
    ubigint result = uvalue / that.uvalue;
    bool sign = is_negative ^ that.is_negative;
    return bigint(result,sign);
}

bigint bigint::operator% (const bigint& that) const {
    ubigint result = uvalue % that.uvalue;
    bool sign = is_negative ^ that.is_negative;
    return bigint(result,sign);
}

bool bigint::operator== (const bigint& that) const {
   return is_negative == that.is_negative and uvalue == that.uvalue;
}

bool bigint::operator< (const bigint& that) const {
   if (is_negative != that.is_negative) return is_negative;
   return is_negative ? uvalue > that.uvalue
                      : uvalue < that.uvalue;
}

ostream& operator<< (ostream& out, const bigint& that) {
   return out << (that.is_negative ? "_" : "") << that.uvalue;
}

