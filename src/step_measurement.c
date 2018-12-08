#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "mul_gen.h"

int step_measurement(FILE *fp, const efw_t f, flags_t flag, char *module){
  int exp = f->exp;
  int frac= f->frac;
  int width=f->width;
  char sub_module[64];
  flag->pipeline_flag = 0;

  if (flag->step1_flag == 1){
    fprintf(fp,
	    "circuit %s{\n"
	    "input a<%d>, b<%d>;\n"
	    "output  As, Aexp<%d>, Am1<%d>, Am2<%d>;\n"
	    "instrin do_in;\n"
	    "sel xmsb, ymsb;\n\n"
	    "sel added_exp<%d>;\n"
	    "\n",
	    module,
	    width, width,
	    exp, frac+1, frac+1,
	    exp+1
	    );

    fprintf(fp,
	    "instruct do_in par{\n"
	    );

    fpmulti_signxor_expadd(fp, exp, frac, width, flag);
    fprintf(fp,
	    "}\n"
	    "}\n"
	    );
    
    fprintf(fp,
	    "declare %s{\n"
	    "input a<%d>, b<%d>;\n"
	    "instrin do_in;\n"
	    "output As, Aexp<%d>, Am1<%d>, Am2<%d>;\n"
	    "instr_arg do_in(a, b);\n"
	    "}\n"
	    "module wrapper{\n"
	    "   input a<%d>, b<%d>;\n"
	    "  instrin do_in;\n"
	    " output As, Aexp<%d>, Am1<%d>, Am2<%d>;\n"
	    "%s step;\n"
	    "reg_wr ina<%d>, inb<%d>;\n"
	    "reg_wr as, aexp<%d>, am1<%d>, am2<%d>;\n"
	  
	    "instruct do_in par{\n"
	    "	ina := a;\n"
	    "	inb := b;\n"
	    "	step.do_in(ina, inb);\n"
	    "	as :=step.As;\n"
	    "	aexp := step.Aexp;\n"
	    "	am1:=step.Am1;\n"
	    "	am2:=step.Am2;\n"
	    "	As = as;\n"
	    "	Aexp = aexp;\n"
	    "	Am1= am1;\n"
	    "	Am2= am2;\n"
	    "   }\n"
	    "}\n",
	    module,
	    width, width,
	    exp, frac+1, frac+1,
	    width, width,
	    exp, frac+1, frac+1,
	    module,
	    width, width,
	    exp, frac+1, frac+1	  
	    );
  }else if(flag->step2_flag == 1){
    char multi_module[64];
    Multiplier_Generator(fp, frac, flag, multi_module);

    fprintf(fp,
	    "circuit %s{\n"
	    "    instrin do_in;\n"
	    "    %s multi;\n"
	    "    input As, Aexp<%d>, Am1<%d>, Am2<%d>;\n"
	    "    output Bs, Bexp<%d>, Bm<%d>, Bsticky<%d>;\n",
	    module,
	    multi_module,
	    exp, frac+1, frac+1,
	    exp, frac+4, frac-2	    
	    );
    fprintf(fp,
	    "    instruct do_in par{	\n"
	    );

    fpmulti_fracmult(fp, exp, frac, flag);
    
    fprintf(fp,
	    "    }\n"
	    "}\n"
	    
	    "declare %s{\n"
	    "    instrin do_in;\n"
	    
	    "    input As, Aexp<%d>, Am1<%d>, Am2<%d>;\n"
	    "    output Bs, Bexp<%d>, Bm<%d>, Bsticky<%d>;\n"
	    
	    "    instr_arg do_in(As, Aexp, Am1, Am2);\n"
	    "}\n"
	    
	    "module wrapper{\n"
	    "    input As, Aexp<%d>, Am1<%d>, Am2<%d>;\n"
	    "    output Bs, Bexp<%d>, Bm<%d>, Bsticky<%d>;\n"
	    "    instrin do_in;\n"
	    
	    "    %s step;\n"
	    
	    "    reg_wr as, aexp<%d>, am1<%d>, am2<%d>;\n"
	    "    reg_wr bs, bexp<%d>, bm<%d>, bsticky<%d>;\n"
	    
	    "    instruct do_in par{\n"
	    "	as := As;\n"
	    "	aexp := Aexp;\n"
	    "	am1:= Am1;\n"
	    "	am2:= Am2;\n"
	    "	step.do_in(as, aexp, am1, am2);\n"
	    "	bs := step.Bs;\n"
	    "	bexp := step.Bexp;\n"
	    "	bm := step.Bm;\n"
	    "   bsticky:=step.Bsticky;\n"
	    "	Bs = bs;\n"
	    "	Bexp = bexp;\n"
	    "	Bm = bm;\n"
	    "   Bsticky = bsticky;\n"
	    "    }\n"
	    "}\n"
	    "\n",
	    module,
	    exp, frac+1, frac+1,
	    exp, frac+4, frac-2,
	    exp, frac+1, frac+1,
	    exp, frac+4, frac-2,
	    module,
	    exp, frac+1, frac+1,
	    exp, frac+4, frac-2

	    );
    
  }else if(flag->step3_flag == 1){
    fprintf(fp,
	    "circuit %s{\n"
	    "    instrin do_in;\n"

	    "    input Bs, Bexp<%d>, Bm<%d>, Bsticky<%d>;\n"
	    "    output Cs, Cexp<%d>, Cm<%d>;\n"

	    "    sel normalized_frac<%d>;\n"
	    "    sel sticky;\n"
	    "    instruct do_in	par{\n",	    
	    module,
	    exp, frac+4, frac-2,
	    exp, frac+4,
	    frac+4
	    );
    
    fpmulti_normalize(fp, exp, frac, flag);
    fprintf(fp,
	    "}\n"
	    "}\n"
	    );
    
    fprintf(fp,
	    "declare %s{\n"
	    "    instrin do_in;\n"

	    "    input Bs, Bexp<%d>, Bm<%d>, Bsticky<%d>;\n"
	    "    output Cs, Cexp<%d>, Cm<%d>;\n"

	    "    instr_arg do_in(Bs, Bexp, Bm, Bsticky);\n"
	    "}\n"

	    "module wrapper{\n"
	    "    instrin do_in;\n"

	    "    input Bs, Bexp<%d>, Bm<%d>, Bsticky<%d>;\n"
	    "    output Cs, Cexp<%d>, Cm<%d>;\n"

	    "    %s step;\n"

	    "    reg_wr bs, bexp<%d>, bm<%d>, bsticky<%d>;\n"
	    "    reg_wr cs, cexp<%d>, cm<%d>;\n"

	    "    instruct do_in par{\n"
	    "	bs:=Bs;\n"
	    "	bexp:=Bexp;\n"
	    "	bm:=Bm;\n"
	    "   bsticky:=Bsticky;\n"
	    "	step.do_in(bs, bexp, bm, bsticky);\n"
	    "	cs:=step.Cs;\n"
	    "	cexp:=step.Cexp;\n"
	    "	cm:=step.Cm;\n"
	    "	Cs=cs;\n"
	    "	Cexp=cexp;\n"
	    "	Cm=cm;\n"
	    "    }\n"
	    "}\n",
	    module,
	    exp, frac+4, frac-2,
	    exp, frac+4,
	    exp, frac+4, frac-2,
	    exp, frac+4,
	    module,
	    exp, frac+4, frac-2,
	    exp, frac+4
	    );
    
  }else if(flag->step4_flag == 1){
    char incfrac[64];
    
    IncFrac_Generator(fp, frac, incfrac);
    fprintf(fp,
	    "circuit %s{\n"
	    "    instrin do_in;\n"
	    
	    "    %s incfrac;\n"
	    
	    "    sel rounded_frac<%d>;\n"
	    
	    "    input Cs, Cexp<%d>, Cm<%d>;\n"
	    "    output out<%d>;\n"
	    "    instruct do_in par{\n",
	    module,
	    incfrac,
	    frac,
	    exp, frac+4,
	    width
	    );

    fpmulti_round(fp, frac, width, flag);
    
    fprintf(fp,
	    "}\n"
	    "}\n"
	    );
    fprintf(fp,
	    "declare %s{\n"
	    "    instrin do_in;\n"
	    "    input Cs, Cexp<%d>, Cm<%d>;\n"
	    "    output out<%d>;\n"
	    "    instr_arg do_in(Cs, Cexp, Cm);\n"
	    "}\n"
	    
	    "module wrapper{\n"
	    "    instrin do_in;\n"
	    "    input Cs, Cexp<%d>, Cm<%d>;\n"
	    "    output out<%d>;\n"

	    "    %s step;\n"
	    
	    "    reg_wr cs, cexp<%d>, cm<%d>;\n"
	    "    reg_wr z<%d>;\n"
	    
	    "    instruct do_in par{\n"
	    "	cs:=Cs;\n"
	    "	cexp:=Cexp;\n"
	    "	cm:=Cm;\n"
	    "	z:=step.do_in(cs, cexp, cm).out;\n"
	    "	out = z;\n"
	    "    }\n"
	    "}\n",
	    module,
	    exp, frac+4,
	    width,
	    exp, frac+4,
	    width,
	    module,
	    exp, frac+4,
	    width
	    );
  }

  fprintf(stdout,
	  "Generate %s module\n",
	  module
	  );
  
  return 0;
}
