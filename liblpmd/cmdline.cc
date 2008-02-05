//
//
//

#include <iostream>
#include <list>

#include <lpmd/cmdline.h>
#include <lpmd/util.h>

using namespace lpmd;

//
class Option
{
 public:
   std::string longname;
   std::string shortname;
   std::string args;
};

//
class lpmd::CmdLineParserImpl
{
 public:
   CmdLineParserImpl() { };
   ~CmdLineParserImpl() { };

   std::list<Option> optlist;
   std::list<std::string> arglist;
};

UnknownCmdLineOption::UnknownCmdLineOption(const std::string & name) { optname = name; }

UnknownCmdLineOption::~UnknownCmdLineOption() throw() { }

const char * UnknownCmdLineOption::what() const throw()
{
 std::string tmp = ("Unknown command line option: " + optname); 
 return tmp.c_str();
}

//
//
//
CmdLineParser::CmdLineParser()
{
 clpimpl = new CmdLineParserImpl();
}

CmdLineParser::CmdLineParser(int argc, char *argv[])
{
 clpimpl = new CmdLineParserImpl();
 Parse(argc, argv);
}

CmdLineParser::~CmdLineParser() 
{ 
 delete clpimpl;
}

void CmdLineParser::DefineOption(const std::string & longname, const std::string & shortname, const std::string & args)
{
 CmdLineParserImpl & impl = *clpimpl;
 Option opt;
 opt.longname = longname;
 opt.shortname = shortname;
 opt.args = args;
 impl.optlist.push_back(opt);
}

void CmdLineParser::Parse(int argc, char *argv[])
{
 CmdLineParserImpl & impl = *clpimpl;
 Option curropt;
 std::list<std::string> optargs;
 for (int i=0;i<argc;++i)
 {
  if ((strlen(argv[i]) > 1) && (argv[i][0] == '-') && (argv[i][1] == '-'))
  {
   // Opcion larga 
   std::string optname(&argv[i][2]);
   bool optfound = false;
   for (std::list<Option>::const_iterator it=impl.optlist.begin();it!=impl.optlist.end();++it)
   {
    if (optname == (*it).longname)
    {
     curropt = (*it);
     optfound = true;
     break;
    }
   }
   if (optfound == false) throw UnknownCmdLineOption("--"+optname);
   optargs = ListOfTokens(curropt.args);
   AssignParameter(curropt.longname, "true");
  }
  else if ((strlen(argv[i]) > 0) && (argv[i][0] == '-'))
  {
   // Opcion corta
   std::string optname(&argv[i][1]);
   bool optfound = false;
   for (std::list<Option>::const_iterator it=impl.optlist.begin();it!=impl.optlist.end();++it)
   {
    if (optname == (*it).shortname)
    {
     curropt = (*it);
     optfound = true;
     break;
    }
   }
   if (optfound == false) throw UnknownCmdLineOption("-"+optname);
   optargs = ListOfTokens(curropt.args);
   AssignParameter(curropt.longname, "true");
  }
  else
  {
   // Argumento de una opcion o argumento general del programa
   if (optargs.size() > 0)
   {
    std::string this_arg = optargs.front();
    optargs.pop_front();
    AssignParameter(curropt.longname+"-"+this_arg, std::string(argv[i]));
   }
   else impl.arglist.push_back(std::string(argv[i]));
  }
 }
}

