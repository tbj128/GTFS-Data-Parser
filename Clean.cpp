#include "Clean.h"
 
// Date constructor
Clean::Clean(int nMonth, int nDay, int nYear) {
    SetDate(nMonth, nDay, nYear);
}
 
// Date member function
void Date::SetDate(int nMonth, int nDay, int nYear) {
    m_nMonth = nMonth;
    m_nDay = nDay;
    m_nYear = nYear;
}