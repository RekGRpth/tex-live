/***** Autogenerated from runlabel.in; changes will be overwritten *****/

#line 1 "runtimebase.in"
/*****
 * runtimebase.in
 * Andy Hammerlindl  2009/07/28
 *
 * Common declarations needed for all code-generating .in files.
 *
 *****/


#line 1 "runlabel.in"
/*****
 * runlabel.in
 *
 * Runtime functions for label operations.
 *
 *****/

#line 1 "runtimebase.in"
#include "stack.h"
#include "types.h"
#include "builtin.h"
#include "entry.h"
#include "errormsg.h"
#include "array.h"
#include "triple.h"
#include "callable.h"
#include "opsymbols.h"

using vm::stack;
using vm::error;
using vm::array;
using vm::read;
using vm::callable;
using types::formal;
using types::function;
using camp::triple;

#define PRIMITIVE(name,Name,asyName) using types::prim##Name;
#include <primitives.h>
#undef PRIMITIVE

typedef double real;

void unused(void *);

namespace run {
array *copyArray(array *a);
array *copyArray2(array *a);
array *copyArray3(array *a);

double *copyTripleArray2Components(array *a, bool square=true, size_t dim2=0,
                                   GCPlacement placement=NoGC);
}

function *realRealFunction();

#define CURRENTPEN processData().currentpen

#line 19 "runlabel.in"
#include "picture.h"
#include "drawlabel.h"
#include "locate.h"

using namespace camp;
using namespace vm;
using namespace settings;

typedef array realarray;
typedef array stringarray;
typedef array penarray;
typedef array patharray;
typedef array patharray2;

using types::realArray;
using types::stringArray;
using types::penArray;
using types::pathArray;
using types::pathArray2;

void cannotread(const string& s) 
{
  ostringstream buf;
  buf << "Cannot read from " << s;
  error(buf);
}

void cannotwrite(const string& s) 
{
  ostringstream buf;
  buf << "Cannot write to " << s;
  error(buf);
}

pair readpair(stringstream& s, double hscale=1.0, double vscale=1.0)
{
  double x,y;
  s >> y;
  s >> x;
  return pair(hscale*x,vscale*y);
}

string ASYx="/ASYx {( ) print ASYX sub 12 string cvs print} bind def";
string ASYy="/ASYy {( ) print ASYY sub 12 string cvs print} bind def";
string pathforall="{(M) print ASYy ASYx} {(L) print ASYy ASYx} {(C) print ASYy ASYx ASYy ASYx ASYy ASYx} {(c) print} pathforall";
string currentpoint="print currentpoint ASYy ASYx ";
string ASYinit="/ASYX currentpoint pop def /ASYY currentpoint exch pop def ";
string ASY1="ASY1 {"+ASYinit+"/ASY1 false def} if ";

void endpath(std::ostream& ps) 
{
  ps << ASY1 << pathforall << " (M) " << currentpoint
     << "currentpoint newpath moveto} bind def" << endl;
}

void fillpath(std::ostream& ps)
{
  ps << "/fill {closepath ";
  endpath(ps);
}

void showpath(std::ostream& ps) 
{
  ps << ASYx << newl
     << ASYy << newl
     << "/ASY1 true def" << newl
     << "/stroke {strokepath ";
  endpath(ps);
  fillpath(ps);
}

