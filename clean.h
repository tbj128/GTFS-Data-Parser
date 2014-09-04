#ifndef CLEAN_H
#define CLEAN_H
 
class Clean {
	private:
		int m_nMonth;
		int m_nDay;
		int m_nYear;
 
		Date() { } // private default constructor
 
	public:
		Date(int nMonth, int nDay, int nYear);
 
		void SetDate(int nMonth, int nDay, int nYear);
 
		int GetMonth() { return m_nMonth; }
		int GetDay()  { return m_nDay; }
		int GetYear() { return m_nYear; }
};
 
#endif