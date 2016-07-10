
#include "os/compat.h"

#include "os/debug.h"
#include "os/stacktrace.h"
#include "misc/sequence.h"

#include <iostream>

#include <stdio.h>

#include "os/extra.h"

#ifdef GCC
#include <signal.h>
#else
#include <windows.h>
#include <imagehlp.h>
#include "os/win32_errors.h"

#pragma comment(lib, "imagehlp.lib")
#endif

int main();

namespace reaper {
namespace os {

using namespace std;
using namespace misc;

namespace { debug::DebugOutput derr("stack"); }

#if defined(GCC) && defined(__i386__)
template<class Out>
void get_stack_trace_addr(Out o, int)
{
	void** fp = (void**)0;
	__asm ("movl %%ebp,%0" : "=r"(fp) : "r"(fp));
	derr << "main frame at: " << (void*)main << '\n';
	for (int i = 0; i < 50; ++i) {
		void* fr = (i == 2) ? *(fp+17) : *(fp+1);
		derr << "stack frame: " << (void*)fr << "\n";
		if (fr > (char*)main + 0x1000000)
			break;
		if (fr < (char*)main - 0x1000000)
			break;
		if (fr == (char*)main)
			break;
		*o++ = fr;
		fp = (void**)*fp;
	}

}

namespace {
	typedef pair<void*,string> Symbol;
	typedef vector<Symbol> SymTbl;
	SymTbl symtbl;
}

bool addr_cmp(const Symbol& p1, const Symbol& p2) {
	return p1.first < p2.first;
}

void get_symbols(string argv0)
{
	string cmd("nm -a ");
	cmd += argv0;
	cmd += " | c++filt";
	FILE* p = reaper::popen(cmd.c_str(), "r");

	while (!feof(p)) {
		char buf[500];
		unsigned int addr;
		char tmp[10], sym[1000];
		fgets(buf, 500, p);
		sscanf(buf, "%x %8s %[^\n]\n", &addr, tmp, sym);
		symtbl.push_back(make_pair((void*)addr, string(sym)));
	}
	sort(symtbl.begin(), symtbl.end(), addr_cmp);
	reaper::pclose(p);
}

template<class Out>
void get_stack_trace(Out o, int q)
{
	vector<void*> st;
	get_stack_trace_addr(back_inserter(st), q);
	for (int i = 0; i < st.size(); ++i) {
		void* a = st[i];
		SymTbl::iterator i = lower_bound(symtbl.begin(), symtbl.end(), 
						 make_pair(a, string("")), addr_cmp);
		--i;
		char buf[300];
		snprintf(buf, 300, "%08x => %s",
				reinterpret_cast<unsigned int>(a),
				(i == symtbl.end()) ? "<unknown>" : i->second.c_str());
		*o++ = string(buf);
	}
}

void sigsegv(int)
{
	printf("sigsegv caught\n");
	signal(SIGSEGV, SIG_DFL);
	vector<string> ptrs;
	get_stack_trace(back_inserter(ptrs), 2);
	printf("got stack trace\n");
	copy(ptrs.begin(), ptrs.end(), ostream_iterator<string>(derr, "\n"));
	*((char*)0) = 0;
}

void get_symbols(FILE* p)
{

	while (!feof(p)) {
		char buf[500];
		unsigned int addr = 0;
		char tmp[10] = "", sym[1000] = "";
		fgets(buf, 500, p);
		sscanf(buf, "%x %8s %s\n", &addr, tmp, sym);
		symtbl.push_back(make_pair((void*)addr, string(sym)));
	}
	sort(symtbl.begin(), symtbl.end(), addr_cmp);
}

void load_symbols_prog(std::string prog)
{
	string cmd("nm -a ");
	cmd += prog;
	FILE* p = reaper::popen(cmd.c_str(), "r");
	get_symbols(p);
	reaper::pclose(p);
}

void load_symbols_file(std::string file)
{
	FILE* p = fopen(file.c_str(), "r");
	get_symbols(p);
	fclose(p);
}

void catch_segfault()
{
	signal(SIGSEGV, sigsegv);
}
#else

# ifdef MSVC

void load_symbols_file(std::string file)
{
}

void load_symbols_prog(std::string prog)
{
}


template<class Out>
void get_stack_trace(Out o, CONTEXT* cxt)
{
	HANDLE proc = GetCurrentProcess();
	HANDLE thr  = GetCurrentThread();

	const int ih_sz = sizeof(IMAGEHLP_SYMBOL) + 200;
	IMAGEHLP_SYMBOL* sym = (IMAGEHLP_SYMBOL *) malloc(ih_sz);

	STACKFRAME s;
	memset( &s, '\0', sizeof(STACKFRAME));

	SymInitialize(proc, 0, 1);

	CONTEXT get_cxt;
	if (!cxt) {
		memset(&get_cxt, '\0', sizeof(CONTEXT));
		get_cxt.ContextFlags = CONTEXT_FULL;
		GetThreadContext(thr, &get_cxt);
		cxt = &get_cxt;
	}

	s.AddrPC.Offset = cxt->Eip;
	s.AddrPC.Mode = AddrModeFlat;
	s.AddrFrame.Offset = cxt->Ebp;
	s.AddrFrame.Mode = AddrModeFlat;

	memset(sym, '\0', ih_sz);
	sym->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
	sym->MaxNameLength = 500;


	for ( ; ; ) {
		if (!StackWalk(IMAGE_FILE_MACHINE_I386, proc, thr, &s, cxt, 0,
				SymFunctionTableAccess, SymGetModuleBase, 0))
			break;

		DWORD ofs = 0;
		if ( s.AddrPC.Offset != 0 && SymGetSymFromAddr(proc, s.AddrPC.Offset, &ofs, sym ) )
			*o++ = sym->Name;

	}

	SymCleanup(proc);
}

LONG __stdcall handle_exc(EXCEPTION_POINTERS* p)
{
	derr << "exception caught, stack:\n";
	vector<string> ptrs;
	get_stack_trace(back_inserter(ptrs), p->ContextRecord);
	std::copy(ptrs.begin(), ptrs.end(), ostream_iterator<string>(derr, "\n"));

	return EXCEPTION_EXECUTE_HANDLER;
}

void catch_segfault()
{
	SetUnhandledExceptionFilter(handle_exc);
}

# else

template<class T, class Q>
void get_stack_trace(T, Q)
{
}


void catch_segfault()
{
}

void load_symbols_prog(std::string prog)
{
}

void load_symbols_file(std::string file)
{
}



# endif

#endif

vector<string> get_stack_frames()
{
	vector<string> v;
	get_stack_trace(back_inserter(v), 0);
	return v;
}

}
}