array *readpath(const string& psname, bool keep, bool pdf=false,
                double hscale=1.0, double vsign=1.0)
{
  double vscale=vsign*hscale;
  array *PP=new array(0);

  char *oldPath=NULL;
  string dir=stripFile(outname());
  if(!dir.empty()) {
    oldPath=getPath();
    setPath(dir.c_str());
  }

  mem::vector<string> cmd;
  cmd.push_back(getSetting<string>("gs"));
  cmd.push_back("-q");
  cmd.push_back("-dBATCH");
  cmd.push_back("-P");
  if(safe) cmd.push_back("-dSAFER");
#ifdef __MSDOS__
  const string null="NUL";
#else
  const string null="/dev/null";
#endif
  cmd.push_back("-sDEVICE=eps2write");
  cmd.push_back("-sOutputFile="+null);
  cmd.push_back(stripDir(psname));
  iopipestream gs(cmd,"gs","Ghostscript");
  while(gs.running()) {
    stringstream buf;
    string s=gs.readline();
    if(s.empty()) break;
    if(!pdf) gs << newl;

// Workaround broken stringstream container in MacOS 10.9 libc++.
#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__FreeBSD_kernel__ )
    for(string::iterator i=s.begin(); i != s.end(); ++i) {
      if(isalpha(*i) && *i != 'e') {buf << " ";}
      buf << *i;
    }
#else    
    buf << s;
#endif
    
    if(verbose > 2) cout << endl;
  
    mem::vector<solvedKnot> nodes;
    solvedKnot node;
    bool cyclic=false;
    bool active=false;
  
    array *P=new array(0);
    PP->push(P);
    
    while(!buf.eof()) {
      char c;
      buf >> c;
      if(c == '>') break;

      switch(c) {
        case 'M':
        {
          if(active) {
            if(cyclic) {
              if(node.point == nodes[0].point)
                nodes[0].pre=node.pre;
              else {
                pair delta=(nodes[0].point-node.point)*third;
                node.post=node.point+delta;
                nodes[0].pre=nodes[0].point-delta;
                node.straight=true;
                nodes.push_back(node);
              }
            } else {
              node.post=node.point;
              node.straight=false;
              nodes.push_back(node);
            }
            if(cyclic) // Discard noncyclic paths.
              P->push(path(nodes,nodes.size(),cyclic));
            nodes.clear();
          }
          active=false;
          cyclic=false;
          node.pre=node.point=readpair(buf,hscale,vscale);
          node.straight=false;
          break;
        }
        case 'L':
        {
          pair point=readpair(buf,hscale,vscale);
          pair delta=(point-node.point)*third;
          node.post=node.point+delta;
          node.straight=true;
          nodes.push_back(node);
          active=true;
          node.pre=point-delta;
          node.point=point;
          break;
        }
        case 'C':
        {
          pair point=readpair(buf,hscale,vscale);
          pair pre=readpair(buf,hscale,vscale);
          node.post=readpair(buf,hscale,vscale);
          node.straight=false;
          nodes.push_back(node);
          active=true;
          node.pre=pre;
          node.point=point;
          break;
        }
        case 'c':
        {
          cyclic=true;
          break;
        }
      }
    }
  }
  
  if(oldPath != NULL)
    setPath(oldPath);
  
  if(!keep)
    unlink(psname.c_str());
  return PP;
}

// Autogenerated routines:



#ifndef NOSYM
#include "runlabel.symbols.h"

