#include "Helper.h"

void xcept(const char *fmt, ...) {
  static char xcept_str[0x100];

	va_list args;
	va_start(args, fmt);
	memset(xcept_str, 0, sizeof(xcept_str));
	//vsnprintf(xcept_str, sizeof(xcept_str)-1, fmt, args);
  vsnprintf_s(xcept_str, sizeof(xcept_str)-1, 0x100, fmt, args);
	throw (const char*)xcept_str;
}

void log(const char *fmt, ...) {
  printf("log_enter\n");

	static Timer logTimer;
	va_list args;
	va_start(args,fmt);
	string s = vformat(fmt,args);
	double t = logTimer.getTime();
	s = format("%4.3f  %s\n",t,s.c_str());
	printf("%s",s.c_str());

  printf("log_exit\n");
  /*
	for (int i=0;i<10 && h_log_file == INVALID_HANDLE_VALUE;i++)
		h_log_file = CreateFile(format("log%d.txt",i).c_str(),GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_DELETE,0,CREATE_ALWAYS,0,0);
	DWORD written;
	WriteFile(h_log_file,s.c_str(),s.size(),&written,0);
  */
};

string format(const char *format, ...) {
	string res("");
	va_list args;
	va_start(args,format);
	size_t len = _vscprintf(format,args);
	res.resize(len+1);
	char*buf = (char*)res.c_str();
	vsprintf_s(buf,len+1,format,args);
	//buf[len]='\0';
	return res.c_str();
}

string vformat(const char *format, va_list args) {
	string res("");
	size_t len = _vscprintf(format,args);
	res.resize(len+1);
	char*buf = (char*)res.c_str();
	vsprintf_s(buf,len+1,format,args);
	//buf[len]='\0';
	return res.c_str();
}
