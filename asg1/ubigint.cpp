// $Id: ubigint.cpp,v 1.14 2016-06-23 17:21:26-07 - - $

#include <cctype>
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
#include <iostream>

#include <math.h>
using namespace std;

#include "ubigint.h"
#include "debug.h"

ubigint::ubigint (unsigned long that): uvalue (that) {
   DEBUGF ('~', this << " -> " << uvalue);
   string str = to_string(uvalue);
   ubig_value = vector<udigit_t>();
   for (char digit: str) {
      if (not isdigit (digit)) {
         throw invalid_argument ("ubigint::ubigint(" + str + ")");
      }
      uvalue = uvalue * 10 + digit - '0';
      ubig_value.push_back(digit - '0');
   }
}

ubigint::ubigint (vector<udigit_t> that): ubig_value(that) {

}

ubigint::ubigint (const string& that): uvalue(0) {
   DEBUGF ('~', "that = \"" << that << "\"");
   for (char digit: that) {
      if (not isdigit (digit)) {
         throw invalid_argument ("ubigint::ubigint(" + that + ")");
      }
      uvalue = uvalue * 10 + digit - '0';
      ubig_value.push_back(digit - '0');

   }
}


// _0 + _0 = -0   atill need to fix this..
ubigint ubigint::operator+ (const ubigint& that) const {
  vector<udigit_t> ret_val;
  unsigned int iterator_length = ubig_value.size();
  unsigned char carry = 0;
  unsigned char res = 0;
  unsigned int first_iterator = ubig_value.size() - 1;
  unsigned int second_iterator = that.ubig_value.size() - 1;
  if (that.ubig_value.size() > ubig_value.size())
  {
     iterator_length = that.ubig_value.size();
  }
  for(unsigned int i = 0; i < iterator_length; i++)
  {
    if(static_cast <unsigned int> (i) >= that.ubig_value.size())
      {
        res = ubig_value.at(first_iterator) + carry;
        first_iterator--;
      }
    else if(static_cast <unsigned int> (i)  >= ubig_value.size())
    {
      res = that.ubig_value.at(second_iterator) + carry;
      second_iterator--;
    }
    else
    {
      res = ubig_value.at(first_iterator) + that.ubig_value.at(second_iterator) + carry;
      first_iterator--;
      second_iterator--;
    }
    if(res >= 10)
    {
      carry = 1;
      res -= 10;
    }
    else
    {
      carry = 0;
    }
    ret_val.insert(ret_val.begin(), 1, res);
  }
   if(carry == 1)
   {
     ret_val.insert(ret_val.begin(), 1, 1);
   }
   if(ret_val.size() == 0)
  {
      ret_val.insert(ret_val.begin(), 1, 0);
  }
   return ubigint(ret_val);
}

ubigint ubigint::operator- (const ubigint& that) const {
   if (*this < that) throw domain_error ("ubigint::operator-(a<b)");

   vector<udigit_t> ret_val;
   unsigned int iterator_length = ubig_value.size();
   unsigned char res = 0;
   unsigned int diff = (ubig_value.size() - that.ubig_value.size());

   for(unsigned int i = 0; i < iterator_length; i++)
   {
     if (static_cast <unsigned int> (i) < (ubig_value.size() - that.ubig_value.size()))
     {
       //cout << "\nThis is ubig_value.at(i): " <<ubig_value.at(i); 
       res = ubig_value.at(i);
     }
     else if(ubig_value.at(i) < that.ubig_value.at(i - diff))
     {
        int index = i - 1;
        //cout << "\nThis is index= i-1: " << index << endl;; 
       
        while(ret_val.at(index) < 1)
       // while(ret_val.at(index) < 0)
        {
          ret_val.at(index) = 9;
          index--;
         // cout << "\nWhile(ret_val.at(index) < 1) : index= " << 
         //     index <<  endl;   
        }
        ret_val.at(index) -= 1;
       // cout << "\nret_val.at(index) -= 1: : " << ret_val.at(index) << endl; 
        res = 10 + ubig_value.at(i) - that.ubig_value.at(i - diff);
       // cout << "(1) res = " << res << endl;
     }
    else
    {
      
      res = ubig_value.at(i) - that.ubig_value.at(i - diff);
      //cout << "\n(2) res =  " << res << endl; 
      
    } 
      //cout << "\n(2) ret_val.end " << ret_val.insert(ret_val.end(), 1, res) << endl;;
      ret_val.insert(ret_val.end(), 1, res);
    }
    for(int i = 0; i < static_cast <int> (ret_val.size()); i++)
    {
      if(ret_val.at(i) > 0)
      {
       // cout << "\n Breaking ";
        break;
      }
      else
      {
      //  cout << "\n(3) ret_val.end " << ret_val.erase(ret_val.begin()+i) << endl;;
        ret_val.erase(ret_val.begin() + i);
        i--;
      }
    }
    if(ret_val.size() == 0)
    {
     // cout << "\nif ret_val.size() ==0 " << ret_val.insert(ret_val.begin(),1,0);  
      ret_val.insert(ret_val.begin(), 1, 0);
    }
   // cout << "\nReturn statement --  " << endl;
   return ubigint (ret_val);
}