#endif
namespace run {
#line 222 "runlabel.in"
// void label(picture *f, string *s, string *size, transform t, pair position,           pair align, pen p);
void gen_runlabel0(stack *Stack)
{
  pen p=vm::pop<pen>(Stack);
  pair align=vm::pop<pair>(Stack);
  pair position=vm::pop<pair>(Stack);
  transform t=vm::pop<transform>(Stack);
  string * size=vm::pop<string *>(Stack);
  string * s=vm::pop<string *>(Stack);
  picture * f=vm::pop<picture *>(Stack);
#line 224 "runlabel.in"
  f->append(new drawLabel(*s,*size,t,position,align,p));
}

#line 228 "runlabel.in"
// bool labels(picture *f);
void gen_runlabel1(stack *Stack)
{
  picture * f=vm::pop<picture *>(Stack);
#line 229 "runlabel.in"
  {Stack->push<bool>(f->havelabels()); return;}
}

#line 233 "runlabel.in"
// realarray* texsize(string *s, pen p=CURRENTPEN);
void gen_runlabel2(stack *Stack)
{
  pen p=vm::pop<pen>(Stack,CURRENTPEN);
  string * s=vm::pop<string *>(Stack);
#line 234 "runlabel.in"
  texinit();
  processDataStruct &pd=processData();
  
  string texengine=getSetting<string>("tex");
  setpen(pd.tex,texengine,p);
  
  double width,height,depth;
  texbounds(width,height,depth,pd.tex,*s);
  
  array *t=new array(3);
  (*t)[0]=width;
  (*t)[1]=height;
  (*t)[2]=depth;
  {Stack->push<realarray*>(t); return;}
}

#line 251 "runlabel.in"
// patharray2* _texpath(stringarray *s, penarray *p);
void gen_runlabel3(stack *Stack)
{
  penarray * p=vm::pop<penarray *>(Stack);
  stringarray * s=vm::pop<stringarray *>(Stack);
#line 252 "runlabel.in"
  size_t n=checkArrays(s,p);
  if(n == 0) {Stack->push<patharray2*>(new array(0)); return;}
  
  string prefix=cleanpath(outname());
  string psname=auxname(prefix,"ps");
  string texname=auxname(prefix,"tex");
  string dviname=auxname(prefix,"dvi");
  bbox b;
  string texengine=getSetting<string>("tex");
  bool xe=settings::xe(texengine) || settings::context(texengine);
  texfile tex(texname,b,true);
  tex.miniprologue();
  
  for(size_t i=0; i < n; ++i) {
    tex.setfont(read<pen>(p,i));
    if(i != 0) {
      if(texengine == "context")
        tex.verbatimline("}\\page\\hbox{%");
      else if(texengine == "luatex" || texengine == "tex" ||
              texengine == "pdftex")
        tex.verbatimline("\\eject");
      else
        tex.verbatimline("\\newpage");
    }
    if(!xe) {
      tex.verbatimline("\\special{ps:");
      tex.verbatimline(ASYx);
      tex.verbatimline(ASYy);
      tex.verbatimline("/ASY1 true def");
      tex.verbatimline("/show {"+ASY1+
                       "currentpoint newpath moveto false charpath "+pathforall+
                       "} bind def");
      tex.verbatimline("/V {"+ASY1+"Ry neg Rx 4 copy 4 2 roll 2 copy 6 2 roll 2 copy (M) print ASYy ASYx (L) print ASYy add ASYx (L) print add ASYy add ASYx (L) print add ASYy ASYx (c) print} bind def}");
    }
    tex.verbatimline(read<string>(s,i)+"\\ %");
  }
  
  tex.epilogue(true);
  tex.close();
  
  int status=opentex(texname,prefix,!xe);
  
  string pdfname,pdfname2,psname2;
  bool keep=getSetting<bool>("keep");
  
  if(!status) {
    if(xe) {
      pdfname=auxname(prefix,"pdf");
      pdfname2=auxname(prefix+"_","pdf");
      psname2=auxname(prefix+"_","ps");
      if(!fs::exists(pdfname)) {Stack->push<patharray2*>(new array(n)); return;}
      std::ofstream ps(psname.c_str(),std::ios::binary);
      if(!ps) cannotwrite(psname);
      
      showpath(ps);

      mem::vector<string> pcmd;
      pcmd.push_back(getSetting<string>("gs"));
      pcmd.push_back("-q");
      pcmd.push_back("-dNOCACHE");
      pcmd.push_back("-dNOPAUSE");
      pcmd.push_back("-dBATCH");
      if(safe) pcmd.push_back("-dSAFER");
      pcmd.push_back("-sDEVICE=pdfwrite");
      pcmd.push_back("-sOutputFile="+pdfname2);
      pcmd.push_back(pdfname);
      status=System(pcmd,0,true,"gs");
      if(status == 0) {
        mem::vector<string> cmd;
        cmd.push_back(getSetting<string>("gs"));
        cmd.push_back("-q");
        cmd.push_back("-dNOCACHE");
        cmd.push_back("-dNOPAUSE");
        cmd.push_back("-dBATCH");
        if(safe) cmd.push_back("-dSAFER");
        cmd.push_back("-sDEVICE=eps2write");
        // Work around eps2write bug that forces all postscript to first page.
        cmd.push_back("-sOutputFile="+psname2+"%d");
        cmd.push_back(pdfname2);
        status=System(cmd,0,true,"gs");
      
        for(unsigned int i=1; i <= n ; ++i) {
          ostringstream buf;
          buf << psname2.c_str() << i;
          const char *name=buf.str().c_str();
          std::ifstream in(name,std::ios::binary);
          ps << in.rdbuf();
          ps << "(>\n) print flush\n";
          if(!keep) unlink(name);
        }
        ps.close();
      }
    } else {
      if(!fs::exists(dviname)) {Stack->push<patharray2*>(new array(n)); return;}
      mem::vector<string> dcmd;
      dcmd.push_back(getSetting<string>("dvips"));
      dcmd.push_back("-R");
      dcmd.push_back("-Pdownload35");
      dcmd.push_back("-D600");
      push_split(dcmd,getSetting<string>("dvipsOptions"));
      if(verbose <= 2) dcmd.push_back("-q");
      dcmd.push_back("-o"+psname);
      dcmd.push_back(dviname);
      status=System(dcmd,0,true,"dvips");
    }
  }

  if(status != 0)
    error("texpath failed");
    
  if(!keep) { // Delete temporary files.
    unlink(texname.c_str());
    if(!getSetting<bool>("keepaux"))
      unlink(auxname(prefix,"aux").c_str());
    unlink(auxname(prefix,"log").c_str());
    if(xe) {
      unlink(pdfname.c_str());
      unlink(pdfname2.c_str());
    } else
      unlink(dviname.c_str());
    if(settings::context(texengine)) {
      unlink(auxname(prefix,"top").c_str());
      unlink(auxname(prefix,"tua").c_str());
      unlink(auxname(prefix,"tuc").c_str());
      unlink(auxname(prefix,"tui").c_str());
    }
  }
  {Stack->push<patharray2*>(xe ? readpath(psname,keep,true,0.1) : 
    readpath(psname,keep,false,0.12,-1.0)); return;}
}

#line 384 "runlabel.in"
// patharray2* textpath(stringarray *s, penarray *p);
void gen_runlabel4(stack *Stack)
{
  penarray * p=vm::pop<penarray *>(Stack);
  stringarray * s=vm::pop<stringarray *>(Stack);
#line 385 "runlabel.in"
  size_t n=checkArrays(s,p);
  if(n == 0) {Stack->push<patharray2*>(new array(0)); return;}
  
  string prefix=cleanpath(outname());
  string outputname=auxname(prefix,getSetting<string>("textoutformat"));

  string textname=auxname(prefix,getSetting<string>("textextension"));
  std::ofstream text(textname.c_str());
  
  if(!text) cannotwrite(textname);

  for(size_t i=0; i < n; ++i) {
    text << getSetting<string>("textprologue") << newl
         << read<pen>(p,i).Font() << newl
         << read<string>(s,i) << newl
         << getSetting<string>("textepilogue") << endl;
  }
  text.close();
  
  string psname=auxname(prefix,"ps");
  std::ofstream ps(psname.c_str());
  if(!ps) cannotwrite(psname);

  showpath(ps);
  
  mem::vector<string> cmd;
  cmd.push_back(getSetting<string>("textcommand"));
  push_split(cmd,getSetting<string>("textcommandOptions"));
  cmd.push_back(textname);
  iopipestream typesetter(cmd);
  typesetter.block(true,false);
  
  mem::vector<string> cmd2;
  cmd2.push_back(getSetting<string>("gs"));
  cmd2.push_back("-q");
  cmd2.push_back("-dNOCACHE");
  cmd2.push_back("-dNOPAUSE");
  cmd2.push_back("-dBATCH");
  cmd2.push_back("-P");
  if(safe) cmd2.push_back("-dSAFER");
  cmd2.push_back("-sDEVICE=eps2write");
  cmd2.push_back("-sOutputFile=-");
  cmd2.push_back("-");
  iopipestream gs(cmd2,"gs","Ghostscript");
  gs.block(false,false);

  // TODO: Simplify by connecting the pipes directly.
  while(true) {
    string out;
    if(typesetter.isopen()) {
      typesetter >> out;
      if(!out.empty()) gs << out;
      else if(!typesetter.running()) {
        typesetter.pipeclose();
        gs.eof();
      }
    } 
    string out2;
    gs >> out2;
    if(out2.empty() && !gs.running()) break;
    ps << out2;
  }
  ps.close();
  
  if(verbose > 2) cout << endl;
  
  bool keep=getSetting<bool>("keep");
  if(!keep) // Delete temporary files.
    unlink(textname.c_str());
  {Stack->push<patharray2*>(readpath(psname,keep,false,0.1)); return;}
}

#line 458 "runlabel.in"
// patharray* _strokepath(path g, pen p=CURRENTPEN);
void gen_runlabel5(stack *Stack)
{
  pen p=vm::pop<pen>(Stack,CURRENTPEN);
  path g=vm::pop<path>(Stack);
#line 459 "runlabel.in"
  array *P=new array(0);
  if(g.size() == 0) {Stack->push<patharray*>(P); return;}
  
  string prefix=cleanpath(outname());
  string psname=auxname(prefix,"ps");
  bbox b;
  psfile ps(psname,false);
  ps.prologue(b);
  ps.verbatimline(ASYx);
  ps.verbatimline(ASYy);
  ps.verbatimline("/stroke {"+ASYinit+pathforall+"} bind def");
  ps.resetpen();
  ps.setpen(p);
  ps.write(g);
  ps.strokepath();
  ps.stroke(p);
  ps.verbatimline("(M) "+currentpoint);
  ps.epilogue();
  ps.close();
  array *a=readpath(psname,getSetting<bool>("keep"));
  {Stack->push<patharray*>(a->size() > 0 ? read<array *>(a,0) : a); return;}
}

} // namespace run

namespace trans {

void gen_runlabel_venv(venv &ve)
{
#line 222 "runlabel.in"
  addFunc(ve, run::gen_runlabel0, primVoid(), SYM(label), formal(primPicture(), SYM(f), false, false), formal(primString(), SYM(s), false, false), formal(primString(), SYM(size), false, false), formal(primTransform(), SYM(t), false, false), formal(primPair(), SYM(position), false, false), formal(primPair(), SYM(align), false, false), formal(primPen(), SYM(p), false, false));
#line 228 "runlabel.in"
  addFunc(ve, run::gen_runlabel1, primBoolean(), SYM(labels), formal(primPicture(), SYM(f), false, false));
#line 233 "runlabel.in"
  addFunc(ve, run::gen_runlabel2, realArray(), SYM(texsize), formal(primString(), SYM(s), false, false), formal(primPen(), SYM(p), true, false));
#line 251 "runlabel.in"
  addFunc(ve, run::gen_runlabel3, pathArray2() , SYM(_texpath), formal(stringArray() , SYM(s), false, false), formal(penArray() , SYM(p), false, false));
#line 384 "runlabel.in"
  addFunc(ve, run::gen_runlabel4, pathArray2() , SYM(textpath), formal(stringArray() , SYM(s), false, false), formal(penArray() , SYM(p), false, false));
#line 458 "runlabel.in"
  addFunc(ve, run::gen_runlabel5, pathArray() , SYM(_strokepath), formal(primPath(), SYM(g), false, false), formal(primPen(), SYM(p), true, false));
}

} // namespace trans