ubigint ubigint::operator* (const ubigint& that) const {
  vector<udigit_t> res(that.ubig_value.size() + ubig_value.size(), 0);
  for(int i = that.ubig_value.size() - 1; i >= 0; i--)
  {
    for(int j = ubig_value.size() - 1; j >= 0; j--)
    {
      res.at(i + j + 1) += (ubig_value.at(j) * that.ubig_value.at(i));
      if(res.at(i + j + 1) >= 10)
      {
        unsigned char div = res.at(i + j + 1) / 10;
        res.at(i + j + 1) -= (div * 10);
        res.at(i + j) += div;
      }
    }
  }
  for(unsigned int i = 0; i < res.size() - 1; i++)
  {
    if(res.at(i) == 0)
    {
      res.erase(res.begin() + i);
      --i;
    }
    else
    {
      break;
    }
  }
  return ubigint (res);
}

void ubigint::multiply_by_2() {
  vector<udigit_t> ret_val;
  unsigned char carry = 0;
  unsigned char res = 0;
  for(int i = ubig_value.size() - 1; i >= 0; i--)
  {
    res = (ubig_value.at(i) * 2) + carry;
    if (res >= 10)
    {
      carry = 1;
      res -= 10;
    }
    else
    {
      carry = 0;
    }
    ret_val.insert(ret_val.begin(), 1, res);
  }
  if(carry == 1)
  { 
     ret_val.insert(ret_val.begin(), 1, 1);
  }
  ubig_value = ret_val;
}

void ubigint::divide_by_2() {
  vector<udigit_t> ret_val;
  unsigned char carry = 0;
  unsigned char res = 0;
  unsigned char intermediateVal = 0;
  for(unsigned int i = 0; i < ubig_value.size(); i++)
  {
    intermediateVal = ubig_value.at(i) + carry;
    res = (ubig_value.at(i) + carry) / 2;
    if(i < (ubig_value.size() - 1))
    {
      if (((ubig_value.at(i) + carry) % 2) != 0)
      {
        carry = 10 * (intermediateVal - (2 * res));
      }
      else
      {
        carry = 0;
      }
    }
    ret_val.insert(ret_val.end(), 1, res);
  }
  for(unsigned int i = 0; i < ret_val.size() - 1; i++)
  {
    if(ret_val.at(i) == 0)
    {
      ret_val.erase(ret_val.begin());
      --i;
    }
    else
    {
      break;
    }
  }
  ubig_value = ret_val;
}


struct quo_rem { ubigint quotient; ubigint remainder; };
quo_rem udivide (const ubigint& dividend, ubigint divisor) {
   // Note: divisor is modified so pass by value (copy).
   ubigint zero {0};
   if (divisor == zero) throw domain_error ("udivide by zero");
   ubigint power_of_2 {1};
   ubigint quotient {0};
   ubigint remainder {dividend}; // left operand, dividend
   while (divisor < remainder) {
      divisor.multiply_by_2();
      power_of_2.multiply_by_2();
   }
   while (power_of_2 > zero) {
      if (divisor <= remainder) {
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
      }
      divisor.divide_by_2();
      power_of_2.divide_by_2();
   }
   return {.quotient = quotient, .remainder = remainder};
}

ubigint ubigint::operator/ (const ubigint& that) const {
   return udivide (*this, that).quotient;
}

ubigint ubigint::operator% (const ubigint& that) const {
   return udivide (*this, that).remainder;
}

bool ubigint::operator== (const ubigint& that) const {
  bool result = false;
   if ( ubig_value.size() == that.ubig_value.size())
   {
    result = true;
    for(unsigned int i = 0; i < ubig_value.size(); i++)
    {
      if (that.ubig_value.at(i) != ubig_value.at(i))
      {
        result = false;
        break;
      }
    }
  }
  return result;
}

bool ubigint::operator< (const ubigint& that) const {
  bool result = ubig_value.size() < that.ubig_value.size();
  bool equivalent = false;
   if ( ubig_value.size() == that.ubig_value.size())
   {
    result = true;
    equivalent = true;
    for(unsigned int i = 0; i < ubig_value.size(); i++)
    {
      if (ubig_value.at(i) > that.ubig_value.at(i))
      {
        result = false;
        break;
      }
      if (ubig_value.at(i) != that.ubig_value.at(i))
      {
        equivalent = false;
        break;
      }
    }
  }
  return result && !equivalent;
}

ostream& operator<< (ostream& out, const ubigint& that) {
  string output;
  for(unsigned int i = 0; i < that.ubig_value.size(); i++)
  {
    output.append(1, static_cast <char> (that.ubig_value.at(i) + '0'));
  }
   return out << output;
}
